//
// File:	src/object.cpp
// Object:	Implementation of the base object
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

#include	"odbcpp/object.h"
#include	<stdlib.h>
#include	<iostream>

using namespace std;

namespace odbcpp
{


/** \class object
 *
 * \brief Base class used to handle smart pointers.
 *
 * This class is a base class used to have a reference counter
 * in all the odbcpp objects.
 *
 * This ensures that higher level objects do not get deleted
 * when they are still in use by some lesser objects.
 */

/** \var object::f_refcount
 *
 * \brief The reference counter of this object.
 *
 * The reference counter is set to 1 on initialization.
 *
 * It is incremented each time the pointer is saved in a smart
 * pointer.
 *
 * It is decremented each time the pointer is replaced in a
 * smart pointer.
 *
 * It is decremented each time a smart pointer is deleted.
 *
 * When the counter reaches 0, the object is deleted.
 *
 * If you create an object on the stack, the delete should never
 * be called since the reference counter will be 1 at the time
 * the object is deleted.
 */


/** \brief Constructor, setting the reference count to 1
 *
 * Initializes the object with a reference count of 1 meaning
 * that at least one person is referencing it (which is correct
 * since once created an object is somehow referenced.)
 */
object::object() :
	f_refcount(1)
{
}

object::object(size_t ref_count) :
	f_refcount(ref_count)
{
}

/** \brief Copy an existing object in a new object.
 *
 * This constructor ensures that the new object has
 * a reference counter of 1.
 *
 * \param[in] obj    Unused parameter.
 */
object::object(const object& obj) :
	f_refcount(1)
{
	// avoid some warnings
	(void) &obj;
}


/** \brief The destructor verifies the reference counter.
 *
 * If an object is being created on the stack, its counter
 * should never reached 0, but at time of destruction it
 * has to be exactly 1.
 *
 * The contructor/destructor cannot know whether the object
 * is created on the stack, as a variable member or was
 * allocated on the heap, so it accepts both values.
 */
object::~object()
{
	if(f_refcount != 0 && f_refcount != 1) {
		// TODO: should be a throw I think
		std::cerr << "object at " << this << " has a refcount of " << f_refcount << "\n";
		std::terminate();
	}
}



/** \brief Copy an object in another.
 *
 * This copy operator ensures that the reference counters
 * are not modified in either object since they are
 * specific to the number of smart pointer and not
 * the copy itself.
 *
 * \param[in] obj   Unused parameter.
 */
object& object::operator = (const object& obj)
{
	// avoid some warnings
	(void) &obj;
	return *this;
}


/** \brief Add one to the reference counter.
 *
 * Each time an object is put in another, you need to
 * call the addref() function.
 *
 * \bug
 * At this time, this function is not thread safe.
 *
 * \return The new reference count.
 */
unsigned long object::addref() const
{
	if(this == 0) {
		return 0;
	}

	return ++f_refcount;
}


/** \brief Subtract one from the reference counter.
 *
 * Each time an object releases another one, call this
 * function to let that other object that you are done
 * with it.
 *
 * If the counter reaches 0, then the object is automatically
 * deleted since no one has a reference to it.
 *
 * \return The new counter value, 0 if the object was deleted
 */
unsigned long object::release() const
{
	if(this == 0) {
		return 0;
	}

	unsigned long result = --f_refcount;

	// done with it?
	if(result == 0) {
		delete this;
	}

	return result;
}



}	// namespace odbcpp


