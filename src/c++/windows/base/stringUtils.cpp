/*
 * Copyright (C) 2007 Funambol, Inc.
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

#include <algorithm>
#include "base/Log.h"
#include "base/util/utils.h"
#include "base/stringUtils.h"
#include "spds/DataTransformerFactory.h"
#include "spds/B64Encoder.h"
#include "spds/B64Decoder.h"
#include "spds/DESEncoder.h"
#include "spds/DESDecoder.h"

using namespace std;



/**
 * Converts a string to lower-case.
 * String passed is modified.
 * @param s   [IN-OUT] the string to be converted
 */
void toLowerCase(string& s) {
    transform(s.begin(), s.end(), s.begin(), tolower);
}

/**
 * Converts a string to lower-case (wstring version).
 * String passed is modified.
 * @param s   [IN-OUT] the string to be converted
 */
void toLowerCase(wstring& s) {
    transform(s.begin(), s.end(), s.begin(), tolower);
}


/**
 * Replace all strings 'source' with 'dest' inside string 'dataString'.
 * @param source       the token to search
 * @param dest         the token to replace
 * @param dataString   [IN-OUT] the string to work on
 * @param startPos     [OPTIONAL] tha start position (default = 0)
 */
void replaceAll(const wstring& source, const wstring& dest, wstring& dataString, const int startPos) {

    if (startPos >= (int)dataString.length()) {
        return;
    }
    wstring::size_type pos = startPos;
    pos = dataString.find(source, pos);

    while (pos != wstring::npos) {
        dataString.replace(pos, source.length(), dest);
        pos = dataString.find(source, pos+1);
    }
}





/**
 * Gets the content of <tag>...</tag> inside the string 'xml'.
 * Returns 0 if tag found, 1 if not found.
 *
 * @param xml     : the input string to search inside
 * @param tag     : the tag we are looking for
 * @param content : [OUT] the content of tag (string passed by ref)
 * @param pos     : (optional) the starting position to search inside 'xml' (default = 0)
 * @return          0 if tag found (even if empty content), 1 if errors
 */
int getElementContent(const wstring& xml, const wstring& tag, wstring& content, unsigned int pos) {

    wstring openTag, closeTag, emptyTag;
    wstring::size_type start, end;
    content = L"";

    //
    // First try to find the empty element <tag/>
    //
    emptyTag  = L"<";
    emptyTag += tag;
    emptyTag += L"/>";

    start = xml.find(emptyTag, pos); 
    if (start != wstring::npos) {
        return 0;  // found!
    }

    //
    // Now search for the <tag>Content</tag>
    //
    openTag   = L"<";
    openTag  += tag;
    openTag  += L">";

    closeTag  = L"</";
    closeTag += tag;
    closeTag += L">";

    start = xml.find(openTag, pos); 
    if (start != wstring::npos) {
        start += openTag.length();

        end = xml.find(closeTag, start);
        if (end == wstring::npos) {
            goto not_found;
        }

        content = xml.substr(start, end-start);
    }
    
    else {
        goto not_found;
    }

    return 0;

not_found:

    return 1;
}



/**
 * Gets the content of <tag>...</tag> inside the string 'xml'.
 * Returns 0 if tag found, 1 if not found.
 *
 * @param xml     : the input string to search inside
 * @param tag     : the tag we are looking for
 * @param content : [OUT] the content of tag (string passed by ref)
 * @param pos     : the starting position to search inside 'xml'
 * @param start   : [OUT] the starting position of content found ('wstring::npos' if not found)
 * @param end     : [OUT] the ending position of content found ('wstring::npos' if not found)
 * @return          0 if tag found (even if empty content), 1 if errors
 * @overload        getElementContent(const wstring& xml, const wstring& tag, wstring& content, unsigned int pos)
 */
int getElementContent(const wstring& xml, const wstring& tag, wstring& content, const wstring::size_type pos, wstring::size_type& start, wstring::size_type& end) {

    wstring openTag, closeTag, emptyTag;
    start   = wstring::npos;
    end     = wstring::npos;
    content = L"";

    //
    // First try to find the empty element <tag/>
    //
    emptyTag  = L"<";
    emptyTag += tag;
    emptyTag += L"/>";

    start = xml.find(emptyTag, pos); 
    if (start != wstring::npos) {
        start += emptyTag.length();
        end = start;
        return 0;  // found!
    }

    //
    // Now search for the <tag>Content</tag>
    //
    openTag   = L"<";
    openTag  += tag;
    openTag  += L">";

    closeTag  = L"</";
    closeTag += tag;
    closeTag += L">";

    start = xml.find(openTag, pos); 
    if (start != wstring::npos) {
        start += openTag.length();

        end = xml.find(closeTag, start);
        if (end == wstring::npos) {
            goto not_found;
        }

        content = xml.substr(start, end-start);
    }
    
    else {
        goto not_found;
    }

    return 0;

not_found:

    return 1;
}




/**
 * Used to encrypt private data (username/password) using 'B64(DES(data))'.
 * @param data   the data to encrypt
 * @return       the data encrypted (new buffer: must be deleted by caller)
 */
char* encryptData(const char* data) {
    
    if (!data) return NULL;
    char*   desData = NULL;
    char*   b64Data = NULL;
    char* data1 = (char*)data;

    //
    // 1. DES encryption
    //
    DataTransformer* dt = new DESEncoder();
    TransformationInfo info;
    if (dt == NULL) {
        goto error;
    }
    info.size = strlen(data1);
    info.password = "SettimioSevero";
    desData = dt->transform(data1, info);
    if (!desData || info.newReturnedData == FALSE) {
        goto error;
    }
    if (dt) delete dt;

    //
    // 2. B64 encoding
    //
    dt = new B64Encoder();
    // info.size is already correct :)
    info.password = NULL;
    b64Data = dt->transform(desData, info);
    goto exit;


error:
    LOG.error(ERR_ENCRYPT_DATA);
    b64Data = stringdup(data);

exit:
    if (desData) delete [] desData;
    if (dt)      delete dt;
    return b64Data;
}



/**
 * Used to decrypt private data (username/password) using 'dec_DES(dec_B64(b64Data))'.
 * @param b64Data   the data to decrypt
 * @return          the data decrypted (new buffer: must be deleted by caller)
 */
char* decryptData(const char* b64Data) {
    
    if (!b64Data) return NULL;
    char* desData = NULL;
    char* data = NULL;

    // MUST copy, this buffer will be modified and 
    // it's the one returned from this function.
    char* b64Data1 = stringdup(b64Data);

    //
    // 1. B64 decoding
    //
    DataTransformer* dt = new B64Decoder();
    TransformationInfo info;
    if (dt == NULL) {
        goto error;
    }
    info.size = strlen(b64Data1);
    info.password = NULL;
    desData = dt->transform(b64Data1, info);
    if (dt) delete dt;


    //
    // 2. DES decryption
    //
    dt = new DESDecoder();
    // info.size is already correct :)
    info.password = "SettimioSevero";
    data = dt->transform(desData, info);
    if (!data) {
        goto error;
    }
    data[info.size] = 0;
    goto exit;

error:
    LOG.error(ERR_DECRYPT_DATA);
    data = stringdup("");

exit:
    // NO! buffer used is the same passed to both decoder!!
    // if (desData) delete [] desData;
    // if (b64Data1) delete [] b64Data1;
    if (dt)       delete dt;
    return data;
}



