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
#ifndef INCL_BODY_PART
#define INCL_BODY_PART
/** @cond DEV */

#include "base/util/ArrayElement.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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


END_NAMESPACE

/** @endcond */
#endif

