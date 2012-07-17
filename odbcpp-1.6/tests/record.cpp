//
// File:	src/record.cpp
// Object:	Sample code used to test a static record
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
//
//
// IMPORTANT NOTE:
//
// This test requires a table with columns as follow:
//
// Column Name    Type
//
//    name        TEXT
//    id          BIGINT
//    count       INTEGER
//    status      SMALLINT
//    key         VARCHAR(22)
//    created     TIMESTAMP
//    code        CHAR(5)
//    in_use      BOOLEAN
//
// You can use the following command to create the table and insert data in it:
//
// CREATE TABLE odbcpp (name TEXT, id BIGINT, count INTEGER, status SMALLINT,
//    key VARCHAR(12), created TIMESTAMP, code CHAR(5), in_use BOOLEAN);
//
// INSERT INTO odbcpp VALUES ('First', 1234567890, 3, 1, 'KEY-123', NOW(), 'A-982', 't');
// INSERT INTO odbcpp VALUES ('Second', 9999999999, 2, 2, 'YEK-321', NOW(), 'B-553', 'f');
// INSERT INTO odbcpp VALUES (null, null, null, null, null, null, null, null);
// INSERT INTO odbcpp VALUES ('Forth', null, null, 2, 'TWELVE CHARS', null, null, 't');
//

#include	"odbcpp/odbcpp.h"
#include	<iostream>
#include	<cstring>
#include	<cstdlib>
#include	<cstdio>


const char *progname;

void usage()
{
	std::cerr << "odbcpp:test: record v" << odbcpp::get_version() << "\n";
	std::cerr << "Usage: " << progname << " [-opts] <dsn> <login> <password> <SQL order>\n";
	std::cerr << "where -opts is one of the following:\n";
	std::cerr << "   -h     print out this help screen\n";
	std::cerr << "   -l     print out license information\n";
	std::cerr << "   -v     be verbose as we work on the SQL order\n";
	exit(1);
}


void license()
{
	std::cerr << "odbcpp::record  Copyright (C) 2008  Made to Order Software Corporation\n";
	std::cerr << "This program comes with ABSOLUTELY NO WARRANTY.\n";
	std::cerr << "This is free software, and you are welcome to redistribute it under\n";
	std::cerr << "certain conditions.\n";
	std::cerr << "Read the COPYING file accompagnying the odbcpp project for more information.\n";
#ifdef _MSC_VER
	std::cerr << "\nType return to close the window.\n";
	getchar();
#endif
	exit(1);
}

class my_record_t : public odbcpp::record
{
public:
	my_record_t()
	{
		bind("name", f_name, &f_name_is_null);
		bind("id", f_id, &f_id_is_null);
		bind("count", f_count, &f_count_is_null);
		bind("status", f_status, &f_status_is_null);
		bind("key", f_key, &f_key_is_null);
		bind("created", f_created, &f_created_is_null);
		bind("code", f_code, &f_code_is_null);
		bind("in_use", f_in_use, &f_in_use_is_null);
	}

	void print()
	{
		if(f_name_is_null) {
			std::cout << "no name";
		}
		else {
			std::cout << f_name;
		}
		if(f_id_is_null) {
			std::cout << " | no identifier";
		}
		else {
			std::cout << " | " << f_id;
		}
		if(f_count_is_null) {
			std::cout << " | no count";
		}
		else {
			std::cout << " | " << f_count;
		}
		if(f_status_is_null) {
			std::cout << " | no status ";
		}
		else {
			std::cout << " | " << f_status;
		}
		if(f_key_is_null) {
			std::cout << " | no key";
		}
		else {
			std::cout << " | " << f_key;
		}
		if(f_created_is_null) {
			std::cout << " | no created";
		}
		else {
			std::cout << " | " << f_created.year << "/" << f_created.month << "/" << f_created.day
				<< " " << f_created.hour << ":" << f_created.minute << ":" << f_created.second;
		}
		if(f_code_is_null) {
			std::cout << " | no code";
		}
		else {
			std::cout << " | " << f_code;
		}
		if(f_in_use_is_null) {
			std::cout << " | no in use";
		}
		else {
			std::cout << " | " << (f_in_use ? "true" : "false");
		}

		std::cout << std::endl;
	}

private:
	

	bool		f_name_is_null;
	std::string	f_name;

	bool		f_id_is_null;
	SQLBIGINT	f_id;

	bool		f_count_is_null;
	SQLINTEGER	f_count;

	bool		f_status_is_null;
	SQLSMALLINT	f_status;

	bool		f_key_is_null;
	std::string	f_key;

	bool		f_created_is_null;
	SQL_TIMESTAMP_STRUCT	f_created;

