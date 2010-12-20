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

#ifndef INCL_ENCODING_HELPER
#define INCL_ENCODING_HELPER
/** @cond DEV */

#include "base/fscapi.h"
#include "base/constants.h"
#include "base/globalsdef.h"
#include "base/util/StringBuffer.h"

BEGIN_NAMESPACE

/**
 * Class responsible to read part of a SyncItem and return a Chunk to the
 * caller (SyncManager). It uses the InputStream of the SyncItem
 */
class EncodingHelper {

private:

    /**
    * Type of encoding used
    */
    StringBuffer encoding;

    /**
    * Type of encryption used
    */
    StringBuffer encryption;
    
    /**
    * Credential info
    */ 
    StringBuffer credential;     
    
    /**
    * The original encoding of the data that needs to be 
    * converted. Bu default it is plain
    */
    StringBuffer from;

    /**
    * The data encoding when the item is modified
    */
    StringBuffer dataEncoding;
    
    char* transformData(const char* name, bool encode, const char* password, 
                                    char* buff, unsigned long *len);

    void setEncoding(const char* encoding);
    void setEncryption(const char* encryption);
    void setCredential(const char* credential);
    
    void setDataEncoding(const char* dataEnc);
    
    /**
    * Changes the encoding according with the one set into the class.
    * 
    * @param buffer - the buffer that needs the transformation
    * @param len[IN/OUT] - the len of the data inside the buffer
    * @param isEncoding - if the transformation is encode or decode
    * @param checkBot - it tries to decode and encode without know about the action to do
    *
    * @return - a new allocated buffer transformed
    */
    char* transform(const char* from, char* buffer, unsigned long *len);

public:

    // Constructor
    EncodingHelper(const char* encoding, const char* encryption, const char* credential);

    ~EncodingHelper();        
        
    StringBuffer getDataEncoding() { return dataEncoding; }

    /**
    * Encodes the buffer using the encoding and the encryption.
    *
    * @param buffer - the buffer where the transformation happen
    * @param size[IN/OUT] - the len of the data inside the buffer
    * 
    * @return a new allocated buffer given the 
    */
    char* encode(const char* from, char* buffer, unsigned long *len);
    
    /**
    * Decode the buffer using the encoding and the encryption
    *
    * @param buffer - the buffer where the transformation happen
    * @param size[IN/OUT] - the len of the data inside the buffer
    * 
    * @return a new allocated buffer given the 
    */
    char* decode(const char* from, char* buffer, unsigned long *len);
        

    /**
    * Calculate the max amount of the data could be asked from a
    * size and according with the encoding/encryption that are set in
    * the class. This method is used when, given a an array of <size> 
    * length, the caller wants to know how many bytes can ask that fit
    * the original array, after the transformation according with 
    * the encoding/encryption.
    *
    * @param size - the size respect to the caller want to know the amount of data
    * @return the max data the caller can ask to fit the <size> limit
    * 
    */
    long getMaxDataSizeToEncode(long size);

    /**
    * return the total amount of data after the encoding. 
    * 
    * @param size - the current known size before the transformation
    */
    long getDataSizeAfterEncoding(long size);

    /**
     * valid encodings for changeDataEncoding() and some helper functions
     */
    struct encodings {
        static const char* const plain;      /**< data is transferred as it is */
        static const char* const escaped;    /**< base64 encoded during transfer */
        static const char* const des;        /**< encrypted with DES and then base64 encoded; beware,
                                                  non-standard and only supported by some servers */

        /** helper function which turns NULL into plain */
        static const char* encodingString(const char* encoding) {
            return encoding ? encoding : plain;
        }

        /** returns true if and only if the encoding is one of the supported ones */
        static const bool isSupported(const char* encoding) {
            const char* enc = encodingString(encoding);
            return !strcmp(enc, plain) ||
                !strcmp(enc, escaped) ||
                !strcmp(enc, des);
        }
    };
};


END_NAMESPACE

/** @endcond */
#endif
