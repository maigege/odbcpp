//
// File:	include/odbcpp/record.h
// Object:	Define the record object of the odbcpp library
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
#ifndef ODBCPP_RECORD
#define ODBCPP_RECORD

#include	"statement.h"
#include	<map>
#include	<sqlucode.h>
#include	<iostream>

namespace odbcpp
{




/** \brief Class used to manage a block of memory.
 *
 * This class is used internally to manage a block of memory necessary
 * to dynamically bind to a database column of data.
 *
 * This class derives from the object class that ensures the reference
 * count is properly managed. Buffers are used in smart pointers and
 * thus can safely be pointed as many times as necessary.
 */
template<class T>
class buffer : public object
{
public:
	/** \brief Allocate the block of data.
	 *
	 * Initializes the buffer by allocating the necessary block of memory.
	 *
	 * \note
	 * Also a buffer can have a size of 0, it is better to avoid such
	 * buffers since nothing can be done with them.
	 *
	 * \param[in] size   The size used to allocate the new buffer.
	 */
	buffer(size_t size = 1) : object(0), f_ptr(new T[size]) {}

	/** \brief Free the block of data.
	 *
	 * The destructor ensures that the block of memory allocated
	 * by the constructor is freed.
	 */
	~buffer() { delete [] f_ptr; }

	/** \brief Retrieve the memory pointer.
	 *
	 * This function returns the memory pointer that the constructor
	 * allocated. You can use it as required, but you cannot delete
	 * it.
	 *
	 * This pointer cannot be null, however, it may pointer to an
	 * empty buffer (i.e. 0 size memory block.)
	 *
	 * \return The allocated memory pointer
	 */
	T *get() const { return const_cast<T *>(f_ptr); }

private:
	/** \brief The memory pointer.
	 *
	 * This variable holds the buffer memory pointer.
	 */
	T *			f_ptr;
};
/// A specialization of the buffer template of type char
typedef buffer<char>		buffer_char_t;



class record_base : public object
{
public:
				record_base();
				record_base(const record_base& base);
	virtual			~record_base() {}

	record_base&		operator = (const record_base& base);

	// whether the binding will happen dynamically
	virtual bool		is_dynamic() const = 0;
	bool			is_bound() const { return f_statement; }

	virtual void		bind(statement& stmt);
	void			unbind() { f_statement.reset(); }
	virtual void		finalize() {}

protected:
	smartptr<statement>	f_statement;

private:
	virtual void		bind_impl() = 0;
};


class record : public record_base
{
public:
	record(void) {}
	~record(void) {}
	virtual bool		is_dynamic() const { return false; }

protected:
	// use to bind your variables by name or column number
	bool			empty() const { return f_bind_by_name.empty() && f_bind_by_col.empty(); }
	size_t			size() const { return f_bind_by_name.size() + f_bind_by_col.size(); }

	// regular and wide strings
	void			bind(const std::string& name, std::string& str, bool *is_null = 0);
	void			bind(SQLSMALLINT col, std::string& str, bool *is_null = 0);
	void			bind(const std::string& name, std::wstring& str, bool *is_null = 0);
	void			bind(SQLSMALLINT col, std::wstring& str, bool *is_null = 0);

	// integers
	void			bind(const std::string& name, SQLCHAR& tiny_int, bool *is_null = 0);	// includes SQL_C_BIT
	void			bind(SQLSMALLINT col, SQLCHAR& tiny_int, bool *is_null = 0);
	void			bind(const std::string& name, SQLSCHAR& tiny_int, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQLSCHAR& tiny_int, bool *is_null = 0);

	void			bind(const std::string& name, SQLSMALLINT& small_int, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQLSMALLINT& small_int, bool *is_null = 0);
	void			bind(const std::string& name, SQLUSMALLINT& small_int, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQLUSMALLINT& small_int, bool *is_null = 0);

	void			bind(const std::string& name, SQLINTEGER& integer, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQLINTEGER& integer, bool *is_null = 0);
	void			bind(const std::string& name, SQLUINTEGER& integer, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQLUINTEGER& integer, bool *is_null = 0);

	void			bind(const std::string& name, SQLBIGINT& big_int, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQLBIGINT& big_int, bool *is_null = 0);
	void			bind(const std::string& name, SQLUBIGINT& big_int, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQLUBIGINT& big_int, bool *is_null = 0);

	// floating points
	void			bind(const std::string& name, SQLREAL& real, bool *is_null = 0);	// C float
	void			bind(SQLSMALLINT col, SQLREAL& real, bool *is_null = 0);

