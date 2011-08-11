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

//#include "base/winadapter.h"

#include "base/fscapi.h"

BEGIN_NAMESPACE

class FolderData : public ArrayElement {

    // ------------------------------------------------------- Private data
    protected:
        StringBuffer parent;
		StringBuffer name;
		StringBuffer created;
		StringBuffer modified;
		StringBuffer accessed;
		StringBuffer attributes;
		bool hidden;
		bool system;
		bool archived;
		bool deleted;
		bool writable;
		bool readable;
		bool executable;
		StringBuffer role;
        ArrayList extended;

        /// The ID of this folder (the key returned to the Server) (called fid istead of id for compilation as Objecitve-C++ under mac builds)
        WString fid;

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
        const char* getValueByName(const char* valName) const;
        void setValueByName(const char* valName, const char* setVal);




    public:
    // ------------------------------------------------------- Constructors
        FolderData();
        ~FolderData();

    // ---------------------------------------------------------- Accessors


        //The value parent is not a syncml compliant value for Folder
        //but it is implemented here to have a smarter access to the
        //Hierarchical synchronization of the folders
        const StringBuffer& getParent() const { return parent; }
        void setParent(const char* v) { parent = v; }
		
        const StringBuffer& getName() const { return name; }
        void setName(const char* v) { name = v; }

		const StringBuffer& getCreated() const { return created; }
		void setCreated(const char* v) { created = v; }

		const StringBuffer& getModified() const { return modified; }
		void setModified(const char* v) { modified = v; }

		const StringBuffer& getAccessed() const { return accessed; }
		void setAccessed(const char* v) { accessed = v; }

		const StringBuffer& getAttributes() const { return attributes; }
		void setAttributes(const char* v) { attributes = v; }

		bool getHidden() const { return hidden; }
		void setHidden(bool v) { hidden = v; }

		bool getSystem() const { return system; }
		void setSystem(bool v) { system = v; }

		bool getArchived() const { return archived; }
		void setArchived(bool v) { archived = v; }

		bool getDel() const { return deleted; }
		void setDel(bool v) { deleted = v; }

		bool getWritable() const { return writable; }
		void setWritable(bool v) { writable = v; }

		bool getReadable() const { return readable; }
		void setReadable(bool v) { readable = v; }

		bool getExecutable() const { return executable; }
		void setExecutable(bool v) { executable = v; }

		const StringBuffer& getRole() const { return role; }
		void setRole(const char* v) { role = v; }

        const WCHAR* getID() const;
        void setID(const WCHAR* val);   


        void setExtList(ArrayList& list){extended = list;};
        /**
         * getter of the Ext list.
         * @returns an ArrayList of KeyValuePairs with all the ext values
         */
        ArrayList& getExtList() { return extended; };


    // ----------------------------------------------------- Public Methods
        int parse(const char *syncmlData, size_t len = StringBuffer::npos) ;
        char *format() ;

        ArrayElement* clone() { return new FolderData(*this); }

};


END_NAMESPACE

/** @endcond */
#endif
