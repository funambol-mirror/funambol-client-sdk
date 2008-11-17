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
#ifndef INCL_MAIL_MESSAGE
    #define INCL_MAIL_MESSAGE
/** @cond DEV */

#include "base/util/ArrayList.h"
#include "base/util/StringBuffer.h"
#include "base/util/BasicTime.h"
#include "spds/BodyPart.h"

/**
 * This class is represent a mail message, and can parse/format
 * it in RFC2822/MIME encoding.
 * Some of the RFC2822 headers are handles as clas fields (To,
 * From, etc.), the others are inserted literally in the headers
 * arraylist. Any string put in this list is inserted in the
 * header section of an outgoing message.
 */

class MailMessage : public ArrayElement {

    private:
        // Used to store the newline format used in the incoming message
        StringBuffer newline;

        //---------------------------------------- Message Headers
        StringBuffer to;
        StringBuffer from;
        StringBuffer cc;
        StringBuffer bcc;

        StringBuffer subject;

        BasicTime date;
        BasicTime received;

        StringBuffer contentType;
        StringBuffer boundary;
        StringBuffer mimeVersion;
        StringBuffer messageId;

        ArrayList headers;

		// can be used by the client to store an internal msg id
		StringBuffer entryId;
        //
		//time_t lastModificationTime;

        BodyPart body;
        //BodyPart *alternate;
        ArrayList attachments;

        int parseHeaders(StringBuffer &rfcHeaders);
        int parseBodyParts(StringBuffer &rfcBody);

    public:

        // Headers
        const char *getTo() const;
        void setTo(const char *to);

        const char *getFrom() const;
        void setFrom(const char *from);

        const char *getCc() const;
        void setCc(const char *cc);

        const char *getBcc() const ;
        void setBcc(const char *bcc);

        const char *getSubject() const ;
        void setSubject(const char *subj);

        const BasicTime& getDate() const ;
        void setDate(const BasicTime& d);

        const BasicTime& getReceived() const ;

        const char * getContentType() const ;
        void setContentType(const char *val);

        const char * getBoundary() const ;
        void setBoundary(const char *val);

        const char * getMimeVersion() const ;
        void setMimeVersion(const char *val);

        const char * getMessageId() const ;
        void setMessageId(const char *val);

		const char*  getEntryID();
		void setEntryID(const char*  id);

        void setHeaders(const char*  extraHeaders);
        const char* getHeaders();

        // Body
		BodyPart & getBody();
		void setBody(BodyPart &body);
        //BodyPart * getAlternate();
        //void setAlternate(BodyPart &alt);

        // Attachment
        BodyPart * getFirstAttachment() ;
        BodyPart * getNextAttachment() ;
        int addAttachment(BodyPart &att);
        int attachmentCount();

        // Conversion methods
        char *format();
        int parse(const char *rfc2822, size_t len = StringBuffer::npos);

        /*
         * Return true if the message is empty
         */
        bool empty();

        ArrayElement* clone();

        bool operator==(MailMessage& that);

};

/** @endcond */
#endif
