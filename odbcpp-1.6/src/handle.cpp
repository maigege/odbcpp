//
// File:	src/handle.cpp
// Object:	Implementation of the low level handle object
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
#include	<iostream>



namespace odbcpp
{



/** \class handle
 *
 * \brief Manage an ODBC handle.
 *
 * This class is the base class for the environment, the
 * connection and the statement classes. It holds the object
 * handle and manages the errors.
 *
 * The handle needs to be allocated by the derived class
 * constructor or other functions will not behave as expected.
 * Especially, the handle destructor expects the f_handle
 * variable member to always be defined.
 *
 * The check() function is the one used to make sure that all
 * errors are being handled.
 *
 * \param[in] handle_type  The SQL handle type (i.e. SQL_HANDLE_ENV)
 *
 * \sa check()
 */



/** \fn handle::get_diagnostic() const
 *
 * \brief Get a reference to the current diagnostic of this handle.
 *
 * Each time an ODBC function is called, a diagnostic is generated.
 * Some of these diagnostics are saved in the handle diagnostic
 * variable member that can be retrieved using this function.
 *
 * The diagnostics hold information such as the last error or warning
 * and the number of rows affected by the last executed SQL statement.
 *
 * \return A reference to the handle diagnostic.
 */

/** \fn handle::get_handle_type() const
 *
 * \brief Retrieve the type of handle.
 *
 * This function returns the type of handle as specified at
 * the time the handle is constructed (it cannot be changed
 * at a later time.)
 *
 * The returned value is the untranslated ODBC type.
 *
 * \return The ODBC type of this handle.
 */

/** \var handle::f_handle
 *
 * \brief The ODBC handle.
 *
 * This variable member is the ODBC handle to be used with the
 * different SQL functions. This is a bare pointer.
 *
 * The handle will be freed at the time the handle is deleted.
 * It should not be freed by hand in any other way.
 *
 * For faster access, the handle is protected instead of private.
 */

/** \var handle::f_handle_type
 *
 * \brief The type of the ODBC handle.
 *
 * This variable member holds the type of the handle.
 *
 * Notice that it is defined at construction time and it is
 * read-only and thus it cannot be changed (in effect, it is
 * hard coded.)
 *
 * This is the type as defined in the ODBC header files.
 */

/** \var handle::f_diag
 *
 * \brief The last command diagnostics.
 *
 * Whenever a command is called, its diagnostics are saved in
 * this variable.
 *
 * A reference to the diagnostics can be retrieved using the
 * handle::get_diagnostic() function.
 *
 * A diagnostic is accessible in an exception when one is
 * generated.
 */

/** \brief Initialize the low level handle
 *
 * This function initialize the handle as expected.
 *
 * At the start, the handle pointer is set to NULL. The super class
 * must define that handle in its constructor.
 */
handle::handle(SQLSMALLINT handle_type) :
	f_handle(SQL_NULL_HANDLE),
	f_handle_type(handle_type)
	//f_diag -- auto-init
{
}


/** \brief Clean up the handle
 *
 * This function will free the handle as expected.
 *
 * If it is necessary to have a function called before
 * the handle can be freed, it needs to be done in the
 * desctructor of the specific handle.
 *
 * For instance, the connection handle needs to get
 * disconnected first.
 */
handle::~handle()
{
	// we cannot check for errors since we're in
	// a destructor and we cannot throw from here
	SQLFreeHandle(f_handle_type, f_handle);
}


/** \brief Check the return code of an SQL function call.
 *
 * This function checks the return code of an SQL function
 * called on this handle. If the return code represents an
 * error, then an odbcpp_error is thrown.
 *
 * In all cases, a diagnostic is generated.
 *
 * The following is how this function is usually used:
 *
 * \code
 *	check(SQLExecDirect( ... ));
 * \endcode
 *
 * And yes, that means most of the necessary error handling
 * is done in this function.
 *
 * If the function may return a code that needs special handling
 * such as SQL_NO_DATA returned by SQLFetch(), then you should
 * test the code before calling the check() function.
 *
 * \param[in] return_code   The code returned by an SQL function
 * \param[in] parent        The parent handle when allocating a child handle
 *
 * \return A copy of the return_code parameter
 */
SQLRETURN handle::check(SQLRETURN return_code, handle *parent) const
{
	// no error and no info
	if(return_code == SQL_SUCCESS) {
		diagnostic d;
		f_diag = d;
		return return_code;
	}

	// retrieve a copy of the diagnostic
	diagnostic d(f_handle_type, f_handle, parent);
	f_diag = d;

	// an error or just success with info?
	if(return_code == SQL_SUCCESS_WITH_INFO) {
		return return_code;
	}

//std::cerr << "return code is " << return_code << "\n";

	throw odbcpp_error(f_diag);
	/*NOTREACHED*/
}



/** \fn handle::get_handle() const
 *
 * \brief Retrieve the SQL handle
 *
 * This function lets you retrieve the SQL handle.
 *
 * \bug
 * Be careful, by getting and using the SQL handle directly
 * you may change the internal state without the
 * odbcpp library knowing and thus later have it generate
 * unexpected errors.
 *
 * \return The SQL handle
 */




}	// namespace odbcpp
