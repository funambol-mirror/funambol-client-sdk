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

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "base/messages.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"


USE_NAMESPACE

const size_t StringBuffer::npos = 0xFFFFFFFF;


StringBuffer::StringBuffer(const char* str, size_t len) {
    size = 0;
    s = 0;

    // if the given string is null, leave this null,
    // otherwise set it, even empty.
    if (str) {
        size_t slen = strlen(str);
        size_t newlen = (len > slen) ? slen : len ;

        if(newlen) {
            getmem(newlen);
            strncpy(s, str, newlen);
            s[newlen]=0;
        }
        else {  // empty string
            getmem(1);
            s[0] = 0;
        }
    }
}

//Copy Costructor
StringBuffer::StringBuffer(const StringBuffer &sb) {
    size = 0;
    s = NULL;
    append(sb.c_str());
}

StringBuffer::~StringBuffer() {
    freemem();
}

StringBuffer& StringBuffer::append(const char* sNew) {
    if (sNew == NULL) {
        return *this;
    }

    unsigned long len = strlen(sNew);

    if (len == 0) {
        return *this;
    }
    if (s) {
        getmem(strlen(s) + len);
        strcat(s, sNew);
    }
    else {
        getmem(len);
        strcpy(s, sNew);
    }

    return *this;
}

StringBuffer& StringBuffer::append(unsigned long i, bool sign) {
    append(StringBuffer().sprintf(sign ? "%ld" : "%lu", i));

    return *this;
}

/* TODO:

StringBuffer& StringBuffer::append(unsigned long i) {
    append(StringBuffer().sprintf("%lu", i));

    return *this;
}

StringBuffer& StringBuffer::append(long i) {
    append(StringBuffer().sprintf("%ld", i));

    return *this;
}

*/

StringBuffer& StringBuffer::append(StringBuffer& str) {
    return append(str.c_str());
}

StringBuffer& StringBuffer::append(StringBuffer* str) {
    return (str) ? append(str->c_str()) : *this;
}

StringBuffer& StringBuffer::assign(const char* sNew) {
    if (sNew) {
        size_t len = strlen(sNew);
        if ( len ) {
            getmem( len );
            strcpy(s, sNew);
        }
        else if (s) {
            s[0]=0; // just make the string empty
        } else {
            getmem( 2 );
            strcpy(s, "");
        }
    }
    else {
        freemem();  // release the string and set it to null
    }

    return *this;
}

/////////////////////////////////////////////////////////////////////////////
// convert.
/////////////////////////////////////////////////////////////////////////////
StringBuffer& StringBuffer::convert(const WCHAR* wc, const char *encoding) {
    
    if(sizeof(WCHAR) != sizeof(char)) {
        char *tmp = toMultibyte(wc, encoding);
        assign(tmp);
        delete [] tmp;
    }
    else {
        assign((const char *)wc);
    }
    return *this;
}

// sprintf ----------------------------
StringBuffer& StringBuffer::sprintf(const char* format, ...) {
    PLATFORM_VA_LIST ap;

    PLATFORM_VA_START(ap, format);
    this->vsprintf(format, ap);
    PLATFORM_VA_END(ap);

    return *this;
}


StringBuffer& StringBuffer::vsprintf(const char* format, PLATFORM_VA_LIST ap) {
    PLATFORM_VA_LIST aq;

    // ensure minimal size for first iteration
    int realsize = 255;

    do {
        // make a copy to keep ap valid for further iterations
#ifdef va_copy
        PLATFORM_VA_COPY(aq, ap);
#else
        aq = ap;
#endif

        if (size < (unsigned long)realsize) {
            s = (char*)realloc(s, (realsize + 1) * sizeof(char));
            if (s == NULL) {
                // Out of memory. Flush the string content and return
                size = 0;
                return *this;
            }
            size = realsize;
        }

        realsize = vsnprintf(s, size + 1, format, aq);
        if (realsize == -1) {
            // old-style vnsprintf: exact len unknown, try again with doubled size
            realsize = size * 2;
        }
        PLATFORM_VA_END(aq);
    } while((unsigned long)realsize > size);

    // free extra memory
    s = (char*)realloc(s, (realsize + 1) * sizeof(char));
    size = realsize;

    return *this;
}



unsigned long StringBuffer::length() const {
    return (s) ? strlen(s) : 0;
}

// Watch out
StringBuffer& StringBuffer::reset() {
    freemem();
    return *this;
}

size_t StringBuffer::find(const char *str, size_t pos) const
{
    if (pos >= length()) {
        return npos;
    }
    char *p = strstr(s+pos, str);
    if(!p)
        return npos;
    return (p-s);
}

size_t StringBuffer::ifind(const char *str, size_t pos) const
{
    if (pos >= length())
        return npos;

    char *ls = strtolower(s+pos);
    char *lstr = strtolower(str);

    char *p = strstr(ls, lstr);

    size_t ret = (p) ? p-ls : npos;

    delete [] ls;
    delete [] lstr;

    return ret;
}

size_t StringBuffer::rfind(const char *str, size_t pos) const
{
    if (pos >= length())
        return npos;

    size_t tmp = StringBuffer::npos;
    int previous = pos;

    tmp = find(str, pos);
    if (tmp == StringBuffer::npos)
        return npos;

    while(1) {

        previous = tmp;
        tmp = find(str, tmp + 1);
        if (tmp == StringBuffer::npos)
            break;
    }

    return previous;

}


