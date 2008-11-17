/*
 * Copyright (C) 2003-2007 Funambol, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */


//
// @author Stefano Fornari @ Funambol
// @version $Id: ContactDetail.h,v 1.4 2007-06-06 08:36:11 mvitolo Exp $
//

#ifndef INCL_PIM_CONTACT_DETAIL
#define INCL_PIM_CONTACT_DETAIL
/** @cond DEV */

#include "base/util/ArrayList.h"

/**
 * An object containing details on how to reach a contact (phone numbers, emails, webpage)
 *
 */
class ContactDetail {

    // ------------------------------------------------------------ Private data

    private:
        ArrayList* phones  ;
        ArrayList* emails  ;
        ArrayList* webPages;

    // -------------------------------------------- Constructors and Destructors
    public:
        /**
         * Creates an empty list of contact details
         */
        ContactDetail();
        ~ContactDetail();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the telephones for this Contact Detail
         *
         * @return the telephones for this Contact Detail or null if not defined
         */
        ArrayList* getPhones();

        /**
         * Sets the phones. The given ArrayList is cloned, so that the
         * caller can independently release it as needed.
         */
        void setPhones(ArrayList& list);

        /**
         * Returns the emails for this Contact Detail
         *
         * @return the emails for this Contact Detail or null if not defined
         */
        ArrayList* getEmails();

        /**
         * Sets the emails. The given ArrayList is cloned, so that the
         * caller can independently release it as needed.
         */
        void setEmails(ArrayList& list);

        /**
         * Returns the webpage for this Contact Detail
         *
         * @return the webpage for this Contact Detail or null if not defined
         */
        ArrayList* getWebPages();

        /**
         * Sets the web pages. The given ArrayList is cloned, so that the
         * caller can independently release it as needed.
         */
        void setWebPages(ArrayList& list);

        /**
         * Creates and returns a new ContactDetail object. The object is created with
         * the C++ new operator and must be deallocated with the delete C++
         * operator
         */
        ContactDetail* clone();

};

/** @endcond */
#endif
