
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
#include "base/fscapi.h"
#include "base/util/utils.h"
#include "spds/spdsutils.h"
#include "base/quoted-printable.h"
#include "base/globalsdef.h"


#define BASE64 "base64"
#define QUOTED_PRINTABLE "quoted-printable"

BEGIN_NAMESPACE

// Base64 encoding for files (with newline)
char *uuencode(const char *msg, int len);

SyncMode syncModeCode(const char* syncMode) {

    if (strcmp(syncMode,"slow") == 0)
        return SYNC_SLOW;
    else if (strcmp(syncMode,"two-way") == 0)
        return SYNC_TWO_WAY;
    else if (strcmp(syncMode,"one-way") == 0)               // deprecated
        return SYNC_ONE_WAY_FROM_SERVER;
    else if (strcmp(syncMode,"one-way-server") == 0 ||      // deprecated
             strcmp(syncMode,"one-way-from-server") == 0 )
        return SYNC_ONE_WAY_FROM_SERVER;
    else if (strcmp(syncMode,"one-way-client") == 0 ||      // deprecated
             strcmp(syncMode,"one-way-from-client") == 0)
        return SYNC_ONE_WAY_FROM_CLIENT;
    else if (strcmp(syncMode,"refresh")             == 0 || // deprecated
             strcmp(syncMode,"refresh-server")      == 0 || // deprecated
             strcmp(syncMode,"refresh-from-server") == 0  )
        return SYNC_REFRESH_FROM_SERVER;
    else if (strcmp(syncMode,"refresh-client") == 0 ||      // deprecated
             strcmp(syncMode,"refresh-from-client") == 0)
        return SYNC_REFRESH_FROM_CLIENT;
    //--------- Funambol extension --------------------
    else if (strcmp(syncMode,"smart-one-way-from-client") == 0)
        return SYNC_SMART_ONE_WAY_FROM_CLIENT;
    else if (strcmp(syncMode,"smart-one-way-from-server") == 0)
        return SYNC_SMART_ONE_WAY_FROM_SERVER;
    else if (strcmp(syncMode,"incremental-smart-one-way-from-client") == 0)
        return SYNC_INCREMENTAL_SMART_ONE_WAY_FROM_CLIENT;
    else if (strcmp(syncMode,"incremental-smart-one-way-from-server") == 0)
        return SYNC_INCREMENTAL_SMART_ONE_WAY_FROM_SERVER;
    else if (strcmp(syncMode, "addrchange") == 0)
        return SYNC_ADDR_CHANGE_NOTIFICATION;
    return SYNC_NONE;
}

const char *syncModeKeyword(SyncMode syncMode) {
    switch (syncMode) {
        case SYNC_SLOW:
            return "slow";
        case SYNC_TWO_WAY:
            return "two-way";
        case SYNC_ONE_WAY_FROM_SERVER:
            return "one-way-from-server";
        case SYNC_ONE_WAY_FROM_CLIENT:
            return "one-way-from-client";
        case SYNC_REFRESH_FROM_SERVER:
            return "refresh-from-server";
        case SYNC_REFRESH_FROM_CLIENT:
            return "refresh-from-client";
        case SYNC_ADDR_CHANGE_NOTIFICATION:
            return "addrchange";
        case SYNC_NONE:
            return "none";
        case SYNC_TWO_WAY_BY_SERVER:
            return "two-way-by-server";
        case SYNC_ONE_WAY_FROM_CLIENT_BY_SERVER:
            return "one-way-from-client-by-server";
        case SYNC_REFRESH_FROM_CLIENT_BY_SERVER:
            return "refresh-from-client-by-server";
        case SYNC_ONE_WAY_FROM_SERVER_BY_SERVER:
            return "one-way-from-server-by-server";
        case SYNC_REFRESH_FROM_SERVER_BY_SERVER:
            return "refresh-from-server-by-server";
        case SYNC_SMART_ONE_WAY_FROM_CLIENT:
            return "smart-one-way-from-client";
        case SYNC_SMART_ONE_WAY_FROM_SERVER:
            return "smart-one-way-from-server";
        case SYNC_INCREMENTAL_SMART_ONE_WAY_FROM_CLIENT:
            return "incremental-smart-one-way-from-client";
        case SYNC_INCREMENTAL_SMART_ONE_WAY_FROM_SERVER:
            return "incremental-smart-one-way-from-server";
    }

    return "";
}


SyncItemStatus** toSyncItemStatusArray(ArrayList& items) {

    int l = items.size();

    if (l < 1) {
        return NULL;
    }

    SyncItemStatus** itemArrayStatus = new SyncItemStatus*[l];

    for (int i=0; i<l; ++i) {
        itemArrayStatus[i] = (SyncItemStatus*)((ArrayElement*)items[i])->clone();
    }

    return itemArrayStatus;
}


SyncItem** toSyncItemArray(ArrayList& items) {
    int l = items.size();

    if (l < 1) {
        return NULL;
    }

    SyncItem** itemArray = new SyncItem*[l];

    for (int i=0; i<l; ++i) {
        itemArray[i] = (SyncItem*)((ArrayElement*)items[i])->clone();
    }

    return itemArray;
}

/*
 * Encode the message in base64, splitting the result in lines of 72 columns
 * each.
 */
char *uuencode(const char *msg, int len)
{
    int i, step=54, dlen=0;

    char *ret = new char[ len * 2 ]; // b64 is 4/3, but we have also the newlines....
    for(i=0; i<len; i+=step) {
        if(len-i < step)
            step = len-i;
        dlen += b64_encode(ret+dlen, (void *)(msg+i), step);
        if(getLastErrorCode() != 0){
            delete [] ret;
            return NULL;
        }
        ret[dlen++]='\n';
    }

    // Terminate the string
    ret[dlen]=0;
    return ret;
}

