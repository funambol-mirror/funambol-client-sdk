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


#ifndef INCL_MAIL
#define INCL_MAIL
/** @cond DEV */

#include "base/util/ArrayElement.h"
#include "base/util/StringBuffer.h"
#include "spds/MailMessage.h"
#include "spds/EmailData.h"

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
                return NULL;
        }


};

/** @endcond */
#endif

