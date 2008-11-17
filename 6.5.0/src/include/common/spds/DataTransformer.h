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

    /**
     * Properties used by a DataTransformer. See the design document
     * for details.
     */
    struct TransformationInfo {

        BOOL newReturnedData;
        long size;
        const char*  username;
        const char*  password;
        const char*  sourceName;

        TransformationInfo() : newReturnedData(FALSE)
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

        DataTransformer(char*  name);

        virtual ~DataTransformer();

        static DataTransformer* getEncoder(const char* name);
        static DataTransformer* getDecoder(const char* name);
        static BOOL isSupportedEncoder(char*  name);
        static BOOL isSupportedDecoder(char*  name);

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

/** @endcond */
#endif
