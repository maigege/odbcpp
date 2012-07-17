//
// File:	include/odbcpp/environment.h
// Object:	Define the environment object of the odbcpp library
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
#ifndef ODBCPP_ENVIRONMENT
#define ODBCPP_ENVIRONMENT

#include	"handle.h"
#include	<sqlext.h>

namespace odbcpp
{



class environment : public handle
{
public:
	enum data_source_type_t {
		DATA_SOURCE_TYPE_ALL,
		DATA_SOURCE_TYPE_USER,
		DATA_SOURCE_TYPE_SYSTEM
	};
	struct data_source_t {
		std::string	f_server;
		std::string	f_description;
	};
	/// Defines a vector of data source information (server/description pairs)
	typedef std::vector<data_source_t>	data_source_vector_t;

				environment(SQLUINTEGER version = SQL_OV_ODBC3);

	void			set_attr(SQLINTEGER attr, SQLINTEGER integer);
	void			set_attr(SQLINTEGER attr, SQLPOINTER ptr, SQLINTEGER length);
	void			get_data_source(data_source_type_t type, data_source_vector_t& sources);
	void			commit();
	void			rollback();

private:
};


}	// namespace odbcpp

#endif		// #ifndef ODBCPP_ENVIRONMENT
