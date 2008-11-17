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


#ifndef INCL_CONTENT_TYPE_INFO
#define INCL_CONTENT_TYPE_INFO
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"


class ContentTypeInfo : public ArrayElement {

     // ------------------------------------------------------------ Private data
    private:
        char*  ctType;
        char*  verCT;


    // ---------------------------------------------------------- Protected data
    public:


        ContentTypeInfo();
        ~ContentTypeInfo();

        /**
         * Creates a new ContentTypeCapability object with the given content type
         * and versione
         *
         * @param ctType corresponds to &lt;CTType&gt; element in the SyncML
         *                    specification - NOT NULL
         * @param verCT corresponds to &lt;VerCT&gt; element in the SyncML
         *                specification - NOT NULL
         *
         */
        ContentTypeInfo(const char*  ctType, const char*  verCT);

        /**
         * Gets the content type properties
         *
         * @return the content type properties
         */
        const char* getCTType();

        /**
         * Sets the content type properties
         *
         * @param ctType the content type properties
         */
        void setCTType(const char*  ctType);

        /**
         * Gets the version of the content type
         *
         * @return the version of the content type
         */
        const char* getVerCT();

        /**
         * Sets the version of the content type
         *
         * @param verCT the version of the content type
         */
        void setVerCT(const char*  verCT);

        ArrayElement* clone();
};

/** @endcond */
#endif
