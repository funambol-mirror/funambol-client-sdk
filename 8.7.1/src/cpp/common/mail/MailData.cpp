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
#include "base/Log.h"
#include "base/util/XMLProcessor.h"
#include "base/quoted-printable.h"
#include "syncml/formatter/Formatter.h"
#include "mail/MailData.h"
#include "base/globalsdef.h"

USE_NAMESPACE


#define EMAIL_READ  "read"
#define EMAIL_FORW  "forwarded"
#define EMAIL_REPL  "replied"
#define EMAIL_TREC  "received"
#define EMAIL_TCRE  "created"
#define EMAIL_TMOD  "modified"
#define EMAIL_DELE  "deleted"
#define EMAIL_FLAG  "flagged"
#define EMAIL_ITEM  "emailitem"
#define EMAIL_ITEM_START  "<emailitem"
#define EMAIL_ITEM_END  "</emailitem>"


static inline bool checkFlag(const char *xml, const char *field)
{
    unsigned int start = 0, end = 0;
    bool ret = false;

    if( XMLProcessor::getElementContent(xml, field, NULL, &start, &end) ) {

        ret = ( strncmp(xml+start, "true", end-start) == 0 ) ;
    }
    return ret;
}

MailData::MailData()
{
    read = false;
    forwarded = false;
    replied = false;
    deleted = false;
    flagged = false;
    remainingBodySize = 0;
    remainingAttachNumber = 0;
    remainingAttachments = NULL;
    extMailData = NULL;
    totalEmailSize = 0;
    isMailPartial = false;

    received = NULL;
    created = NULL;
    modified = NULL;
}

MailData::~MailData()
{
    if (extMailData) {
        delete extMailData;
        extMailData = NULL;
    }
    if (remainingAttachments) {
        remainingAttachments->clear();
        delete remainingAttachments;
        remainingAttachments = NULL;
    }
}

/*
* The parse method returns:
* -1: it means the <emailitem> doesn't containt CDATA section. This could be right
*     if the <email> is an update from server of flags only, as <read>, <forwarded>...
      The rightness must be choosen by the caller. It could be or not an error
*
* -2: it means there is an error into the format of the <emailitem>. It must be treated as an error
*
*/

