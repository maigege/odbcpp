//
// File:	include/odbcpp/odbcpp.h
// Object:	Include all the necessary files to use the odbcpp library
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

#ifndef ODBCPP_ODBCPP
#define ODBCPP_ODBCPP

// The following list of #include is used to
// make sure that all the public odbcpp includes
// are included.
#include	"record.h"


namespace odbcpp
{

const char *	get_version();

SQLPOINTER	int_to_ptr(SQLINTEGER integer);


}		// namespace odbcpp



#endif		// #ifndef ODBCPP_ODBCPP
