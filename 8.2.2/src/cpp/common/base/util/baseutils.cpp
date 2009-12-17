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

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/*
 * Deletes the given char[] buffer if it is not NULL
 * and sets the pointer to NULL
 *
 */
void safeDelete(char* p[]) {
    if (*p) {
        delete [] *p; *p = NULL;
    }
}

void safeDel(char** p) {
    if (*p) {
        delete [] *p; *p = NULL;
    }
}

char* stringdup(const char* s, size_t len)
{
    if ( !s )
        return NULL;

    int l = (len==STRINGDUP_NOLEN)?strlen(s):len;

    char* news = new char[l+1];

    strncpy(news, s, l);
    news[l]=0;

    return news;
}

WCHAR* wstrdup(const WCHAR* s, size_t len)
{
    if ( !s )
        return NULL;

    int l = (len==STRINGDUP_NOLEN)?wcslen(s):len;

    WCHAR* news = new WCHAR[l+1];

    wcsncpy(news, s, l);
    news[l]=0;

    return news;
}

char* strtolower(const char *s)
{
    char* l = NULL;
    char* p = NULL;

    for(l = p = stringdup(s); *p; ++p) {
        *p=tolower(*p);
    }
    return l;
}

char* strtoupper(const char *s)
{
    char* u = NULL;
    char* p = NULL;

    for(u = p = stringdup(s); *p; ++p) {
        *p=toupper(*p);
    }
    return u;
}


WCHAR* wcstolower(const WCHAR *s)
{
    WCHAR* l = NULL;
    WCHAR* p = NULL;

    for(l = p = wstrdup(s); *p; ++p) {
        *p=towlower(*p);
    }

    return l;
}

WCHAR* wcstoupper(const WCHAR *s)
{
    WCHAR* u = NULL;
    WCHAR* p = NULL;

    for(u = p = wstrdup(s); *p; ++p) {
        *p=towupper(*p);
    }

    return u;
}

/**
 * find a substring from the end, with optional string lenght
 */
const char *brfind(const char *s1, const char *s2, size_t len)
{
    const char *sc1, *sc2, *ps1;

    if (!s1)
        return NULL;

    if (*s2 == '\0')
        return s1;

    if(len < strlen(s1)){
        ps1 = s1 + len;
    }
    else {
        ps1 = s1 + strlen(s1);
    }

    while(ps1 > s1) {
        --ps1;
        for (sc1 = ps1, sc2 = s2; *sc1 != *sc2; sc1++, sc2++) {
            if (*sc2 == '\0')
                return (ps1);
        }
    }
    return NULL;
}


void timestampToAnchor(unsigned long timestamp, char anchor[21]) {
    sprintf(anchor, "%lu", timestamp);
}

unsigned long anchorToTimestamp(const char* anchor) {
    unsigned long timestamp;

    return sscanf(anchor, "%lu", &timestamp) == 1 ? timestamp : 0;
}

bool wcscmpIgnoreCase(const char* p, const char* q) {

    bool ret = false;
    if (p == NULL || q == NULL)
        return ret;

    unsigned int lenp = 0, lenq = 0;
    lenp = strlen(p);
    lenq = strlen(q);

    if (lenp != lenq) {
        return ret;
    }

    for (unsigned int i = 0; i < lenp; i++) {
        if ( towlower(p[i]) != towlower(q[i]))
            return ret;
    }
    ret = true;
    return ret;
}


char* itow(int i) {
    char* ret = new char[10];
    memset(ret, 0, 10*sizeof(char) );
    sprintf(ret, "%i", i);
    return ret;
}

char* ltow(long i) {
    char* ret = new char[20];
    memset(ret, 0, 20*sizeof(char));
    sprintf(ret, "%ld", i);
    return ret;
}


/*
* It implements algo for authentication with MD5 method.
* It computes digest token according with follow:
* Let H   : MD5 Function represents by calculateMD5 method
* Let B64 : Base64 encoding Function represents by encodeBase64 method
* Data: H (B64(H(username:password)):nonce)
*/

char* MD5CredentialData(const char* userName, const char* password, const char* nonce) {

    int len = 0, lenNonce = 0, totLen = 0;

    char cnonce      [64];
    char digest      [16];
    char base64      [64];
    char base64Nonce [64];
    char token      [512];
    char* md5Digest = NULL;
    char ch          [3];

    memset(digest,      0, 16);
    memset(base64,      0, 64);
    memset(base64Nonce, 0, 64);
    memset(cnonce,      0, 64);
    memset(token,       0, 512);
    sprintf(ch, ":");

    sprintf(token, "%s:%s", userName, password);
    len = strlen(token);

    // H(username:password)
    calculateMD5((void*)token, len, digest);

    // B64(H(username:password))
    len = b64_encode((char*)base64, digest, 16);


    // decode nonce from stored base64 to bin
    strcpy(cnonce, nonce);
    lenNonce = b64_decode(cnonce, cnonce);

    memcpy(base64Nonce, base64, len);
    memcpy(&base64Nonce[len], ch, 1);
    memcpy(&base64Nonce[len+1], cnonce, lenNonce);

    totLen = len + 1 + lenNonce;

    memset(digest, 0, 16);
    calculateMD5(base64Nonce, totLen, digest);
    b64_encode(base64, digest, 16);

    // return new value
    md5Digest = stringdup(base64);
    return md5Digest;
}



char* calculateMD5(const void* token, int len, char* wdigest) {

    //algo for md5 digest
    char dig [18];
    md5_state_t state;
    md5_byte_t digest[16];
    int di;
    char* ret = NULL;

    md5_init  (&state);
    md5_append(&state, (const md5_byte_t *)token, len);
    md5_finish(&state, digest);
    for (di = 0; di < 16; ++di) {
        sprintf(dig + di, "%c", digest[di]);
    }
    if (wdigest == NULL) {
        ret = new char[16];
        memcpy(ret, dig, 16);
        return ret;
    } else {
        memcpy(wdigest, dig, 16);
        return NULL;
    }
}


size_t fgetsize(FILE *f)
{
    size_t size;

    fseek(f, 0, SEEK_END);
    size=ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

size_t fgetsize(const char* fileName) {
    if (fileName) {
        FILE* f = fopen(fileName, "rb");
        if (f) {
            size_t size = fgetsize(f);
            fclose(f);
            return size;
        }
    }
    return 0;
}

//Returns the file name, given its full (absolute path) name.
StringBuffer getFileNameFromPath(const StringBuffer& fullName) {
    
    StringBuffer fileName("");
    
    unsigned long pos = fullName.rfind("/");
    if (pos == StringBuffer::npos) {
        pos = fullName.rfind("\\");
        if (pos == StringBuffer::npos) {
            // fullName is already the file name
            return fullName;
        }
    }
    // Move to the first char of the filename
    pos += 1;
    
    fileName = fullName.substr(pos, fullName.length() - pos);
    return fileName;
}


END_NAMESPACE

