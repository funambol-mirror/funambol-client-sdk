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
#include "spds/FileData.h"
#include "base/quoted-printable.h"


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


FileData::FileData()
{
    size = 0;
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

FileData::~FileData()
{
    accessed.reset();
    attributes.reset();
    enc.reset();
    file.reset();
    modified.reset();
    name.reset();
    created.reset();
    body.reset();
    cttype.reset();

}

int FileData::parse(const void *syncmlData, size_t len) {
    StringBuffer* s = new StringBuffer(syncmlData, len);
    return parse(s);


}

int FileData::parse(const char *syncmlData, size_t len) {
    StringBuffer* s = new StringBuffer(syncmlData, len);
    return parse(s);

}

int FileData::parse(StringBuffer* s)
{
    int ret = 0;
    unsigned int start, end;
    //StringBuffer* s = new StringBuffer(syncmlData, len);
    StringBuffer bodyattr;

    // FIXME: remove these replace once the server has fixed the message.
    s->replaceAll("&lt;", "<");
    s->replaceAll("&gt;", ">");
    s->replaceAll("&amp;", "&");

	/*
    // Get the CDATA content
    if(XMLProcessor::getElementContent(s->c_str(), "CDATA", NULL, &start, &end) == 0) {
        LOG.error("FileData: can't find outer CDATA section.");
        return -1;
    }
    StringBuffer msg = s->substr(start, end-start);

    delete s;
	*/

	if(XMLProcessor::getElementContent(s->c_str(), "File", NULL, &start, &end) == 0) {
		LOG.error("FileData: can't find outer FILE section.");
		return -1;
	}
	StringBuffer msg = s->substr(start, end-start);

    // Get attributes
    if( XMLProcessor::getElementContent (msg, FILE_HIDDEN, NULL, &start, &end) ) {
        hidden = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isHiddenPresent = true;
    }
    else hidden = false;

    if( XMLProcessor::getElementContent (msg, FILE_SYSTEM, NULL, &start, &end) ) {
        system = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isSystemPresent = true;
    }
    else system = false;

    if( XMLProcessor::getElementContent (msg, FILE_ARCHIVED, NULL, &start, &end) ) {
        archived = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isArchivedPresent = true;
    }
    else archived = false;

    if( XMLProcessor::getElementContent (msg, FILE_DELETE, NULL, &start, &end) ) {
        deleted = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isDeletedPresent = true;
    }
    else deleted = false;

    if( XMLProcessor::getElementContent (msg, FILE_WRITABLE, NULL, &start, &end) ) {
        writable = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isWritablePresent = true;
    }
    else writable = false;

    if( XMLProcessor::getElementContent (msg, FILE_READABLE, NULL, &start, &end) ) {
        readable = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isReadablePresent = true;
    }
    else readable = false;

    if( XMLProcessor::getElementContent (msg, FILE_EXECUTABLE, NULL, &start, &end) ) {
        executable = ( strncmp(msg.c_str()+start, "true", end-start) == 0 ) ;
        isExecutablePresent = true;
    }
    else executable = false;

    if( XMLProcessor::getElementContent (msg, FILE_ACCESSED, NULL, &start, &end) ) {
        accessed = msg.substr(start, end-start);
    }
    else accessed = TEXT("");

    if( XMLProcessor::getElementContent (msg, FILE_MODIFIED, NULL, &start, &end) ) {
        modified = msg.substr(start, end-start);
    }
    else modified = TEXT("");

    if( XMLProcessor::getElementContent (msg, FILE_CREATED, NULL, &start, &end) ) {
        created = msg.substr(start, end-start);
    }
    else created = TEXT("");

    if ( XMLProcessor::getElementContent (msg, FILE_SIZE, NULL, &start, &end) ) {
        size = atoi(msg.substr(start, end-start));
    }

    if( XMLProcessor::getElementContent (msg, FILE_BODY, NULL, &start, &end) ) {
        body = msg.substr(start, end-start);
    }
    else body = "";
    if ( XMLProcessor::getElementAttributes (msg, FILE_BODY, &start, &end) ) {
        bodyattr = msg.substr(start, end-start);
        size_t attrstart = bodyattr.ifind("enc");
        if (attrstart!= StringBuffer::npos) {
            enc = bodyattr.substr(attrstart + 4);
            if (!enc.empty() && (enc != TEXT("\"base64\"")) &&
                (enc != TEXT("\"quoted-printable\"")))
            {
                enc = TEXT("");
            }
            else
            {
                int repNo = enc.replaceAll(TEXT("\""),TEXT(""));
            }
        }
        else
            enc = TEXT("");
    }
    else
        enc = TEXT("");

    if (!enc.empty() && (enc == TEXT("base64")))
    {
        int len = b64_decode((void *)body.c_str(), body.c_str());
    }

    if (!enc.empty() && (enc == TEXT("quoted-printable")))
    {
        body = qp_decode(body.c_str());
    }


    if( XMLProcessor::getElementContent (msg, FILE_NAME, NULL, &start, &end) ) {
        name = msg.substr(start, end-start);
    }
    else name = TEXT("");

	delete s;

    return ret;
}

void FileData::setBody(const char* v, int len)
{
    if (size == 0)
    {
        body = v;
    }
    else
    {
        char*   base64    = NULL;
        int     encodeLen = lengthForB64(len);
        base64 = new char[encodeLen + 1];
        memset(base64, 0, encodeLen + 1);
        b64_encode(base64, (char*)v, len);
        body = base64;
        delete [] base64;
    }
}


char* FileData::format() {

    StringBuffer out;

    out.reserve(150);

    out = "<File>\n";
    if (name.length() > 0)
        out += XMLProcessor::makeElement(FILE_NAME, _wcc(name));
    if (created.length() > 0)
        out += XMLProcessor::makeElement(FILE_CREATED, _wcc(created));
    if (modified.length() > 0)
        out += XMLProcessor::makeElement(FILE_MODIFIED, _wcc(modified));
    if (accessed.length() > 0)
        out += XMLProcessor::makeElement(FILE_ACCESSED, _wcc(accessed));

    StringBuffer attributes;

    if (isHiddenPresent)
        attributes += XMLProcessor::makeElement(FILE_HIDDEN, hidden);
    if (isSystemPresent)
        attributes += XMLProcessor::makeElement(FILE_SYSTEM, system);
    if (isArchivedPresent)
        attributes += XMLProcessor::makeElement(FILE_ARCHIVED, archived);
    if (isDeletedPresent)
        attributes += XMLProcessor::makeElement(FILE_DELETE, deleted);
    if (isWritablePresent)
        attributes += XMLProcessor::makeElement(FILE_WRITABLE, writable);
    if (isReadablePresent)
        attributes += XMLProcessor::makeElement(FILE_READABLE, readable);
    if (isExecutablePresent)
        attributes += XMLProcessor::makeElement(FILE_EXECUTABLE, executable);
    if (!attributes.empty())
        out += XMLProcessor::makeElement(FILE_ATTRIBUTES, attributes);

    if (enc.empty()){
        int len = b64_decode((void*)body.c_str(), body.c_str());
        out += XMLProcessor::makeElement(FILE_BODY, body);
    }
    else
    {
        ArrayList attrList;
        KeyValuePair attr("enc", _wcc(enc.c_str()));
        attrList.add(attr);
        out += XMLProcessor::makeElement(FILE_BODY, body.c_str(), attrList);
    }
    if (size > 0)
        out += XMLProcessor::makeElement(FILE_SIZE, size);
    out += "</File>\n";
    return stringdup(out.c_str());
}

int FileData::lengthForB64(int len) {

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