// Get a line from the char buffer msg
// line endings are discarded
// Return the first character after the newline
static const char *getLine(const char *msg, char **line) {
    // Null message?
    if (!msg)
        return 0;
    // End of string
    if ( *msg == 0)
        return 0;

    const char *next = strpbrk(msg, "\r\n\0");
    int linelen;

    if(!next) {
        linelen = strlen(msg);
        next = msg+linelen;
    }
    else
        linelen = next-msg;

    *line= new char[linelen+1];
    strncpy(*line, msg, linelen );
    (*line)[linelen]=0;

    while (*next == '\r' || *next == '\n') {
        next++;
    }
    return next;
}

char* b64EncodeWithSpaces(const char *msg, int len) {
    int i, step=54, dlen=0;
    
    char* res = new char[len*3]; // b64 is 4/3, but we have also the newlines....
    memset(res, 0, len*3);
    res[0] = ' ';
    res[1] = ' ';
    res[2] = ' ';
    res[3] = ' ';
    char* ret = &res[4];
    for(i=0; i<len; i+=step) {
        if(len-i < step) {
            step = len-i;
        }
        dlen += b64_encode(ret+dlen, (void *)(msg+i), step);
        ret[dlen++]='\r';
        ret[dlen++]='\n';
        ret[dlen++]=' ';
        ret[dlen++]=' ';
        ret[dlen++]=' ';
        ret[dlen++]=' ';
    }
    
    // Terminate the string
    ret[dlen]=0;
    int ll = strlen(res);
    return res;
}


// This functions works for standard encoded files with new line every
// 72 characters. It does not work if the line length is not multiple of 4.
int uudecode(const char *msg, char **binmsg, size_t *binlen)
{
    // Convert the string
    const char *buf = msg;
    if (!buf)
        return -1;

    const char *cursor = buf;
    char *line;
    // Make room for the destination (3/4 of the original)
    int outlen = strlen(buf)/4 * 3 + 1;
    char *out = new char[outlen+1];
    memset(out, 0, outlen);
    int len = 0, nl=0;

    while( (cursor=getLine(cursor, &line)) != 0) {
        if (strstr(line, "]]") != 0)
            break;
        nl++;
        len += b64_decode(out+len, line);
        if ( getLastErrorCode() != 0){
            delete [] line;
            return -1;
        }

        delete [] line;
    }
    //delete [] buf;
    // Terminate the string
    out[len]=0;
    // Set return parameters
    *binmsg = out;
    *binlen = len;
    return 0;
}

char *loadAndConvert(const char *filename, const char *encoding)
{
    char *msg = 0;
    bool binary = true;
    size_t msglen=0;
    char *ret = 0;

    if(!filename)
        return 0;

    if( strcmp(encoding, "base64") == 0 ) {
        binary = true;
    }

    // Read file
    if(!readFile(filename, &msg, &msglen, binary))
        return 0;
    // Encode the file
    if( strcmp(encoding, BASE64) == 0 ) {
        ret = uuencode(msg, msglen);
        delete [] msg;
    }
    else if( strcmp(encoding, QUOTED_PRINTABLE) == 0 ) {
        if(qp_isNeed(msg))
            ret = qp_encode(msg);
        delete [] msg;
    }
    else {  // Default 8bit
        ret = msg;
    }
    return ret;
}

int convertAndSave(const char *filename,
                   const char *s,
                   const char *encoding)
{
    char *buf, *name = stringdup(filename);
    bool binary = true;
    size_t len;

    if(!name)
        return -1;

    // Decode the file
    if( strcmp(encoding, BASE64) == 0 ) {
        if( uudecode(s, &buf, &len) ) {
            return -1;
        }
        binary = true;
    } else if( strcmp(encoding, QUOTED_PRINTABLE) == 0 ) {
        if (s == NULL)
            return -1;

        buf = qp_decode(s);
        len = strlen(buf);
        binary = true;
    }
    else {      // Default UTF-8
        buf = stringdup(s);
        len = strlen(buf);
    }
    saveFile(name, buf, len, binary);
    delete [] buf;
    delete [] name;
    return 0;
}

const char* getSourceName(const char *uri)
{
#if 0
// FIXME
    char nodeName = new char[];
    strcpy(nodeName, rootContext); strcat(nodeName, CONTEXT_SPDS_SOURCES);

    node = dmt->readManagementNode(nodeName);
    if ( ! node ) {
        //lastErrorCode = ERR_INVALID_CONTEXT;
        //sprintf(lastErrorMsg, ERRMSG_INVALID_CONTEXT, nodeName);
        setErrorf(ERR_INVALID_CONTEXT, ERRMSG_INVALID_CONTEXT, nodeName);
        goto finally;
    }
    n = node->getChildrenMaxCount();
    for()
#else
        // FIXME
        return stringdup(uri);
#endif
}

int indent(StringBuffer& content, int space){
    
    StringBuffer buf;
    char* startingBuf = new char[space +1];
    memset(startingBuf, ' ', space);
    startingBuf[space] = 0;
    buf = startingBuf;

    char* spacebuf = new char[space +2];
    spacebuf[0] = '\n';
    memset((spacebuf+1), ' ', space);
    spacebuf[space+1] = 0;
    content.replaceAll("\n", spacebuf);
    buf.append(content);
    content = buf;
    delete [] spacebuf;
    delete [] startingBuf;
    return 0;
}

END_NAMESPACE