int MailData::parse(const char *msg, size_t /* len */)
{
    int ret = 0;
    unsigned int start = 0, end = 0;
    isMailPartial = false;

    // Get attributes
    read      = checkFlag(msg, EMAIL_READ);
    forwarded = checkFlag(msg, EMAIL_FORW);
    replied   = checkFlag(msg, EMAIL_REPL);
    deleted   = checkFlag(msg, EMAIL_DELE);
    flagged   = checkFlag(msg, EMAIL_FLAG);

    if( XMLProcessor::getElementContent (msg, EMAIL_TREC, NULL, &start, &end) ) {
        received = StringBuffer(msg+start, end-start);
    }
    else received = "";

    if( XMLProcessor::getElementContent (msg, EMAIL_TCRE, NULL, &start, &end) ) {
        created = StringBuffer(msg+start, end-start);
    }
    else created = "";

    if( XMLProcessor::getElementContent (msg, EMAIL_TMOD, NULL, &start, &end) ) {
        modified = StringBuffer(msg+start, end-start);
    }
    else modified = "";

    // Get content
    StringBuffer itemtmp(msg);
    start = itemtmp.find(EMAIL_ITEM_START);
    end = itemtmp.rfind(EMAIL_ITEM_END);
    if (start != StringBuffer::npos && end != StringBuffer::npos) {
        totalEmailSize = itemtmp.length(); // the size of the current piece of mail
        itemtmp = NULL;
        //if( XMLProcessor::getElementContent(msg, EMAIL_ITEM, NULL, &start, &end) ) {
        StringBuffer item(msg+start, end-start);
        unsigned int startAttr=0, endAttr=0;

        //currently emailitem is not escaped so false!!
        if(XMLProcessor::getElementAttributes(msg, EMAIL_ITEM, &startAttr,
                                              &endAttr, false)) {
            StringBuffer attrlist(msg+startAttr, endAttr-startAttr);
            if(attrlist.ifind("quoted-printable") != StringBuffer::npos) {
                char *decoded = qp_decode(item);
                item = decoded;
                delete [] decoded;
            }
        }
        // item must start with CDATA
        size_t item_start = item.find("![CDATA");
        if(item_start > 50){ // it could be <emailitem ENC="QUOTED-PRINTABLE"><![CDATA[
            LOG.error("MailData: can't find inner CDATA section.");
            return -1;
        }
        size_t item_end = item.rfind("]]>");

        // In emailitem the last &gt; close the CDATA of emailitem tag and is escaped, so it is needed
        // to be found the follow. Usually the first is skipped
        //
        if(item.length() - item_end > 10){
            item_end = item.rfind("]]&gt;");
            if(item.length() - item_end > 10){
                LOG.error("MailData: can't find CDATA end tag.");
                return -2;
            }
        }
        // okay, move the start pointer to the end of
        item_start += strlen("![CDATA[");

        ret=emailItem.parse( item.c_str()+item_start, item_end - item_start );

    }
    else {
        // LOG.info("MailData: no <emailitem> tag.");
        // It is not an error, just log it for info.
    }
    /*
    char tmpExt[] = "<Ext><XNam>x-funambol-body</XNam><XVal>15000</XVal></Ext> \
                     <Ext><XNam>x-funambol-attach-n</XNam><XVal>1</XVal></Ext> \
                     <Ext><XNam>x-funambol-attach</XNam><XVal>att1.txt</XVal><XVal>10000</XVal></Ext>";
    */
    //char tmpExt[] = "";
    // find the Ext stuffs
    if (end != StringBuffer::npos) {
        unsigned int pos = end;
        unsigned int previous = end;
        start = 0, end = 0;
        char* ext = NULL;

        // for try
        //pos = 0;

        //while( (ext = XMLProcessor::copyElementContent(&tmpExt[pos], "Ext", &pos)) ) { // for try
        while( (ext = XMLProcessor::copyElementContent(&msg[pos], "Ext", &pos)) ) {
            char* xnam = XMLProcessor::copyElementContent(ext, "XNam", 0);
            if (!xnam)
                break;

            if (strcmp(xnam, "x-funambol-body") == 0) {
                char* val = XMLProcessor::copyElementContent(ext, "XVal", 0);
                if (val) {
                    setRemainingBodySize(atol(val));
                    totalEmailSize += atol(val);
                    delete [] val; val = NULL;
                    isMailPartial = true;
                }
            } else if (strcmp(xnam, "x-funambol-attach-n") == 0) {
                char* val = XMLProcessor::copyElementContent(ext, "XVal", 0);
                if (val) {
                    setRemainingAttachNumber(atol(val));
                    delete [] val; val = NULL;
                    isMailPartial = true;
                }
            } else if (strcmp(xnam, "x-funambol-attach") == 0) {
                if (!remainingAttachments) {
                    remainingAttachments = new ArrayList();
                }
                extMailData = new ExtMailData();
                unsigned int from = 0;
                int prev = 0;
                char* val = XMLProcessor::copyElementContent(ext, "XVal", &from);
                if (val) {
                    extMailData->attachName = stringdup(MailMessage::decodeHeader(val).c_str());
                    delete [] val; val = NULL;
                }
                prev = from;
                val = XMLProcessor::copyElementContent(&ext[from], "XVal", &from);
                if (val) {
                    extMailData->attachSize = atol(val);
                    totalEmailSize += atol(val);
                    delete [] val; val = NULL;
                }
                from += prev;
                prev = from;
                val = XMLProcessor::copyElementContent(&ext[from], "XVal", &from);
                if (val) {
                    extMailData->attachMime = stringdup(MailMessage::decodeHeader(val).c_str());                    
                    delete [] val; val = NULL;
                }
                from += prev;
                prev = from;
                val = XMLProcessor::copyElementContent(&ext[from], "XVal", &from);
                if (val) {
                    extMailData->attachURL = stringdup(MailMessage::decodeHeader(val).c_str());                    
                    delete [] val; val = NULL;
                }
                if (extMailData->attachName && extMailData->attachSize != 0) {
                    remainingAttachments->add(*extMailData);
                    isMailPartial = true;
                } else {
                    setRemainingAttachNumber(getRemainingAttachNumber() == 0 ? 0 
                        : (getRemainingAttachNumber() - 1));
                }
                delete extMailData;
                extMailData = NULL;
            }

            pos += previous;
            previous = pos;

            if (xnam) {
                delete [] xnam; xnam = NULL;
            }
            if (ext) {
                delete [] ext; ext = NULL;
            }

        }

    }



    return ret;
}

char *MailData::format() {
    StringBuffer out;

    out.reserve(150);

    out = "<Email>\n";
    out += XMLProcessor::makeElement(EMAIL_READ, read);
    out += XMLProcessor::makeElement(EMAIL_FORW, forwarded);
    out += XMLProcessor::makeElement(EMAIL_REPL, replied);
    out += XMLProcessor::makeElement(EMAIL_TREC, received);
    out += XMLProcessor::makeElement(EMAIL_TCRE, created);
    out += XMLProcessor::makeElement(EMAIL_TMOD, modified);
    out += XMLProcessor::makeElement(EMAIL_DELE, deleted);
    out += XMLProcessor::makeElement(EMAIL_FLAG, flagged);
    char *item = emailItem.format();
    if ( item ) {
    out += "<emailitem>\n<![CDATA[";
        out += item;
        delete [] item;
        out += "]]&gt;\n</emailitem>\n";
    }
    out += "</Email>\n";
    return stringdup(out.c_str());
}

