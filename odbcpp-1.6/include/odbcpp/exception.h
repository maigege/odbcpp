//
// File:	include/odbcpp/exception.h
// Object:	Define the exception objects of the odbcpp library
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

#ifndef ODBCPP_EXCEPTION
#define ODBCPP_EXCEPTION

#include	"diagnostic.h"
#include	<stdexcept>

namespace odbcpp
{



class odbcpp_error : public std::runtime_error
{
public:
	/// No error (should not be used with odbcpp_error)
	static const SQLINTEGER		ODBCPP_NONE = 0;
	/// An internal error occured; this should never happen, if it does, please report!
	static const SQLINTEGER		ODBCPP_INTERNAL = 1;
	/// Expected data was not available
	static const SQLINTEGER		ODBCPP_NO_DATA = 2;
	/// The library was not used as expected (order of calls, generally)
	static const SQLINTEGER		ODBCPP_INCORRECT_USE = 3;
	/// This feature is not yet implemented (sorry!)
	static const SQLINTEGER		ODBCPP_NOT_IMPLEMENTED = 4;
	/// The type of a variable (column) does not correspond to the type defined
	static const SQLINTEGER		ODBCPP_TYPE_MISMATCH = 5;
	/// The specified item (a column?) was not found
	static const SQLINTEGER		ODBCPP_NOT_FOUND = 6;

			odbcpp_error(const diagnostic& diag) :
				std::runtime_error(diag.msg()),
				f_diag(diag)
			{
			}

	diagnostic	get() const { return f_diag; }

private:
	diagnostic	f_diag;
};



}	// namespace odbcpp


#endif		// #ifndef ODBCPP_EXCEPTION
