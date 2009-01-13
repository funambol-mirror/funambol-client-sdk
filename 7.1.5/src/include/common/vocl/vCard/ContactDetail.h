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


//
// @author Stefano Fornari @ Funambol
// @version $Id: ContactDetail.h,v 1.6 2008-04-11 08:17:13 marco_garatti Exp $
//

#ifndef INCL_PIM_CONTACT_DETAIL
#define INCL_PIM_CONTACT_DETAIL
/** @cond DEV */

#include "base/util/ArrayList.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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


END_NAMESPACE

/** @endcond */
#endif
