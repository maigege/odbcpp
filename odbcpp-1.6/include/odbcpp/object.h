//
// File:	include/odbcpp/object.h
// Object:	Define the base object of the odbcpp library
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
#ifndef ODBCPP_OBJECT
#define ODBCPP_OBJECT

#include <iostream>
#include <typeinfo>

using namespace std;

namespace odbcpp
{

class object
{
public:
				object(void);
				object(size_t ref_count);
				object(const object& obj);
				virtual ~object();

	object&			operator = (const object& obj);

	unsigned long		addref() const;
	unsigned long		release() const;

private:
	mutable unsigned long	f_refcount;
};


/// Ensure proper management of objects
template<class T> class smartptr
{
public:
	/// Define an empty smart pointer.
	smartptr() : f_ptr(0) {}

	/// Define a smart pointer with the specified bare pointer.
	// \param[in] obj  The object to be managed by smart pointers
	smartptr(T *obj) : f_ptr(obj) { f_ptr->addref(); }

	/// Define a smart pointer as a copy of another, this has the effect of calling addref(); the object is not duplicated.
	// \param[in] ptr  The object to be copied in another smart pointer
	smartptr(const smartptr<T>& ptr) : f_ptr(ptr.f_ptr) { f_ptr->addref(); }

	/// Relase a smart pointer (call release() on the object)
	~smartptr() { if (f_ptr->release() == 0) f_ptr = NULL; }

	/// Switch the bare pointer with another
	// \param[in] obj  The object to be manage by smart pointers
	void reset(T *obj = 0) { obj->addref(); f_ptr->release(); f_ptr = obj; }

	/// Set the smart pointer with another, same as reset(obj).
	// \param[in] obj  The object to be manage by smart pointers
	smartptr& operator = (T *obj) { reset(obj); return *this; }

	/// Copy a smart pointer into this smart pointer.
	// \param[in] ptr  The smart pointer to copy in another smart pointer
	smartptr& operator = (const smartptr& ptr) { reset(ptr.f_ptr); return *this; }

	/// Compare this smart pointer with a bare pointer.
	// \param[in] obj  Check whether \p obj is equal to the pointer in this smart pointer
	bool operator == (const T *obj) const { return f_ptr == obj; }

	/// Compare this smart pointer with a bare reference.
	// \param[in] obj  Check whether \p obj is equal to the pointer in this smart pointer
	bool operator == (const T& obj) const { return f_ptr == &obj; }

	/// Compare two smart pointers (what they point too).
	// \param[in] ptr  Check whether both smart pointer are managing the same pointer
	bool operator == (const smartptr& ptr) const { return f_ptr == ptr.f_ptr; }

	/// Check whetehr the pointer is null (false) or not (true).
	operator bool () const { return f_ptr != 0; }

	/// Check whether the pointer is null (true) or not (false).
	bool operator ! () const { return f_ptr == 0; }

	/// Return the bare pointer as a const pointer.
	const T *operator -> () const { return f_ptr; }

	/// Return the bare pointer.
	T *operator -> () { return f_ptr; }

	/// Cast to the bare pointer as a const pointer.
	operator T * () const { return f_ptr; }

	/// Cast to the bare pointer.
	operator T * () { return f_ptr; }

private:
	/// The bare pointer
	T *f_ptr;
};



}	// namespace odbcpp

#endif		// #ifndef ODBCPP_OBJECT


