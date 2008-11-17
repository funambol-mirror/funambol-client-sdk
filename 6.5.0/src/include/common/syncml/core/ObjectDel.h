/*
 * Copyright (C) 2003-2007 Funambol, Inc
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


#ifndef INCL_OBJECT_DEL
#define INCL_OBJECT_DEL
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "syncml/core/TagNames.h"


// void deleteAll              (int count, char** s, ...);
void deleteStringBuffer     (StringBuffer** s);
//void deleteAllStringBuffer  (int count, StringBuffer** s, ...);
// BOOL NotNullCheck           (int count, char*  s, ...);
BOOL NotZeroCheck           (int count, int s, ...);
//BOOL NotZeroArrayLenght     (int count, ArrayList* s, ...);
// BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, ...); //XXX

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


BOOL NotZeroSingleArrayLenght(ArrayList* s);
BOOL NotZeroArrayLenght(int count, ArrayList* s);
BOOL NotZeroArrayLenght(int count, ArrayList* s, ArrayList* s1);
BOOL NotZeroArrayLenght(int count, ArrayList* s, ArrayList* s1, ArrayList* s2);

BOOL SingleNotNullCheck(char*  s);
BOOL NotNullCheck(int count, char*  s);
BOOL NotNullCheck(int count, char*  s, char*  s1);
BOOL NotNullCheck(int count, char*  s, char*  s1, char*  s2);
BOOL NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3);
BOOL NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4);
BOOL NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4, char*  s5);
BOOL NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4, char*  s5, char*  s6);
BOOL NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4, char*  s5, char*  s6, char*  s7);
BOOL NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4, char*  s5, char*  s6, char*  s7, char*  s8);
BOOL NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4, char*  s5, char*  s6, char*  s7, char*  s8, char*  s9);
BOOL NotNullCheck(int count, char*  s, char*  s1, char*  s2, char*  s3, char*  s4, char*  s5, char*  s6, char*  s7, char*  s8, char*  s9, char*  s10);

void deleteAll(int count, char** s);
void deleteAll(int count, char** s, char** s1);
void deleteAll(int count, char** s, char** s1, char** s2);
void deleteAll(int count, char** s, char** s1, char** s2, char** s3);
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4);
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4, char** s5);
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4, char** s5, char** s6);
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4, char** s5, char** s6, char** s7);
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4, char** s5, char** s6, char** s7, char** s8);

BOOL NotZeroSingleStringBufferLenght(StringBuffer* s);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3, StringBuffer* s4);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7) ;
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11) ;
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12, StringBuffer* s13);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12, StringBuffer* s13, StringBuffer* s14);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12, StringBuffer* s13, StringBuffer* s14, StringBuffer* s15);
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12, StringBuffer* s13, StringBuffer* s14, StringBuffer* s15,
                                         StringBuffer* s16) ;
/** @endcond */
#endif


