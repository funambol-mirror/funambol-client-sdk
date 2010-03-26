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

#ifndef INCL_ITEM_READER
#define INCL_ITEM_READER
/** @cond DEV */

#include "base/fscapi.h"
#include "base/constants.h"
#include "base/globalsdef.h"
#include "base/util/StringBuffer.h"
#include "base/util/EncodingHelper.h"
#include "spds/Chunk.h"
#include "spds/SyncItem.h"


BEGIN_NAMESPACE

/**
 * Class responsible to read part of a SyncItem and return a Chunk to the
 * caller (SyncManager). It uses the InputStream of the SyncItem
 */
class ItemReader {

private:

    /**
    * Internal reference to the SyncItem 
    */
    SyncItem* syncItem;

    /**
    * The max size of the Chunk that the reader can fill
    */
    unsigned long maxChunkSize;
    
    /**
    * default encoding of the sync source
    */ 
    StringBuffer defaultEnconding;

    /**
    * default encryption of the sync source
    */ 
    StringBuffer defaultEncyption;
    
    /**
    *
    */
    StringBuffer defaultCredentialInfo;

    /**
    * The internal buffer where to read the data. This buffer should contains the
    * data read from the input stream and it used to create a chunk. So the creation
    * of the buffer takes care of the last char for the \0.
    */
    char* buffer;
    
    /**
    * Reset the current internal buffer put it to 0. Moreover it checks that if the size
    * is greater than the maxMsgSize set at the beginning, it frees the existing
    * buffer and create a new one.
    */
    void resetBuffer(unsigned long size);       

    EncodingHelper& helper;

public:

    // Constructor
    ItemReader(unsigned long size, EncodingHelper& helper);

    ~ItemReader();
    
    /**
    * Set the SyncItem reference to the internal one
    *
    * @param item - the SyncItem to be set internally
    */
    void setSyncItem(SyncItem* item);
   
    /**
    * Return a new Chunk object filled with the data retrieved by the SyncItem.
    * The size is the max amount of data it is possible to get, transformation
    * included. The caller needs <size> data and the itemReader is 
    * responsible to give back this amount. If no data are available or some errors
    * occurr it returns NULL. 
    * Since the Chunk returned is a new object, the caller is responsible to free it
    *
    * @param size - the max amount of data requested by the caller
    * @return - a new Chunk object if possible. NULL if error occurred
    */
    Chunk* getNextChunk(unsigned long size);     

};


END_NAMESPACE

/** @endcond */
#endif
