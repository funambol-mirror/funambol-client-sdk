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


#ifndef INCL_FOLDERDATA
#define INCL_FOLDERDATA
/** @cond DEV */

#include "base/util/ArrayElement.h"
#include "base/util/WString.h"
#include "base/util/StringBuffer.h"

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


    // ----------------------------------------------------- Public Methods
        int parse(const char *syncmlData, size_t len = WString::npos) ;
        char *format() ;

        ArrayElement* clone() { return new FolderData(*this); }

};

/** @endcond */
#endif