	void			bind(const std::string& name, SQLFLOAT& dbl, bool *is_null = 0);	// C double
	void			bind(SQLSMALLINT col, SQLFLOAT& dbl, bool *is_null = 0);

	// binary (bookmarks, C-strings, etc.)
	void			bind(const std::string& name, SQLCHAR *binary, SQLLEN length, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQLCHAR *binary, SQLLEN length, bool *is_null = 0);

	// structures
	void			bind(const std::string& name, SQL_DATE_STRUCT& date, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQL_DATE_STRUCT& date, bool *is_null = 0);
	void			bind(const std::string& name, SQL_TIME_STRUCT& time, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQL_TIME_STRUCT& time, bool *is_null = 0);
	void			bind(const std::string& name, SQL_TIMESTAMP_STRUCT& timestamp, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQL_TIMESTAMP_STRUCT& timestamp, bool *is_null = 0);
	void			bind(const std::string& name, SQL_NUMERIC_STRUCT& numeric, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQL_NUMERIC_STRUCT& numeric, bool *is_null = 0);
	void			bind(const std::string& name, SQLGUID& guid, bool *is_null = 0);
	void			bind(SQLSMALLINT col, SQLGUID& guid, bool *is_null = 0);

private:
	struct bind_info_t: public object {
				bind_info_t() :
					object(0),
					//f_name -- auto-init
					f_col(0),
					f_target_type(SQL_UNKNOWN_TYPE),
					f_data(NULL),
					f_size(0),
					f_fetch_size(0),
					f_is_null(0),
					f_string(NULL)
					//f_wstring(NULL) -- same as f_string(NULL)
				{
				}

		std::string		f_name;		// if empty, use col
		SQLSMALLINT		f_col;		// if 0, use name
		SQLSMALLINT		f_target_type;	// the type of the data
		SQLPOINTER		f_data;		// pointer to the data to bind with
		SQLULEN			f_size;		// size of the data buffer
		SQLLEN			f_fetch_size;	// sized defined after the fetch calls
		bool *			f_is_null;	// a pointer to mark as TRUE is the value is NULL in the database
		smartptr<buffer_char_t>	f_data_buffer;	// for strings
		union {
			std::string *	f_string;	// pointer to the user string
			std::wstring *	f_wstring;	// pointer to the user string
		};
	};
	/// A map that links a column name and the column bind information
	typedef std::map<const std::string, smartptr<bind_info_t> >	bind_info_name_map_t;
	/// A pair with the column name and its information
	typedef std::pair<const std::string, smartptr<bind_info_t> >	bind_info_name_t;
	/// A map that links a column index and the column bind information
	typedef std::map<const SQLSMALLINT, smartptr<bind_info_t> >	bind_info_col_map_t;
	/// A pair with the column index and its information
	typedef std::pair<const SQLSMALLINT, smartptr<bind_info_t> >	bind_info_col_t;

	virtual void		bind_impl();
	virtual void		finalize();
	void			finalize_info(smartptr<bind_info_t> &info);

	bind_info_name_map_t	f_bind_by_name;
	bind_info_col_map_t	f_bind_by_col;
};



class dynamic_record : public record_base
{
public:
	dynamic_record(void) {}
	~dynamic_record(void) {}
	
	virtual bool		is_dynamic() const { return true; }

	// use to get the record data by name or column number
	bool			empty() const { return f_bind_by_name.empty() && f_bind_by_col.empty(); }
	size_t			size() const { return f_bind_by_col.size(); }
	const std::string&	column_name(SQLSMALLINT col) const;
	SQLSMALLINT		column_number(const std::string& name) const;

	// check columns and types
	bool			exists(const std::string& name);
	SQLSMALLINT		get_type(const std::string& name) const;
	SQLSMALLINT		get_type(SQLSMALLINT col) const;
	SQLSMALLINT		get_decimal_digits(const std::string& name) const;
	SQLSMALLINT		get_decimal_digits(SQLSMALLINT col) const;
	SQLSMALLINT		get_is_null(const std::string& name) const;
	SQLSMALLINT		get_is_null(SQLSMALLINT col) const;
	SQLLEN			get_size(const std::string& name) const;
	SQLLEN			get_size(SQLSMALLINT col) const;

	// regular and wide strings
	void			get(const std::string& name, std::string& str) const;
	void			get(SQLSMALLINT col, std::string& str) const;
	void			get(const std::string& name, std::wstring& str) const;
	void			get(SQLSMALLINT col, std::wstring& str) const;

