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


#ifndef INCL_PIM_BUSINESS_DETAIL
#define INCL_PIM_BUSINESS_DETAIL
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "vocl/vCard/Address.h"
#include "vocl/vCard/ContactDetail.h"
#include "vocl/vCard/vCardProperty.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

typedef WCHAR* WSTRING;
/**
 * An object containing the business details of a contact
 *
 */
class BusinessDetail {

    // ------------------------------------------------------------ Private data
    private:
        Address*       address;
        ContactDetail* contactDetail;
        vCardProperty*      role;
        ArrayList*     titles;
        vCardProperty*      company;
        vCardProperty*      department;
        vCardProperty*      logo;
        WCHAR*       manager;
        WCHAR*       assistant;

        /**
         * Sets the given property address to a clone of the given property. If
         * oldProperty is not null the pointed object is deleted.
         *
         * @param oldProperty Property** to the address of the property to set
         * @param newProperty the new property to set
         */
        void set(vCardProperty** oldProperty, vCardProperty& newProperty);


        /**
         * Sets internal members releasing the currently allocated memory (if
         * any was allocate). The passed value is doplicated so that the caller
         * can independently release it.
         *
         * @param property the address of the pointer to set to the new
         *                 allocated memory
         * @param v the value to set into the property
         */
        void set(WCHAR** p, WCHAR* v);


    // -------------------------------------------- Constructors and Destructors
    public:
        /**
         * Creates an empty list of business details
         */
        BusinessDetail();
        ~BusinessDetail();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the role for this Businnes Detail
         *
         * @return the role for this Businnes Detail or NULL if not specified
         */
        vCardProperty* getRole();

        /**
         * Sets the role. The given property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setRole(vCardProperty& p);

        /**
         * Returns the title for this Businnes Detail
         *
         * @return the title for this Businnes Detail or NULL if not specified
         */
        ArrayList* getTitles();

        /**
         * Sets the titles. The given titles are cloned, so that the
         * caller can independently release them as needed.
         */
        void setTitles(WCHAR* titleArray[], int n);

        /**
         * Returns the address for this Businnes Detail
         *
         * @return the address for this Businnes Detail or NULL if not specified
         */
        Address* getAddress();

        /**
         * Sets the address. The given Address is cloned, so that the
         * caller can independently release it as needed.
         */
        void setAddress(Address& a);

        /**
         * Returns the company for this Businnes Detail
         *
         * @return the company for this Businnes Detail or NULL if not specified
         */
        vCardProperty* getCompany();

        /**
         * Sets the company. The given Property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setCompany(vCardProperty& a);

        /**
         * Returns the department for this Businnes Detail
         *
         * @return the department for this Businnes Detail or NULL if not specified
         */
        vCardProperty* getDepartment();

        /**
         * Sets the department. The given Property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setDepartment(vCardProperty& a);

        /**
         * Returns the contact details for this Businnes Detail
         *
         * @return the contact details for this Businnes Detail or NULL if not specified
         */
        ContactDetail* getContactDetail();

        /**
         * Sets the contact detail. The given ContactDetail is cloned, so that the
         * caller can independently release it as needed.
         */
        void setContactDetail(ContactDetail& a);

        /**
         * Returns the manager for this Businnes Detail
         *
         * @param manager if not NULL, the value is copied in this buffer
         * @param size manager buffer size
         *
         * @return the internal buffer if manager was NULL, manager otherwise
         *
         */
        WCHAR* getManager(WCHAR* manager = NULL, int size=-1);

        /**
         * Returns the assistant for this Businnes Detail
         *
         * @param assistant if not NULL, the value is copied in this buffer
         * @param size assistant buffer size
         *
         * @return the internal buffer if manager was NULL, manager otherwise
         */
        WCHAR* getAssistant(WCHAR* assistant = NULL, int size=-1);

        /**
         * Returns the logo for this Businnes Detail
         *
         * @return the logo for this Businnes Detail or NULL if not specified
         */
        vCardProperty* getLogo();

        /**
         * Sets the logo. The given Property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setLogo(vCardProperty& a);

        /**
         * Sets the manager for this Businnes Detail
         *
         * @param manager the manager to set
         */
        void setManager (WCHAR* manager);

        /**
         * Sets the assistant for this Businnes Detail
         *
         * @param assistant the assistant to set
         */
        void setAssistant (WCHAR* assistant);

        /**
         * Creates and returns a new BusinessDetail object. The object is created with
         * the C++ new operator and must be deallocated with the delete C++
         * operator
         */
        BusinessDetail* clone();

};


END_NAMESPACE

/** @endcond */
#endif
