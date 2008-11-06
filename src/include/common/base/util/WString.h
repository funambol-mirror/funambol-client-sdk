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

#ifndef INCL_WSTRING
#define INCL_WSTRING
/** @cond DEV */

#include "base/util/ArrayElement.h"

class ArrayList;
class StringBuffer;

#define DEFAULT_DELIMITER (const WCHAR* )TEXT(" ")

/**
 * Awful implementation of a WString!
 */
class WString: public ArrayElement {
    public:
        // Constant value for an invalid pos (returned by find and rfind)
        static const size_t npos;

        WString(const WCHAR* str = NULL, size_t len = npos);

        WString(const WString &sb);

        ~WString();

        WString& append(const WCHAR*);

        WString& append(unsigned long u, BOOL sign = TRUE);

        WString& append(WString& s);

        WString& append(WString* str);

        WString& set(const WCHAR*);

        /**
         * Release the string buffer.
         */
        WString& reset();

        /**
         * Get the WCHAR array, same as the cast operator
         */
        const WCHAR* getChars() const;
        inline const WCHAR* c_str() const { return s; };

        /**
         * Find the first occurrence of substring str.
         *
         * @return - the position or WString::npos if not found.
         */
        size_t find(const WCHAR *str, size_t pos = 0) const;

        /**
         * Find the first occurrence of substring str, using case insensitive compare.
         *
         * @return - the position or WString::npos if not found.
         */
        size_t ifind(const WCHAR *str, size_t pos = 0) const;

        /**
         * Replace the first occurrence of substring 'from' with string 'to'.
         *
         * @return - the position of the first token replaced or WString::npos if
         *           not found.
         */
        size_t replace(const WCHAR *from, const WCHAR *to, size_t pos = 0);
        /**
         * Replace all the occurrences of substring 'from' with string 'to'.
         *
         * @return - the number of tokens replaced
         */
        int replaceAll(const WCHAR *from, const WCHAR *to, size_t pos = 0);

        /**
         * Splits string on each occurrence of any of the characters in
         * delimiters.
         *
         * @return - the position or WString::npos if not found.
         */
        ArrayList &split (ArrayList &tokens,
                          const WCHAR *delimiters = DEFAULT_DELIMITER ) const;

        /**
         * Joins all the tokens in the given ArrayList, using separator to
         * contatenate them, appending them to the WString
         *
         * @return - the WString
         */
        WString& join(ArrayList &tokens, const WCHAR *separator);

        /**
         * Return the substring between pos and pos+len.
         * If pos is greater then the string length, or len is 0, return an
         * empty string
         * If len is greater then the string length, the last is used.
         */
        WString substr(size_t pos, size_t len = npos) const;

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
        WString& upperCase() ;

        /**
         * Make the string lower case
         */
        WString& lowerCase() ;

        /**
         * Perform case insensitive compare
         */
        bool icmp(const WCHAR *sc) const ;

        /**
         * True if the string is null or empty
         */
        bool empty() const;

        /**
         * True if the string is null
         */
        bool null() const;

        /**
         * Clone the string
         */
        ArrayElement* clone() ;

        /**
         * Class operators
         */
        WString& operator= (const WCHAR* sc) ;
        WString& operator= (const WString& s) ;
        WString& operator= (const StringBuffer& s) ;
        WString& operator+= (const WCHAR* sc) ;
        WString& operator+= (const WString& s) ;
        WString& operator+= (const StringBuffer& s) ;
        bool operator== (const WCHAR* sc) const ;
        bool operator== (const WString& sb) const ;
        bool operator== (const StringBuffer& sb) const ;
        bool operator!= (const WCHAR* sc) const ;
        bool operator!= (const WString& s) const ;
        bool operator!= (const StringBuffer& sb) const ;

        inline operator const WCHAR*() const { return s; } ;

    private:
        WCHAR* s;
        size_t size;

        // Allocator
        void getmem(size_t len);
        // Deallocator
        void freemem();
};

WString operator+(const WString& x, const WCHAR *y);

/** @endcond */
#endif

