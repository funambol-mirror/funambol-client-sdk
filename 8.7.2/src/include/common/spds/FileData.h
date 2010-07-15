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


#ifndef INCL_FILEDATA
#define INCL_FILEDATA
/** @cond DEV */

#include "base/util/ArrayElement.h"
#include "base/util/WString.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

#define FILE_ITEM       TEXT("File")
#define FILE_HIDDEN     "h"
#define FILE_SYSTEM     "s"
#define FILE_ARCHIVED   "a"
#define FILE_DELETE     "d"
#define FILE_WRITABLE   "w"
#define FILE_READABLE   "r"
#define FILE_EXECUTABLE "e"
#define FILE_ACCESSED   "accessed"
#define FILE_ATTRIBUTES "attributes"
#define FILE_BODY       "body"
#define FILE_CTTYTPE    "cttype"
#define FILE_ENC        "enc"
#define FILE_MODIFIED   "modified"
#define FILE_NAME       "name"
#define FILE_SIZE       "size"
#define FILE_CREATED    "created"

BEGIN_NAMESPACE

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


END_NAMESPACE

/** @endcond */
#endif

