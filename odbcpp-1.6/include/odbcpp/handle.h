//
// File:	include/odbcpp/handle.h
// Object:	Base class for the different handles
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

#ifndef ODBCPP_HANDLE
#define ODBCPP_HANDLE

#include	"exception.h"
#include	"object.h"

namespace odbcpp
{

class handle : public object
{
public:
				handle(SQLSMALLINT handle_type);
				~handle();

	SQLRETURN		check(SQLRETURN return_code, handle *parent = 0) const;

	SQLHANDLE		get_handle() const { return f_handle; }
	SQLSMALLINT		get_handle_type() const { return f_handle_type; }
	const diagnostic&	get_diagnostic() const { return f_diag; }

protected:
	SQLHANDLE		f_handle;
	const SQLSMALLINT	f_handle_type;

private:
	/// Cannot copy handles, really, won't work!
				handle(const handle& hdl) : f_handle(SQL_NULL_HANDLE), f_handle_type(SQL_HANDLE_ENV) {}
	/// Cannot copy handles, really, won't work!
	handle&			operator = (const handle& hdl) { return *this; }

	mutable diagnostic	f_diag;
};



}	// namespace odbcpp

#endif		// #ifndef ODBCPP_HANDLE
