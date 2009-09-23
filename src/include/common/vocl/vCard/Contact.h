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


#ifndef INCL_PIM_CONTACT
#define INCL_PIM_CONTACT
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "vocl/VObject.h"
#include "vocl/vCard/BusinessDetail.h"
#include "vocl/vCard/Name.h"
#include "vocl/vCard/Note.h"
#include "vocl/vCard/Title.h"
#include "vocl/vCard/PersonalDetail.h"
#include "vocl/vCard/Phone.h"
#include "vocl/vCard/Email.h"
#include "vocl/vCard/WebPage.h"

#define MAX_TITLES          10
#define MAX_VPROPERTY_VALUE 200
#define BUSINESS            0
#define HOME                1
#define OTHER               2
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class Contact : public VObject {

    // ------------------------------------------------------------ Private data

    private:
        Name* name;
        PersonalDetail* personalDetail;
        BusinessDetail* businessDetail;
        ArrayList* notes;

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
        vCardProperty* getPropertyFromVProperty(VProperty* vp);
        VProperty* getVPropertyFromProperty(const WCHAR* name, vCardProperty* prop);
        VProperty* composeVAddress(Address* adr);
        Address* composeAddress(VProperty* vp, int type);
        Address* addLabelAddress(VProperty* vp, int type);

    //--------------------------------------------- Constructors and Destructors

    public:
        Contact();
        ~Contact();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the UID of this contact
         *
         * @return the uid of this contact or NULL if not specified
         */
        WCHAR* getUID(WCHAR* buf = NULL, int size = -1);

        /**
         * Returns the timezone for this contact
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the timezone for this contact or NULL if not specified
         */
        WCHAR* getTimezone (WCHAR* buf = NULL, int size = -1);

        /**
         * Returns the note for this contact
         *
         * @return the note for this contact or NULL if not specified
         */
        ArrayList* getNotes();

        /**
         * Sets the notes. The given ArrayList is cloned, so that the
         * caller can independently release it as needed.
         */
        void setNotes(ArrayList& list);

        /**
         * Returns the revision of this contact
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the revision of this contact of NULL if not specified
         */
        WCHAR* getRevision(WCHAR* buf = NULL, int size = -1);

        /**
         * Returns the name of this contact
         *
         * @return the name of this contact or NULL if not specified
         */
        Name* getName();

        /**
         * Sets the contact name. The given Name is cloned, so that the
         * caller can independently release it as needed.
         */
        void setName(Name& n);

        /**
         * Returns the business details of this contact
         *
         * @return the business details of this contact of NULL if not specified
         */
        BusinessDetail* getBusinessDetail ();

        /**
         * Sets the business detail. The given BusinessDetail is cloned, so that the
         * caller can independently release it as needed.
         */
        void setBusinessDetail(BusinessDetail& d);

        /**
         * Returns the personal details of this contact
         *
         * @return the personaldetails of this contact or NULL if not specified
         */
        PersonalDetail* getPersonalDetail();

        /**
         * Sets the personal detail. The given PersonalDetail is cloned, so that the
         * caller can independently release it as needed.
         */
        void setPersonalDetail(PersonalDetail& d);

        /**
         * Sets the UID of this contact
         *
         * @param uid the UID to set
         */
        void setUID (WCHAR* uid);

        /**
         * Sets the timezone for this contact
         *
         * @param timezone the timezone to set
         */
        void setTimezone (WCHAR* tz);

        /**
         * Sets the revision of this contact
         *
         * @param revision the revision to set
         */
        void setRevision (WCHAR* revision);

        Contact* clone();
        WCHAR* toString();
};


END_NAMESPACE

/** @endcond */
#endif
