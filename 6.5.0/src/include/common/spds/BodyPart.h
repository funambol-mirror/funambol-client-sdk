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
#ifndef INCL_BODY_PART
#define INCL_BODY_PART
/** @cond DEV */

#include "base/util/ArrayElement.h"
#include "base/util/StringBuffer.h"

class BodyPart : public ArrayElement {
    private:
        StringBuffer mimeType;
        StringBuffer encoding;
        StringBuffer charset;
        StringBuffer content;
        StringBuffer disposition;
        StringBuffer name;
        StringBuffer filename;

    public:

        BodyPart();

        // The mime type as specified by MIME standard
        const char *getMimeType() const ;
        void setMimeType(const char *type) ;

        // The character set: UTF-8
        const char *getCharset() const ;
        void setCharset(const char *cs) ;

        // The content encoding: 7bit, 8bit, base64, quoted-printable
        const char *getEncoding() const ;
        void setEncoding(const char *type) ;

        // The content is the real content for the body
        // or a path name to a temp file for the attachment
        const char *getContent() const ;
        void setContent(const char *cont) ;

        void appendContent(const char *text);

        // For multipart message.
        // Values: inline, attachment
        const char *getDisposition() const ;
        void setDisposition(const char *type) ;

        // For multipart message.
        // It is the name of the file attached (without path)
        const char *getFilename() const ;
        void setFilename(const char *type) ;

        // For multipart message.
        // It is the visible name of the attachement (can be the subject
        // of an attached mail, for instance)
        const char *getName() const ;
        void setName(const char *type) ;

		ArrayElement* clone() ;
};

/** @endcond */
#endif

