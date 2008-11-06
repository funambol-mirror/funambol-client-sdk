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

// void deleteAll              (int count, char** s, ...);
void deleteStringBuffer     (StringBuffer** s);
//void deleteAllStringBuffer  (int count, StringBuffer** s, ...);
// bool NotNullCheck           (int count, char*  s, ...);
bool NotZeroCheck           (int count, int s, ...);
//bool NotZeroArrayLenght     (int count, ArrayList* s, ...);
// bool NotZeroStringBufferLenght(int count, StringBuffer* s, ...); //XXX

void deleteTarget           (Target ** s);
void deleteSource           (Source ** s);
void deleteSourceArray      (SourceArray ** s);
void deleteMeta             (Meta ** s);
void deleteMetInf           (MetInf ** s);
void deleteCred             (Cred ** s);
void deleteAuthentication   (Authentication ** s);
void deleteAlert            (Alert ** s);
void deleteItem             (Item ** s);
void deleteArrayList        (ArrayList ** s);
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

void deleteAllStringBuffer(int count, StringBuffer** s);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2) ;
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10, StringBuffer** s11);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10, StringBuffer** s11, StringBuffer** s12);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10, StringBuffer** s11, StringBuffer** s12, StringBuffer** s13);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10, StringBuffer** s11, StringBuffer** s12, StringBuffer** s13, StringBuffer** s14);

void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10, StringBuffer** s11, StringBuffer** s12, StringBuffer** s13, StringBuffer** s14, StringBuffer** s15);
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10, StringBuffer** s11, StringBuffer** s12, StringBuffer** s13, StringBuffer** s14, StringBuffer** s15, StringBuffer** s16);


bool NotZeroSingleArrayLenght(ArrayList* s);
bool NotZeroArrayLenght(int count, ArrayList* s);
bool NotZeroArrayLenght(int count, ArrayList* s, ArrayList* s1);
bool NotZeroArrayLenght(int count, ArrayList* s, ArrayList* s1, ArrayList* s2);

bool SingleNotNullCheck(char*  s);
bool NotNullCheck(int count, char*  s);
bool NotNullCheck(int count, char*  s, char*  s1);
bool NotNullCheck(int count, char*  s, char*  s1, char*  s2);
bool NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3);
bool NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4);
bool NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4, char*  s5);
bool NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4, char*  s5, char*  s6);
bool NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4, char*  s5, char*  s6, char*  s7);
bool NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4, char*  s5, char*  s6, char*  s7, char*  s8);
bool NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4, char*  s5, char*  s6, char*  s7, char*  s8, char*  s9);
bool NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4, char*  s5, char*  s6, char*  s7, char*  s8, char*  s9, char*  s10);

void deleteAll(int count, char** s);
void deleteAll(int count, char** s, char** s1);
void deleteAll(int count, char** s, char** s1, char** s2);
void deleteAll(int count, char** s, char** s1, char** s2, char** s3);
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4);
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4, char** s5);
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4, char** s5, char** s6);
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4, char** s5, char** s6, char** s7);
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4, char** s5, char** s6, char** s7, char** s8);

bool NotZeroSingleStringBufferLenght(StringBuffer* s);
bool NotZeroStringBufferLenght(int count, StringBuffer* s);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3, StringBuffer* s4);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7) ;
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11) ;
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12, StringBuffer* s13);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12, StringBuffer* s13, StringBuffer* s14);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12, StringBuffer* s13, StringBuffer* s14, StringBuffer* s15);
bool NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12, StringBuffer* s13, StringBuffer* s14, StringBuffer* s15,
                                         StringBuffer* s16) ;

END_NAMESPACE

/** @endcond */
#endif


