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

#include "base/util/utils.h"
#include "base/util/EncodingHelper.h"
#include "base/Log.h"
#include "spds/DataTransformerFactory.h"

USE_NAMESPACE

const char* const EncodingHelper::encodings::plain = "bin";
const char* const EncodingHelper::encodings::escaped = "b64";
const char* const EncodingHelper::encodings::des = "des;b64";

// Constructor
EncodingHelper::EncodingHelper(const char* encoding, const char* encryption, const char* credential) {
    setEncoding(encoding);
    setEncryption(encryption);
    setCredential(credential ? credential : "");
    from = encodings::plain;
}

EncodingHelper::~EncodingHelper() {}

void EncodingHelper::setEncoding(const char* value) {
    encoding = encodings::encodingString(value);
    if (encoding == "") {
        encoding = encodings::plain;
    }
}

void EncodingHelper::setEncryption(const char* value) {
    encryption = value;
}

void EncodingHelper::setCredential(const char* value) {
    credential = value;
}

void EncodingHelper::setDataEncoding(const char* dataEnc) {
    dataEncoding = dataEnc;
}

long EncodingHelper::getMaxDataSizeToEncode(long size) {
    
    // ret = size is fine when encoding = encodings::plain
    long ret = size;
    
    if (encoding == encodings::escaped) {
        ret = ((unsigned long)(size/4)) * 3;
    }
/*@TODO...    
    if (encryption == "des") {
        int mod = (ret%8);
        ret = ret - 8 - mod;
    }
*/
    return ret;
}

long EncodingHelper::getDataSizeAfterEncoding(long size) {
        
    long ret = size;
    
    if (encoding == encodings::escaped) {
        int modules = size % 3;
        if (modules == 0) {
            ret = 4 * (size/3);
        } else {
            ret = 4 * ((size/3) + 1);
        }
    }
/*@TODO...    
    if (encryption == "des") {
        int mod = (ret%8);
        ret = ret - 8 - mod;
    }
*/
    return ret;
}

char* EncodingHelper::encode(const char* from, char* buffer, unsigned long *len) {
    return transform(from, buffer, len);
}

char* EncodingHelper::decode(const char* from, char* buffer, unsigned long *len) {
    return transform(from, buffer, len);
}

char* EncodingHelper::transform(const char* from, char* buffer, unsigned long *len) {

    char* ret = NULL;    
    StringBuffer encToUse;
    char* pBuffer = buffer;
    StringBuffer originalEncoding(encodings::encodingString(from));

    if (encryption == "des") {
        encToUse = encodings::des;
    }
    else {
        encToUse = encoding;        
    }

    // nothing to be done?
    if (!buffer) {
        //!strcmp(encodings::encodingString(encoding), encodings::encodingString(encToUse))) 
        LOG.info("EncodingHelper: nothing to be done: buffer NULL or lenght <= 0");
        return ret;
    }

    if (len == 0) {
        ret = stringdup("");
        //setDataEncoding(originalEncoding);
        LOG.debug("EncodingHelper: nothing to be done: buffer empty or lenght = 0");
        return ret;
    }
    
    if (encToUse == originalEncoding) {
        ret = new char[*len + 1];         
        memcpy(ret, buffer, *len);
        ret[*len] = 0;
        setDataEncoding(originalEncoding);
        LOG.debug("EncodingHelper: no transformation done. Only returned the new array");        
        return ret;
    }
    
    // sanity check: both encodings must be valid
    if (!encodings::isSupported(encToUse.c_str()) ||
        !encodings::isSupported(encoding.c_str())) {
            LOG.error("EncodingHelper: encoding not supported");
            return ret;
    }
        
    if (encToUse != originalEncoding) {
        // DECODING
        if (originalEncoding != encodings::plain) {
            if ((originalEncoding == encodings::escaped) || (originalEncoding == encodings::des)) {            
                ret = transformData("b64", false, credential.c_str(), pBuffer, len);
                if (ret == NULL) {
                    return ret;
                }
                pBuffer = ret;
            }
            if (originalEncoding == encodings::des) {            
                ret = transformData("des", false, credential.c_str(), pBuffer, len);
                if (pBuffer != buffer) { 
                    delete [] pBuffer;
                }
                if (ret == NULL) {
                    return ret;
                }                
            }
            setDataEncoding(encodings::plain);
        }     
        
        // ENCODING: convert to new encoding               
        if (encToUse == encodings::des) {
            ret = transformData("des", true, credential.c_str(), pBuffer, len);
            if (ret == NULL) {
                return ret;
            }
            pBuffer = ret;            
        }
        if (encToUse == encodings::escaped || encToUse == encodings::des ) {
            ret = transformData("b64", true, credential.c_str(), pBuffer, len);
            if (pBuffer != buffer) { // it means it was assigned pBuffer = ret since the pointer is differen
                delete [] pBuffer;
            }
            if (ret == NULL) {                
                return ret;
            }
        }  
            
        setDataEncoding(encToUse.c_str());
    }
    return ret;

}

char* EncodingHelper::transformData(const char* name, bool encode, 
                                    const char* password, char* buff, 
                                    unsigned long *len) {

    char* buffer = NULL;
    DataTransformer *dt = encode ?
        DataTransformerFactory::getEncoder(name) :
    DataTransformerFactory::getDecoder(name);
    TransformationInfo info;
    int res = ERR_NONE;

    if (dt == NULL) {
        res = getLastErrorCode();
        goto exit;
    }

    info.size = *len;
    info.password = password;
    buffer = dt->transform(buff, info);
    if (!buffer) {
        res = getLastErrorCode();
        goto exit;
    }
    *len = info.size;
    if (info.newReturnedData == false) {        
        buffer = new char[info.size + 1];
        memset(buffer, 0, info.size + 1);        
        memcpy(buffer, buff, info.size);       
    }

exit:        

    delete dt;
    return buffer;
}
