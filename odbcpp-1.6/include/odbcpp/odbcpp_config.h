//
// File:	include/odbcpp/odbcpp_config.h
// Object:	Configure the odbcpp library as required
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

#ifndef ODBCPP_CONFIG
#define ODBCPP_CONFIG


#if defined(_MSC_VER) || defined(WIN32)
// we need windows.h to be able to include sql.h
#include	<windows.h>
#define	BIG_ENDIAN	1234
#define	LITTLE_ENDIAN	4321
#define	BYTE_ORDER	LITTLE_ENDIAN
#endif

// used by sqltype.h
// determine whether 'long long' exists; this was taken from
// /usr/include/features.h under Linux.
// it is ignored by Microsoft headers
#if ! defined(HAVE_LONG_LONG) \
  &&   (defined __GNUC__ \
    || (defined __PGI && defined __i386__ ) \
    || (defined __INTEL_COMPILER && (defined __i386__ || defined __ia64__)) \
    || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L))
#define	HAVE_LONG_LONG	1
#endif




#endif		// #ifndef ODBCPP_CONFIG
