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


#ifndef INCL_FOLDERDATA
#define INCL_FOLDERDATA
/** @cond DEV */

#include "base/util/ArrayElement.h"
#include "base/util/WString.h"
#include "base/util/StringBuffer.h"
#include "base/util/ArrayList.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class FolderData : public ArrayElement {

    // ------------------------------------------------------- Private data
    private:
        //WString folder;
		WString name;
		WString created;
		WString modified;
		WString accessed;
		WString attributes;
		bool hidden;
		bool system;
		bool archived;
		bool deleted;
		bool writable;
		bool readable;
		bool executable;
		WString role;
        ArrayList extended;

        // represents the presence of their equivalent tag
        bool isHiddenPresent;
        bool isSystemPresent;
        bool isArchivedPresent;
        bool isDeletedPresent;
        bool isWritablePresent;
        bool isReadablePresent;
        bool isExecutablePresent;

        /*
        * return the length for the base64 array starting from length of the original array
        */
        int lengthForB64(int len);

    public:
    // ------------------------------------------------------- Constructors
        FolderData();
        ~FolderData();

    // ---------------------------------------------------------- Accessors
		/*const WCHAR* getFolder() { return folder; }
		void setFolder(const WCHAR* v) { folder = v; } */

		const WCHAR* getName() { return name; }
		void setName(const WCHAR* v) { name = v; }

		const WCHAR* getCreated() { return created; }
		void setCreated(const WCHAR* v) { created = v; }

		const WCHAR* getModified() { return modified; }
		void setModified(const WCHAR* v) { modified = v; }

		const WCHAR* getAccessed() { return accessed; }
		void setAccessed(const WCHAR* v) { accessed = v; }

		const WCHAR* getAttributes() { return attributes; }
		void setAttributes(const WCHAR* v) { attributes = v; }

		bool getHidded() { return hidden; }
		void setHidden(bool v) { hidden = v; }

		bool getSystem() { return system; }
		void setSystem(bool v) { system = v; }

		bool getArchived() { return archived; }
		void setArchived(bool v) { archived = v; }

		bool getDeleted() { return deleted; }
		void setDeleted(bool v) { deleted = v; }

		bool getWritable() { return writable; }
		void setWritable(bool v) { writable = v; }

		bool getReadable() { return readable; }
		void setReadable(bool v) { readable = v; }

		bool getExecutable() { return executable; }
		void setExecutable(bool v) { executable = v; }

		const WCHAR* getRole() { return role; }
		void setRole(const WCHAR* v) { role = v; }

        void setExtList(ArrayList& list){extended = list;};
        /**
         * getter of the Ext list.
         * @returns an ArrayList of KeyValuePairs with all the ext values
         */
        ArrayList getExtList() { return extended; };


    // ----------------------------------------------------- Public Methods
        int parse(const char *syncmlData, size_t len = WString::npos) ;
        char *format() ;

        ArrayElement* clone() { return new FolderData(*this); }

};


END_NAMESPACE

/** @endcond */
#endif
