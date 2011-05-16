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

#ifndef INCL_BASE_STRING_BUFFER
#define INCL_BASE_STRING_BUFFER
/** @cond DEV */

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "base/util/ArrayElement.h"

BEGIN_NAMESPACE

class ArrayList;

/**
 * Portable char string implementation.
 *
 * Characteristic:
 * <li> methods and operators similar to std::string
 * <li> based only on c-library
 * <li> a StringBuffer can be empty or null, and the two states can be tested
 *   separately (keeping the same semantic of a char buf null or "".
 */
class StringBuffer: public ArrayElement {
    public:
        // Constant value for an invalid pos (returned by find and rfind)
        static const size_t npos;

        /**
         * Creates a new StringBuffer with the given initial value.
         * The default is empty string.
         * Possible calls are:
         * <li> Stringbuffer s; // construct a string with an empty value
         * <li> Stringbuffer s(NULL); // construct a string a null value
         * <li> Stringbuffer s("string"); // construct a string containing "string"
         * <li> Stringbuffer s(buf, len); // construct a string with the content of buf and lenght len.
         *
         * In the latter example, buf may or may not be null-terminated. If it's null-terminated, and
         * the lenght of buf is smaller than len, the first wins.
         *
         */
        StringBuffer(const char* str = "", size_t len = npos);

        /**
         * Copy constructor.
         */
        StringBuffer(const StringBuffer &sb);
        
        /**
         * Destructor
         */
        ~StringBuffer();
    
        /**
         * Append a char string to the StringBuffer. See also operator+=
         */
        StringBuffer& append(const char* );

        /**
         * Append a char string to the StringBuffer but do not copy more than
         * len chars.
         */
        StringBuffer& append(const char* sNew, unsigned long len);

        /**
         * Append a long to the StringBuffer with the specified sign
         */
        StringBuffer& append(unsigned long u, bool sign = true);

        /**
         * Append the content of another StringBuffer to this one. See also operator+=
         */
        StringBuffer& append(StringBuffer& s);

        /**
         * Append the content of another StringBuffer to this one. See also operator+=
         */
        StringBuffer& append(StringBuffer* str);

        /**
         * Assign a new value to this StringBuffer. See also operator=
         */
        StringBuffer& assign(const char* );

        /** @deprecated use assign instead */
        StringBuffer& set(const char* other) { return assign(other);};

        /**
         * Convert the Wide char string wc, using the  given encoding, and assign
         * the converted char string to this StringBuffer.
         * If WCHAR size is equal to char (i.e. wide chars are not used in the build,
         * no conversion is made and the value of wc is assigned to the StringBuffer.
         */
        StringBuffer& convert(const WCHAR* wc, const char *encoding = 0);

        /**
         * Executes a sprintf(), overwriting the current string buffer
         * and enlarging it as necessary. The parameters are that of a
         * normal sprintf().
         */
        StringBuffer& sprintf(const char* format, ...)
#ifdef __GNUC__
            /* enables GCC checking of format <-> parameter mismatches */
            __attribute__ ((format (printf, 2, 3)))
#endif
            ;

        /**
         * Executes a vsprintf(), overwriting the current string
         * buffer and enlarging it as necessary. The parameters are
         * that of a normal vsprintf().
         */
        StringBuffer& vsprintf(const char* format, PLATFORM_VA_LIST ap)
#ifdef __GNUC__
            /* enables GCC checking of format <-> parameter mismatches */
            __attribute__ ((format (printf, 2, 0)))
#endif
            ;


        /**
         * Release the string buffer.
         */
        StringBuffer& reset();

        /**
         * @deprecated user c_str() instead.
         */
        const char*  getChars() const { return c_str(); };

        /**
         * Return a const pointer to the internal char buffer.
         */
        const char*  c_str() const { return s; };

        /**
         * Find the first occurrence of substring str, starting from pos.
         *
         * @param - start position
         * @return - the position or StringBuffer::npos if not found.
         */
        size_t find(const char *str, size_t pos = 0) const;

        /**
         * Find the first occurrence of substring str, using case insensitive compare.
         *
         * @return - the position or StringBuffer::npos if not found.
         */
        size_t ifind(const char *str, size_t pos = 0) const;

        /**
         * Find the last occurrence of substring str.
         *
         * @return - the position or StringBuffer::npos if not found.
         */
        size_t rfind(const char *str, size_t pos = 0) const;

        /**
         * Replace the first occurrence of substring 'from' with string 'to'.
         *
         * @return - the position of the first token replaced or StringBuffer::npos if
         *           not found.
         */
        size_t replace(const char *from, const char *to, size_t pos = 0);
        /**
         * Replace all the occurrences of substring 'from' with string 'to'.
         *
         * @return - the number of tokens replaced
         */
        int replaceAll(const char *from, const char *to, size_t pos = 0);


        /**
         * Splits string in tokens using the parameter separator
         *
		 * @param tokens an arrayList to fill with the tokens
		 * @param sep    a string to be used as separator (e.g. "\r\n"). Default is space.
         * @return       the given token ArrayList
         */
        ArrayList &split (ArrayList &tokens,
                         const char *separator = " ") const;

        /**
         * Joins all the tokens in the given ArrayList, using separator to
         * contatenate them, appending them to the StringBuffer
         *
         * @return - the StringBuffer
         */
        StringBuffer& join(ArrayList &tokens, const char *separator);

        /**
         * Return the substring between pos and pos+len.
         * If pos is greater then the string length, or len is 0, return an
         * empty string
         * If len is greater then the string length, the last is used.
         */
        StringBuffer substr(size_t pos, size_t len = npos) const;

        /**
         * Return the length of the string, or zero if the internal pointer
         * is NULL.
         */
        unsigned long length() const;

        /**
         * Reserve len amount of space for the string.
         */
        void reserve(size_t len);

        /**
         * Make the string upper case
         */
        StringBuffer& upperCase() ;

        /**
         * Make the string lower case
         */
        StringBuffer& lowerCase() ;

        /**
         * Perform case insensitive compare
         * @return true if equals
         */
        bool icmp(const char *sc) const ;

        /**
         * True if the string is null or empty
         */
        bool empty() const;

        /**
         * True if the string is null
         */
        bool null() const;

        /**
         * True if the string terminates with ch
         */
        bool endsWith(char ch) const;

        /**
         * True if the string terminates with str
         */
        bool endsWith(const char* str) const;
        
        /**
         * Trim the string, by removing any occurrence of trimchr from
		 * the beginning or the end of the string. Default character is space.
		 *
		 * @param  trimchr the character to trim out. Default is ' '.
		 * @return the string reference
         */
        StringBuffer& trim(char trimchr=' ');

        /**
         * Clone the string
         */
        ArrayElement* clone() ;

        /**
         * Class operators
         */
        StringBuffer& operator= (const char*  sc) ;
        StringBuffer& operator= (const StringBuffer& s) ;
        StringBuffer& operator+= (const char*  sc) ;
        StringBuffer& operator+= (const StringBuffer& s) ;
        bool operator== (const char*  sc) const ;
        bool operator== (const StringBuffer& sb) const ;
        bool operator!= (const char*  sc) const ;
        bool operator!= (const StringBuffer& s) const ;

        /**
         * Returns the character at position index. If index is invalid,
         * the method returns -1
         */
        char operator[]( int index ) const;

        operator const char* () const { return s; } ;

    private:
        char*  s;
        size_t size;

        // Allocator
        void getmem(size_t len);
        // Deallocator
        void freemem();
};

StringBuffer operator+(const StringBuffer& x, const char *y);


END_NAMESPACE

/** @endcond */
#endif