size_t StringBuffer::replace(const char *from, const char *to, size_t pos)
{
    size_t ret = npos;

    if (!s)
        return npos;

    if(pos>=length())
        return npos;

    char *p = strstr(s+pos, from);
    if (p) {
        size_t fpos = p - s;
        size_t flen = strlen(from), tlen = strlen(to);
        char *tail = 0;
        int ldiff = tlen - flen ;

        // reallocate if needed
        getmem(length() + ldiff);
        p = s + fpos;            // ensure that p is valid again
        // check is there is a remainder after the replaced token
        if( p[flen] ) {
            tail = new char[length()];
            strcpy(tail, p+flen);
        }
        // copy to in place of from
        strcpy(p, to);
        // copy the remainder of old string, if there is one
        if( tail ) {
            strcpy(p+tlen, tail);
            delete [] tail;
        }
        ret = p - s;
    }
    return ret;
}

// TODO: implement some smarter argorithm to avoid multiple reallocations
int StringBuffer::replaceAll(const char *from, const char *to, size_t pos) {
    int i=0;
    int len = strlen(to);
    size_t next;

    for(next=replace(from, to, pos); next != npos; next=replace(from,to,next + len) ) {
        i++;
    }
    return i;
}

ArrayList& StringBuffer::split(ArrayList &tokens, const char *separator) const {
    tokens.clear();
    size_t seplen = strlen(separator);
    char *base = s;
    char *p = strstr( base, separator );

    while( p )
    {
        StringBuffer token(base, p-base);
        tokens.add( token );
        base = p + seplen;
        p = strstr( base, separator );
    }
    StringBuffer token(base);
    tokens.add( token );

    return tokens;
}

StringBuffer& StringBuffer::join(ArrayList &tokens, const char *separator) {
    StringBuffer *line;
    size_t totlen = 0, seplen = strlen(separator);
    // Calc total size
    for (line=(StringBuffer *)tokens.front();
         line;
         line=(StringBuffer *)tokens.next() ) {
        totlen += line->length() + seplen;
    }
    // Reserve mem
    reserve(totlen);
    // Join strings
    for (line=(StringBuffer *)tokens.front();
         line;
         line=(StringBuffer *)tokens.next() ) {
        this->append(line->c_str());
        this->append(separator);
    }
    return *this;
}

StringBuffer StringBuffer::substr(size_t pos, size_t len) const {
    if(pos > strlen(s))
        return StringBuffer("");

    return (StringBuffer(s+pos, len));
}

void StringBuffer::reserve(size_t len) {
    getmem(len);
}

StringBuffer& StringBuffer::upperCase() {
    char* p = NULL;

    for(p = s; *p; p++) {
        *p=toupper(*p);
    }

    return *this;
}

StringBuffer& StringBuffer::lowerCase() {
    char* p = NULL;

    for(p = s; *p; p++) {
        *p=tolower(*p);
    }

    return *this;
}

/**
 * Perform case insensitive compare.
 * @return true if equals
 */
bool StringBuffer::icmp(const char *sc) const {
    return wcscmpIgnoreCase(s, sc);
}

ArrayElement* StringBuffer::clone() {
    return new StringBuffer(s);
}

bool StringBuffer::empty() const {
    if(!s)
        return true;
    if(!*s)
        return true;
    return false;
}

bool StringBuffer::null() const {
    return (s==0);
}

bool StringBuffer::endsWith(char ch) const {
    if (empty()) {
        return false;
    }

    if (s[length()-1] == ch) {
        return true;
    } else {
        return false;
    }
}

bool StringBuffer::endsWith(const char* str) const {
    if (empty() || str == NULL || *str == (char)0) {
        return false;
    }
    size_t strLength = strlen(str);
    if (strLength > length()) {
        return false;
    }

    char* start = (s + length()) - strLength;
    return !strcmp(start, str);
}


// Member Operators
StringBuffer& StringBuffer::operator= (const char* sc)
    { return assign(sc); }
StringBuffer& StringBuffer::operator= (const StringBuffer& sb)
    { return assign(sb); }
StringBuffer& StringBuffer::operator+= (const char* sc)
    { append(sc); return *this; }
StringBuffer& StringBuffer::operator+= (const StringBuffer& s)
    { append(s); return *this; }
bool  StringBuffer::operator== (const char* sc) const
{
    if(!s)
        return (sc) ? false : true ;
    if (!sc)
        return false;

    return strcmp(s, sc) == 0;
}
bool  StringBuffer::operator== (const StringBuffer& sb) const
{
    return *this == sb.c_str();
}

bool  StringBuffer::operator!= (const char* sc) const
{
    return !(*this == sc);
}
bool  StringBuffer::operator!= (const StringBuffer& s) const
{
    return !(*this == s.c_str());
}


BEGIN_NAMESPACE

// Function operators
StringBuffer operator+(const StringBuffer& x, const char *y)
{
  StringBuffer result(x);
  result.append(y);
  return result;
}

END_NAMESPACE


//size_t StringBuffer_memcount = 0;

// Private allocator
void StringBuffer::getmem(size_t len)
{
    //fprintf(stderr, "GETMEM len=%ld s=%lx size=%ld\n", len, s, size);
    if(len > size) {
        // Remember the old length (0 for the null string)
        size_t oldlen = length();
        // Realloc the string (like malloc when s is null)
        s = (char *)realloc(s, (len+1) * sizeof(char) );
        //StringBuffer_memcount += (len-size);
        size = len;
        // Make sure s is terminated at the old position
        // (malloc may fill the buffer with rubbish)
        s[oldlen] = 0;
        //fprintf(stderr, "\t\ts=%lx size=%ld alloc=%ld\n", s, size, alloclen);
    }
}

// Private deallocator
void StringBuffer::freemem()
{
    if(s) {
        free(s); s = 0;
        //fprintf(stderr, "FREEMEM s=%lx size=%ld\n", s, size);
    }
    //StringBuffer_memcount -= size;
    size = 0;
}


