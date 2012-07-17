//
// File:	include/odbcpp/connection.h
// Object:	Define the connect object of the odbcpp library
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
#ifndef ODBCPP_CONNECTION
#define ODBCPP_CONNECTION

#include	"environment.h"

namespace odbcpp
{



class connection : public handle
{
public:
				connection(environment& env);
				~connection();

	bool			is_connected() const { return f_connected; }
	void			set_attr(SQLINTEGER attr, SQLINTEGER integer);
	void			set_attr(SQLINTEGER attr, SQLPOINTER ptr, SQLINTEGER length);
	void			connect(const std::string& dns, const std::string& login, const std::string& passwd);
	void			disconnect();
	void			commit();
	void			rollback();

private:
	smartptr<environment>	f_environment;
	bool			f_connected;
};


}	// namespace odbcpp

#endif		// #ifndef ODBCPP_CONNECTION

