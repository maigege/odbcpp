//
// File:	src/environment.cpp
// Object:	Environment object encapsulation 
// Project:	http://www.m2osw.com/odbcpp
// Author:	alexis_wilke@sourceforge.net
//
// Copyright (C)   2008-2011 Made to Order Software Corp.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
// or <http://gpl3.m2osw.com/>.
//

#include	"odbcpp/odbcpp.h"

namespace odbcpp
{



/** \class environment
 *
 * \brief An ODBC environment.
 *
 * Whenever you need to access one or more databases, you first need
 * to create an environment. Then create one or more connections to
 * different databases.
 *
 * Note that it is possible to create multiple environments, in which
 * case the commit() and rollback() calls will be specific to the
 * connections of each specific environment.
 */

/** \enum environment::data_source_type_t
 *
 * \brief The type of data source to get.
 *
 * This values defined by this enumeration are used to 
 * read all or a specific data source.
 *
 * \sa get_data_source()
 */

/** \var environment::DATA_SOURCE_TYPE_ALL
 *
 * \brief Read any data source.
 *
 * Use this value with get_data_source() to read the USER
 * and SYSTEM data sources.
 */

/** \var environment::DATA_SOURCE_TYPE_USER
 *
 * \brief Read the USER data sources.
 *
 * Use this value with get_data_source() to read the USER
 * data sources. (i.e. the user defined odbc.ini file,
 * usually ~/.odbc.ini)
 */

/** \var environment::DATA_SOURCE_TYPE_SYSTEM
 *
 * \brief Read the SYSTEM data sources.
 *
 * Use this value with get_data_source() to read the SYSTEM
 * data sources. (i.e. the system defined odbc.ini file,
 * usually /etc/odbc.ini)
 */

/** \brief Allocate an ODBC environment
 *
 * This function initializes an environment object.
 *
 * It immediately allocates an ODBC environment and thus
 * it can right away be used to create connections.
 *
 * When creating an environment, it is required to specify
 * the version to use. Because of this, the environment
 * constructor forces you to specify the version. The
 * version can be set to SQL_OV_ODBC2 or SQL_OV_ODBC3.
 * Version 1 won't work.
 *
 * At this time, the default version is SQL_OV_ODBC3.
 *
 * \param[in] version   The version to use with the environment.
 *
 * \exception odbc_exception_no_environment
 * If the allocation fails, an odbc_exception_no_environment
 * will be thrown.
 */
environment::environment(SQLUINTEGER version) :
	handle(SQL_HANDLE_ENV)
{
	// we right away allocate an environment
	// throw if it fails
	check(SQLAllocHandle(f_handle_type, SQL_NULL_HANDLE, &f_handle));

	// setup the current version, without that we run in problems
	// when trying to create the connection in a constructor
	set_attr(SQL_ATTR_ODBC_VERSION, version);
}



/** \brief Set an integer environment attribute.
 *
 * This function sets an environment attribute in this environment.
 *
 * \param[in] attr    The environment attribute to set
 * \param[in] integer The integer to set in this environment
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 */
void environment::set_attr(SQLINTEGER attr, SQLINTEGER integer)
{
	// Note: the length parameter is ignore in this case
	check(SQLSetEnvAttr(f_handle, attr, int_to_ptr(integer), 0));
}


/** \brief Set a pointer environment attribute.
 *
 * This function sets an environment attribute in this environment.
 *
 * \param[in] attr    The environment attribute to set
 * \param[in] ptr     The pointer to the data to set
 * \param[in] length  The size of the data being passed to the environment
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 */
void environment::set_attr(SQLINTEGER attr, SQLPOINTER ptr, SQLINTEGER length)
{
	check(SQLSetEnvAttr(f_handle, attr, ptr, length));
}


/** \brief Read the data source names and descriptions.
 *
 * This function reads the data source names and descriptions of all
 * the DSN, the User DSN or the Sytem DSN.
 *
 * The name of the server, as defined here, is the name of the section
 * defined between [ and ] in the system and user odbc.ini files.
 *
 * It looks as if the description was actually the name of the driver
 * not the actual description of the data source.
 *
 * The \p sources array will be cleared before new data sources are
 * read.
 *
 * \param[in]  type     The type of source to return
 * \param[out] sources  The vector of sources
 *
 * \bug
 * At this time you cannot distinguish between user and server data sources.
 */
void environment::get_data_source(data_source_type_t type, data_source_vector_t& sources)
{
	SQLRETURN	return_code;
	SQLUSMALLINT	direction;
	SQLCHAR		server_str[256], description_str[1024];
	data_source_t	src;

	sources.clear();

	switch(type) {
	default: //case DATA_SOURCE_TYPE_ALL:
		direction = SQL_FETCH_FIRST;
		break;

	case DATA_SOURCE_TYPE_USER:
		direction = SQL_FETCH_FIRST_USER;
		break;

	case DATA_SOURCE_TYPE_SYSTEM:
		direction = SQL_FETCH_FIRST_SYSTEM;
		break;

	}

	for(;;) {
		return_code = SQLDataSources(f_handle, direction,
					server_str, sizeof(server_str), NULL,
					description_str, sizeof(description_str), NULL);
		if(return_code == SQL_NO_DATA) {
			// done
			return;
		}
		check(return_code);

		src.f_server = reinterpret_cast<char *>(server_str);
		src.f_description = reinterpret_cast<char *>(description_str);

		sources.push_back(src);

		direction = SQL_FETCH_NEXT;
	}
}


/** \brief Immediately commit all the transactions.
 *
 * This function sends a commit to all the transactions of all the
 * connections present in this environment.
 *
 * If there are no connections, nothing happens.
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 */
void environment::commit()
{
	check(SQLEndTran(f_handle_type, f_handle, SQL_COMMIT));
}



/** \brief Immediately rollback all the transactions.
 *
 * This function sends a rollback to all the transactions of all the
 * connections present in this environment.
 *
 * If there are no connections, nothing happens.
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 */
void environment::rollback()
{
	check(SQLEndTran(f_handle_type, f_handle, SQL_ROLLBACK));
}



/** \class environment::data_source_t
 *
 * \brief Holds information about a data source
 *
 * This structure holds the name of the server and its
 * description.
 */

/** \var environment::data_source_t::f_server
 *
 * \brief Holds the name of the server.
 *
 * This variable member holds the name of the server which is
 * the name written between [ and ] in your different odbc.ini
 * files.
 */

/** \var environment::data_source_t::f_description
 *
 * \brief Holds the description of the server.
 *
 * This variable member holds the description of the
 * server.
 *
 * At this time, this looks like the name of the driver.
 * (i.e. the name found between [ and ] in your odbcinst.ini
 * file.)
 */


}	// namespace odbcpp


