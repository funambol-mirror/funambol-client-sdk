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


#ifndef INCL_FILEDATA
#define INCL_FILEDATA
/** @cond DEV */

#include "base/util/ArrayElement.h"
#include "base/util/WString.h"
#include "base/util/StringBuffer.h"

class FileData : public ArrayElement {

    // ------------------------------------------------------- Private data
    private:
        WString file;
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
		WString cttype;
		StringBuffer body;
		WString enc;
		int size;

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
        FileData();
        ~FileData();

    // ---------------------------------------------------------- Accessors
		const WCHAR* getFile() { return file; }
		void setFile(const WCHAR* v) { file = v; }

		const WCHAR* getName() { return name; }
		void setName(const WCHAR* v) { name = v; }

		const WCHAR* getCreated() { return created; }
		void setCreated(const WCHAR* v) { created = v; }

		const WCHAR* getModified() { return modified; }
		void setModified(const WCHAR* v) { modified = v; }

		const WCHAR* getAccessed() { return accessed; }
		void setAccessed(const WCHAR* v) { accessed = v; }

		const WCHAR* getAttributes() { return file; }
		void setAttributes(const WCHAR* v) { attributes = v; }

		bool getHiddied() { return hidden; }
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

		const WCHAR* getCttype() { return cttype; }
		void setCttype(const WCHAR* v) { cttype = v; }

        const char* getBody() { return body; }
		void setBody(const char* v, int len);

		const WCHAR* getEnc() { return enc; }
		void setEnc(const WCHAR* v) { enc = v; }

		int getSize() { return size; }
		void setSize(int v) { size = v; }

        int parse(StringBuffer* s) ;

    // ----------------------------------------------------- Public Methods
        int parse(const char *syncmlData, size_t len = WString::npos) ;
        int parse(const void *syncmlData, size_t len) ;

        char *format() ;

        ArrayElement* clone() { return new FileData(*this); }

};

/** @endcond */
#endif

