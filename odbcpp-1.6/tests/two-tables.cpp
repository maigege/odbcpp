//
// File:	src/two-tables.cpp
// Object:	Sample code used to test reading multiple tables at once
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
// This test requires 3 tables with columns as follow:
//
// Table 1 -- name: variables
//
//    id          INTEGER
//    value       INTEGER
//
// Table 2 -- name: names
//
//    id          INTEGER
//    name        TEXT
//
// Table 3 -- name: users
//
//    id          INTEGER
//    name        TEXT
//
// The idea is to have a variable defined by an identifier for fast access,
// given a name in the names table and attached to a user in the users table.
//
// Use the following command to create the tables and insert data in them:
//
// CREATE TABLE variables (id INTEGER NOT NULL DEFAULT 0, value INTEGER NOT NULL);
// CREATE TABLE names (id INTEGER NOT NULL DEFAULT 0, name TEXT);
// CREATE TABLE users (id INTEGER NOT NULL DEFAULT 0, name TEXT);
//
// INSERT INTO variables VALUES (1, 123);
// INSERT INTO variables VALUES (2, 555);
// INSERT INTO variables VALUES (3, 911);
// INSERT INTO variables VALUES (4, 415);
//
// INSERT INTO names VALUES (1, 'Var123');
// INSERT INTO names VALUES (2, 'c555');
// INSERT INTO names VALUES (3, 'police');
// INSERT INTO names VALUES (4, 'sfac');
//
// INSERT INTO users VALUES (1, 'alexis');
// INSERT INTO users VALUES (2, 'doug');
// INSERT INTO users VALUES (3, 'john');
// INSERT INTO users VALUES (4, 'halk');
//

#include	"odbcpp/odbcpp.h"
#include	<iostream>
#include	<sstream>
#include	<cstring>
#include	<cstdlib>
#include	<cstdio>


const char *progname;

void usage()
{
	std::cerr << "odbcpp:test: two-tables v" << odbcpp::get_version() << "\n";
	std::cerr << "Usage: " << progname << " [-opts] <dsn> <login> <password>\n";
	std::cerr << "where -opts is one of the following:\n";
	std::cerr << "   -h     print out this help screen\n";
	std::cerr << "   -l     print out license information\n";
	std::cerr << "   -v     be verbose as we work on the SQL order\n";
	exit(1);
}


void license()
{
	std::cerr << "odbcpp::two-tables  Copyright (C) 2008  Made to Order Software Corporation\n";
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

class variable_t : public odbcpp::record
{
public:
	variable_t()
	{
		bind("id", f_id);
		bind("value", f_value);
	}

	SQLINTEGER get_id() const
	{
		return f_id;
	}

	void print()
	{
		std::cout << "variable #" << f_id << " = " << f_value << std::endl;
	}

private:
	SQLINTEGER	f_id;
	SQLINTEGER	f_value;
};

class name_t : public odbcpp::record
{
public:
	name_t()
	{
		bind("id", f_id);
		bind("name", f_name, &f_name_is_null);
	}

	void print()
	{
		std::cout << "name #" << f_id << " = ";
		if(f_name_is_null) {
			std::cout << "<undefined>";
		}
		else {
			std::cout << f_name;
		}
		std::cout << std::endl;
	}

private:
	SQLINTEGER	f_id;
	bool		f_name_is_null;
	std::string	f_name;
};

class user_t : public odbcpp::record
{
public:
	user_t()
	{
		bind("id", f_id);
		bind("name", f_name, &f_name_is_null);
	}

	void print()
	{
		std::cout << "user #" << f_id << " = ";
		if(f_name_is_null) {
			std::cout << "<undefined>";
		}
		else {
			std::cout << f_name;
		}
		std::cout << std::endl;
	}

private:
	SQLINTEGER	f_id;
	bool		f_name_is_null;
	std::string	f_name;
};

int main(int argc, char *argv[])
{
	int		i;
	const char	*dsn;
	const char	*login;
	const char	*passwd;
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

	if(login == 0) {
		std::cerr << "odbc:connect:error: the server name, login, and password are all mandatory parameters.\n";
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
			std::cerr << "Create the statements.\n";
		}
		odbcpp::statement stmt(conn);
		odbcpp::statement user_stmt(conn);
		odbcpp::statement name_stmt(conn);
		if(verbose) {
			std::cerr << "Make cursors dynamic.\n";
		}
		conn.set_attr(SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_DYNAMIC);
		if(verbose) {
			std::cerr << "Make cursors scrollable (i.e. SQLFetchScroll() can be used)\n";
		}
		// read all the variables
		if(verbose) {
			std::cerr << "Read the variables.\n";
		}
		stmt.execute("SELECT * FROM variables");
		if(verbose) {
			std::cerr << "Create records.\n";
		}
		variable_t rec_variable;
		name_t rec_name;
		user_t rec_user;
		unsigned int row = 1;
		if(verbose) {
			std::cerr << "Read the records.\n\n";
		}
		std::cerr << "Fetching " << stmt.rows() << " rows with " << stmt.cols() << " columns from the variables table.\n";
		//stmt.set_no_direct_fetch();
		while(stmt.fetch(rec_variable)) {
			rec_variable.print();

			// we could also check the identifier in C++ in the loops below...
			std::ostringstream user_sql;
			user_sql << "SELECT * FROM users WHERE id = " << rec_variable.get_id();
			if(verbose) {
				std::cerr << "Read the users for this variable " << user_sql.str() << ".\n";
			}
			user_stmt.execute(user_sql.str());

			std::ostringstream name_sql;
			name_sql << "SELECT * FROM names WHERE id = " << rec_variable.get_id();
			if(verbose) {
				std::cerr << "Read the names for this variable " << name_sql.str() << ".\n";
			}
			name_stmt.execute(name_sql.str());

			while(user_stmt.fetch(rec_user)) {
				rec_user.print();
			}
			while(name_stmt.fetch(rec_name)) {
				rec_name.print();
			}

			user_stmt.close_cursor();
			name_stmt.close_cursor();
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
