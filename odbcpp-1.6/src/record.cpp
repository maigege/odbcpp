//
// File:	src/record.cpp
// Object:	Implementation of the record object in the odbcpp library
// Project:	http://www.m2osw.com/odbcpp
// Author:	alexis_wilke@sourceforge.net
//
// Copyright (C)   2008 Made to Order Software Corp.
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

#include	"odbcpp/record.h"
#include	<sqlucode.h>
#include	<sstream>
#include	<iostream>
#include	<cstring>

#ifdef _MSC_VER
#pragma warning(disable: 4146)
#endif

using namespace std;

namespace odbcpp
{


/** \class record_base
 *
 * \brief The record base class, used mainly internally
 *
 * This is a base class used to create records used to fetch data
 * from a database.
 *
 * You are not supposed to derive directly from the record_base.
 * Instead, use the record or the dynamic_record classes.
 *
 * \sa odbcpp::record
 * \sa odbcpp::dynamic_record
 */


/** \brief Initialize a record base object.
 *
 * This function initializes a record.
 */
record_base::record_base()
	//f_statement -- auto-init
{
}

/** \brief Initialize a record from another.
 *
 * This function copies a record in another.
 *
 * The new record base \p f_statement field will remain set to NULL.
 * Thus, one can copy a record base, but the new record looks
 * unbound.
 *
 * \param[in] rec   The record to be copied.
 */
record_base::record_base(const record_base& rec)
	//f_statement -- auto-init
{
	// avoid warnings
	(void) &rec;
}

/** \fn record_base::~record_base()
 *
 * \brief Ensure proper functioning of the virtual tables.
 */

/** \var record_base::f_statement
 *
 * \brief A link back to the statement that generated this record.
 *
 * This variable member is set the first time a fetch() is called
 * with this record.
 *
 * If you call unbind() then the statement pointer is reset to
 * NULL. This means the next call to a fetch() function will
 * re-bind the statement with the record. This lets you use the
 * same record with a different statement.
 */

/** \brief Copy a record in another.
 *
 * This function copies a record in another.
 *
 * The destination record base \p f_statement field is reset with a call
 * to unbind().
 *
 * \param[in] rec   The record to be copied.
 *
 * \sa record_base::record_base()
 * \sa record_base::record_base(const record_base& rec)
 * \sa record_base::unbind()
 */
record_base& record_base::operator = (const record_base& rec)
{
	unbind();

	// avoid warnings
	(void) &rec;

	return *this;
}

/** \fn record_base::is_dynamic() const
 *
 * \brief Bound all the columns to variables automatically?
 *
 * If this function returns true, then the record will dynamically
 * bound the columns to variables.
 *
 * \return true if the variables will automatically allocated and bound
 */

/** \fn record_base::is_bound() const
 *
 * \brief Bound was applied?
 *
 * Check whether the data defined in that record need to be bound
 * to the database or not. This is an internal flag and it should
 * not be necessary to use it outside of the odbcpp library.
 *
 * \return true if the bound was already applied
 */


/** \brief Binds a record to a statement
 *
 * This function binds this record to the specified statement.
 *
 * Note that one record cannot be bound more than once and especially
 * not to two different statements unless unbound first. Call the
 * record_base::unbind() function to unbind a record from a statement.
 *
 * \param[in] stmt   The statement to which this record is to be bound
 *
 * \sa bind_impl()
 */
void record_base::bind(statement& stmt)
{
	// already bound to this very statement?
	if(f_statement == stmt) {
		return;
	}

	// bound with another statement?!
	if(f_statement) {
		diagnostic d(odbcpp_error::ODBCPP_INCORRECT_USE, std::string("records can be used with at most one statement"));
		throw odbcpp_error(d);
	}

	f_statement = &stmt;

	// okay, we can bind then
	bind_impl();
}



/** \fn record_base::unbind()
 *
 * \brief Unbinds a record from its statement.
 *
 * This function unbinds the statement from this record. This is
 * used if you do not have direct control over the record and thus
 * cannot know when the record will be freed and release the statement.
 *
 * After a fetch() the data in a record is defined and thus keeping
 * it bound is not necessary. However, if you are to reuse the same
 * record over and over again, keeping it bound will make it a lot
 * faster to read the next row.
 *
 * \sa bind(statement& stmt)
 * \sa bind_impl()
 */

/** \fn record_base::finalize()
 *
 * \brief Called after each call to SQLFetch()
 *
 * After we read a row, there can be some data that we need to copy
 * from the internal buffer to the user buffer. Namely, we copy C
 * strings to std::string's in pre-bound records.
 */


/** \fn record_base::bind_impl()
 *
 * \brief Proceed with binding the variables.
 *
 * This function is the one used to bind a record variables
 * to an ODBC statement.
 *
 * \bug
 * The dynamic record binding changes the target type from
 * the SQL type to the corresponding C type. The table below
 * shows the different conversions.
 *
 * \code
 * The following shows what the SQL types are bound with.
 *
 * (=) SQL_<name> and SQL_C_<name> are equal
 * (<>) SQL_<name> and SQL_C_<name> differ and a mapping is necessary
 *
 * We do not make use of SQL_C_DEFAULT at this time.
 *
 *   SQL data types                    C data types
 *
 * SQL_UNKNOWN_TYPE       0            <undefined>
 * SQL_CHAR               1            SQL_C_CHAR (=)
 * SQL_NUMERIC            2            SQL_C_NUMERIC (=, SQL_C_CHAR up to v3.0 and thus <>)
 * SQL_DECIMAL            3            SQL_C_CHAR (<>)
 * SQL_INTEGER            4            SQL_C_LONG (=)
 * SQL_SMALLINT           5            SQL_C_SHORT (=)
 * SQL_FLOAT              6            SQL_C_DOUBLE (<>)
 * SQL_REAL               7            SQL_C_FLOAT (=)
 * SQL_DOUBLE             8            SQL_C_DOUBLE (=)
 * SQL_DATETIME           9            ignore, same as SQL_DATE
 * SQL_DATE               9            SQL_C_DATE (=)
 * SQL_INTERVAL          10            ignore, same as SQL_TIME
 * SQL_TIME              10            SQL_C_TIME (=)
 * SQL_TIMESTAMP         11            SQL_C_TIMESTAMP (=)
 * SQL_VARCHAR           12            SQL_C_CHAR (<>)
 * SQL_TYPE_DATE         91            SQL_C_TYPE_DATE (=)
 * SQL_TYPE_TIME         92            SQL_C_TYPE_TIME (=)
 * SQL_TYPE_TIMESTAMP    93            SQL_C_TYPE_TIMESTAMP (=)
 * SQL_LONGVARCHAR     (-1)            SQL_C_CHAR (<>)
 * SQL_BINARY          (-2)            SQL_C_BINARY (=)
 * SQL_VARBINARY       (-3)            SQL_C_CHAR (<>)
 * SQL_LONGVARBINARY   (-4)            SQL_C_CHAR (<>)
 * SQL_BIGINT          (-5)            SQL_C_SBIGINT (<>)
 * SQL_TINYINT         (-6)            SQL_C_TINYINT (=)
 * SQL_BIT             (-7)            SQL_C_BIT (=)
 * SQL_WCHAR           (-8)            SQL_C_WCHAR (=)
 * SQL_WVARCHAR        (-9)            SQL_C_WCHAR (<>)
 * SQL_WLONGVARCHAR   (-10)            SQL_C_WCHAR (<>)
 * SQL_GUID           (-11)            SQL_C_CHAR (<>)
 * \endcode
 *
 * \exception odbcpp_error
 * A record with no variable will raise an odbcpp_error.
 */


/** \class record
 *
 * \brief Manage a static (pre-bound) record to fetch data.
 *
 * This is a base class you will derive from in order to generate
 * a pre-defined record.
 *
 * This class is particularly adapted if you know the exact structure
 * of a table at compile time. It is also the fastest way to get data
 * from the database.
 *
 * Whenever you fetch data using a record object, the data is read
 * directly into your record variables. This makes it really fast.
 *
 * On the other hand, if you do not know what columns will be available,
 * then use a dynamic_record instead.
 *
 * To use a record properly:
 *
 * 1. derive from it in your own class;
 * 2. add variable members to your class;
 * 3. bind the variables on initialization (in the constructor).
 *
 * For instance, with a table including a SMALLINT and a VARCHAR one
 * can write:
 *
 * \code
 * class my_table : public record
 * {
 * public:
 * 	my_table() :
 * 		m_identifier(0)
 *		//m_name -- auto-init
 * 	{
 * 		bind(1, m_identifier);
 * 		bind(2, m_name);
 * 	}
 *
 * private:
 * 	short		m_identifier;
 * 	std::string	m_name;
 * };
 * \endcode
 *
 * Note that it is possible to use the name of the column instead of
 * the number. This can be a good idea if it is possible that the
 * order changes between machines.
 *
 * You should not bind a column more than once. Only the last bind()
 * will be in effect.
 *
 * \bug
 * You cannot bound a column by name and by index at the same time.
 * There is nothing that prevents this from happening. You have to
 * watch out.
 *
 * \bug
 * You cannot change the binding after a call to statement::fetch().
 *
 * \code
 * bind("id", m_identifier);
 * bind("name", m_name);
 * \endcode
 *
 * \sa odbcpp::record_base
 * \sa odbcpp::dynamic_record
 */


/** \fn record::empty() const
 *
 * \brief Check whether any bounds were added to this record.
 *
 * In order to use a record in a fetch, it is necessary to
 * have at least one variable defined.
 *
 * \return true if the record has no bounds yet
 */

/** \fn record::size() const
 *
 * \brief Returns the number of variables bound.
 *
 * This function returns the number of variables that you
 * have bound in this record.
 *
 * \return the number of variables that have been bound
 */

/** \var record::f_bind_by_name
 *
 * \brief Map holding the column binding information by name.
 *
 * This variable holds the named columns you bound to this
 * record. They are ordered by name.
 */

/** \var record::f_bind_by_col
 *
 * \brief Map holding the column binding information by index.
 *
 * This variable holds the indexed columns you bound to this
 * record. They are ordered by index.
 */


/** \brief Bind a string to the specified column
 *
 * This function binds the string variable to the specified
 * colunm.
 *
 * \param[in] name      The name of the column
 * \param[in] str       The string variable to bind
 * \param[in] is_null   A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, std::string& str, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_CHAR;
	//bi->f_data -- dynamic
	//bi->f_size -- dynamic
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	bi->f_string = &str;
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a string to the specified column
 *
 * This function binds the string variable to the specified
 * colunm.
 *
 * \param[in] col       The column number, starting at 1
 * \param[in] str       The string variable to bind
 * \param[in] is_null   A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, std::string& str, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_CHAR;
	//bi->f_data -- dynamic
	//bi->f_size -- dynamic
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	bi->f_string = &str;
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a string to the specified column
 *
 * This function binds the string variable to the specified
 * colunm.
 *
 * \param[in] name      The name of the column
 * \param[in] str       The string variable to bind
 * \param[in] is_null   A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, std::wstring& str, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_WCHAR;
	//bi->f_data -- dynamic
	//bi->f_size -- dynamic
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	bi->f_wstring = &str;
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a string to the specified column
 *
 * This function binds the string variable to the specified
 * colunm.
 *
 * \param[in] col       The column number, starting at 1
 * \param[in] str       The string variable to bind
 * \param[in] is_null   A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, std::wstring& str, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_WCHAR;
	//bi->f_data -- dynamic
	//bi->f_size -- dynamic
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	bi->f_wstring = &str;
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a tiny integer to the specified column
 *
 * This function binds the tiny integer variable to the specified
 * colunm.
 *
 * \param[in] name      The name of the column
 * \param[in] tiny_int  The tiny integer variable to bind
 * \param[in] is_null   A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQLCHAR& tiny_int, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_TINYINT;
	bi->f_data = &tiny_int;
	bi->f_size = sizeof(SQLSCHAR);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a tiny integer to the specified column
 *
 * This function binds the tiny integer variable to the specified
 * colunm.
 *
 * \param[in] col       The column number, starting at 1
 * \param[in] tiny_int  The tiny integer variable to bind
 * \param[in] is_null   A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQLCHAR& tiny_int, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_TINYINT;
	bi->f_data = &tiny_int;
	bi->f_size = sizeof(SQLSCHAR);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a tiny integer to the specified column
 *
 * This function binds the tiny integer variable to the specified
 * colunm.
 *
 * \param[in] name      The name of the column
 * \param[in] tiny_int  The tiny integer variable to bind
 * \param[in] is_null   A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQLSCHAR& tiny_int, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_TINYINT;
	bi->f_data = &tiny_int;
	bi->f_size = sizeof(SQLSCHAR);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a tiny integer to the specified column
 *
 * This function binds the tiny integer variable to the specified
 * colunm.
 *
 * \param[in] col       The column number, starting at 1
 * \param[in] tiny_int  The tiny integer variable to bind
 * \param[in] is_null   A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQLSCHAR& tiny_int, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_TINYINT;
	bi->f_data = &tiny_int;
	bi->f_size = sizeof(SQLSCHAR);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a small integer to the specified column
 *
 * This function binds the small integer variable to the specified
 * colunm.
 *
 * \param[in] name       The name of the column
 * \param[in] small_int  The small integer variable to bind
 * \param[in] is_null    A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQLSMALLINT& small_int, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_SHORT;
	bi->f_data = &small_int;
	bi->f_size = sizeof(SQLSMALLINT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a small integer to the specified column
 *
 * This function binds the small integer variable to the specified
 * colunm.
 *
 * \param[in] col        The column number, starting at 1
 * \param[in] small_int  The small integer variable to bind
 * \param[in] is_null    A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQLSMALLINT& small_int, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_SHORT;
	bi->f_data = &small_int;
	bi->f_size = sizeof(SQLSMALLINT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a small integer to the specified column
 *
 * This function binds the small integer variable to the specified
 * colunm.
 *
 * \param[in] name       The name of the column
 * \param[in] small_int  The small integer variable to bind
 * \param[in] is_null    A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQLUSMALLINT& small_int, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_USHORT;
	bi->f_data = &small_int;
	bi->f_size = sizeof(SQLUSMALLINT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a small integer to the specified column
 *
 * This function binds the small integer variable to the specified
 * colunm.
 *
 * \param[in] col        The column number, starting at 1
 * \param[in] small_int  The small integer variable to bind
 * \param[in] is_null    A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQLUSMALLINT& small_int, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_USHORT;
	bi->f_data = &small_int;
	bi->f_size = sizeof(SQLUSMALLINT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind an integer to the specified column
 *
 * This function binds the integer variable to the specified
 * colunm.
 *
 * \param[in] name     The name of the column
 * \param[in] integer  The integer variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQLINTEGER& integer, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_LONG;
	bi->f_data = &integer;
	bi->f_size = sizeof(SQLINTEGER);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind an integer to the specified column
 *
 * This function binds the integer variable to the specified
 * colunm.
 *
 * \param[in] col      The column number, starting at 1
 * \param[in] integer  The integer variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQLINTEGER& integer, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_LONG;
	bi->f_data = &integer;
	bi->f_size = sizeof(SQLINTEGER);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind an integer to the specified column
 *
 * This function binds the integer variable to the specified
 * colunm.
 *
 * \param[in] name     The name of the column
 * \param[in] integer  The integer variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQLUINTEGER& integer, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_ULONG;
	bi->f_data = &integer;
	bi->f_size = sizeof(SQLUINTEGER);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind an integer to the specified column
 *
 * This function binds the integer variable to the specified
 * colunm.
 *
 * \param[in] col      The column number, starting at 1
 * \param[in] integer  The integer variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQLUINTEGER& integer, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_ULONG;
	bi->f_data = &integer;
	bi->f_size = sizeof(SQLUINTEGER);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a big integer to the specified column
 *
 * This function binds the big integer variable to the specified
 * colunm.
 *
 * \param[in] name     The name of the column
 * \param[in] big_int  The big integer variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQLBIGINT& big_int, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_SBIGINT;
	bi->f_data = &big_int;
	bi->f_size = sizeof(SQLBIGINT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a big integer to the specified column
 *
 * This function binds the big integer variable to the specified
 * colunm.
 *
 * \param[in] col      The column number, starting at 1
 * \param[in] big_int  The big integer variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQLBIGINT& big_int, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_SBIGINT;
	bi->f_data = &big_int;
	bi->f_size = sizeof(SQLBIGINT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a big integer to the specified column
 *
 * This function binds the big integer variable to the specified
 * colunm.
 *
 * \param[in] name     The name of the column
 * \param[in] big_int  The big integer variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQLUBIGINT& big_int, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_UBIGINT;
	bi->f_data = &big_int;
	bi->f_size = sizeof(SQLUBIGINT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a big integer to the specified column
 *
 * This function binds the big integer variable to the specified
 * colunm.
 *
 * \param[in] col      The column number, starting at 1
 * \param[in] big_int  The big integer variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQLUBIGINT& big_int, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_UBIGINT;
	bi->f_data = &big_int;
	bi->f_size = sizeof(SQLUBIGINT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a real (C/C++ float) to the specified column
 *
 * This function binds the real variable to the specified
 * colunm.
 *
 * \warning
 * An SQLREAL is really a float in C/C++.
 *
 * \param[in] name     The name of the column
 * \param[in] real     The real variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQLREAL& real, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_FLOAT;
	bi->f_data = &real;
	bi->f_size = sizeof(SQLREAL);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a real (C/C++ float) to the specified column
 *
 * This function binds the real variable to the specified
 * colunm.
 *
 * \warning
 * An SQLREAL is really a float in C/C++.
 *
 * \param[in] col      The column number, starting at 1
 * \param[in] real     The real variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQLREAL& real, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_FLOAT;
	bi->f_data = &real;
	bi->f_size = sizeof(SQLREAL);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a float (C/C++ double) to the specified column
 *
 * This function binds the float variable to the specified
 * colunm.
 *
 * \warning
 * An SQLFLOAT is really a double in C/C++.
 *
 * \param[in] name     The name of the column
 * \param[in] dbl      The double variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQLFLOAT& dbl, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_DOUBLE;
	bi->f_data = &dbl;
	bi->f_size = sizeof(SQLFLOAT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a float (C/C++ double) to the specified column
 *
 * This function binds the float variable to the specified
 * colunm.
 *
 * \warning
 * An SQLFLOAT is really a double in C/C++.
 *
 * \param[in] col      The column number, starting at 1
 * \param[in] dbl      The double variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQLFLOAT& dbl, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_DOUBLE;
	bi->f_data = &dbl;
	bi->f_size = sizeof(SQLFLOAT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a binary buffer to the specified column
 *
 * This function binds the binary variable to the specified
 * colunm.
 *
 * \param[in] name     The name of the column
 * \param[in] binary   The binary variable to bind
 * \param[in] length   The size of the binary buffer in bytes
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQLCHAR *binary, SQLLEN length, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_BINARY;
	bi->f_data = binary;
	bi->f_size = length;
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a binary buffer to the specified column
 *
 * This function binds the binary variable to the specified
 * colunm.
 *
 * \param[in] col      The column number, starting at 1
 * \param[in] binary   The binary variable to bind
 * \param[in] length   The size of the binary buffer in bytes
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQLCHAR *binary, SQLLEN length, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_BINARY;
	bi->f_data = binary;
	bi->f_size = length;
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a date to the specified column
 *
 * This function binds the date variable to the specified
 * colunm.
 *
 * \param[in] name     The name of the column
 * \param[in] date     The date variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQL_DATE_STRUCT& date, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_DATE;
	bi->f_data = &date;
	bi->f_size = sizeof(SQL_DATE_STRUCT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a date to the specified column
 *
 * This function binds the date variable to the specified
 * colunm.
 *
 * \param[in] col      The column number, starting at 1
 * \param[in] date     The date variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQL_DATE_STRUCT& date, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_DATE;
	bi->f_data = &date;
	bi->f_size = sizeof(SQL_DATE_STRUCT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a time to the specified column
 *
 * This function binds the time variable to the specified
 * colunm.
 *
 * \param[in] name     The name of the column
 * \param[in] time     The time variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQL_TIME_STRUCT& time, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_TIME;
	bi->f_data = &time;
	bi->f_size = sizeof(SQL_TIME_STRUCT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a time to the specified column
 *
 * This function binds the time variable to the specified
 * colunm.
 *
 * \param[in] col      The column number, starting at 1
 * \param[in] time     The time variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQL_TIME_STRUCT& time, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_TIME;
	bi->f_data = &time;
	bi->f_size = sizeof(SQL_TIME_STRUCT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a timestamp to the specified column
 *
 * This function binds the timestamp variable to the specified
 * colunm.
 *
 * \param[in] name       The name of the column
 * \param[in] timestamp  The timestamp variable to bind
 * \param[in] is_null    A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQL_TIMESTAMP_STRUCT& timestamp, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_TIMESTAMP;
	bi->f_data = &timestamp;
	bi->f_size = sizeof(SQL_TIMESTAMP_STRUCT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a timestamp to the specified column
 *
 * This function binds the timestamp variable to the specified
 * colunm.
 *
 * \param[in] col        The column number, starting at 1
 * \param[in] timestamp  The timestamp variable to bind
 * \param[in] is_null    A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQL_TIMESTAMP_STRUCT& timestamp, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_TIMESTAMP;
	bi->f_data = &timestamp;
	bi->f_size = sizeof(SQL_TIMESTAMP_STRUCT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a Numeric to the specified column
 *
 * This function binds the numeric variable to the specified
 * colunm.
 *
 * \param[in] name     The name of column
 * \param[in] numeric  The numeric variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQL_NUMERIC_STRUCT& numeric, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_NUMERIC;
	bi->f_data = &numeric;
	bi->f_size = sizeof(SQL_NUMERIC_STRUCT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a Numeric to the specified column
 *
 * This function binds the numeric variable to the specified
 * colunm.
 *
 * \param[in] col      The column number, starting at 1
 * \param[in] numeric  The numeric variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQL_NUMERIC_STRUCT& numeric, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_NUMERIC;
	bi->f_data = &numeric;
	bi->f_size = sizeof(SQL_NUMERIC_STRUCT);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


/** \brief Bind a GUID to the specified column
 *
 * This function binds the GUID variable to the specified
 * colunm.
 *
 * \param[in] name     The name of the column
 * \param[in] guid     The guid variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(const std::string& name, SQLGUID& guid, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	bi->f_name = name;
	//bi->f_col -- unused
	bi->f_target_type = SQL_C_GUID;
	bi->f_data = &guid;
	bi->f_size = sizeof(SQLGUID);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- dynamic
	f_bind_by_name.insert(bind_info_name_t(name, bi));
}


/** \brief Bind a GUID to the specified column
 *
 * This function binds the GUID variable to the specified
 * colunm.
 *
 * \param[in] col      The column number, starting at 1
 * \param[in] guid     The guid variable to bind
 * \param[in] is_null  A pointer to a boolean variable set to true whenever this column is NULL in the database
 */
void record::bind(SQLSMALLINT col, SQLGUID& guid, bool *is_null)
{
	bind_info_t	*bi = new bind_info_t;

	//bi->f_name -- unused
	bi->f_col = col;
	bi->f_target_type = SQL_C_GUID;
	bi->f_data = &guid;
	bi->f_size = sizeof(SQLGUID);
	//bi->f_fetch_size -- dynamic
	bi->f_is_null = is_null;
	//bi->f_data_buffer -- auto
	//bi->f_string -- unused
	f_bind_by_col.insert(bind_info_col_t(col, bi));
}


// documented in record_base
void record::bind_impl()
{
	SQLSMALLINT	idx, max, type, name_length;
	SQLULEN		size;
	SQLCHAR		column_name[256];
	bind_info_t	*info;

	// TODO: should we check the column types (at least in debug)
	//	 against the target type?

	max = static_cast<SQLSMALLINT>(f_statement->cols());
	for(idx = 1; idx <= max; ++idx) {

		// if the user defined this by column, bind blindly
		bind_info_col_map_t::iterator by_col;
		by_col = f_bind_by_col.find(idx);
		if(by_col == f_bind_by_col.end()) {
			// search by name then...
			name_length = sizeof(column_name);	// probably useless
			f_statement->check(SQLDescribeCol(
				f_statement->get_handle(),// StatementHandle
				idx,			// ColumnNumber
				column_name,		// ColumnName
				sizeof(column_name),	// BufferLength
				&name_length,		// NameLengthPtr
				&type,			// DataTypePtr
				&size,			// ColumnSizePtr
				NULL,			// DecimalDigitsPtr
				NULL));			// NullablePtr

			// make totally sure that the string is null terminated
			column_name[sizeof(column_name) - 1] = '\0';
			bind_info_name_map_t::iterator by_name;
			by_name = f_bind_by_name.find(reinterpret_cast<char *>(column_name));
			if(by_name == f_bind_by_name.end()) {
				// could not find by column nor name, skip this column
				continue;
			}
			info = by_name->second;
		}
		else {
			info = by_col->second;
			size = 0;
		}

		// got some info, let's bind
		if(info->f_target_type == SQL_C_CHAR
		|| info->f_target_type == SQL_C_WCHAR) {
			if(size == 0) {
				f_statement->check(SQLDescribeCol(
					f_statement->get_handle(),// StatementHandle
					idx,			// ColumnNumber
					NULL,			// ColumnName
					0,			// BufferLength
					NULL,			// NameLengthPtr
					NULL,			// DataTypePtr
					&info->f_size,		// ColumnSizePtr
					NULL,			// DecimalDigitsPtr
					NULL));			// NullablePtr
			}
			else {
				info->f_size = size;
			}
			if(info->f_size == 0) {
				// a column cannot say that the string is always empty, can it?
				info->f_size = 8 * 1024;	// default to 8Kb
			}
			if(info->f_target_type == SQL_C_WCHAR) {
				// make sure the size is even if we read SQLWCHAR characters
				// (SQLWCHAR characters are UCS-2, UTF-16 or UCS-4)
				info->f_size = (info->f_size + 1) * sizeof(SQLWCHAR);
			}
			else {
				info->f_size += sizeof(SQLCHAR);
			}
			// add a character so CHAR(1) generates a buffer of 2 characters
			// we use sizeof(SQLWCHAR) since it is the larger than sizeof(SQLCHAR)
		//	std::cout << "new buffer!!!" << endl;
			info->f_data_buffer.reset(new buffer_char_t(info->f_size + sizeof(SQLWCHAR)));
			info->f_data = info->f_data_buffer->get();
		}

		f_statement->check(SQLBindCol(
			f_statement->get_handle(),
			idx,
			info->f_target_type,
			info->f_data,
			info->f_size,
			&info->f_fetch_size));
	}
}



// documented in the record_base
void record::finalize()
{
	bind_info_col_map_t::iterator by_col(f_bind_by_col.begin());
	while(by_col != f_bind_by_col.end()) {
		finalize_info(by_col->second);
		++by_col;
	}

	bind_info_name_map_t::iterator by_name(f_bind_by_name.begin());
	while(by_name != f_bind_by_name.end()) {
		finalize_info(by_name->second);
		++by_name;
	}
}


/** \brief Finalize the setup of the given info.
 *
 * This function finalizes the specified info.
 *
 * In effect it will update the necessary null pointer boolean values
 * and when a string was bound, it will copy the C-string data to the
 * C++ string.
 *
 * \param[in] info   The info to finalize
 */
void record::finalize_info(smartptr<record::bind_info_t> &info)
{
	if(info->f_is_null != 0) {
		*info->f_is_null = info->f_fetch_size == SQL_NULL_DATA;
	}

	// We want to clear all the strings in case no data is available for them
	// it is a good idea to have a default like this.
	if(info->f_string != 0) {
		if(info->f_target_type == SQL_C_CHAR) {
			info->f_string->clear();
		}
		else if(info->f_target_type == SQL_C_WCHAR) {
			// the pointer f_string & f_wstring is shared thus we know it isn't null
			info->f_wstring->clear();
		}
	}

	if(info->f_data != 0
	&& info->f_string != 0
	&& info->f_size > 0
	&& info->f_fetch_size != SQL_NULL_DATA) {
		if(info->f_target_type == SQL_C_CHAR) {
			// make sure that the data buffer is null terminated
			// note that we have 1 extra character in the buffer, see bind_impl()
			char *s = reinterpret_cast<char *>(info->f_data);
			s[info->f_size] = '\0';
			*info->f_string = s;
		}
		else if(info->f_target_type == SQL_C_WCHAR) {
			// make sure that the data buffer is null terminated
			// note that we have 1 extra character in the buffer, see bind_impl()
			SQLWCHAR *s = reinterpret_cast<SQLWCHAR *>(info->f_data);
			s[info->f_size / sizeof(SQLWCHAR)] = '\0';
			while(*s != static_cast<SQLWCHAR>(0)) {
				// TODO: if we detect an 0xFFFE or 0xFEFF we could also
				//	 swap the bytes as required...
				*info->f_wstring += static_cast<wchar_t>(*s++);
			}
		}
	}
}



/** \class record::bind_info_t
 *
 * \brief The structure used to hold the binding information
 *
 * This structure stores the binding information for each column
 * you are interested in this record.
 *
 * It includes the name, index, data type and a pointer to your
 * variable.
 */

/** \fn record::bind_info_t::bind_info_t()
 *
 * \brief The constructor used to initialize the info.
 *
 * This contructor initializes the info to defaults so the
 * structure is always valid.
 */

/** \var record::bind_info_t::f_name
 *
 * \brief Holds the name of the column.
 *
 * This name is used to bind the column to your variable.
 *
 * If the name is not defined, then f_col is used instead.
 */

/** \var record::bind_info_t::f_col
 *
 * \brief Holds the column index.
 *
 * This index is used to bind the column to your variable.
 *
 * If the name is defined, then f_name is used instead.
 */

/** \var record::bind_info_t::f_target_type
 *
 * \brief The expected type of the column.
 *
 * This value represents the type of the column such as SQL_C_SHORT
 * or SQL_C_TIMESTAMP.
 */

/** \var record::bind_info_t::f_data
 *
 * \brief The pointer to your variable.
 *
 * In most cases, this is the pointer to your variable. The only
 * two exceptions at this time are the string and wstring types.
 *
 * This pointer is never freed. It has to be done by you. Do not
 * free your variable until after you are done with calls to
 * the statement::fetch() function.
 */

/** \var record::bind_info_t::f_size
 *
 * \brief The number of bytes in this variable buffer.
 *
 * This value represents the exact size of the variable buffer in
 * bytes. The fetch() command will never write more data to your
 * variable.
 */

/** \var record::bind_info_t::f_fetch_size
 *
 * \brief The number of bytes the last fetch() command read.
 *
 * This value represents the exact size of the variable buffer that
 * has been defined by the last fetch() command.
 */

/** \var record::bind_info_t::f_is_null
 *
 * \brief A pointer to a variable defining whether a column is null.
 *
 * This variable is set by the statement::fetch() call. Whenever the
 * column is undefined (null), the boolean pointed by f_is_null is set
 * to true. Otherwise it is set to false.
 *
 * The binding of the variable has no effect on its content. It will
 * be changed only by a call to fetch().
 */

/** \var record::bind_info_t::f_data_buffer
 *
 * \brief The buffer used to hold dynamically allocated data pointers.
 *
 * This variable is allocated only when a dynamically allocated data
 * buffer is necessary. This is the case for the string and wstring
 * types.
 */

/** \var record::bind_info_t::f_string
 *
 * \brief A pointer to the caller string.
 *
 * This variable holds a pointer to the user string. It is used in
 * the finalize() function to copy the string read with fetch()
 * in the caller string.
 */

/** \var record::bind_info_t::f_wstring
 *
 * \brief A pointer to the caller string.
 *
 * This variable holds a pointer to the user string. It is used in
 * the finalize() function to copy the string read with fetch()
 * in the caller string.
 */





/** \class dynamic_record
 *
 * \brief A record that is automatically linked at run time.
 *
 * This type of record, you usually do not derive from. It is used
 * to read rows of data from a table of which columns aren't known
 * until run time.
 *
 * If you do know the columns at the time you compile your code,
 * you may want to consider using the record class instead. It will
 * be a little faster (avoid one copy for most data.)
 *
 * \sa record
 */

/** \fn dynamic_record::empty() const
 *
 * \brief Check whether the bounds were added to this record.
 *
 * Before a dynamic record is used in a statmenet::fetch(), it is first
 * bound to dynamically allocated variables. This function returns
 * true if these dynamic variables have not yet been allocated.
 *
 * \return true if the dynamic record was not yet allocated
 */

/** \fn dynamic_record::size() const
 *
 * \brief Return the number of bound records.
 *
 * After a successful call to statmenet::fetch(), a dynamic record
 * includes a set of dynamic variables that can be read using the
 * get() functions.
 *
 * The get() functions accepts the column name or a record index
 * starting at 1 and going up to size() inclusive.
 *
 * \return The number of records (variables, columns) allocated in this dynamic record
 */

/** \var dynamic_record::f_bind_by_name
 *
 * \brief Map holding the column binding information by name.
 *
 * This variable holds all the named columns ordered by name.
 *
 * It is used internally to quickly find the columns by name
 * (i.e. whenever you use one of the get() functions with a column
 * name instead of an index.)
 */

/** \var dynamic_record::f_bind_by_col
 *
 * \brief Array holding the column binding information by index.
 *
 * This variable holds an array of all the columns.
 *
 * This array is used whenever you get a column using an
 * index.
 */


/** \brief Return the name of a column.
 *
 * This function retrieves the name of the specified column.
 * Quite practicle if you do not know the column names.
 *
 * \note
 * It is possible that a column was not given a name.
 * When that happens, this function returns an empty string.
 *
 * \param[in] col   The column number.
 *
 * \return The name of the column.
 */
const std::string& dynamic_record::column_name(SQLSMALLINT col) const
{
	return find_column(col, SQL_UNKNOWN_TYPE)->f_name;
}

/** \brief Return the number of a named column.
 *
 * This function returns the indice of the named column.
 * Quite practicle if you only know the column name but
 * want to access the data many times and thus want to
 * use the indice which is faster.
 *
 * \param[in] name   The name of the column.
 *
 * \return The column number.
 */
SQLSMALLINT dynamic_record::column_number(const std::string& name) const
{
	return find_column(name, SQL_UNKNOWN_TYPE)->f_col;
}


// documented in record_base
void dynamic_record::bind_impl()
{
	SQLSMALLINT	idx, max, name_length;
	SQLCHAR		column_name[256];

	max = static_cast<SQLSMALLINT>(f_statement->cols());
	for(idx = 1; idx <= max; ++idx) {
		// we want the info to be reset on each loop
		bind_info_t *info = new bind_info_t;
		info->f_col = idx;

		// get the next column info
		name_length = sizeof(column_name);	// probably useless
		f_statement->check(SQLDescribeCol(
			f_statement->get_handle(),// StatementHandle
			idx,			// ColumnNumber
			column_name,		// ColumnName
			sizeof(column_name),	// BufferLength
			&name_length,		// NameLengthPtr
			&info->f_target_type,	// DataTypePtr
			&info->f_size,		// ColumnSizePtr in characters
			&info->f_decimal_digits,	// DecimalDigitsPtr
			NULL));			// NullablePtr

		// We must change the SQL type of a corresponding C type
		switch(info->f_target_type) {
		case SQL_VARCHAR:
		case SQL_LONGVARCHAR:	// also named TEXT
		case SQL_BINARY:	// also named OBJECT
		case SQL_VARBINARY:
		case SQL_LONGVARBINARY:
		case SQL_GUID:
		case SQL_DECIMAL:
			info->f_bind_type = SQL_C_CHAR;
			info->f_size += sizeof(SQLCHAR);
			break;

		case SQL_WLONGVARCHAR:
		case SQL_WVARCHAR:
			info->f_bind_type = SQL_C_WCHAR;
			info->f_size = (info->f_size + 1) * sizeof(SQLWCHAR);
			break;

		case SQL_CHAR:
			info->f_bind_type = info->f_target_type;
			info->f_size += sizeof(SQLCHAR);
			break;

		case SQL_WCHAR:
			info->f_bind_type = info->f_target_type;
			info->f_size = (info->f_size + 1) * sizeof(SQLWCHAR);
			break;

		case SQL_BIGINT:
			info->f_bind_type = SQL_C_SBIGINT;
			break;

		case SQL_FLOAT:
			info->f_bind_type = SQL_C_DOUBLE;
			break;

		default:
			info->f_bind_type = info->f_target_type;
			break;

		}

		// copy the name if any
		if(name_length >= sizeof(column_name)) {
			name_length = sizeof(column_name) - 1;
		}
		column_name[name_length] = '\0';
		info->f_name = reinterpret_cast<char *>(column_name);

		// at this point info->f_size is the buffer size in bytes,
		// it can be longer than necessary for SQLBindCol()
		info->f_data.reset(new buffer_char_t(info->f_size));

		f_statement->check(SQLBindCol(
			f_statement->get_handle(),
			idx,
			info->f_bind_type,
			info->f_data->get(),
			info->f_size,
			&info->f_fetch_size));
		if(!info->f_name.empty()) {
			f_bind_by_name.insert(bind_info_name_t(info->f_name, info));
		}
		f_bind_by_col.push_back(info);
	}
}

/** \brief Search for a column by name.
 *
 * This function searches for a column using its name.
 * If the column cannot be found, the function returns
 * false.
 *
 * If you call one of the get() functions with a column
 * name that does not exist, the get() functions will
 * throw an exception.
 *
 * \param[in] name   The name of the column to search for
 *
 * \return true if the named column exists; false otherwise
 */
bool dynamic_record::exists(const std::string& name)
{
	bind_info_name_map_t::iterator	itr;

	itr = f_bind_by_name.find(name);
	return itr != f_bind_by_name.end();
}


/** \brief Search a column by index and check its type.
 *
 * This function searches a column using its index.
 *
 * If the column does not exist, then an exception is
 * thrown. The index is limited between 1 and size().
 *
 * If the column exists, but does not have the corresponding type,
 * then an exception is thrown.
 *
 * Set target_type to SQL_UNKNOWN_TYPE to avoid the check of
 * the type.
 *
 * The \p except_null parameter can be set to true to get a column
 * only if the content is not null. This is used internally by all
 * the get() function to avoid returning an invalid value for null
 * columns.
 *
 * \param[in] name           The name of the column to search
 * \param[in] target_type    The expected type for the found column
 * \param[in] except_null    Refuse the column if the data is set to NULL
 *
 * \return A pointer to the corresponding column information
 *
 * \exception odbcpp_error
 * If the column is missing or has a different type,
 * then this function throws.
 */
const smartptr<dynamic_record::bind_info_t>& dynamic_record::find_column(const std::string& name, SQLSMALLINT target_type, bool except_null) const
{
	bind_info_name_map_t::const_iterator	itr;

	// find the column by name
	itr = f_bind_by_name.find(name);
	if(itr == f_bind_by_name.end()) {
		diagnostic d(odbcpp_error::ODBCPP_NOT_FOUND, "the column \"" + name + "\" was not found in this record");
		throw odbcpp_error(d);
	}

	// verify the column info and if correct return it
	return verify_column(itr->second, target_type, except_null);
}



/** \brief Search a column by name and check its type.
 *
 * This function searches a column using its name.
 *
 * If the column does not exist, then an exception is
 * thrown.
 *
 * If the column exists, but does not have the corresponding type,
 * then an exception is thrown.
 *
 * Set target_type to SQL_UNKNOWN_TYPE to avoid the check of
 * the type.
 *
 * \param[in] col            The number of the column to find
 * \param[in] target_type    The expected type for the found column
 * \param[in] except_null    Refuse the column if the data is set to NULL
 *
 * \return A pointer to the corresponding column information
 *
 * \exception odbcpp_error
 * If the column is missing or has a different type,
 * then this function throws.
 */
const smartptr<dynamic_record::bind_info_t>& dynamic_record::find_column(SQLSMALLINT col, SQLSMALLINT target_type, bool except_null) const
{
	--col;
	if(static_cast<SQLUSMALLINT>(col) >= f_bind_by_col.size()) {
		diagnostic d(odbcpp_error::ODBCPP_NOT_FOUND, std::string("there is not that many columns in this record"));
		throw odbcpp_error(d);
	}

	// verify the column info and if correct return it
	return verify_column(f_bind_by_col[col], target_type, except_null);
}



/** \brief Verify that the column matches what the user wants.
 *
 * This function is a private function that verifies that the
 * found column has the correct type.
 *
 * \param[in] info           The column information structure
 * \param[in] target_type    The expected type for the found column
 * \param[in] except_null    Refuse the column if the data is set to NULL
 *
 * \return A pointer to the corresponding column information
 *
 * \exception odbcpp_error
 * If the column has a different type or the column cannot be
 * null (i.e. except_null is true), then this function throws.
 */
const smartptr<dynamic_record::bind_info_t>& dynamic_record::verify_column(const smartptr<bind_info_t>& info, SQLSMALLINT target_type, bool except_null) const
{
	// verify that the column is of the proper type
	if(target_type != SQL_UNKNOWN_TYPE
	&& info->f_bind_type != target_type) {
		// handle special cases
		switch(target_type) {
		case SQL_C_CHAR:
			target_type = SQL_VARCHAR;
			break;

		case SQL_C_DATE: // 2.x versus 3.x
			target_type = SQL_TYPE_DATE;
			break;

		case SQL_C_TIME: // 2.x versus 3.x
			target_type = SQL_TYPE_TIME;
			break;

		case SQL_C_TIMESTAMP: // 2.x versus 3.x
			target_type = SQL_TYPE_TIMESTAMP;
			break;

		}
		if(info->f_bind_type != target_type) {
			std::ostringstream err;
			err << "type mismatch for column \"" + info->f_name + "\"";
			err << " (expected " << info->f_bind_type << ", requested " << target_type << ")";
			diagnostic d(odbcpp_error::ODBCPP_TYPE_MISMATCH, err.str());
			throw odbcpp_error(d);
		}
	}

	if(except_null && info->f_fetch_size == SQL_NULL_DATA) {
		diagnostic d(odbcpp_error::ODBCPP_NO_DATA, std::string("this column is NULL and cannot be retrieved"));
		throw odbcpp_error(d);
	}

	return info;
}



/** \brief Retrieve the type of the named column
 *
 * This function searches for the named column and then
 * return its type. This gives you a way to dynamically
 * browse all the columns of a dynamic record if you have
 * no clue what these are.
 *
 * \param[in] name   The name of the column to check
 *
 * \return The SQL type of the specified column
 *
 * \exception odbcpp_error
 * If the named column cannot be found, this function will
 * throw an error.
 */
SQLSMALLINT dynamic_record::get_type(const std::string& name) const
{
	return find_column(name, SQL_UNKNOWN_TYPE)->f_target_type;
}


/** \brief Retrieve the type of the column
 *
 * This function searches for the column and then
 * return its type. This gives you a way to dynamically
 * browse all the columns of a dynamic record if you have
 * no clue what these are.
 *
 * The column number goes from 1 to dynamic_record::size()
 * inclusive.
 *
 * \param[in] col   The number of the column to check
 *
 * \return The SQL type of the specified column
 *
 * \exception odbcpp_error
 * If the column does not exist, this function will
 * throw an error.
 */
SQLSMALLINT dynamic_record::get_type(SQLSMALLINT col) const
{
	return find_column(col, SQL_UNKNOWN_TYPE)->f_target_type;
}


/** \brief Retrieve the number of decimal digits of the named column
 *
 * This function searches for the named column and then
 * return its decimal digits (precision and/or scale). This
 * determines the number of digits in the seconds fraction
 * and numerics.
 *
 * \param[in] name   The name of the column to check
 *
 * \return The number of decimal digits
 *
 * \exception odbcpp_error
 * If the named column cannot be found, this function will
 * throw an error.
 */
SQLSMALLINT dynamic_record::get_decimal_digits(const std::string& name) const
{
	return find_column(name, SQL_UNKNOWN_TYPE)->f_decimal_digits;
}


/** \brief Retrieve the number of decimal digits of the column
 *
 * This function searches for the named column and then
 * return its decimal digits (precision and/or scale). This
 * determines the number of digits in the seconds fraction
 * and numerics.
 *
 * The column number goes from 1 to dynamic_record::size()
 * inclusive.
 *
 * \param[in] col   The number of the column to check
 *
 * \return The number of decimal digits
 *
 * \exception odbcpp_error
 * If the column does not exist, this function will
 * throw an error.
 */
SQLSMALLINT dynamic_record::get_decimal_digits(SQLSMALLINT col) const
{
	return find_column(col, SQL_UNKNOWN_TYPE)->f_decimal_digits;
}


/** \brief Check whether the named column is NULL.
 *
 * This function searches for the named column and then
 * return true if it is NULL (as defined in the Database.)
 *
 * \param[in] name   The name of the column to check
 *
 * \return true if the column data is NULL, false otherwise
 *
 * \exception odbcpp_error
 * If the named column cannot be found, this function will
 * throw an error.
 */
SQLSMALLINT dynamic_record::get_is_null(const std::string& name) const
{
	return find_column(name, SQL_UNKNOWN_TYPE)->f_fetch_size == SQL_NULL_DATA;
}


/** \brief Retrieve the type of the column
 *
 * This function searches for the named column and then
 * return true if it is NULL (as defined in the Database.)
 *
 * The column number goes from 1 to dynamic_record::size()
 * inclusive.
 *
 * \param[in] col   The number of the column to check
 *
 * \return true if the column data is NULL, false otherwise
 *
 * \exception odbcpp_error
 * If the column does not exist, this function will
 * throw an error.
 */
SQLSMALLINT dynamic_record::get_is_null(SQLSMALLINT col) const
{
	return find_column(col, SQL_UNKNOWN_TYPE)->f_fetch_size == SQL_NULL_DATA;
}


/** \brief Retrieve the size of the named column.
 *
 * This function searches for the named column and then
 * return its size in bytes. This is useful for any
 * column that has a variable size.
 *
 * Null terminated strings can have their size set to
 * SQL_NTS.
 *
 * Columns that are not set (i.e. are NULL) have a
 * size set to SQL_NULL_DATA.
 *
 * \param[in] name   The name of the column to check
 *
 * \return the size of the column in bytes
 *
 * \exception odbcpp_error
 * If the named column cannot be found, this function will
 * throw an error.
 */
SQLLEN dynamic_record::get_size(const std::string& name) const
{
	return find_column(name, SQL_UNKNOWN_TYPE)->f_fetch_size;
}


/** \brief Retrieve the size of the column.
 *
 * This function searches for the named column and then
 * return its size in bytes. This is useful for any
 * column that has a variable size.
 *
 * Null terminated strings can have their size set to
 * SQL_NTS.
 *
 * Columns that are not set (i.e. are NULL) have a
 * size set to SQL_NULL_DATA.
 *
 * The column number goes from 1 to dynamic_record::size()
 * inclusive.
 *
 * \param[in] col   The number of the column to check
 *
 * \return the size of the column in bytes
 *
 * \exception odbcpp_error
 * If the column does not exist, this function will
 * throw an error.
 */
SQLLEN dynamic_record::get_size(SQLSMALLINT col) const
{
	return find_column(col, SQL_UNKNOWN_TYPE)->f_fetch_size;
}


/** \brief Retrieve a column data as a string.
 *
 * This function attempt to retrieve the data of a column
 * as a string. If the column is not a string, then an
 * exception is generated.
 *
 * \param[in] name    The name of the column to retrieve
 * \param[in] str     The user string set to the data
 *
 * \exception
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, std::string& str) const
{
	str = reinterpret_cast<const char *>(find_column(name, SQL_C_CHAR, true)->f_data->get());
}


/** \brief Retrieve a column data as a string.
 *
 * This function attempt to retrieve the data of a column
 * as a string. If the column is not a string, then an
 * exception is generated.
 *
 * \param[in] col     The index of the column to retrieve
 * \param[in] str     The user string set to the data
 *
 * \exception
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, std::string& str) const
{
	str = reinterpret_cast<const char *>(find_column(col, SQL_C_CHAR, true)->f_data->get());
}


/** \brief Retrieve a column data as a string.
 *
 * This function attempt to retrieve the data of a column
 * as a string. If the column is not a string, then an
 * exception is generated.
 *
 * \param[in] name    The name of the column to retrieve
 * \param[in] str     The user string set to the data
 *
 * \exception
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, std::wstring& str) const
{
	const SQLWCHAR *s = reinterpret_cast<const SQLWCHAR *>(find_column(name, SQL_C_WCHAR, true)->f_data->get());
	str.clear();
	while(*s != '\0') {
		str += static_cast<wchar_t>(*s++);
	}
}


/** \brief Retrieve a column data as a string.
 *
 * This function attempt to retrieve the data of a column
 * as a string. If the column is not a string, then an
 * exception is generated.
 *
 * \param[in] col     The index of the column to retrieve
 * \param[in] str     The user string set to the data
 *
 * \exception
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, std::wstring& str) const
{
	const SQLWCHAR *s = reinterpret_cast<const SQLWCHAR *>(find_column(col, SQL_C_WCHAR, true)->f_data->get());
	str.clear();
	while(*s != '\0') {
		str += static_cast<wchar_t>(*s++);
	}
}


/** \brief Retrieve a column data as a tiny integer.
 *
 * This function attempt to retrieve the data of a column
 * as a tiny integer. If the column is not a tiny integer, then an
 * exception is generated.
 *
 * \param[in] name        The name of the column to retrieve
 * \param[in] tiny_int    The user tiny integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQLCHAR& tiny_int) const
{
	tiny_int = *reinterpret_cast<SQLCHAR *>(find_column(name, SQL_C_UTINYINT, true)->f_data->get());
}


/** \brief Retrieve a column data as a tiny integer.
 *
 * This function attempt to retrieve the data of a column
 * as a tiny integer. If the column is not a tiny integer, then an
 * exception is generated.
 *
 * \param[in] col         The index of the column to retrieve
 * \param[in] tiny_int    The user tiny integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQLCHAR& tiny_int) const
{
	tiny_int = *reinterpret_cast<SQLCHAR *>(find_column(col, SQL_C_UTINYINT, true)->f_data->get());
}


/** \brief Retrieve a column data as a tiny integer.
 *
 * This function attempt to retrieve the data of a column
 * as a tiny integer. If the column is not a tiny integer, then an
 * exception is generated.
 *
 * \param[in] name        The name of the column to retrieve
 * \param[in] tiny_int    The user tiny integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQLSCHAR& tiny_int) const
{
	tiny_int = *reinterpret_cast<SQLSCHAR *>(find_column(name, SQL_C_TINYINT, true)->f_data->get());
}


/** \brief Retrieve a column data as a tiny integer.
 *
 * This function attempt to retrieve the data of a column
 * as a tiny integer. If the column is not a tiny integer, then an
 * exception is generated.
 *
 * \param[in] col         The index of the column to retrieve
 * \param[in] tiny_int    The user tiny integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQLSCHAR& tiny_int) const
{
	tiny_int = *reinterpret_cast<SQLSCHAR *>(find_column(col, SQL_C_TINYINT, true)->f_data->get());
}


/** \brief Retrieve a column data as a small integer.
 *
 * This function attempt to retrieve the data of a column
 * as a small integer. If the column is not a small integer,
 * then an exception is generated.
 *
 * \param[in] name         The name of the column to retrieve
 * \param[in] small_int    The user small integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQLSMALLINT& small_int) const
{
	small_int = *reinterpret_cast<SQLSMALLINT *>(find_column(name, SQL_C_SHORT, true)->f_data->get());
}


/** \brief Retrieve a column data as a small integer.
 *
 * This function attempt to retrieve the data of a column
 * as a small integer. If the column is not a small integer,
 * then an exception is generated.
 *
 * \param[in] col          The index of the column to retrieve
 * \param[in] small_int    The user small integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQLSMALLINT& small_int) const
{
	small_int = *reinterpret_cast<SQLSMALLINT *>(find_column(col, SQL_C_SHORT, true)->f_data->get());
}


/** \brief Retrieve a column data as a small integer.
 *
 * This function attempt to retrieve the data of a column
 * as a small integer. If the column is not a small integer,
 * then an exception is generated.
 *
 * \param[in] name         The name of the column to retrieve
 * \param[in] small_int    The user small integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQLUSMALLINT& small_int) const
{
	small_int = *reinterpret_cast<SQLUSMALLINT *>(find_column(name, SQL_C_USHORT, true)->f_data->get());
}


/** \brief Retrieve a column data as a small integer.
 *
 * This function attempt to retrieve the data of a column
 * as a small integer. If the column is not a small integer,
 * then an exception is generated.
 *
 * \param[in] col          The index of the column to retrieve
 * \param[in] small_int    The user small integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQLUSMALLINT& small_int) const
{
	small_int = *reinterpret_cast<SQLUSMALLINT *>(find_column(col, SQL_C_USHORT, true)->f_data->get());
}


/** \brief Retrieve a column data as an integer.
 *
 * This function attempt to retrieve the data of a column
 * as an integer. If the column is not an integer,
 * then an exception is generated.
 *
 * \param[in] name         The name of the column to retrieve
 * \param[in] integer    The user integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQLINTEGER& integer) const
{
	integer = *reinterpret_cast<SQLINTEGER *>(find_column(name, SQL_C_LONG, true)->f_data->get());
}


/** \brief Retrieve a column data as an integer.
 *
 * This function attempt to retrieve the data of a column
 * as an integer. If the column is not an integer,
 * then an exception is generated.
 *
 * \param[in] col        The index of the column to retrieve
 * \param[in] integer    The user integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQLINTEGER& integer) const
{
	integer = *reinterpret_cast<SQLUINTEGER *>(find_column(col, SQL_C_LONG, true)->f_data->get());
}


/** \brief Retrieve a column data as an integer.
 *
 * This function attempt to retrieve the data of a column
 * as an integer. If the column is not an integer,
 * then an exception is generated.
 *
 * \param[in] name         The name of the column to retrieve
 * \param[in] integer    The user integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQLUINTEGER& integer) const
{
	integer = *reinterpret_cast<SQLUINTEGER *>(find_column(name, SQL_C_ULONG, true)->f_data->get());
}


/** \brief Retrieve a column data as an integer.
 *
 * This function attempt to retrieve the data of a column
 * as an integer. If the column is not an integer,
 * then an exception is generated.
 *
 * \param[in] col        The index of the column to retrieve
 * \param[in] integer    The user integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQLUINTEGER& integer) const
{
	integer = *reinterpret_cast<SQLUINTEGER *>(find_column(col, SQL_C_ULONG, true)->f_data->get());
}


/** \brief Retrieve a column data as a big integer.
 *
 * This function attempt to retrieve the data of a column
 * as a big integer. If the column is not a big integer,
 * then an exception is generated.
 *
 * \param[in] name       The name of the column to retrieve
 * \param[in] big_int    The user big integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQLBIGINT& big_int) const
{
	big_int = *reinterpret_cast<SQLBIGINT *>(find_column(name, SQL_C_SBIGINT, true)->f_data->get());
}


/** \brief Retrieve a column data as a big integer.
 *
 * This function attempt to retrieve the data of a column
 * as a big integer. If the column is not a big integer,
 * then an exception is generated.
 *
 * \param[in] col        The index of the column to retrieve
 * \param[in] big_int    The user big integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQLBIGINT& big_int) const
{
	big_int = *reinterpret_cast<SQLBIGINT *>(find_column(col, SQL_C_SBIGINT, true)->f_data->get());
}


/** \brief Retrieve a column data as a big integer.
 *
 * This function attempt to retrieve the data of a column
 * as a big integer. If the column is not a big integer,
 * then an exception is generated.
 *
 * \param[in] name       The name of the column to retrieve
 * \param[in] big_int    The user big integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQLUBIGINT& big_int) const
{
	big_int = *reinterpret_cast<SQLUBIGINT *>(find_column(name, SQL_C_UBIGINT, true)->f_data->get());
}


/** \brief Retrieve a column data as a big integer.
 *
 * This function attempt to retrieve the data of a column
 * as a big integer. If the column is not a big integer,
 * then an exception is generated.
 *
 * \param[in] col        The index of the column to retrieve
 * \param[in] big_int    The user big integer set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQLUBIGINT& big_int) const
{
	big_int = *reinterpret_cast<SQLUBIGINT *>(find_column(col, SQL_C_UBIGINT, true)->f_data->get());
}


/** \brief Retrieve a column data as a real number.
 *
 * This function attempt to retrieve the data of a column
 * as a real number. If the column is not a real number,
 * then an exception is generated.
 *
 * \param[in] name        The name of the column to retrieve
 * \param[in] real       The user real number set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQLREAL& real) const
{
	real = *reinterpret_cast<SQLREAL *>(find_column(name, SQL_C_FLOAT, true)->f_data->get());
}


/** \brief Retrieve a column data as a real number.
 *
 * This function attempt to retrieve the data of a column
 * as a real number. If the column is not a real number,
 * then an exception is generated.
 *
 * \param[in] col        The index of the column to retrieve
 * \param[in] real       The user real number set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQLREAL& real) const
{
	real = *reinterpret_cast<SQLREAL *>(find_column(col, SQL_C_FLOAT, true)->f_data->get());
}


/** \brief Retrieve a column data as a float number.
 *
 * This function attempt to retrieve the data of a column
 * as a float number. If the column is not a float number,
 * then an exception is generated.
 *
 * \param[in] name        The name of the column to retrieve
 * \param[in] dbl        The user float number set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQLFLOAT& dbl) const
{
	dbl = *reinterpret_cast<SQLFLOAT *>(find_column(name, SQL_C_DOUBLE, true)->f_data->get());
}


/** \brief Retrieve a column data as a float number.
 *
 * This function attempt to retrieve the data of a column
 * as a float number. If the column is not a float number,
 * then an exception is generated.
 *
 * \param[in] col        The index of the column to retrieve
 * \param[in] dbl        The user float number set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQLFLOAT& dbl) const
{
	dbl = *reinterpret_cast<SQLFLOAT *>(find_column(col, SQL_C_DOUBLE, true)->f_data->get());
}


/** \brief Retrieve a column data as a binary buffer.
 *
 * This function attempt to retrieve the data of a column
 * as a binary buffer. The type of the column is not checked
 * since any type of data can be viewed as binary data. If
 * you need to make sure that this column is indeed a binary
 * column, use the get_type() function.
 *
 * The number of bytes copied in the binary buffer may be less
 * than length. Check the return value to know how many bytes
 * were copied in your binary buffer.
 *
 * \param[in] name        The name of the column to retrieve
 * \param[in] binary     The user binary buffer pointer to copy the data
 * \param[in] length     The size of the user binary buffer
 *
 * \return The number of bytes that have been copied in the binary buffer
 *
 * \exception odbcpp_error
 * If the column cannot be found or is null, an odbcpp_error is raised.
 */
SQLULEN dynamic_record::get(const std::string& name, SQLCHAR *binary, SQLLEN length) const
{
	if(length < 0) {
		diagnostic d(odbcpp_error::ODBCPP_INCORRECT_USE, std::string("a binary buffer cannot have a negative size"));
		throw odbcpp_error(d);
	}
	if(binary == 0) {
		diagnostic d(odbcpp_error::ODBCPP_INCORRECT_USE, std::string("a binary buffer cannot be a NULL pointer"));
		throw odbcpp_error(d);
	}

	const smartptr<bind_info_t>& info = find_column(name, SQL_UNKNOWN_TYPE, true);

	SQLLEN size = static_cast<SQLUINTEGER>(length) < info->f_fetch_size ? length : info->f_size;

	memcpy(binary, info->f_data->get(), size);

	return size;
}


/** \brief Retrieve a column data as a binary buffer.
 *
 * This function attempt to retrieve the data of a column
 * as a binary buffer. The type of the column is not checked
 * since any type of data can be viewed as binary data. If
 * you need to make sure that this column is indeed a binary
 * column, use the get_type() function.
 *
 * The number of bytes copied in the binary buffer may be less
 * than length. Check the return value to know how many bytes
 * were copied in your binary buffer.
 *
 * \param[in] col        The index of the column to retrieve
 * \param[in] binary     The user binary buffer pointer to copy the data
 * \param[in] length     The size of the user binary buffer
 *
 * \return The number of bytes that have been copied in the binary buffer
 *
 * \exception odbcpp_error
 * If the column cannot be found or is null, an odbcpp_error is raised.
 */
SQLULEN dynamic_record::get(SQLSMALLINT col, SQLCHAR *binary, SQLLEN length) const
{
	if(length < 0) {
		diagnostic d(odbcpp_error::ODBCPP_INCORRECT_USE, std::string("a binary buffer cannot have a negative size"));
		throw odbcpp_error(d);
	}
	if(binary == 0) {
		diagnostic d(odbcpp_error::ODBCPP_INCORRECT_USE, std::string("a binary buffer cannot be a NULL pointer"));
		throw odbcpp_error(d);
	}

	const smartptr<bind_info_t>& info = find_column(col, SQL_UNKNOWN_TYPE, true);

	SQLLEN size = static_cast<SQLUINTEGER>(length) < info->f_fetch_size ? length : info->f_size;

	memcpy(binary, info->f_data->get(), size);

	return size;
}


/** \brief Retrieve a column data as a date structure.
 *
 * This function attempt to retrieve the data of a column
 * as a date structure. If the column is not a date structure,
 * then an exception is generated.
 *
 * \param[in] name        The name of the column to retrieve
 * \param[in] date        The user date structure set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQL_DATE_STRUCT& date) const
{
	date = *reinterpret_cast<SQL_DATE_STRUCT *>(find_column(name, SQL_C_DATE, true)->f_data->get());
}


/** \brief Retrieve a column data as a date structure.
 *
 * This function attempt to retrieve the data of a column
 * as a date structure. If the column is not a date structure,
 * then an exception is generated.
 *
 * \param[in] col         The index of the column to retrieve
 * \param[in] date        The user date structure set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQL_DATE_STRUCT& date) const
{
	date = *reinterpret_cast<SQL_DATE_STRUCT *>(find_column(col, SQL_C_DATE, true)->f_data->get());
}


/** \brief Retrieve a column data as a time structure.
 *
 * This function attempt to retrieve the data of a column
 * as a time structure. If the column is not a time structure,
 * then an exception is generated.
 *
 * \param[in] name        The name of the column to retrieve
 * \param[in] time        The user time structure set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQL_TIME_STRUCT& time) const
{
	time = *reinterpret_cast<SQL_TIME_STRUCT *>(find_column(name, SQL_C_TIME, true)->f_data->get());
}


/** \brief Retrieve a column data as a time structure.
 *
 * This function attempt to retrieve the data of a column
 * as a time structure. If the column is not a time structure,
 * then an exception is generated.
 *
 * \param[in] col         The index of the column to retrieve
 * \param[in] time        The user time structure set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQL_TIME_STRUCT& time) const
{
	time = *reinterpret_cast<SQL_TIME_STRUCT *>(find_column(col, SQL_C_TIME, true)->f_data->get());
}


/** \brief Retrieve a column data as a timestamp structure.
 *
 * This function attempt to retrieve the data of a column
 * as a timestamp structure. If the column is not a timestamp structure,
 * then an exception is generated.
 *
 * \param[in] name        The name of the column to retrieve
 * \param[in] timestamp   The user timestamp structure set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQL_TIMESTAMP_STRUCT& timestamp) const
{
	timestamp = *reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(find_column(name, SQL_C_TIMESTAMP, true)->f_data->get());
}


/** \brief Retrieve a column data as a timestamp structure.
 *
 * This function attempt to retrieve the data of a column
 * as a timestamp structure. If the column is not a timestamp structure,
 * then an exception is generated.
 *
 * \param[in] col         The index of the column to retrieve
 * \param[in] timestamp   The user timestamp structure set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQL_TIMESTAMP_STRUCT& timestamp) const
{
	timestamp = *reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(find_column(col, SQL_C_TIMESTAMP, true)->f_data->get());
}


/** \brief Retrieve a column data as a numeric structure.
 *
 * This function attempt to retrieve the data of a column
 * as a numeric structure. If the column is not a numeric structure,
 * then an exception is generated.
 *
 * \param[in] name        The name of the column to retrieve
 * \param[in] numeric     The user numeric structure set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQL_NUMERIC_STRUCT& numeric) const
{
	numeric = *reinterpret_cast<SQL_NUMERIC_STRUCT *>(find_column(name, SQL_C_NUMERIC, true)->f_data->get());
}


/** \brief Retrieve a column data as a numeric structure.
 *
 * This function attempt to retrieve the data of a column
 * as a numeric structure. If the column is not a numeric structure,
 * then an exception is generated.
 *
 * \param[in] col         The index of the column to retrieve
 * \param[in] numeric     The user numeric structure set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQL_NUMERIC_STRUCT& numeric) const
{
	numeric = *reinterpret_cast<SQL_NUMERIC_STRUCT *>(find_column(col, SQL_C_NUMERIC, true)->f_data->get());
}


/** \brief Retrieve a column data as a guid structure.
 *
 * This function attempt to retrieve the data of a column
 * as a guid structure. If the column is not a guid structure,
 * then an exception is generated.
 *
 * \param[in] name        The name of the column to retrieve
 * \param[in] guid        The user guid structure set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(const std::string& name, SQLGUID& guid) const
{
	guid = *reinterpret_cast<SQLGUID *>(find_column(name, SQL_C_GUID, true)->f_data->get());
}


/** \brief Retrieve a column data as a guid structure.
 *
 * This function attempt to retrieve the data of a column
 * as a guid structure. If the column is not a guid structure,
 * then an exception is generated.
 *
 * \param[in] col         The index of the column to retrieve
 * \param[in] guid        The user guid structure set to the data
 *
 * \exception odbcpp_error
 * If the column data does not match or is null, this function generates an
 * odbcpp_error exception.
 */
void dynamic_record::get(SQLSMALLINT col, SQLGUID& guid) const
{
	guid = *reinterpret_cast<SQLGUID *>(find_column(col, SQL_C_GUID, true)->f_data->get());
}






/** \class dynamic_record::bind_info_t
 *
 * \brief Structure holding bind information for a dynamically allocated column.
 *
 * Whenever the statement::fetch() function is called, it wants to save the
 * row data in user variables. This is done by first binding variables to
 * a statement for each column.
 *
 * This structure is used to define one such variable (a column).
 *
 * The data is dynamically allocated and thus the f_data member must remain
 * valid the whole time the fetch() command is to be used.
 */

/** \fn dynamic_record::bind_info_t::bind_info_t
 *
 * \brief Initializes the structure to defaults.
 *
 * The constructor ensures that the structure is initialized with
 * proper defaults so it can be used in all cases.
 */

/** \var dynamic_record::bind_info_t::f_name
 *
 * \brief The name of the column when available.
 *
 * This variable is set to the name of the column the dynamic record
 * was bound to whenever the system could give us that name.
 */


/** \var dynamic_record::bind_info_t::f_col
 *
 * \brief The column number of this bind information.
 *
 * This number represents the column number of this bind information.
 *
 * The column numbers start at 1.
 */


/** \var dynamic_record::bind_info_t::f_target_type
 *
 * \brief The type of the column.
 *
 * This entry represents the type of the column. This represents
 * the data type such as SMALLINT or VARCHAR. This type is not
 * modified from what the system tells us it is.
 *
 * It is expected to be used with ODBC C types such as
 * SQL_C_SHORT and SQL_C_VARTIME.
 */


/** \var dynamic_record::bind_info_t::f_bind_type
 *
 * \brief The type used to bind the column.
 *
 * This entry is the type used to bind the column. The exact column
 * type is defined in the dynamic_record::f_target_type instead.
 *
 * It is expected to be used with ODBC C types such as
 * SQL_C_SHORT and SQL_C_VARTIME.
 */


/** \var dynamic_record::bind_info_t::f_data
 *
 * \brief The data buffer where the variable will be loaded.
 *
 * In the dynamic record, the data buffer is always dynamically
 * allocated. The smart pointer and the buffer class automatically
 * handle the memory buffer allocation and deallocation.
 */


/** \var dynamic_record::bind_info_t::f_size
 *
 * \brief The size of the buffer in bytes.
 *
 * This variable holds the size of the variable as it was read
 * by the SQLColDescribe() function.
 *
 * This size may be larger than necessary for the type of the
 * column.
 */


/** \var dynamic_record::bind_info_t::f_decimal_digits
 *
 * \brief The number of digits after the decimal point.
 *
 * This number represents the number of digits after the decimal point
 * in a numeric or a date type.
 *
 * This number can be zero, but it cannot be negative.
 */

/** \var dynamic_record::bind_info_t::f_fetch_size
 *
 * \brief The number of bytes that the fetch() command copied in this column variable.
 *
 * This variable is used by the statement::fetch() call to save the size of
 * the column in bytes at the time the data is read in our dynamic variable.
 *
 * This should be equal or smaller to the f_size parameter.
 */

}	// namespace odbcpp
