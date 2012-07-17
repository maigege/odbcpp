//
// File:	src/statement.cpp
// Object:	Implementation of the statement object
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
#include	<iostream>


namespace odbcpp
{


/** \class statement
 *
 * \brief The handle to run SQL statements.
 *
 * Create a statement in order to send an SQL order to a database.
 *
 * \todo
 * We need to receive a signal if the connection is closed since
 * the statement results won't match the connection. The statement
 * itself can however be reused, so we're fine on that.
 */


/** \brief The constructor allocates a statement handle.
 *
 * This function allocates a statement handle.
 *
 * The statement is empty.
 *
 * \param[in] conn   The parent connection where this statement will execute.
 *
 * \exception odbcpp_error
 * If the statement handle cannot be allocated, throw an error.
 */
statement::statement(connection& conn) :
	handle(SQL_HANDLE_STMT),
	f_connection(&conn),
	f_has_data(false),
	f_no_direct_fetch(false)
{
	// we right away allocate a connection
	// throw if it fails
	check(SQLAllocHandle(f_handle_type, f_connection->get_handle(), &f_handle), f_connection);
}



/** \brief Set a integer statement attribute.
 *
 * This function sets one attribute in the statement.
 *
 * \param[in] attr     The attribute to be modified
 * \param[in] integer  The integer attribute data
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 */
void statement::set_attr(SQLINTEGER attr, SQLINTEGER integer)
{
	// TODO: do we need to use the SQL_IS_[U]INTEGER types?
	check(SQLSetStmtAttr(f_handle, attr, int_to_ptr(integer), 0));
}


/** \brief Set a pointer statement attribute.
 *
 * This function sets one attribute in the statement.
 *
 * \param[in] attr   The attribute to be modified
 * \param[in] ptr    The pointer to the attribute data
 * \param[in] length The size of the data pointed by \p ptr, can be SQL_NTS for strings
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 */
void statement::set_attr(SQLINTEGER attr, SQLPOINTER ptr, SQLINTEGER length)
{
	// TODO: if ptr is a pointer to a known structure, length should
	//	 be set to SQL_IS_POINTER
	check(SQLSetStmtAttr(f_handle, attr, ptr, length));
}



/** \brief Set whether the SQLFecth() instruction can be used.
 *
 * There seems to be a bug in the PostgreSQL driver at this
 * time (v8.2, Ubuntu Hardy) that makes the SQLFecth() command
 * crash. Since there has been talked about marking that
 * function as deprecated, it is not a bad thing to mark it
 * as a "don't use".
 *
 * However, by default we still keep it since many systems
 * still use it.
 *
 * Note that to be able to use SQLFetchScroll() you need to
 * make use of a dynamic cursor. For that purpose, you need
 * to call the following first:
 *
 * \code
 * conn.set_attr(SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_DYNAMIC);
 * \endcode
 *
 * \param[in] no_direct_fetch   If false, use SQLFecth() when
 *                              orientation is SQL_FETCH_NEXT,
 *                              if true, never use SQLFetch(),
 *                              use SQLFetchScroll() instead.
 */
void statement::set_no_direct_fetch(bool no_direct_fetch)
{
	f_no_direct_fetch = no_direct_fetch;
}



/** \brief Execute an SQL statement
 *
 * This function executes an SQL statement. The result is kept with the
 * statement object and can be queried after this call.
 *
 * The execution of SQL statements happen in the backend (the server) and
 * thus are asynchroneous. If a statement is too long, it can be stopped
 * using the cancel() function.
 *
 * \param[in] order   The SQL order(s) to send the database
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 *
 * \sa fetch()
 * \sa cols()
 * \sa rows()
 * \sa cancel()
 */
void statement::execute(const std::string& order)
{
	f_has_data = false;

	check(SQLExecDirect(f_handle,
		const_cast<SQLCHAR *>(reinterpret_cast<const SQLCHAR *>(order.c_str())),
		SQL_NTS));

	f_has_data = true;
}


/** \brief Function used to check whether data is available.
 *
 * Whenever the statement is queried for some kind of data, this
 * function is called. If no SQL data is available, this function
 * generates an exception.
 *
 * SQL data is available only after a call to execute() and until
 * one of close_cursor(), cancel() and alike functions is called.
 *
 * \exception odbcpp_error
 * This error is thrown if the statement has not yet executed a
 * valid SQL statement. If the last statement executed generated
 * an error, then this function also throws the error.
 */
void statement::has_data() const
{
	if(!f_has_data) {
		diagnostic d(odbcpp_error::ODBCPP_NO_DATA, std::string("no SQL command was executed, the statement has no data available"));
		throw odbcpp_error(d);
	}
}


/** \brief Begin an SQL transaction
 *
 * This function sends the BEGIN instruction to the driver.
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 *
 * \sa execute()
 */
void statement::begin()
{
	execute("BEGIN");
}


/** \brief Commit an SQL transaction
 *
 * This function sends the COMMIT instruction to the driver.
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 *
 * \sa execute()
 */
void statement::commit()
{
	execute("COMMIT");
}


/** \brief Rollback an SQL transaction
 *
 * This function sends the ROLLBACK instruction to the driver.
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 *
 * \sa execute()
 */
void statement::rollback()
{
	execute("ROLLBACK");
}


/** \brief Cancel an SQL statement
 *
 * This function cancels the last SQL statement sent to the driver
 * with the execute function.
 *
 * \bug
 * With older versions of the ODBC libraries, the cancel() function
 * may actually free the statement under the hood.
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 *
 * \sa execute()
 */
void statement::cancel()
{
	has_data();
	f_has_data = false;

	check(SQLCancel(f_handle));
}


/** \brief Stop using the current cursor
 *
 * This function cancels the last SQL statement sent to the driver
 * with the execute function.
 *
 * \bug
 * With older versions of the ODBC libraries, the cancel() function
 * may actually free the statement under the hood.
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 *
 * \sa execute()
 */
void statement::close_cursor()
{
	has_data();
	f_has_data = false;

	check(SQLCloseCursor(f_handle));
}



/** \brief Query the number of cols resulting from a query.
 *
 * This function must be called after the execute() function was
 * successfully called.
 *
 * \return The number of rows that the SQL command generated.
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 */
SQLLEN statement::cols() const
{
	SQLSMALLINT	count;

	has_data();

	check(SQLNumResultCols(const_cast<SQLHANDLE>(f_handle), &count));

	return static_cast<SQLLEN>(count);
}



/** \brief Query the number of rows resulting from a query.
 *
 * This function must be called after the execute() function was
 * successfully called.
 *
 * \return The number of rows that the SQL command generated.
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 */
SQLLEN statement::rows() const
{
	SQLLEN	count;

	has_data();

	check(SQLRowCount(const_cast<SQLHANDLE>(f_handle), &count));

	return count;
}



/** \brief Fetch one row at the specified position.
 *
 * The fetch() function reads one row of data. Right after a call
 * to the execute() function, you need to call the fetch() function
 * before you can retrieve data.
 *
 * The orientation and offset pair is defined as follow:
 *
 * \code
 *	  Orientation                Offset
 *
 *	SQL_FETCH_NEXT           Unused
 *	SQL_FETCH_PRIOR          Unused
 *	SQL_FETCH_FIRST          Unused
 *	SQL_FETCH_LAST           Unused
 *	SQL_FETCH_ABSOLUTE       The exact position
 *	SQL_FETCH_RELATIVE       Added to the current position
 *	SQL_FETCH_BOOKMARK       Offset in the SQL_ATTR_FETCH_BOOKMARK_PTR array
 * \endcode
 *
 * \param[in,out] rec           The record where the row data is saved
 * \param[in]     orientation   The direction for the offset
 * \param[in]     offset        The offset used to move to that position to fetch
 *
 * \return true if the function fetched a row, false if there is no more data
 *
 * \todo
 * We need to handle truncated data. When a string buffer is too small,
 * we need to enlarge it and try to read the row again (too bad that we
 * cannot cleanly query the size of the cell as required!)
 *
 * \bug
 * At this time, the function handles the SQL_STILL_EXECUTING return
 * code as an error and generates an exception. I think that calling
 * the rows() function first will prevent this problem.
 *
 * \bug
 * Some ODBC drivers may not support all the orientations. If you do not
 * specify the orientation nor the offset, the simple SQLFetch() function
 * will be used as it has the same effect as the SQL_FETCH_NEXT orientation.
 *
 * \exception odbcpp_error
 * And odbcpp_error will be thrown if the SQL function returns an error.
 *
 * \sa rows()
 * \sa execute()
 */
bool statement::fetch(record_base& rec, SQLSMALLINT orientation, SQLLEN offset)
{
	SQLRETURN	return_code;

	// in case the record is not bound yet, do it now
	rec.bind(*this);

	// make sure we sent an SQL statement
	has_data();

	// fetch the row
	if(orientation == SQL_FETCH_NEXT && !f_no_direct_fetch) {
		return_code = SQLFetch(f_handle);
	}
	else {
		return_code = SQLFetchScroll(f_handle, orientation, offset);
	}
	if(return_code == SQL_NO_DATA) {
		return false;
	}

	//
	// TODO: handle the case were we have a buffer truncation
	//
	// i.e. we would need to call rec.bind() again and try to
	//      fetch again...
	//

	// check the returned code, if error, throw
	check(return_code);

	// some data may need to be copied...
	rec.finalize();

	// it worked!
	return true;
}




/** \var statement::f_connection
 *
 * \brief A pointer to the parent connection.
 *
 * Each statement is created within a specific connection. This is
 * the pointer back to the parent connection.
 */

/** \var statement::f_has_data
 *
 * \brief Defines whether data is available.
 *
 * This variable is set to true after each successful execute() call.
 * This enables the object to have a state and avoid fetching data
 * if the last execute failed or execute was not called.
 */

/** \var statement::f_no_direct_fetch
 *
 * \brief Defines whether SQLFetch() can be used.
 *
 * This variable determines whether the SQLFetch() function can be used
 * or not. By default, the function is used if the orientation is set
 * to SQL_FETCH_NEXT.
 *
 * There are cases when it is better to always use the SQLFetchScroll()
 * function. In these cases cann the set_no_direct_fetch() function to
 * avoid the SQLFetch() function altogether.
 *
 * \sa statement::set_no_direct_fetch()
 */




}	// namespace odbcpp
