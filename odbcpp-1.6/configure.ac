dnl The "configure" script is made from this script by running GNU "autoreconf"
dnl You may simply run the accompagnying autogen.sh script.
dnl
dnl File:	configure.ac
dnl Object:	Configuration script for the odbcpp library.
dnl Project:	http://www.m2osw.com/odbcpp
dnl Author:	alexis_wilke@sourceforge.net
dnl
dnl Copyright (C)   2008-2011 Made to Order Software Corp.
dnl
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>
dnl or <http://gpl3.m2osw.com/>.
dnl
dnl 



dnl The name of the configuration script input
dnl
AC_INIT([odbc library],[1.6],[alexis_wilke@sourceforge.net],[odbcpp])
AC_CONFIG_AUX_DIR(dev/config)


AC_SUBST(PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AC_PREREQ(2.56)

dnl Copyright notice of the odbcpp library package
AC_COPYRIGHT([ Copyright (C)   2008 Made to Order Software Corp.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>
 or <http://gpl3.m2osw.com/>.])

dnl Define the configuration file and automake version
dnl
AM_INIT_AUTOMAKE([gnu])

dnl specify the supported languages
ALL_LINGUAS="en"

dnl check for programs
AC_LANG_CPLUSPLUS
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
dnl AC_PROG_DOXYGEN -- requires newer version...

dnl libtool program
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

dnl check for headers
AC_HAVE_HEADERS([string.h stdarg.h stdio.h stdlib.h limits.h errno.h ctype.h])

AC_CHECK_HEADER([sql.h], [], [], [
#if defined(_MSC_VER) || defined(WIN32)
#include <windows.h>
#endif
])

dnl the mingw32 version requires a -lodbc32 to link
case $host_os in
*mingw*)
	LIBS="$LIBS -lodbc32"
	;;
esac

dnl --enable-tests to compile the tests
compile_tests=false
AC_ARG_ENABLE(tests,
  AC_HELP_STRING([--enable-tests],[enable compiling tests (default is disabled)]),
  [if test "x$enable_tests" != "xno"; then
    compile_tests=true
  fi],
[])
AM_CONDITIONAL(COMPILE_TESTS, test $compile_tests = true)


ODBCPP_VERSION=1:0:0
AC_SUBST(ODBCPP_VERSION)


dnl Output the final makefiles
dnl
AC_OUTPUT(
	odbcpp.pc
	Makefile
	include/Makefile
	src/Makefile
	doc/Makefile
	doc/odbcpp.doxy
	tests/Makefile
	win32/Makefile
)

dnl vim: ts=8 sw=8
