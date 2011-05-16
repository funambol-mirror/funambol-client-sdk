/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */


#ifndef INCL_BASE_UTIL_ARRAY_LIST
#define INCL_BASE_UTIL_ARRAY_LIST
/** @cond DEV */

#include "base/fscapi.h"

#include "base/util/ArrayElement.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * This class implements a simple linked list that can be accessed by index too.
 * This class does not make use of C++ templates by choice, since it must be
 * as much easier and portable as possible.
 *
 * Each list element must be an instance of ArrayElement, which must be
 * considered an abstract class. Implementing classes must define the destructor
 * and the method clone(), which is used to replicate a given object. This is
 * used by insertion methods: they always clone the item and store the cloned
 * element so that the version inside the list and the caller can independently
 * release their memory. Note that clone methods MUST use the C++ operator to
 * allocate new object, since ArrayList will delete them calling the C++ delete
 * operator.
 */

//struct Element;
// Internal structure for the list
struct Element {
    // Initialize the structure
    Element(ArrayElement *_e = NULL, Element* _n = NULL): e(_e), n(_n) {};

    ArrayElement* e; // the element value
    Element* n;      // the next element (NULL for the last)
};


class ArrayList {
    private:
        Element* head;
        Element* lastElement;

        Element* iterator;

        int count;

        ArrayList& set (const ArrayList & other);

        Element ghost;

    protected:

        

    public:
        /**
         * Can be used to reset the iterator, so that next call to next restart from
         * the beginning.
         */
        void resetIterator() { iterator = 0; }


        ArrayList();
        ArrayList(const ArrayList &other);
        ~ArrayList();

        /**
         * Is this list empty?
         */
        bool isEmpty() const;

        /**
         * Adds a new element at the specified index. If index is greater than
         * the list size the element is appended.
         * The element is dinamically duplicated so that the caller can release
         * the element at any time. 
         *
         * @param index the insertion position
         * @param element the element to insert
         * @return the position (0 based) at which the element has been
         *         inserted. It can be different by index if index is out of
         *         the array bounds, in that case element is appended as last
         *         element. It returns -1 in case of errors.
         */
        int add(int index, ArrayElement& element);

        /**
         * Same as add(index, element, size), but append at the end of the array.
         *
         * @param element the element to insert
         */
        int add(ArrayElement& element);

        /**
         * Add all the ArrayElement of the given ArrayList to the current
         * array list
         */
        int add(ArrayList* list);

        /**
         * Frees the list. All elements are freed as well.
         */
        void clear();

        /**
         * Frees the list and all its elements, regardless the value of
         * autoDeleteElements.
         */
        void clearAll();

        /**
         * Removes the item at the given index. The item is deleted and memory
         * freed.
         * If the index is beyond the last item, then the last item is removed.
         * If the index is negative, the method returns without removing
         * anything.
         *
         * @param index the element position
         */
        int removeElementAt(int index);

        /**
         * Returns the index-th element of the array or NULL if index is out of
         * the array bounds.
         *
         * Note that the retuned element will be released at
         * list destruction. Clone it if it must have a different life cycle.
         *
         * @param index the element position
         * @return the index-th element of the array or NULL if index is out of
         *         the array bounds
         */
        ArrayElement* get(int index) const;

        /**
         * Returns the first element of the array and set here the internal iterator.
         *
         * Note that the retuned element will be released at list destruction.
         * Clone it if it must have a different life cycle.
         *
         * @return - the first element of the array, or NULL if empty.
         */
        ArrayElement* front();

        /**
         * Returns the next element of the array and increment the internal iterator.
         *
         * Note that the retuned element will be released at list destruction.
         * Clone it if it must have a different life cycle.
         *
         * @return - the next element of the array, or NULL if beyond the last.
         */
        ArrayElement* next();

        /**
         * Returns the previous element of the array and decrement the internal 
         * iterator.
         *
         * Note that the retuned element will be released at list destruction.
         * Clone it if it must have a different life cycle.
         *
         * @return - the previous element of the array, or NULL if before the first.
         */
        ArrayElement* prev();

        /**
         * Returns the last element of the array and set here the internal iterator.
         *
         * Note that the retuned element will be released at list destruction.
         * Clone it if it must have a different life cycle.
         *
         * @return - the first element of the array, or NULL if empty.
         */
        ArrayElement* back();
        
        /**
         * Returns true if it is the last one, false otherwise
         *
         * @return - true if the iterator is at the last element, false otherwise.
         */
        bool last() const;

        /**
         * Returns the array size.
         */
        int size() const;

        /**
         * Same as get(index)
         */
        ArrayElement* operator[] (int index) const;

        /**
         * Copy the ArrayList
         */
        ArrayList& operator= (const ArrayList &v);


        /**
         * Clones the arrayList a return a pointer to a new one.
         */
        ArrayList* clone();

};

END_NAMESPACE

/** @endcond */
#endif
