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


#ifndef INCL_MAIL
#define INCL_MAIL
/** @cond DEV */

#include "base/util/ArrayElement.h"
#include "base/util/StringBuffer.h"
#include "spds/MailMessage.h"
#include "spds/EmailData.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class EmailData : public ArrayElement {

    // ------------------------------------------------------- Private data
    private:
        bool read;
        bool forwarded;
        bool replied;
        StringBuffer received;
        StringBuffer created;
        StringBuffer modified;
        bool deleted;
        bool flagged;

        MailMessage emailItem;

        // these are fields added for the inclusive filters
        class ExtMailData : public ArrayElement {
            public:
                ExtMailData() {
                    attachName = 0;
                    attachSize = 0;
                }
                ~ExtMailData() {
                    if (attachName) {
                        delete [] attachName; attachName = NULL;
                    }
                }
                char*       attachName;
                long        attachSize;
                ArrayElement* clone() {
                    ExtMailData* ret = new ExtMailData();
                    ret->attachName = stringdup(attachName);
                    ret->attachSize = attachSize;
                    return ret;
                }

        } *extMailData;


        unsigned long remainingBodySize;
        unsigned long remainingAttachNumber;
        unsigned long totalEmailSize;
        bool isMailPartial;
        ArrayList* remainingAttachments;

    public:
    // ------------------------------------------------------- Constructors
        EmailData();
        ~EmailData();

    // ---------------------------------------------------------- Accessors
        bool getRead() { return read; }
        void setRead(bool v) { read=v; }

        bool getForwarded() { return forwarded; }
        void setForwarded(bool v) { forwarded=v; }

        bool getReplied() { return replied; }
        void setReplied(bool r) { replied=r; }

        const char * getReceived() { return received; }
        void setReceived(const char * v) { received=v; }

        const char * getCreated() { return created; }
        void setCreated(const char * v) { created=v; }

        const char * getModified() { return modified; }
        void setModified(const char * v) { modified=v; }

        bool getDeleted() { return deleted; }
        void setDeleted(bool v) { deleted=v; }

        bool getFlagged() { return flagged; }
        void setFlagged(bool v) { flagged=v; }

        MailMessage& getEmailItem() { return emailItem; }
        void setEmailItem(const MailMessage& v) { emailItem = v; }

    // ----------------------------------------------------- Public Methods
        int parse(const char *syncmlData, size_t len = StringBuffer::npos) ;
        char *format() ;

        ArrayElement* clone() { return new EmailData(*this); }

        unsigned long getRemainingBodySize() { return remainingBodySize; }
        void setRemainingBodySize(unsigned long v) { remainingBodySize = v; }

        unsigned long getRemainingAttachNumber() { return remainingAttachNumber; }
        void setRemainingAttachNumber(unsigned long v) { remainingAttachNumber = v; }

        unsigned long getTotalEmailSize() { return totalEmailSize; }
        void setTotalEmailSize(unsigned long v) { totalEmailSize = v; }

        bool getIsMailPartial() { return isMailPartial; }

        const char* getAttachmentName(unsigned int index) {
            if (remainingAttachments)
                return ((ExtMailData*)remainingAttachments->get(index))->attachName;
            else
                return NULL;
        }
        unsigned long getAttachmentSize(unsigned int index) {
            if (remainingAttachments)
                return ((ExtMailData*)remainingAttachments->get(index))->attachSize;
            else
                return 0;
        }


};


END_NAMESPACE

/** @endcond */
#endif

