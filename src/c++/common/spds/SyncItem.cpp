/*
 * Copyright (C) 2003-2007 Funambol, Inc.
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



#include <string.h>
#include <stdlib.h>

#include "base/util/utils.h"
#include "spds/SyncItem.h"
#include "spds/DataTransformerFactory.h"

const char* const SyncItem::encodings::plain = "bin";
const char* const SyncItem::encodings::escaped = "b64";
const char* const SyncItem::encodings::des = "des;b64";

/*
 * Default constructor
 */
SyncItem::SyncItem() {
    initialize();
}


/*
 * Constructs a new SyncItem identified by the given key. The key must
 * not be longer than DIM_KEY (see SPDS Constants).
 *
 * @param key - the key
 */
SyncItem::SyncItem(const WCHAR* itemKey) {
    initialize();
    wcsncpy(key, itemKey, DIM_KEY);
    key[DIM_KEY-1] = 0;
}

/**
 * Initializes private members
 */
void SyncItem::initialize() {
    type[0] = 0;
    data = NULL;
    encoding = NULL;
    size = -1;
    lastModificationTime = -1;
    key[0] = 0;
    targetParent = NULL;
    sourceParent = NULL;
}

/*
 * Destructor. Free the allocated memory (if any)
 */
SyncItem::~SyncItem() {
    if (data) {
        delete [] data; data = NULL;
    }
    if (encoding) {
        delete [] encoding; encoding = NULL;
    }
    if (targetParent) {
        delete [] targetParent; targetParent = NULL;
    }
    if (sourceParent) {
        delete [] sourceParent; sourceParent = NULL;
    }
}

const char* SyncItem::getDataEncoding() {
    return encoding;
}

void SyncItem::setDataEncoding(const char* enc) {
    if (encoding) {
        delete [] encoding;
    }
    encoding = stringdup(enc);
}



int SyncItem::changeDataEncoding(const char* enc, const char* encryption, const char* credentialInfo) {
    int res = ERR_NONE;
    char encToUse[30];

    // First: if encryption not NULL and valid, it is used and 'enc'
    // value is ignored.
    if ( (encryption) && (!strcmp(encryption, "des")) ) {
        strcpy(encToUse, encodings::des);
    }
    else {
        strcpy(encToUse, enc);
    }

    // nothing to be done?
    if (getDataSize() <= 0 ||
        !strcmp(encodings::encodingString(encoding), encodings::encodingString(encToUse))) {
        return ERR_NONE;
    }

    // sanity check: both encodings must be valid
    if (!encodings::isSupported(encToUse) ||
        !encodings::isSupported(encoding)) {
        return ERR_UNSPECIFIED;
    }

    // always convert to plain encoding first
    if (strcmp(encodings::encodingString(encoding), encodings::plain)) {
        if (!strcmp(encoding, encodings::escaped) ||
            !strcmp(encoding, encodings::des)) {
            res = transformData("b64", FALSE, credentialInfo);
            if (res) {
                return res;
            }
        }
        if (!strcmp(encoding, encodings::des)) {
            res = transformData("des", FALSE, credentialInfo);
            if (res) {
                return res;
            }
        }
        setDataEncoding(encodings::plain);
    }

    // now convert to new encoding
    if (strcmp(encodings::encodingString(encoding), encodings::encodingString(encToUse))) {
        if (!strcmp(encToUse, encodings::des)) {
            res = transformData("des", TRUE, credentialInfo);
            if (res) {
                return res;
            }
        }
        if (!strcmp(encToUse, encodings::escaped) ||
            !strcmp(encToUse, encodings::des)) {
            res = transformData("b64", TRUE, credentialInfo);
            if (res) {
                return res;
            }
        }

        setDataEncoding(encodings::encodingString(encToUse));
    }

    return ERR_NONE;
}

int SyncItem::transformData(const char* name, BOOL encode, const char* password)
{
    char* buffer = NULL;
    DataTransformer *dt = encode ?
        DataTransformerFactory::getEncoder(name) :
        DataTransformerFactory::getDecoder(name);
    TransformationInfo info;
    int res = ERR_NONE;

    if (dt == NULL) {
        res = lastErrorCode;
        goto exit;
    }

    info.size = getDataSize();
    info.password = password;
    buffer = dt->transform((char*)getData(), info);
    if (!buffer) {
        res = lastErrorCode;
        goto exit;
    }
    // danger, transformer may or may not have manipulated the data in place
    if (info.newReturnedData) {
        setData(buffer, info.size);
    } else {
        buffer = NULL;
        setDataSize(info.size);
    }

  exit:
    if (buffer) {
        delete [] buffer;
    }
    if (dt) {
        delete dt;
    }
    return res;
}

