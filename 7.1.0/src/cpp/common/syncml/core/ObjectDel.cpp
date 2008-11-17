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


#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "syncml/core/ObjectDel.h"

BEGIN_NAMESPACE

/*
* delete all the char* type in the list.
* The first parameter is the number of char* pointer array to delete
*
*/

// To be developed
void deleteAll(int count, char** s, ...) {

    PLATFORM_VA_LIST ap;
    int i = 0;

    // Delete the first one
    safeDel(s);

    // Delete all the others
    PLATFORM_VA_START (ap, s);

    for (i = 0; i < count - 1; i++) {
        char** pp = PLATFORM_VA_ARG(ap, char**);
        safeDel(pp);
    }

    PLATFORM_VA_END (ap);
}

void deleteStringBuffer(StringBuffer** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

//To be developed....
void deleteAllStringBuffer(int count, StringBuffer** s, ...) {

    PLATFORM_VA_LIST ap;
    int i = 0;

    // Delete the first one
    deleteStringBuffer(s);

    // Delete all the others

    PLATFORM_VA_START (ap, s);

    for (i = 0; i < count -1; i++) {
        StringBuffer** ss = PLATFORM_VA_ARG (ap, StringBuffer**);
        deleteStringBuffer(ss);
    }
    PLATFORM_VA_END (ap);

}

static bool SingleNotNullCheck(const char* s) {
    return (s) ? true : false;
}

/*
* return true if an element of the char* list is not NULL
*/
bool NotNullCheck(int count, const char* s, ...) {

    PLATFORM_VA_LIST ap;
    int i = 0;

    if (s) {
        return true;
    }

    PLATFORM_VA_START (ap, s);

    for(i = 0; i < count - 1; i++) {
        const char *t = PLATFORM_VA_ARG (ap, const char*);

        if (SingleNotNullCheck(t)) {
            return true;
        }
    }
    PLATFORM_VA_END (ap);
    return false;
}

bool NotZeroCheck(int count, int s, ...) {

    PLATFORM_VA_LIST ap;
    int i = 0;

    if (s != 0) {
        return true;
    }

    PLATFORM_VA_START (ap, s);

    for(i = 0; i < count - 1; i++) {
        if (PLATFORM_VA_ARG (ap, int) != 0) {
            return true;
        }
    }

    PLATFORM_VA_END (ap);
    return false;
}

/*
* return true if at least an arrayList as lenght > 0
* To be developed
*/
bool NotZeroSingleArrayLength(ArrayList* s) {
    bool ret = false;
    if (s) {
        if (s->size() > 0)
            ret = true;
    }
    return ret;
}

bool NotZeroArrayLength(int count, ArrayList* s, ...) {

    PLATFORM_VA_LIST ap;
    int i    = 0;

    if (NotZeroSingleArrayLength(s)) {
        return true;
    }

    PLATFORM_VA_START (ap, s);

    for(i = 0; i < count - 1; i++) {
        ArrayList* p = PLATFORM_VA_ARG (ap, ArrayList*);

        if (NotZeroSingleArrayLength(p)) {
            return true;
        }
    }

    PLATFORM_VA_END (ap);
    return false;
}


bool NotZeroSingleStringBufferLength(StringBuffer* s) {
    bool ret = false;
    if (s) {
        if (s->length() > 0)
            ret = true;
    }
    return ret;
}

/*
* return true if at least one StringBuffer has lenght > 0
*/
bool NotZeroStringBufferLength(int count, StringBuffer* s, ...) {

    PLATFORM_VA_LIST ap;
    int i    = 0;

    if (NotZeroSingleStringBufferLength(s)) {
        return true;
    }

    PLATFORM_VA_START (ap, s);

    for(i = 0; i < count - 1; i++) {
        StringBuffer* p = PLATFORM_VA_ARG (ap, StringBuffer*);

        if (NotZeroSingleStringBufferLength(p)) {
            return true;
        }
    }
    PLATFORM_VA_END (ap);
    return false;
}


void deleteTarget(Target ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteSource(Source ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteSourceArray(SourceArray ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteCred(Cred ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteMeta(Meta ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteMetInf(MetInf ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteNextNonce(NextNonce ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteAlert(Alert ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteItem(Item ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteCmdID(CmdID ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteAuthentication(Authentication ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteAnchor(Anchor ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteMem(Mem ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteSyncHdr(SyncHdr ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteSyncBody(SyncBody ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteSessionID(SessionID ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteVerDTD(VerDTD ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteVerProto(VerProto ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteTargetRef(TargetRef ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteSourceRef(SourceRef ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteStatus(Status ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteChal(Chal ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteData(Data ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteMap(Map ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteMapItem(MapItem ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteComplexData(ComplexData ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteAdd(Add ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteReplace(Replace ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteDelete(Delete ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteCopy(Copy ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteSync(Sync ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteSequence(Sequence ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteAtomic(Atomic ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteGet(Get ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deletePut(Put ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteDataStore(DataStore ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteSyncType(SyncType ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteContentTypeInfo(ContentTypeInfo ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteSyncCap(SyncCap ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteDSMem(DSMem ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteCTCap(CTCap ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteExt(Ext ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteStringElement(StringElement ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteResults(Results ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteExec(Exec ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteSearch(Search ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

void deleteSyncML(SyncML ** s) {
    if (s) {
        delete *s; *s = NULL;
    }
}

END_NAMESPACE

