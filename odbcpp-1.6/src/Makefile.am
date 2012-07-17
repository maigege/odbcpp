#
# File:		src/Makefile.am
# Object:	Makefile to compile the odbcpp library
# Project:	http://www.m2osw.com/odbcpp
# Author:	alexis_wilke@sourceforge.net
#
# Copyright (C)   2008-2011 Made to Order Software Corp.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
# or <http://gpl3.m2osw.com/>.
#

AM_CPPFLAGS = $(all_includes) -I$(top_srcdir)/include \
	-DPACKAGE_STRING="\"$(PACKAGE_STRING)\"" -DPACKAGE_VERSION="\"$(PACKAGE_VERSION)\""

# all the libraries to generate
lib_LTLIBRARIES = libodbcpp.la

libodbcpp_la_SOURCES = \
	connection.cpp      \
	diagnostic.cpp      \
	environment.cpp     \
	exception.cpp       \
	handle.cpp          \
	object.cpp          \
	odbcpp.cpp          \
	record.cpp          \
	statement.cpp


libodbcpp_la_LDFLAGS = -version-info $(ODBCPP_VERSION) \
	-release $(PACKAGE_VERSION) $(NO_UNDEFINED)
libodbcpp_la_LIBADD = $(LIBS)

