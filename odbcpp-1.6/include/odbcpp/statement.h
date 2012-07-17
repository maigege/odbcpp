//
// File:	include/odbcpp/statement.h
// Object:	Define the statement object of the odbcpp library
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
#ifndef ODBCPP_STATEMENT
#define ODBCPP_STATEMENT

#include	"connection.h"

namespace odbcpp
{

class record_base;

class statement : public handle
{
public:
				statement(connection& conn);

	void			set_attr(SQLINTEGER attr, SQLINTEGER integer);
	void			set_attr(SQLINTEGER attr, SQLPOINTER ptr, SQLINTEGER length);
	void			set_no_direct_fetch(bool no_direct_fetch = true);
	void			execute(const std::string& order);
	void			begin();
	void			commit();
	void			rollback();
	void			cancel();
	void			close_cursor();
	SQLLEN			cols() const;
	SQLLEN			rows() const;
	bool			fetch(record_base& rec, SQLSMALLINT orientation = SQL_FETCH_NEXT, SQLLEN offset = 0);

private:
	void			has_data() const;

	smartptr<connection>	f_connection;
	bool			f_has_data;
	bool			f_no_direct_fetch;	// if true, avoid SQLFetch(), use SQLFetchScroll() instead
};


}	// namespace odbcpp

#endif		// #ifndef ODBCPP_STATEMENT