	bool		f_code_is_null;
	std::string	f_code;

	bool		f_in_use_is_null;
	SQLCHAR		f_in_use;
};

int main(int argc, char *argv[])
{
	int		i;
	const char	*dsn;
	const char	*login;
	const char	*passwd;
	const char	*order;
	bool		verbose;

	progname = strrchr(argv[0], '/');
	if(progname == 0) {
		progname = argv[0];
	}
	else {
		++progname;
	}

	dsn = 0;
	login = 0;
	passwd = 0;
	order = 0;
	verbose = false;

	i = 1;
	while(i < argc) {
		if(argv[i][0] == '-') {
			switch(argv[i][1]) {
			case 'h':
				usage();
				break;

			case 'l':
				license();
				break;

			case 'v':
				verbose = true;
				break;

			default:
				std::cerr << argv[0] << ":error: unrecognized option \"-" << argv[i][1] << "\".\n";
#ifdef _MSC_VER
				std::cerr << "\nType return to close the window.\n";
				getchar();
#endif
				exit(1);

			}
			if(argv[i][2]) {
				std::cerr << argv[0] << ":error: multi-option not supported; try -h.\n";
#ifdef _MSC_VER
				std::cerr << "\nType return to close the window.\n";
				getchar();
#endif
				exit(1);
			}
		}
		else if(dsn == 0) {
			dsn = argv[i];
		}
		else if(login == 0) {
			login = argv[i];
		}
		else if(passwd == 0) {
			passwd = argv[i];
		}
		else if(order == 0) {
			order = argv[i];
		}
		else {
			std::cerr << argv[0] << ":error: too many arguments; try -h.\n";
#ifdef _MSC_VER
			std::cerr << "\nType return to close the window.\n";
			getchar();
#endif
			exit(1);
		}
		++i;
	}

	if(order == 0) {
		std::cerr << "odbc:connect:error: the server name, login, password and order are all mandatory parameters.\n";
#ifdef _MSC_VER
		std::cerr << "\nType return to close the window.\n";
		getchar();
#endif
		exit(1);
	}

	try {
		if(verbose) {
			std::cerr << "Creating an odbcpp environment.\n";
		}
		odbcpp::environment env;
		if(verbose) {
			std::cerr << "Create a connection.\n";
		}
		odbcpp::connection conn(env);
		if(verbose) {
			std::cerr << "Set TIMEOUT to 5.\n";
		}
		conn.set_attr(SQL_LOGIN_TIMEOUT, (SQLPOINTER *) 5, SQL_IS_UINTEGER);
		if(verbose) {
			std::cerr << "Connecting to " << dsn << " as " << login << ".\n";
		}
		conn.connect(dsn, login, passwd);
		if(verbose) {
			std::cerr << "Create the statement.\n";
		}
		odbcpp::statement stmt(conn);
		if(verbose) {
			std::cerr << "Make cursors dynamic.\n";
		}
		conn.set_attr(SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_DYNAMIC);
		if(verbose) {
			std::cerr << "Make cursors scrollable (i.e. SQLFetchScroll() can be used)\n";
		}
		stmt.execute(order);
		if(verbose) {
			std::cerr << "Create a record.\n";
		}
		my_record_t rec;
		unsigned int row = 1;
		if(verbose) {
			std::cerr << "Read the records.\n\n";
		}
		std::cerr << "Fetching " << stmt.rows() << " rows with " << stmt.cols() << " columns\n";
		//stmt.set_no_direct_fetch();
		while(stmt.fetch(rec)) {
			rec.print();
		}
	}
	catch(odbcpp::odbcpp_error& err) {
		fflush(stdout);
		std::cerr << "\nodbcpp:error: " << err.what() << std::endl;
		std::cerr << "Program Abort.\n";
#ifdef _MSC_VER
		std::cerr << "\nType return to close the window.\n";
		getchar();
#endif
		exit(1);
	}
	catch(std::runtime_error& err) {
		fflush(stdout);
		std::cerr << "\nruntime:error: " << err.what() << std::endl;
		std::cerr << "Program Abort.\n";
#ifdef _MSC_VER
		std::cerr << "\nType return to close the window.\n";
		getchar();
#endif
		exit(1);
	}
	catch(...) {
		fflush(stdout);
		std::cerr << "\nunknown exception...\n";
		std::cerr << "Program Abort.\n";
#ifdef _MSC_VER
		std::cerr << "\nType return to close the window.\n";
		getchar();
#endif
		throw;
	}

#ifdef _MSC_VER
	std::cerr << "\nType return to close the window.\n";
	getchar();
#endif

	return 0;
}

// vim: ts=8 sw=8
