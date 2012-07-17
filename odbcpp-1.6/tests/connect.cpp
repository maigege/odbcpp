//
// File:	src/connect.cpp
// Object:	Sample code used to connect to a database
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

#include	"odbcpp/odbcpp.h"
#include	<iostream>
#include	<cstring>
#include	<cstdlib>
#include	<cstdio>


const char *progname;

void usage()
{
	std::cerr << "odbcpp:test: connect v" << odbcpp::get_version() << "\n";
	std::cerr << "Usage: " << progname << " [-opts] <dsn> <login> <password> <SQL order>\n";
	std::cerr << "where -opts is one of the following:\n";
	std::cerr << "   -h     print out this help screen\n";
	std::cerr << "   -l     print out license information\n";
	std::cerr << "   -v     be verbose as we work on the SQL order\n";
	exit(1);
}


void license()
{
	std::cerr << "odbcpp::connect  Copyright (C) 2008  Made to Order Software Corporation\n";
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
		/*
		conn.set_attr(SQL_ATTR_CURSOR_SCROLLABLE, SQL_SCROLLABLE);
		if(verbose) {
			std::cerr << "Run the statement.\n";
		}
		*/
		stmt.execute(order);
		if(verbose) {
			std::cerr << "Create a record.\n";
		}
		odbcpp::dynamic_record rec;
		unsigned int row = 1;
		if(verbose) {
			std::cerr << "Read the next record.\n";
		}
		std::cerr << "Fetching " << stmt.rows() << " rows with " << stmt.cols() << " columns\n";
		stmt.set_no_direct_fetch();
		while(stmt.fetch(rec)) {
			if(verbose) {
				std::cerr << "Read a record.\n";
			}
			SQLSMALLINT col, max = static_cast<SQLSMALLINT>(rec.size());
			if(verbose) {
				std::cerr << "Number of columns in the record: " << max << std::endl;
			}
			std::cout << row;
			for(col = 1; col <= max; ++col) {
				if(rec.get_is_null(col)) {
					std::cout << " | (null)";
				}
				else switch(rec.get_type(col)) {
				case SQL_C_SHORT:
					{
					SQLSMALLINT v;
					rec.get(col, v);
					std::cout << " | " << v;
					}
					break;

				case SQL_C_LONG:
					{
					SQLINTEGER v;
					rec.get(col, v);
					std::cout << " | " << v;
					}
					break;

				case SQL_C_SBIGINT:
				case SQL_BIGINT:	// here we get this one instead of SQL_C_SBIGINT
					{
					SQLBIGINT v;
					rec.get(col, v);
					std::cout << " | " << v;
					}
					break;

				case SQL_C_CHAR:
				case SQL_VARCHAR:	// no C corresponding type...
				case SQL_LONGVARCHAR:	// no C corresponding type...
					{
					std::string s;
					rec.get(col, s);
					if(s.size() == 1 && (s[0] == '0' || s[0] == '1') && s[1] == '\0') {
						// boolean case
						std::cout << " | " << (s[0] == '1' ? "true" : "false");
					}
					else {
						std::cout << " | " << s;
					}
					}
					break;

				case SQL_C_WCHAR:
				case SQL_WVARCHAR:	// no C corresponding type...
				case SQL_WLONGVARCHAR:
					{
					std::wstring s;
					rec.get(col, s);
					std::wcout << L" | " << s;
					}
					break;

				case SQL_C_DATE:
				case SQL_TYPE_DATE:
					{
					SQL_DATE_STRUCT date;
					rec.get(col, date);
					std::cout << " | <date>";
					}
					break;

				case SQL_C_TIME:
				case SQL_TYPE_TIME:
					{
					SQL_TIME_STRUCT time;
					rec.get(col, time);
					std::cout << " | <time>";
					}
					break;

				case SQL_C_TIMESTAMP:
				case SQL_TYPE_TIMESTAMP:
					{
					SQL_TIMESTAMP_STRUCT timestamp;
					rec.get(col, timestamp);
					std::cout << " | <timestamp>";
					}
					break;

				default:
					if(row == 1) {
						std::cerr << "unknown column type " << rec.get_type(col) << " for column #" << col << "; skipping.\n";
					}
					break;

				}
			}
			std::cout << "\n";
			++row;
			if(verbose) {
				std::cerr << "Read the next record.\n";
			}
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
