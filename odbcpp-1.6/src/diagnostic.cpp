//
// File:	src/diagnostic.cpp
// Object:	Object handling the diagnostics (generally errors)
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

#include	"odbcpp/handle.h"
#include	<sstream>
#include	<iostream>


namespace odbcpp
{


/** \class diag_t
 *
 * \brief Records the data read from the diagnostic function.
 *
 * This class is used to encompass all the useful information
 * from the diagnostic applied on to commands.
 *
 * \todo
 * We would need to add a function to extract the first three
 * entries of the f_message as defined in the ODBC
 * documentation.
 */


/** \var diag_t::f_server
 *
 * \brief The name of the server
 *
 * If the error comes from the ODBC library and you are connected
 * to a server (i.e. a connection or statement only), then this
 * variable will include the name of the server you are connected
 * to.
 *
 * This is the name defined between [ and ] in your odbc.ini file.
 */


/** \var diag_t::f_connection
 *
 * \brief The name of this connection.
 *
 * If the error happens and is read on a connection or a statement,
 * this field may be named after that connection.
 *
 * However, the connection name is not always available.
 */


/** \var diag_t::f_message
 *
 * \brief The corresponding error/warning/info message.
 *
 * Whenever a diagnostic is returned to us, a message is assiciated
 * to it. This is that message.
 *
 * The message may include specific information at the start written
 * between square brackets ([ and ]).
 */


/** \var diag_t::f_native_errno
 *
 * \brief The direct error number of the RDBM.
 *
 * The untranslated RDBM error. This can be useful for debug purposes.
 * It should not be presented to the end users. It can however be logged.
 */


/** \var diag_t::f_odbc_state
 *
 * \brief The ODBC error number.
 *
 * The translated RDBM error. Whenever the RDBM generates an error, its
 * driver converts that internal error number into an ODBC error. This
 * makes it a lot easier to manage errors from the outside.
 *
 * Note that this state is a string of exactly 5 characters. The content
 * of the string is defined in three parts:
 *
 * \li char[0] and char[1] -- the main error code (00, HY, etc.)
 * \li char[2] -- the sub-error code (0, S, L, etc.)
 * \li char[3] and char[4] -- the error number (01, 02, 03, etc.)
 *
 * The state can represent a warning. The parts should not be used
 * to infer the category of the error since they tend to overlap.
 */



/** \brief Initializes a diag_t structure
 *
 * This function initializes a diag_t structure to an empty state.
 *
 * These are used in diag classes.
 *
 * \sa diag
 */
diag_t::diag_t() :
	// f_server -- auto-init
	// f_connection -- auto-init
	// f_message -- auto-init
	// f_odbc_state -- auto-init
	f_native_errno(0)
{
}


/** \brief Generate an error message from this diag_t structure
 *
 * This function merges all the available information to create an
 * error message that can be displayed to the end user.
 *
 * \return a string with the message
 */
std::string diag_t::msg() const
{
	std::ostringstream	result;

	result << "[server:" << f_server
		<< "][connection:" << f_connection
		<< "][state:" << f_odbc_state
		<< "][native_errno:" << f_native_errno
		<< "] " << f_message;

	return result.str();
}





/** \class diagnostic
 *
 * \brief A class holding the information of a complete diagnostic
 *
 * This class is used to read the diagnostics from a handle after
 * a function was called on that handle. In general that happens
 * after calling the handle::check() function and just before
 * throwing an exception.
 *
 * This diagnostic will be saved in the odbcpp_error object.
 *
 * \sa odbcpp_error
 */


/** \fn diagnostic::diagnostic()
 *
 * \brief Create an empty diagnostic object.
 *
 * This constructor initialize an empty diagnostic object.
 *
 * You can later set an error by calling the set() function.
 *
 * \sa set()
 */


/** \brief Initializes a diagnostic object with a simple message
 *
 * Some odbcpp functions do not have access to any handle
 * and whenever they need to generate a diagnostic for an
 * exception, it uses this constructor with a message.
 *
 * \param[in] odbcpp_errno  One of the odbcpp errors
 * \param[in] message       An error message
 */
diagnostic::diagnostic(SQLINTEGER odbcpp_errno, const std::string& message)
{
	diag_t d;

	//d.f_server = ... -- none, empty is good
	//d.f_connection = ... -- none, empty is good
	d.f_message = message;
	d.f_odbc_state = "HY000";
	d.f_native_errno = odbcpp_errno;

	f_diag.push_back(d);
}


/** \brief Initializes a diagnostic object from a handle.
 *
 * This constructor queries the specified handle for the error
 * messages and numbers as available in the diag_t structure.
 *
 * \param[in] handle_type   The SQL type of handle
 * \param[in] hdl           The SQL handle to query
 * \param[in] parent        The parent handle when allocating a handle
 *
 * \sa set()
 */
diagnostic::diagnostic(SQLSMALLINT handle_type, SQLHANDLE hdl, handle *parent)
{
	if(hdl == 0) {
		if(parent == 0) {
			diag_t d;

			//d.f_server = ... -- none, empty is good
			//d.f_connection = ... -- none, empty is good
			d.f_message = "no handle available to read an error";
			d.f_odbc_state = "HY001";
			d.f_native_errno = odbcpp_error::ODBCPP_INTERNAL;

			f_diag.push_back(d);
		}
		else {
			set(parent->get_handle_type(), parent->get_handle());
		}
	}
	else {
		set(handle_type, hdl);
	}
}




/** \fn diagnostic::get_affected_rows() const
 *
 * \brief Retrieve the number of affected rows.
 *
 * Whenever an SQL command is executed, it affects a certain number of
 * rows. This is saved in an ODBC variable read by the diagnostic object.
 * The saved data can later be retrieved with this function.
 *
 * This number can be zero if there was no match.
 *
 * \return The number of rows that were selected, updated, deleted, inserted.
 */

/** \var diagnostic::f_affected_rows
 *
 * \brief Holds the number of rows that were affected by the last SQL command.
 *
 * This variable is used to retrieve the number of affected rows whenever a
 * diagnostic is created. This number can be zero when no row matched a WHERE
 * clause.
 */



/** \brief Initializes a diagnostic object from a handle.
 *
 * This function queries the specified handle for the error
 * messages and numbers as available in the diag_t structure.
 *
 * \param[in] handle_type   The SQL type of handle
 * \param[in] handle        The SQL handle to query
 */
void diagnostic::set(SQLSMALLINT handle_type, SQLHANDLE handle)
{
	SQLSMALLINT	record;

	// in case of a delete, update or insert, the number of rows affected
	// (note: that call may fail, we ignore the fact, it fails when the
	// last statement was not a SELECT, INSERT, UPDATE, DELETE and similar.)
	get_length(handle_type, handle, 0, SQL_DIAG_ROW_COUNT, f_affected_rows);

	// get all the possible state records
	for(record = 1;; ++record) {
		diag_t d;
		if(!get_string(handle_type, handle, record, SQL_DIAG_SERVER_NAME, d.f_server)) {
			return;
		}
		if(!get_string(handle_type, handle, record, SQL_DIAG_CONNECTION_NAME, d.f_connection)) {
			return;
		}
		if(!get_string(handle_type, handle, record, SQL_DIAG_MESSAGE_TEXT, d.f_message)) {
			return;
		}
		if(!get_integer(handle_type, handle, record, SQL_DIAG_NATIVE, d.f_native_errno)) {
			return;
		}
		if(!get_string(handle_type, handle, record, SQL_DIAG_SQLSTATE, d.f_odbc_state)) {
			return;
		}
		f_diag.push_back(d);
	}
}


/** \brief Retrieve a complete error message
 *
 * This function generates a complete error message from the
 * set of diagnostics. This can be huge.
 *
 * \return a string with the message
 */
std::string diagnostic::msg() const
{
	std::string	result;
	SQLSMALLINT	record;

	for(record = 0; record < size(); ++record) {
		if(!result.empty()) {
			result += "\n";
		}
		result += f_diag[record].msg();
	}

	return result;
}


/** \fn diagnostic::size() const
 *
 * \brief Returns the number of records defined in this diagnostic.
 *
 * This function returns the number of record defined in this diagnostic.
 *
 * The number of records can be zero.
 *
 * \return The number of record defined in this diagnostic
 */

/** \fn diagnostic::get(SQLSMALLINT record) const
 *
 * \brief Get the specified diagnostic record.
 *
 * Retrieve a diag_t object with the specified record number.
 *
 * Note that the record numbers start at 1 and go on up to
 * size() inclusive.
 *
 * \exception std::runtime_error
 * This function will throw an exception if the requested
 * record does not exist.
 *
 * \param[in] record  A record number from 1 to size() inclusive
 *
 * \return a copy of the diag_t at the specified position
 */

/** \fn diagnostic::operator [] (int record) const
 *
 * \brief Retrives the specified diagnostic.
 *
 * Retrieve a diag_t object with the specified record number.
 *
 * Note that the record numbers start at 1 and go on up to
 * size() inclusive.
 *
 * \exception std::runtime_error
 * This function will throw an exception if the requested
 * record does note exist.
 *
 * \param[in] record  A record number from 1 to size() inclusive
 *
 * \return a copy of the diag_t at the specified position
 */



/** \brief Get a diagnostic string
 *
 * This function retrieves a string from the specified handle.
 *
 * This function should be called after a call to any SQL function
 * other than diagnostic functions.
 *
 * \note
 * The \p string parameter is automatically set to "" by default.
 * If the function fails, you will get an empty string.
 *
 * \param[in]  handle_type   A valid SQL handle type
 * \param[in]  handle        A valid SQL handle
 * \param[in]  record        The record number starting at 1
 * \param[in]  identifier    The identifier to be retrieved; must be a string
 * \param[out] string        Where to put the resulting query
 *
 * \return true if the string was properly retrieved.
 */
bool diagnostic::get_string(SQLSMALLINT handle_type, SQLHANDLE handle,
		SQLSMALLINT record, SQLSMALLINT identifier, std::string& string)
{
	SQLCHAR		buffer[1024];
	SQLSMALLINT	size;
	SQLRETURN	return_code;

	// make the string empty by default
	string.clear();

	// retrieve the diagnostic
	size = sizeof(buffer);	// as far as I know, size should be an OUT only...
	return_code = SQLGetDiagField(handle_type, handle, record,
		identifier, buffer, sizeof(buffer), &size);

	switch(return_code) {
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:	// truncation occured... more than 1kb message?!
		break;

	// at this time, we do not need to have this seperate
	//case SQL_NO_DATA: -- we're done
	//	return false;

	default:
	// SQL_INVALID_HANDLE or SQL_ERROR
		// some error occured
		// TODO: what do we do here?!
		return false;

	}
	// make 100% sure that the string is null terminated
	buffer[sizeof(buffer) - 1] = '\0';
	string = reinterpret_cast<char *>(buffer);

	return true;
}


/** \brief Get a diagnostic integer
 *
 * This function retrieves an integer from the specified handle.
 *
 * This function should be called after a call to any SQL function
 * other than diagnostic functions.
 *
 * \note
 * The \p integer parameter is automatically set to 0 by default.
 * If the function fails, you will get zero.
 *
 * \param[in]  handle_type   A valid SQL handle type
 * \param[in]  handle        A valid SQL handle
 * \param[in]  record        The record number starting at 1
 * \param[in]  identifier    The identifier to be retrieved; must be a string
 * \param[out] integer       Where to put the resulting query
 *
 * \return true if the integer was properly retrieved.
 */
bool diagnostic::get_integer(SQLSMALLINT handle_type, SQLHANDLE handle,
		SQLSMALLINT record, SQLSMALLINT identifier, SQLINTEGER& integer)
{
	SQLSMALLINT	size;
	SQLRETURN	return_code;

	// set some default value, just in case
	integer = 0;

	size = sizeof(integer);
	return_code = SQLGetDiagField(handle_type, handle, record,
			identifier, &integer, sizeof(integer), &size);
	switch(return_code) {
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:	// truncation occured... more than 1kb message?!
		break;

	// at this time, we do not need to have this seperate
	//case SQL_NO_DATA: -- we're done
	//	return false;

	default:
	// SQL_INVALID_HANDLE or SQL_ERROR
		// some error occured
		// TODO: what do we do here?!
		return false;

	}

	return true;
}


/** \brief Get a diagnostic length
 *
 * This function retrieves a diagnostic length from the specified handle.
 *
 * This function should be called after a call to any SQL function
 * other than diagnostic functions.
 *
 * \note
 * The \p length parameter is automatically set to 0 by default.
 * If the function fails, you will get zero.
 *
 * \param[in]  handle_type   A valid SQL handle type
 * \param[in]  handle        A valid SQL handle
 * \param[in]  record        The record number starting at 1
 * \param[in]  identifier    The identifier to be retrieved; must be an SQLLEN variable
 * \param[out] length        Where to put the resulting query
 *
 * \return true if the integer was properly retrieved.
 */
bool diagnostic::get_length(SQLSMALLINT handle_type, SQLHANDLE handle,
		SQLSMALLINT record, SQLSMALLINT identifier, SQLLEN& length)
{
	SQLSMALLINT	size;
	SQLRETURN	return_code;

	// set some default value, just in case
	length = 0;

	size = sizeof(length);
	return_code = SQLGetDiagField(handle_type, handle, record,
			identifier, &length, sizeof(length), &size);
	switch(return_code) {
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:	// truncation occured?!
		break;

	// at this time, we do not need to have this seperate
	//case SQL_NO_DATA: -- we're done
	//	return false;

	default:
	// SQL_INVALID_HANDLE or SQL_ERROR
		// some error occured
		// TODO: what do we do here?!
		return false;

	}

	return true;
}


/** \var diagnostic::f_diag
 *
 * \brief The array of diagnostics
 *
 * This variable holds all the diag_t that were found in the
 * handle specified on the constructor of the diagnostic object.
 *
 * If only an error code and message were specified, then this
 * array has one entry.
 *
 * Use the size() function to retrive the size and the get()
 * or operator [] to retrieve the content.
 *
 * \sa size()
 * \sa get()
 */




}	// namespace odbcpp