/*
 * Returns the SyncItem's key. If key is NULL, the internal buffer is
 * returned; if key is not NULL, the value is copied in the caller
 * allocated buffer and the given buffer pointer is returned.
 *
 * @param key - buffer where the key will be stored
 */
const WCHAR* SyncItem::getKey() {
        return key;
    }

/*
 * Changes the SyncItem key. The key must not be longer than DIM_KEY
 * (see SPDS Constants).
 *
 * @param key - the key
 */
void SyncItem::setKey(const WCHAR* itemKey) {
    wcsncpy(key, itemKey, DIM_KEY);
    key[DIM_KEY-1] = 0;
}

/*
 * Sets the SyncItem modification timestamp. timestamp is a milliseconds
 * timestamp since a reference time (which is platform specific).
 *
 * @param timestamp - last modification timestamp
 */
 void SyncItem::setModificationTime(long timestamp) {
     lastModificationTime = timestamp;
 }

/*
 * Returns the SyncItem modeification timestamp. The returned value
 * is a milliseconds timestamp since a reference time (which is
 * platform specific).
 */
long SyncItem::getModificationTime() {
    return lastModificationTime;
}

/*
 * Sets the SyncItem content data. The passed data are copied into an
 * internal buffer so that the caller can release the buffer after
 * calling setData(). The buffer is fred in the destructor.
 * If when calling setData, there was an existing allocated data block,
 * it is reused (shrinked or expanded as necessary).
 */
void* SyncItem::setData(const void* itemData, long dataSize) {
    if (data) {
        delete [] data; data = NULL;
    }

    size = dataSize;

    // Not yet set.
    if (size == -1) {
        data = NULL;
        return data;
    }

    data = new char[size + 1];
    if (data == NULL) {
        lastErrorCode = ERR_NOT_ENOUGH_MEMORY;
        sprintf(lastErrorMsg, ERRMSG_NOT_ENOUGH_MEMORY, dataSize);
        return NULL;
    }

    if (itemData) {
        memcpy(data, itemData, size);
        data[size] = 0;  // FIXME: needed?
    } else {
        memset(data, 0, size + 1);
    }

    return data;
}

/*
 * Returns the SyncItem data buffer. It is deleted in the destructor.
 */
void* SyncItem::getData() {
    return data;
}

/*
 * Returns the SyncItem data size.
 */
long SyncItem::getDataSize() {
    return size;
}

/*
 * Sets the SyncItem data size.
 */
void SyncItem::setDataSize(long s) {
    size = s;
}

/*
 * Sets the SyncItem data mime type
 *
 * @param - type the content mimetype
 */
void SyncItem::setDataType(const WCHAR* mimeType) {
    wcsncpy(type, mimeType, DIM_MIME_TYPE);
    type[DIM_MIME_TYPE-1] = 0;
}

/*
 * Returns the SyncItem data mime type.
 *
 */
const WCHAR* SyncItem::getDataType() {
    return type;
}

/*
 * Sets the SyncItem state
 *
 * @param state the new SyncItem state
 */
void SyncItem::setState(SyncState newState) {
    state = newState;
}

/*
 * Gets the SyncItem state
 */
SyncState SyncItem::getState() {
    return state;
}

/**
 * Gets the taregtParent property
 *
 * @return the taregtParent property value
 */
const WCHAR* SyncItem::getTargetParent() {
    return targetParent;
}

/**
 * Sets the taregtParent property
 *
 * @param parent the taregtParent property
 */
void SyncItem::setTargetParent(const WCHAR* parent) {
    if (targetParent) {
        delete [] targetParent; targetParent = NULL;
    }
    targetParent = wstrdup(parent);
}

/**
 * Gets the sourceParent property
 *
 * @return the sourceParent property value
 */
const WCHAR* SyncItem::getSourceParent() {
    return sourceParent;
}

/**
 * Sets the sourceParent property
 *
 * @param parent the sourceParent property
 */
void SyncItem::setSourceParent(const WCHAR* parent) {
    if (sourceParent) {
        delete [] sourceParent; sourceParent = NULL;
    }
    sourceParent = wstrdup(parent);
}

ArrayElement* SyncItem::clone() {
    SyncItem* ret = new SyncItem(key);

    ret->setData(data, size);
    ret->setDataType(type);
    ret->setModificationTime(lastModificationTime);
    ret->setState(state);
    ret->setSourceParent(sourceParent);
    ret->setTargetParent(targetParent);

    return ret;
}
