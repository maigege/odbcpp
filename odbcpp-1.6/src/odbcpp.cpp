//
// File:	src/odbcpp.cpp
// Object:	Main Documentation of the odbcpp library
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


/** \mainpage
 *
 * The odbcpp library is a C++ wrapper around the ODBC API.
 *
 * \section project The odbcpp project
 *
 * The odbcpp project was created by Made to Order Software Corp.
 * And it holds the copyright.
 *
 * The project home page can be found on the corporate website:
 *
 * http://www.m2osw.com/odbcpp
 *
 * The project documentation can be found online here:
 *
 * http://www.m2osw.com/odbcpp_docs
 *
 * \section license odbcpp license
 *
 * The free version of the odbcpp library is under the GPL. This means
 * you are welcome to make use of the odbcpp library in your open source
 * and other free projects (where your users/customers have access to all
 * the source code.)
 *
 * If you need a commercial license for your company because you do not
 * want (or can) release your source code, then you need to purchase a
 * license from Made to Order Software Corp. Once you have such a license
 * you are free to sell this library linked against your closed source
 * software.
 *
 * \section error_handling Error Handling
 *
 * The error handling could be ameliorated.
 *
 * At this time, anything other than success or success with info generates
 * an exception. We will see whether usage tells us that it is too much or
 * whether it works. Please, feel free to report what you have to do in order
 * for your application to function so we can fix the library if necessary.
 *
 * \section known_bugs Known Bugs
 *
 * Using ODBC means using a driver between the ODBC library and the backend
 * database. This driver may include bugs. The following is a list of
 * problems we are aware of. These problem may disappear with time. And also
 * they may not be bugs but rather gotchas that you want to avoid.
 *
 * \li PostgreSQL supports a boolean type. It can be tricky to use the type
 *     properly. If you use a static record, bind the type to SQLCHAR and
 *     you will get 0 or 1 in that byte. If you use a dynamic record, you
 *     will get a string of "0" or "1".
 *
 * \par It is possible to add two variables in our of your odbc.ini file
 *      to control the boolean type. The \c BoolsAsChar=0 so the boolean come
 *      out as integers isntead. Then \c TrueIsMinus1=0 to get the value 1
 *      whenever it is true (instead of -1).
 *
 * \section not_implemented What is not yet implemented
 *
 * The following functions are not yet implemented in the wrapper. They
 * may make it there one day, though.
 * 
 * \li SQLBindParameter
 * \li SQLBrowseConnect
 * \li SQLBulkOperation
 * \li SQLColAtribute
 * \li SQLColumnPrivileges
 * \li SQLColumns
 * \li SQLDescribeParam
 *
 * \section unused Unused ODBC functions
 *
 * The following functions are not used either because they have been marked
 * as deprecated or because their are just way too complex and most of the
 * time unnecessary.
 *
 * \li SQLAllocConnect (uses SQLAllocHandle)
 * \li SQLAllocEnv (uses SQLAllocHandle)
 * \li SQLAllocStmt (uses SQLAllocHandle)
 * \li SQLColAtributes (uses SQLColAttribute)
 * \li SQLCopyDesc (not necessary)
 * \li SQLFreeStmt (uses SQLCancel(), SQLCloseCursor(), SQLFreeHandle())
 * \li SQLDriverConnect (we may need this one later)
 *
 */


#include	"odbcpp/odbcpp.h"
#include	<cstring>

// we get a warning if sizeof(ptr) == sizeof(integer)
#ifdef _MSC_VER
#pragma warning(disable: 4318)
#endif


/// The odbcpp wrapper is fully enclosed in the odbcpp namespace
namespace odbcpp
{


/** \brief Retrieve the library version.
 *
 * \return a constant string pointer that represents
 * the version of the library.
 */
const char *get_version()
{
	return PACKAGE_VERSION;
}


/** \brief Convert an integer to a pointer.
 *
 * Some compilers rightfully generate warnings and/or errors
 * when simply casting an integer to a pointer. This is done
 * with all the setattr() calls.
 *
 * \return a pointer with the content of the integer
 */
SQLPOINTER int_to_ptr(SQLINTEGER integer)
{
	SQLPOINTER	ptr;

	if(sizeof(ptr) < sizeof(integer)) {
		diagnostic d(odbcpp_error::ODBCPP_INTERNAL, std::string("the size of an SQLINTEGER is larger than SQLPOINTER?!"));
		throw odbcpp_error(d);
	}
#if BYTE_ORDER == LITTLE_ENDIAN
	// in this case, little endian works great
	memcpy(&ptr, &integer, sizeof(integer));
	if(sizeof(ptr) > sizeof(integer)) {
		memset(reinterpret_cast<char *>(&ptr) + sizeof(ptr) - sizeof(integer),
				0, sizeof(ptr) - sizeof(integer));
	}
#else
	memcpy(reinterpret_cast<char *>(&ptr) + sizeof(ptr) - sizeof(integer),
		&integer,
		sizeof(integer));
	if(sizeof(ptr) > sizeof(integer)) {
		memset(&ptr, 0, sizeof(ptr) - sizeof(integer));
	}
#endif

	return ptr;
}



}	// namespace odbcpp

