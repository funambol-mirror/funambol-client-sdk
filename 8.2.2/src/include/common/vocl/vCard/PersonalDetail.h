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
// @version $Id: PersonalDetail.h,v 1.8 2008-04-11 08:17:13 marco_garatti Exp $
//


#ifndef INCL_PIM_PERSONAL_DETAIL
#define INCL_PIM_PERSONAL_DETAIL
/** @cond DEV */

#include "vocl/vCard/Address.h"
#include "vocl/vCard/ContactDetail.h"
#include "vocl/vCard/vCardProperty.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * An object containing the personal details of a contact
 *
 */
class PersonalDetail {

    // ------------------------------------------------------------ Private data

    private:
        Address*       address      ;
        Address*       otherAddress ;
        ContactDetail* contactDetail;
        vCardProperty* photo        ;
        WCHAR*       spouse       ;
        WCHAR*       children     ;
        WCHAR*       anniversary  ;
        WCHAR*       birthday     ;
        WCHAR*       gender       ;

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
         * Creates an empty list of personal details
         */
        PersonalDetail();
        ~PersonalDetail();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the contact details for this Personal Detail
         *
         * @return the contact details for this Personal Detail
         */
        ContactDetail* getContactDetail();

        /**
         * Sets the contact detail. The given ContactDetail is cloned, so that the
         * caller can independently release it as needed.
         */
        void setContactDetail(ContactDetail& d);

        /**
         * Returns the address for this Personal Detail
         *
         * @return the address for this Personal Detail
         */
        Address* getAddress();

        /**
         * Sets the address. The given Address is cloned, so that the
         * caller can independently release it as needed.
         */
        void setAddress(Address& a);

        /**
         * Returns the other address for this Personal Detail
         *
         * @return the other address for this Personal Detail
         */
        Address* getOtherAddress();

        /**
         * Sets the other address. The given Address is cloned, so that the
         * caller can independently release it as needed.
         */
        void setOtherAddress(Address& p);

        /**
         * Returns the spouse for this Personal Detail
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the spouse for this Personal Detail
         */
        WCHAR* getSpouse(WCHAR* buf = NULL, int size = -1);

        /**
         * Returns the children for this Personal Detail
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the children for this Personal Detail
         */
        WCHAR* getChildren(WCHAR* buf = NULL, int size = -1);

        /**
         * Returns the anniversary for this Personal Detail
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the anniversary for this Personal Detail
         */
        WCHAR* getAnniversary(WCHAR* buf = NULL, int size = -1);

        /**
         * Returns the birthday for this Personal Detail
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the birthday for this Personal Detail
         */
        WCHAR* getBirthday(WCHAR* buf = NULL, int size = -1);

        /**
         * Returns the gender for this Personal Detail
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the gender for this Personal Detail
         */
        WCHAR* getGender(WCHAR* buf = NULL, int size = -1);

        /**
         * Returns the photo for this Personal Detail
         *
         * @return the photo for this Personal Detail
         */
        vCardProperty* getPhoto();

        /**
         * Sets the photo. The given Property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setPhoto(vCardProperty& p);

        /**
         * Sets the spouse for this Personal Detail
         *
         * @param spouse the spouse to set
         */
        void setSpouse (WCHAR* spouse);

        /**
         * Sets the children for this Personal Detail
         *
         * @param children the children to set
         */
        void setChildren (WCHAR* children);

        /**
         * Sets the anniversary for this Personal Detail
         *
         * @param anniversary the anniversary to set
         */
        void setAnniversary (WCHAR* anniversary);

        /**
         * Sets the birthday for this Personal Detail
         *
         * @param birthday the spouse to set
         */
        void setBirthday (WCHAR* birthday);

        /**
         * Sets the gender for this Personal Detail
         *
         * @param gender the gender to set
         */
        void setGender (WCHAR* gender);

        /**
         * Creates and returns a new PeronalDetail object. The object is created with
         * the C++ new operator and must be deallocated with the delete C++
         * operator
         */
        PersonalDetail* clone();
};


END_NAMESPACE

/** @endcond */
#endif
