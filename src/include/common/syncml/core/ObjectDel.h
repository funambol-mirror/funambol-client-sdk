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


#ifndef INCL_OBJECT_DEL
#define INCL_OBJECT_DEL
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "syncml/core/TagNames.h"

BEGIN_NAMESPACE

void deleteAll              (int count, char** s, ...);
void deleteStringBuffer     (StringBuffer** s);
void deleteAllStringBuffer  (int count, StringBuffer** s, ...);
bool NotNullCheck           (int count, const char*  s, ...);
bool NotZeroCheck           (int count, int s, ...);
bool NotZeroArrayLength     (int count, ArrayList* s, ...);
bool NotZeroStringBufferLength(int count, StringBuffer* s, ...); //XXX

void deleteTarget           (Target ** s);
void deleteSource           (Source ** s);
void deleteSourceArray      (SourceArray ** s);
void deleteMeta             (Meta ** s);
void deleteMetInf           (MetInf ** s);
void deleteCred             (Cred ** s);
void deleteAuthentication   (Authentication ** s);
void deleteAlert            (Alert ** s);
void deleteItem             (Item ** s);
void deleteNextNonce        (NextNonce ** s);
void deleteCmdID            (CmdID ** s);
void deleteComplexData      (ComplexData ** s);
void deleteAnchor           (Anchor ** s);
void deleteComplexData      (ComplexData ** s);
void deleteMem              (Mem ** s);
void deleteSyncHdr          (SyncHdr ** s);
void deleteSyncBody         (SyncBody ** s);
void deleteSyncML           (SyncML ** s);
void deleteSessionID        (SessionID ** s);
void deleteVerProto         (VerProto ** s);
void deleteVerDTD           (VerDTD ** s);
void deleteStatus           (Status ** s);
void deleteTargetRef        (TargetRef ** s);
void deleteSourceRef        (SourceRef ** s);
void deleteChal             (Chal ** s);
void deleteData             (Data ** s);
void deleteAdd              (Add ** s);
void deleteSync             (Sync ** s);
void deleteReplace          (Replace ** s);
void deleteDelete           (Delete ** s);
void deleteMap              (Map ** s);
void deleteCopy             (Copy ** s);
void deleteMapItem          (MapItem ** s);
void deleteSequence         (Sequence ** s);
void deleteAtomic           (Atomic ** s);
void deleteGet              (Get ** s);
void deletePut              (Put ** s);
void deleteDataStore        (DataStore ** s);
void deleteSyncType         (SyncType ** s);
void deleteContentTypeInfo  (ContentTypeInfo ** s);
void deleteSyncCap          (SyncCap ** s);
void deleteDSMem            (DSMem ** s);
void deleteCTCap            (CTCap ** s);
void deleteExt              (Ext ** s);
void deleteStringElement    (StringElement ** s);
void deleteResults          (Results ** s);
void deleteExec             (Exec ** s);
void deleteSearch           (Search ** s);

END_NAMESPACE

/** @endcond */
#endif


