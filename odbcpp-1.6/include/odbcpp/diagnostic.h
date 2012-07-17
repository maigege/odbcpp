//
// File:	include/odbcpp/diagnostic.h
// Object:	Define the diagnostic objects of the odbcpp library
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

#ifndef ODBCPP_DIAG
#define ODBCPP_DIAG

#include	"odbcpp_config.h"
#include	<sql.h>
#include	<string>
#include	<vector>

namespace odbcpp
{

class diag_t
{
public:
			diag_t();

	std::string	msg() const;

	std::string	f_server;
	std::string	f_connection;
	std::string	f_message;
	std::string	f_odbc_state;
	SQLINTEGER	f_native_errno;
};
/// A vector of diagnostics; used by the diagnostic class
typedef std::vector<diag_t>	diag_vector_t;


class handle;


class diagnostic
{
public:
			diagnostic() {}
			diagnostic(SQLINTEGER odbcpp_errno, const std::string& message);
			diagnostic(SQLSMALLINT handle_type, SQLHANDLE hdl, handle *parent = 0);

	void		set(SQLSMALLINT handle_type, SQLHANDLE handle);
	std::string	msg() const;

	SQLLEN		get_affected_rows() const { return f_affected_rows; }
	SQLSMALLINT	size() const { return static_cast<SQLSMALLINT>(f_diag.size()); }
	diag_t		get(SQLSMALLINT record) const { return f_diag[static_cast<int>(record) - 1]; }
	diag_t		operator [] (int record) const { return f_diag[record - 1]; }

	static bool	get_string(SQLSMALLINT handle_type, SQLHANDLE handle, SQLSMALLINT record, SQLSMALLINT identifier, std::string& string);
	static bool	get_integer(SQLSMALLINT handle_type, SQLHANDLE handle, SQLSMALLINT record, SQLSMALLINT identifier, SQLINTEGER& integer);
	static bool	get_length(SQLSMALLINT handle_type, SQLHANDLE handle, SQLSMALLINT record, SQLSMALLINT identifier, SQLLEN& length);

private:
	SQLLEN		f_affected_rows;
	diag_vector_t	f_diag;
};



}	// namespace odbcpp


#endif		// #ifndef ODBCPP_DIAG
