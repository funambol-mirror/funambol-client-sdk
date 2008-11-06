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
#include "base/util/utils.h"

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
    sprintf(ret, "%i", i);
    return ret;
}


int round(double val) {
    int v = (int)val;
    return ((val - v) > 0.5) ? v+1 : v;
}



/*
* It implements algo for authentication with MD5 method.
* It computes digest token according with follow:
* Let H   : MD5 Function represents by calculateMD5 method
* Let B64 : Base64 encoding Function represents by encodeBase64 method
* Data: H (B64(H(username:password)):nonce)
*/

char* MD5CredentialData(char* userName, char* password, char* nonce) {

    int len = 0, lenNonce = 0, totLen = 0;

    char cnonce      [64];
    char digest      [16];
    char base64      [64];
    char base64Nonce [64];
    char token      [512];
    char* md5Digest = NULL;
    char ch          [3];
    char* dig = NULL;

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
