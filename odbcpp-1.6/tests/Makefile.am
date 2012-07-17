#
# File:		tests/Makefile.am
# Object:	Makefile to compile the odbcpp tests
# Project:	http://www.m2osw.com/odbcpp
# Author:	alexis_wilke@sourceforge.net
#
# Copyright (C)   2008 Made to Order Software Corp.
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

AM_CPPFLAGS = $(all_includes) -I$(top_srcdir)/include

# all the libraries to generate
if COMPILE_TESTS
ODBCPP_TESTS=connect record two-tables
endif

noinst_PROGRAMS = $(ODBCPP_TESTS)

connect_SOURCES = \
	connect.cpp

connect_LDADD = ../src/libodbcpp.la -lodbc


record_SOURCES = \
	record.cpp

record_LDADD = ../src/libodbcpp.la -lodbc


two_tables_SOURCES = \
	two-tables.cpp

two_tables_LDADD = ../src/libodbcpp.la -lodbc

