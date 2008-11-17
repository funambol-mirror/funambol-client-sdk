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

#ifndef INCL_RAW_FILE_SYNC_SOURCE
#define INCL_RAW_FILE_SYNC_SOURCE
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "client/FileSyncSource.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * In contrast to the FileSyncSource this class does not
 * wrap the file content and attributes in an XML format
 * when exchanging it with the server. Therefore it can
 * be used as a peer for a servers addressbook or calendar
 * URIs.
 *
 * The type of the items is taken from the sync source
 * type property.
 */
class RawFileSyncSource : public FileSyncSource {
  public:
    RawFileSyncSource(const WCHAR* name, AbstractSyncSourceConfig* sc);

    int addItem(SyncItem& item);
    int updateItem(SyncItem& item);

  protected:
    bool setItemData(SyncItem* syncItem);
};



END_NAMESPACE

/** @} */
/** @endcond */
#endif // INCL_RAW_FILE_SYNC_SOURCE
