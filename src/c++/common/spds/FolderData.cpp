/*
 * Copyright (C) 2003-2007 Funambol, Inc.
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

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/XMLProcessor.h"
#include "syncml/formatter/Formatter.h"
#include "spds/FolderData.h"
#include "base/quoted-printable.h"


#define FOLDER_ITEM       TEXT("Folder")
#define FOLDER_HIDDEN     "h"
#define FOLDER_SYSTEM     "s"
#define FOLDER_ARCHIVED   "a"
#define FOLDER_DELETE     "d"
#define FOLDER_WRITABLE   "w"
#define FOLDER_READABLE   "r"
#define FOLDER_EXECUTABLE "e"
#define FOLDER_ACCESSED   "accessed"
#define FOLDER_ATTRIBUTES "attributes"
#define FOLDER_ROLE       "role"
#define FOLDER_MODIFIED   "modified"
#define FOLDER_NAME       "name"
#define FOLDER_CREATED    "created"
#define FOLDER_EXT    "ext"
#define FOLDER_XNAM    "XNam"
#define FOLDER_XVAL    "XVal"


FolderData::FolderData()
{
    hidden = false;
	system = false;
	archived = false;
	deleted = false;
	writable = false;
	readable = false;
	executable = false;
    isHiddenPresent = false;
    isSystemPresent = false;
    isArchivedPresent = false;
    isDeletedPresent = false;
    isWritablePresent = false;
    isReadablePresent = false;
    isExecutablePresent = false;

}

FolderData::~FolderData()
{
    accessed.reset();
    attributes.reset();
    //folder.reset();
    modified.reset();
    name.reset();
    created.reset();
    role.reset();

}
int FolderData::parse(const char *syncmlData, size_t len)
{
    int ret = 0;
    unsigned int start, end;
    StringBuffer msg(syncmlData, len);

    msg.replaceAll("&lt;", "<");
    msg.replaceAll("&amp;", "&");

    // Get attributes
    if( XMLProcessor::getElementContent (msg, FOLDER_HIDDEN, NULL, &start, &end) ) {
        hidden = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isHiddenPresent = true;
    }
    else hidden = false;

    if( XMLProcessor::getElementContent (msg, FOLDER_SYSTEM, NULL, &start, &end) ) {
        system = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isSystemPresent = true;
    }
    else system = false;

    if( XMLProcessor::getElementContent (msg, FOLDER_ARCHIVED, NULL, &start, &end) ) {
        archived = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isArchivedPresent = true;
    }
    else archived = false;

    if( XMLProcessor::getElementContent (msg, FOLDER_DELETE, NULL, &start, &end) ) {
        deleted = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isDeletedPresent = true;
    }
    else deleted = false;

    if( XMLProcessor::getElementContent (msg, FOLDER_WRITABLE, NULL, &start, &end) ) {
        writable = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isWritablePresent = true;
    }
    else writable = false;

    if( XMLProcessor::getElementContent (msg, FOLDER_READABLE, NULL, &start, &end) ) {
        readable = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isReadablePresent = true;
    }
    else readable = false;

    if( XMLProcessor::getElementContent (msg, FOLDER_EXECUTABLE, NULL, &start, &end) ) {
        executable = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isExecutablePresent = true;
    }
    else executable = false;

    if( XMLProcessor::getElementContent (msg, FOLDER_ACCESSED, NULL, &start, &end) ) {
        accessed = msg.substr(start, end-start);
    }
    else accessed = TEXT("");

    if( XMLProcessor::getElementContent (msg, FOLDER_MODIFIED, NULL, &start, &end) ) {
        modified = msg.substr(start, end-start);
    }
    else modified = TEXT("");

    if( XMLProcessor::getElementContent (msg, FOLDER_CREATED, NULL, &start, &end) ) {
        created = msg.substr(start, end-start);
    }
    else created = TEXT("");

    if( XMLProcessor::getElementContent (msg, FOLDER_ROLE, NULL, &start, &end) ) {
        role = msg.substr(start, end-start);
    }
    else role = TEXT("");


    if( XMLProcessor::getElementContent (msg, FOLDER_NAME, NULL, &start, &end) ) {
        name = msg.substr(start, end-start);
    }
    else{
        name = TEXT("");
        ret = -1;
    }

    if( XMLProcessor::getElementContent (msg, FOLDER_EXT, NULL, &start, &end) ) {
        ;
        // In this version the extention fields are not suported
    }

    return ret;
}


char* FolderData::format() {

    StringBuffer out;

    out.reserve(150);

    out = "<Folder>\n";
    if (name.length() > 0)
        out += XMLProcessor::makeElement(FOLDER_NAME, _wcc(name));
    if (created.length() > 0)
        out += XMLProcessor::makeElement(FOLDER_CREATED, _wcc(created));
    if (modified.length() > 0)
        out += XMLProcessor::makeElement(FOLDER_MODIFIED, _wcc(modified));
    if (accessed.length() > 0)
        out += XMLProcessor::makeElement(FOLDER_ACCESSED, _wcc(accessed));

    StringBuffer attributes;

    if (isHiddenPresent)
        attributes += XMLProcessor::makeElement(FOLDER_HIDDEN, hidden);
    if (isSystemPresent)
        attributes += XMLProcessor::makeElement(FOLDER_SYSTEM, system);
    if (isArchivedPresent)
        attributes += XMLProcessor::makeElement(FOLDER_ARCHIVED, archived);
    if (isDeletedPresent)
        attributes += XMLProcessor::makeElement(FOLDER_DELETE, deleted);
    if (isWritablePresent)
        attributes += XMLProcessor::makeElement(FOLDER_WRITABLE, writable);
    if (isReadablePresent)
        attributes += XMLProcessor::makeElement(FOLDER_READABLE, readable);
    if (isExecutablePresent)
        attributes += XMLProcessor::makeElement(FOLDER_EXECUTABLE, executable);

    if (!attributes.empty())
        out += XMLProcessor::makeElement(FOLDER_ATTRIBUTES, attributes);

    if (role.length() > 0)
        out += XMLProcessor::makeElement(FOLDER_ROLE, _wcc(role));

    out += "</Folder>\n";
    return stringdup(out.c_str());
}

int FolderData::lengthForB64(int len) {

    int modules = 0;
    int ret     = 0;

    modules = len % 3;
    if (modules == 0) {
        ret = 4 * (len / 3);

    } else {
        ret = 4 * ((len/3) + 1);

    }
    return ret;


}
