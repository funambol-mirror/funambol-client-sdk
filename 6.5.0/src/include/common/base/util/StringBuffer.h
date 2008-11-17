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

#ifndef INCL_BASE_STRING_BUFFER
#define INCL_BASE_STRING_BUFFER
/** @cond DEV */

#include "base/util/ArrayElement.h"

#include <stdarg.h>

class ArrayList;

/**
 * Awful implementation of a StringBuffer!
 */
class StringBuffer: public ArrayElement {
    public:
        // Constant value for an invalid pos (returned by find and rfind)
        static const size_t npos;

        StringBuffer(const char*  str = NULL, size_t len = npos);

        StringBuffer(const StringBuffer &sb);

        StringBuffer(const void* str, size_t len);

        ~StringBuffer();

        StringBuffer& append(const char* );

        StringBuffer& append(unsigned long u, BOOL sign = TRUE);

        StringBuffer& append(StringBuffer& s);

        StringBuffer& append(StringBuffer* str);

        StringBuffer& set(const char* );

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
        StringBuffer& vsprintf(const char* format, va_list ap)
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
         * Get the char array, same as the cast operator
         */
        const char*  getChars() const;
        inline const char*  c_str() const { return s; };

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
         * Splits string on each occurrence of any of the characters in
         * delimiters.
         *
         * @return - the position or StringBuffer::npos if not found.
         */
        ArrayList &split (ArrayList &tokens,
                          const char *delimiters = " ") const;

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

/** @endcond */
#endif

