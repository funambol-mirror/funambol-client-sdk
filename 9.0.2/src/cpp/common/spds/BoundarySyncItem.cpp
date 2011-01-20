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



#include <string.h>
#include <stdlib.h>

#include "base/util/utils.h"
#include "spds/BoundarySyncItem.h"
#include "spds/DataTransformerFactory.h"
#include "base/globalsdef.h"
#include "inputStream/BoundaryInputStream.h"
#include "base/util/EncodingHelper.h"
#include "base/Log.h"

USE_NAMESPACE


/*
 * Default constructor
 */
BoundarySyncItem::BoundarySyncItem():SyncItem() {
}


/*
 * Constructs a new SyncItem identified by the given key. The key must
 * not be longer than DIM_KEY (see SPDS Constants).
 *
 * @param key - the key
 */
BoundarySyncItem::BoundarySyncItem(const WCHAR* itemKey):SyncItem(itemKey) {
}


/*
 * Destructor. Free the allocated memory (if any)
 */
BoundarySyncItem::~BoundarySyncItem() {
}
/*
 * Sets the SyncItem content data. The passed data are copied into an
 * internal buffer so that the caller can release the buffer after
 * calling setData(). The buffer is fred in the destructor.
 * If when calling setData, there was an existing allocated data block,
 * it is reused (shrinked or expanded as necessary).
 */
void* BoundarySyncItem::setData(const void* itemData, long dataSize) {
    
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
        //lastErrorCode = ERR_NOT_ENOUGH_MEMORY;
        //sprintf(lastErrorMsg, ERRMSG_NOT_ENOUGH_MEMORY, (int)dataSize);
        setErrorF(ERR_NOT_ENOUGH_MEMORY, ERRMSG_NOT_ENOUGH_MEMORY, (int)dataSize);
        return NULL;
    }

    if (itemData) {
        memcpy(data, itemData, size);
        data[size] = 0;  // FIXME: needed?
    } else {
        memset(data, 0, size + 1);
    }

    // Set a new BufferInputStream linked to this data
    if (inputStream) {
        inputStream->close();
        delete inputStream;
    }
    inputStream = new BoundaryInputStream(data, size);

    return data;
}
