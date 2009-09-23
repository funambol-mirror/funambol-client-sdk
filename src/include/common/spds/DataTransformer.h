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

/**
 * This class represents a data encoder or decoder
 *
 * A DataTransformer is a component that give some data in some format
 * as input, it generates a transformation of the source data in output.
 * Examples of transformations are:
 *
 * - base64 encoding/decoding
 * - encryption/decription
 */

 #ifndef INCL_DATA_TRANSFORMER
    #define INCL_DATA_TRANSFORMER
/** @cond DEV */

    #include "base/fscapi.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

    /**
     * Properties used by a DataTransformer. See the design document
     * for details.
     */
    struct TransformationInfo {

        bool newReturnedData;
        long size;
        const char*  username;
        const char*  password;
        const char*  sourceName;

        TransformationInfo() : newReturnedData(false)
                             , size(-1)
                             , username(NULL)
                             , password(NULL)
                             , sourceName(NULL) {}

    };

    class DataTransformer {

    private:

        char*  name;

    public:
        /*
         * Default constructor-destructor
         */
        DataTransformer();

        DataTransformer(const char*  name);

        virtual ~DataTransformer();

        static DataTransformer* getEncoder(const char* name);
        static DataTransformer* getDecoder(const char* name);
        static bool isSupportedEncoder(char*  name);
        static bool isSupportedDecoder(char*  name);

        void setName(const char* name);
        const char* getName();

        /**
         * Performs the transformation. data is the pointer to the
         * data buffer to transform and info is a TransformationInfo
         * object containing some session propertiues that can be
         * used by a transformer. The function allocates the returned data
         * with the new operator; the caller is responsible to free
         * the allocated memory with the delete operator.
         * In case of error NULL is returned and lastErrorCode and
         * lastErrorMsg are set accordingly.
         *
         * @param data the data block to transform
         * @param info transformation info
         */
        virtual char* transform(char* data, TransformationInfo& info) = 0;

    };


END_NAMESPACE

/** @endcond */
#endif
