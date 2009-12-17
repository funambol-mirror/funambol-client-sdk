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
#include "spds/ItemReader.h"
#include "inputStream/InputStream.h"
#include "base/Log.h"

USE_NAMESPACE

ItemReader::ItemReader(unsigned long size, EncodingHelper& help) : helper(help){ 
    maxChunkSize = size; 
    buffer = new char[maxChunkSize + 1];         
}

ItemReader::~ItemReader() {
    delete [] buffer;
}

void ItemReader::setSyncItem(SyncItem* item) {
    syncItem = item;    
}

void ItemReader::resetBuffer(unsigned long size) {
    if (size > maxChunkSize) {
        delete [] buffer;
        buffer = new char[size + 1];
        maxChunkSize = size;
    }
    memset(buffer, 0, maxChunkSize + 1);
}


// set the right encoding/encryption to have the buffer properly ok
// what to with the des encoding? b64 ok but des not
Chunk* ItemReader::getNextChunk(unsigned long size) {
        
    resetBuffer(size);
    unsigned long bytesRead = 0;
    unsigned long toRead    = size;
    Chunk* chunk            = NULL;
    bool res                = true;
    char* value             = NULL;
    bool first              = true;
    bool last               = true;
    bool useSyncItemEncoding  = (syncItem->getDataEncoding() == NULL) ?
                                false :
                                true;

    if (syncItem == NULL) {
        LOG.error("ItemReader: the syncItem is null");
        return NULL;
    }
    
    InputStream* istream = syncItem->getInputStream();
    
    if (istream->getPosition() != 0) {
        first = false;
    }
    
    if (useSyncItemEncoding) {
        toRead = size;
    } else {
        // ths item doesn't have its own encoding
        toRead = helper.getMaxDataSizeToEncode(size);
    } 
    
    bytesRead = istream->read((void*)buffer, toRead);
    
    if (istream->getTotalSize() == 0) {
        value = stringdup("");
    } else if (bytesRead == 0) {
        LOG.error("ItemReader: could not read from the InputStream");
        return NULL;
    } else {
        if (useSyncItemEncoding) {
            // consider that the buffer should be a char since the chunk is a buffer
            value = stringdup(buffer);
        } else {
            value = helper.encode(EncodingHelper::encodings::plain, buffer, &bytesRead);     
            if (value == NULL) {
                LOG.info("ItemReader: getNextChunk NULL after transformation");
                return NULL;
            }
        }
        
    }
    if (istream->eof() == 0) {
        last = false; 
    }
   
    chunk = new Chunk(value);
    
    chunk->setFirst(first);
    chunk->setLast(last);    

    if (useSyncItemEncoding) {
        chunk->setTotalDataSize(syncItem->getDataSize());
        chunk->setDataEncoding(syncItem->getDataEncoding());
    } else {
        chunk->setTotalDataSize(helper.getDataSizeAfterEncoding(syncItem->getDataSize()));
        chunk->setDataEncoding(helper.getDataEncoding());
    }
    delete [] value;
    
    return chunk;
}

