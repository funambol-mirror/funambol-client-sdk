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

#if defined(WIN32) && !defined(_WIN32_WCE)
#define _WIN32_WINNT 0x0600
#endif

#include <windows.h>
#include <wincrypt.h>

#include "base/util/utils.h"
#include "spds/DESDecoder.h"
#include "base/globalsdef.h"

USE_NAMESPACE


DESDecoder::DESDecoder() : DataTransformer(DT_DES) {
}

DESDecoder::~DESDecoder() {
}

char* DESDecoder::transform(char* data, TransformationInfo& info) {
    BOOL res;

    HCRYPTPROV prov = 0;
    HCRYPTKEY key = 0;

    HCRYPTHASH hash=0;

    DWORD size = info.size; // I reassign it to a DWORD
                            // just in case a long is not
                            // of the same size of a DWORD
    DWORD dwParam = 0;

    char* password = stringdup(info.password);

    // --------------------------------------------------------

    res = CryptAcquireContext(
        &prov,             // Variable to hold returned handle.
        NULL,               // Use default key container.
        MS_ENHANCED_PROV,   // Use enhanced CSP.
        PROV_RSA_FULL,      // Type of provider to acquire.
        CRYPT_VERIFYCONTEXT
    );

    if (res == FALSE) {
        //lastErrorCode = ERR_DT_FAILURE;
        //sprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        setErrorF(ERR_DT_FAILURE, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }


    res = CryptCreateHash(
        prov,      // CSP handle
        CALG_MD5,   // hash algorith ID
        0,          // Key not used
        0,          // flags not used
        &hash      // handle to the hash object
    );

    if (res == FALSE) {
        //lastErrorCode = ERR_DT_FAILURE;
        //sprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        setErrorF(ERR_DT_FAILURE, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }

    // hash password
    res = CryptHashData(
        hash,                          // hash handle
        (unsigned char*) password,// pointer to the data buffer
        strlen(password),         // data length
        0                              // flags not used
    );

    if (res == FALSE) {
        //lastErrorCode = ERR_DT_FAILURE;
        //sprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        setErrorF(ERR_DT_FAILURE, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }

    // Derive a session key from the hash object.
    res = CryptDeriveKey (
        prov,
        CALG_DES,
        hash,
        0,
        &key
    );

    if (res == FALSE) {
        //lastErrorCode = ERR_DT_FAILURE;
        //sprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        setErrorF(ERR_DT_FAILURE, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }

    // set encryption mode to ECB
    dwParam=CRYPT_MODE_ECB;
    res = CryptSetKeyParam(
        key,                      // key handle
        KP_MODE,                   // set key mode flag
        (unsigned char*) &dwParam, // new mode value
        0                          // flags not used
    );

    if (res == FALSE) {
        //lastErrorCode = ERR_DT_FAILURE;
        //sprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        setErrorF(ERR_DT_FAILURE, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }

    // set padding mode to PKCS5
    dwParam=PKCS5_PADDING;
    res = CryptSetKeyParam(
        key,                      // key handle
        KP_PADDING,                   // set key mode flag
        (unsigned char*) &dwParam, // new mode value
        0                          // flags not used
    );

    if (res == FALSE) {
        //lastErrorCode = ERR_DT_FAILURE;
        //sprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        setErrorF(ERR_DT_FAILURE, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }

    res = CryptDecrypt (
        key,
        0,
        TRUE,
        0,
        (unsigned char*)data,
        &size
    );

    if (res == FALSE) {
        //lastErrorCode = ERR_DT_FAILURE;
        //sprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        setErrorF(ERR_DT_FAILURE, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }

    info.size = size;
    info.newReturnedData = false;

 exit:

   // Destroy the session key.
   if (key)
     CryptDestroyKey (key);

   // Destroy the hash object.
   if (hash)
     CryptDestroyHash (hash);

   // Release the provider handle.
   if (prov)
     CryptReleaseContext (prov, 0);

   if (password) {
       delete [] password;
   }

   return data;

}