	// integers
	void			get(const std::string& name, SQLCHAR& tiny_int) const;	// includes SQL_C_BIT
	void			get(SQLSMALLINT col, SQLCHAR& tiny_int) const;
	void			get(const std::string& name, SQLSCHAR& tiny_int) const;
	void			get(SQLSMALLINT col, SQLSCHAR& tiny_int) const;

	void			get(const std::string& name, SQLSMALLINT& small_int) const;
	void			get(SQLSMALLINT col, SQLSMALLINT& small_int) const;
	void			get(const std::string& name, SQLUSMALLINT& small_int) const;
	void			get(SQLSMALLINT col, SQLUSMALLINT& small_int) const;

	void			get(const std::string& name, SQLINTEGER& integer) const;
	void			get(SQLSMALLINT col, SQLINTEGER& integer) const;
	void			get(const std::string& name, SQLUINTEGER& integer) const;
	void			get(SQLSMALLINT col, SQLUINTEGER& integer) const;

	void			get(const std::string& name, SQLBIGINT& big_int) const;
	void			get(SQLSMALLINT col, SQLBIGINT& big_int) const;
	void			get(const std::string& name, SQLUBIGINT& big_int) const;
	void			get(SQLSMALLINT col, SQLUBIGINT& big_int) const;

	// floating points
	void			get(const std::string& name, SQLREAL& real) const;	// C float
	void			get(SQLSMALLINT col, SQLREAL& real) const;

	void			get(const std::string& name, SQLFLOAT& dbl) const;	// C double
	void			get(SQLSMALLINT col, SQLFLOAT& dbl) const;

	// binary (bookmarks, C-strings, etc.)
	SQLULEN			get(const std::string& name, SQLCHAR *binary, SQLLEN length) const;
	SQLULEN			get(SQLSMALLINT col, SQLCHAR *binary, SQLLEN length) const;

	// structures
	void			get(const std::string& name, SQL_DATE_STRUCT& date) const;
	void			get(SQLSMALLINT col, SQL_DATE_STRUCT& date) const;
	void			get(const std::string& name, SQL_TIME_STRUCT& time) const;
	void			get(SQLSMALLINT col, SQL_TIME_STRUCT& time) const;
	void			get(const std::string& name, SQL_TIMESTAMP_STRUCT& timestamp) const;
	void			get(SQLSMALLINT col, SQL_TIMESTAMP_STRUCT& timestamp) const;
	void			get(const std::string& name, SQL_NUMERIC_STRUCT& numeric) const;
	void			get(SQLSMALLINT col, SQL_NUMERIC_STRUCT& numeric) const;
	void			get(const std::string& name, SQLGUID& guid) const;
	void			get(SQLSMALLINT col, SQLGUID& guid) const;

private:
	struct bind_info_t: public object {
					bind_info_t() :
						object(0),
						//f_name -- auto-init
						f_col(0),
						f_target_type(0),
						f_bind_type(0),
						f_decimal_digits(0),
						//f_data -- auto-init
						f_size(0),
						f_fetch_size(0)
					{
					}

		std::string		f_name;		// if empty, can only use col
		SQLSMALLINT		f_col;		// 1 to # of columns
		SQLSMALLINT		f_target_type;	// the type of the data
		SQLSMALLINT		f_bind_type;	// the type used to bind the column
		SQLSMALLINT		f_decimal_digits;	// number of digits after decimal point
		smartptr<buffer_char_t>	f_data;		// pointer to the data to bind with
		SQLULEN			f_size;		// size of the data buffer
		SQLLEN			f_fetch_size;	// sized defined after the fetch calls
	};
	/// A map that links a column name and the column bind information
	typedef std::map<const std::string, smartptr<bind_info_t> >	bind_info_name_map_t;
	/// A pair with the column name and its information
	typedef std::pair<const std::string, smartptr<bind_info_t> >	bind_info_name_t;
	/// A vector of columns to sort them by index
	typedef std::vector<smartptr<bind_info_t> >			bind_info_col_vector_t;

	virtual void		bind_impl();
	const smartptr<bind_info_t>& find_column(const std::string& name, SQLSMALLINT target_type, bool except_null = false) const;
	const smartptr<bind_info_t>& find_column(SQLSMALLINT col, SQLSMALLINT target_type, bool except_null = false) const;
	const smartptr<bind_info_t>& verify_column(const smartptr<bind_info_t> &info, SQLSMALLINT target_type, bool except_null) const;

	bind_info_name_map_t	f_bind_by_name;
	bind_info_col_vector_t	f_bind_by_col;		// offset 0 is column 1, etc.
};



}	// namespace odbcpp

#endif		// #ifndef ODBCPP_RECORD
