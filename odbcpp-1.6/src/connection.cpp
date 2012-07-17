//
// File:	src/connection.cpp
// Object:	Implementation of the connection object
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

#include	"odbcpp/connection.h"

namespace odbcpp
{


/** \class connection
 *
 * \brief Handles a connection between you and the database server.
 *
 * This class is the one you need to use to connect to a database.
 *
 * To use this object follow the following steps:
 *
 * \li Create an environment object
 * \li Create a connection object using that environment as the parent
 * \li Setup attributes as required
 * \li Call connect() with valid parameters
 *
 * \sa connection(environment& env)
 * \sa set_attr(SQLINTEGER attr, SQLINTEGER integer)
 * \sa set_attr(SQLINTEGER attr, SQLPOINTER ptr, SQLINTEGER length)
 * \sa connect(const std::string& dsn, const std::string& login, const std::string& passwd)
 */


/** \var connection::f_connected
 *
 * \brief A flag indicating whether the connection object is connected.
 *
 * Because there is no way to tell whether a connection is active or not,
 * we use a flag to let ourselves know. This is not 100% rock solid as
 * a network connection to a backend server could very well break and
 * we still would indicate that the connection is live.
 *
 * Use the connection::is_connected() const function to check this
 * flag.
 */

/** \var connection::f_environment
 *
 * \brief The parent environment of this connection
 *
 * The environment in which the connection is made.
 */

/** \brief The constructor allocates a connection handle.
 *
 * This function allocates a connection handle.
 *
 * The connection is in the DISCONNECTED state until you call the
 * connect() function.
 *
 * \param[in] env   The parent environment of this connection
 *
 * \exception odbcpp_error
 * If the connection handle cannot be allocated, throw an error.
 */
connection::connection(environment& env) :
	handle(SQL_HANDLE_DBC),
	f_environment(&env),
	f_connected(false)
{
	// we right away allocate a connection
	// throw if it fails
	check(SQLAllocHandle(f_handle_type, f_environment->get_handle(), &f_handle), f_environment);
}


/** \brief Ensures that the connection is closed.
 *
 * The destructor calls the disconnect function to
 * ensure that the connection is closed before freeing
 * the connection.
 */
connection::~connection()
{
	try { disconnect(); } catch(...) {}
}


/** \brief Set a integer connection attribute.
 *
 * This function sets one attribute in the connection.
 *
 * \param[in] attr     The attribute to be modified
 * \param[in] integer  The integer attribute data
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 */
void connection::set_attr(SQLINTEGER attr, SQLINTEGER integer)
{
	// TODO: see whether SQL_IS_INTEGER works even if SQL_IS_UINTEGER is required?
#ifdef _MSC_VER
	SQLPOINTER ptr;
	if(sizeof(ptr) < sizeof(integer)) {
		diagnostic d(odbcpp_error::ODBCPP_INTERNAL, std::string("the size of an SQLINTEGER is larger than SQLPOINTER?!"));
		throw odbcpp_error(d);
	}
	memcpy(&ptr, &integer, sizeof(integer));
	check(SQLSetConnectAttr(f_handle, attr, ptr, 0));
#else
	check(SQLSetConnectAttr(f_handle, attr, reinterpret_cast<SQLPOINTER>(integer), 0));
#endif
}


/** \brief Set a pointer connection attribute.
 *
 * This function sets one attribute in the connection.
 *
 * \param[in] attr   The attribute to be modified
 * \param[in] ptr    The pointer to the attribute data
 * \param[in] length The size of the data pointed by \p ptr, can be SQL_NTS for strings
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 */
void connection::set_attr(SQLINTEGER attr, SQLPOINTER ptr, SQLINTEGER length)
{
	check(SQLSetConnectAttr(f_handle, attr, ptr, length));
}


/** \brief Connect to a database.
 *
 * This function attempts to connect to a database.
 *
 * If it is possible that the connection fails, make sure that you
 * try/catch this function call.
 *
 * \param[in] dsn     The virtual name of the database to connect to
 * \param[in] login   The login name of the person connecting
 * \param[in] passwd  The password of the person connecting
 *
 * \exception odbcpp_error
 * If the connection fails, or an invalid name is specified, this
 * function will throw an odbcpp_error exception.
 */
void connection::connect(const std::string& dsn, const std::string& login, const std::string& passwd)
{
	check(SQLConnect(f_handle,
			(SQLCHAR *) dsn.c_str(), static_cast<SQLSMALLINT>(dsn.length()),
			(SQLCHAR *) login.c_str(), static_cast<SQLSMALLINT>(login.length()),
			(SQLCHAR *) passwd.c_str(), static_cast<SQLSMALLINT>(passwd.length())));
	f_connected = true;
}


/** \brief Disconnect from the server.
 *
 * This function ensures that the connection gets closed.
 *
 * If the connection is already closed, then the function will
 * most certainly generate an odbcpp_error.
 *
 * \exception odbcpp_error
 * If the connection fails, or an invalid name is specified, this
 * function will throw an odbcpp_error exception.
 */
void connection::disconnect()
{
	f_connected = false;
	check(SQLDisconnect(f_handle));
}


/** \fn connection::is_connected() const
 *
 * \brief Check whether the connection is currently active.
 *
 * This function returns true if the connection object was successfully
 * connected. This does not mean that the connection is still valid.
 * Only that it was properly connected.
 *
 * \return true if a call to connect() succeeded
 *
 * \sa connect()
 * \sa disconnect()
 */



/** \brief Immediately commit all the transactions.
 *
 * This function sends a commit to all the transactions running
 * on this connection.
 *
 * If there are no transactions to commit, nothing happens.
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 */
void connection::commit()
{
	check(SQLEndTran(f_handle_type, f_handle, SQL_COMMIT));
}



/** \brief Immediately rollback all the transactions.
 *
 * This function sends a rollback to all the transactions running
 * on this connection.
 *
 * If there are no transactions to rollback, nothing happens.
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 */
void connection::rollback()
{
	check(SQLEndTran(f_handle_type, f_handle, SQL_ROLLBACK));
}




}	// namespace odbcpp
