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
#include "base/quoted-printable.h"
#include "base/Log.h"
#include "spds/spdsutils.h"
#include "spds/MailMessage.h"
#include "base/globalsdef.h"

USE_NAMESPACE

//------------------------------------------------------------------ Defines

// Headers names
#define NL          "\n"
#define FROM        "From: "
#define TO          "To: "
#define CC          "CC: "
#define BCC         "BCC: "
#define DATE        "Date: "
#define RECEIVED    "Received:"
#define SUBJECT     "Subject: "
#define MIMETYPE    "Content-Type: "
#define CT_NAME     "name="
#define CT_CHARSET  "charset="
#define MIMEVERS    "Mime-Version: "
#define MESSAGEID   "Message-ID: "
#define DISPOSITION "Content-Disposition:"
#define CD_FILENAME "filename="
#define ENCODING    "Content-Transfer-Encoding: "
#define IMPORTANCE  "Importance: "
#define X_PRIORITY  "X-Priority: "

#define IMP_NORMAL  "normal"
#define IMP_HIGH    "high"
#define IMP_LOW     "low"

#define MULTIPART   "multipart/"


// Header names' length
static const unsigned char FROM_LEN = strlen(FROM);
static const unsigned char TO_LEN   = strlen(TO);
static const unsigned char CC_LEN   = strlen(CC);
static const unsigned char BCC_LEN  = strlen(BCC);
static const unsigned char DATE_LEN  = strlen(DATE);
static const unsigned char SUBJECT_LEN = strlen(SUBJECT);
static const unsigned char MIMETYPE_LEN = strlen(MIMETYPE);
static const unsigned char MIMEVERS_LEN = strlen(MIMEVERS);
static const unsigned char MESSAGEID_LEN = strlen(MESSAGEID);
static const unsigned char DISPOSITION_LEN = strlen(DISPOSITION);
static const unsigned char ENCODING_LEN = strlen(ENCODING);
static const unsigned char IMPORTANCE_LEN = strlen(IMPORTANCE);
static const unsigned char X_PRIORITY_LEN = strlen(X_PRIORITY);

//---------------------------------------------------------------- Accessors

const char *MailMessage::getTo() const { return to.c_str(); }
void MailMessage::setTo(const char *to) { this->to = to; }

const char *MailMessage::getFrom() const { return from.c_str(); }
void MailMessage::setFrom(const char *from) { this->from = from; }

const char *MailMessage::getCc() const { return cc.c_str(); }
void MailMessage::setCc(const char *cc) { this->cc = cc; }

const char *MailMessage::getBcc() const { return bcc.c_str(); }
void MailMessage::setBcc(const char *bcc) { this->bcc = bcc; }

//        int addHeader(const char *name, const char *content);
const char *MailMessage::getSubject() const { return subject.c_str(); }
void MailMessage::setSubject(const char *subj) { subject = subj; }

const BasicTime& MailMessage::getDate() const { return date; }
void MailMessage::setDate(const BasicTime& v) { date = v; }

const BasicTime& MailMessage::getReceived() const { return received; }

const char * MailMessage::getContentType() const { return contentType; }
void MailMessage::setContentType(const char *val) { contentType = val; }

const char * MailMessage::getBoundary() const { return boundary; }
void MailMessage::setBoundary(const char *val) { boundary = val; }

const char * MailMessage::getMimeVersion() const { return mimeVersion; }
void MailMessage::setMimeVersion(const char *val) { mimeVersion = val; }

const char * MailMessage::getMessageId() const { return messageId; }
void MailMessage::setMessageId(const char *val) { messageId = val; }

const char* MailMessage::getEntryID() { return entryId.c_str(); }
void MailMessage::setEntryID(const char* id) { entryId = id; }

const char* MailMessage::getImportance() { return importance; }
void MailMessage::setImportance(const char* imp) { importance = imp; }


BodyPart & MailMessage::getBody() { return body; }
void MailMessage::setBody(BodyPart &body) { this->body = body; }

// Alternate representation
//BodyPart * MailMessage::getAlternate() { return alternate; };
//void MailMessage::setAlternate(BodyPart &alt) { alternate = new BodyPart(alt); };

BodyPart * MailMessage::getFirstAttachment() {
    return (BodyPart *)attachments.front();
}

BodyPart * MailMessage::getNextAttachment() {
    return (BodyPart *)attachments.next();
}

int MailMessage::addAttachment(BodyPart &body) {
    return attachments.add(body);
}

int MailMessage::attachmentCount() {
    return attachments.size();
}

MailMessage::MailMessage() {

    newline = NULL;

    to = NULL;
    from = NULL;
    cc = NULL;
    bcc = NULL;
    subject = NULL;

    contentType = NULL;
    boundary = NULL;
    mimeVersion = NULL;
    messageId = NULL;
    entryId = NULL;
    importance = NULL;
}
//----------------------------------------------------------- Static Functions

static StringBuffer formatBodyPart(const BodyPart &part)
{
    StringBuffer ret;    
    LOG.debug("FormatBodyPart START");

    ret = MIMETYPE;
    
    ret += part.getMimeType(); ret += ";";
    if (!part.getFilename()) {
        LOG.debug("It doesn't contains an attachment. It is the body");
        ret +=" "; ret += CT_CHARSET; ret += part.getCharset(); 
    }
    ret += NL;
    
    if( part.getFilename() ) {
        ret += "        "; ret += CT_NAME; ret += "\""; ret += part.getFilename(); ret += "\"\n";
    }
    if( part.getEncoding() ) {
        ret += ENCODING; ret += part.getEncoding(); ret += NL;
    }
    if( part.getFilename() ) {
        if( part.getDisposition() ) {
            ret += DISPOSITION; ret += part.getDisposition(); ret += ";\n";
        }
        else {
            ret += DISPOSITION; ret += "attachment;\n";
        }

        ret += "      "; ret += CD_FILENAME; ret += "\""; ret += part.getFilename();
        ret += "\"\n";
    }
    // End of part headers
    ret += NL;
    // Content
    if( part.getFilename() ) {
        char *content = loadAndConvert(part.getContent(), part.getEncoding());
        ret += content;
        delete [] content;
    }
    else
        ret += part.getContent();

    LOG.debug("FormatBodyPart END");
    return ret;
}

inline static size_t findNewLine(StringBuffer &str, size_t offset) {
    size_t nl = str.find("\n", offset)+1;
    if(nl == StringBuffer::npos)
        return nl;
    return (str[(int)nl] == '\r') ? nl+1 : nl ;
}

static size_t getHeadersLen(StringBuffer &s, StringBuffer &newline)
{
    // detect the newline used in headers
    size_t pos1 = s.find("\n");
    if(pos1 == StringBuffer::npos){
        LOG.error("MailMessage: no newlines in message?");
        return pos1;
    }
    size_t pos2 = pos1 + 1 ;

    while (s[(int)(pos1-1)] == '\r'){
        pos1--;
    }
    newline = s.substr(pos1, pos2-pos1);

    StringBuffer emptyline = newline + newline ;

    // Split headers and body
    size_t hdrlen = s.find(emptyline);
    if(hdrlen == StringBuffer::npos) {
        // Empty body, get the message anyway.
        hdrlen = s.length();
    }
    return hdrlen;
}


static StringBuffer getTokenValue(const StringBuffer* line, const char* token, bool toLower = true) {

    StringBuffer ret("");
    if (line->ifind(token) == StringBuffer::npos)
        return ret;

    size_t begin = line->ifind(token) + strlen(token);
    size_t end = begin;
    size_t quote = line->find("\"", begin);
    size_t semicolon = line->find(";", begin);

    if (quote != StringBuffer::npos){
        if (semicolon != StringBuffer::npos) {
            if (quote < semicolon) {
                begin = quote + 1;
                end = line->find("\"", begin) ;
            } else {
                end = line->find(";", begin) ;
            }
        } else {
            begin = quote + 1;
            end = line->find("\"", begin) ;
        }
    } else {
        end = line->find(";", begin) ;
        if (end == StringBuffer::npos) {
            end = line->find(" ", begin);
        }
    }
    ret = line->substr(begin, end-begin);
    if (toLower) {
        ret = ret.lowerCase();
    }
    return ret;
}


StringBuffer MailMessage::decodeHeader(StringBuffer line) {

    if (!line || line.empty()) {
        return line;
    }
    
    size_t startPos = 0;
    StringBuffer ret;
    StringBuffer charset;
    while( (startPos = line.find("=?", startPos)) != StringBuffer::npos) {
        // Skip the '=?'
        startPos += 2;
        // Find the first '?'
        size_t firstMark = line.find("?", startPos);
        if (firstMark == StringBuffer::npos) {
            LOG.error("Invalid encoded header");
            return line;
        }
        // Find the second '?'
        size_t secondMark = line.find("?", firstMark+1);
        if (secondMark == StringBuffer::npos) {
            LOG.error("Invalid encoded header");
            return line;
        }
        // Find the final '?='
        size_t endPos = line.find("?=", secondMark+1);
        if (endPos == StringBuffer::npos) {
            LOG.error("Invalid encoded header");
            return line;
        }

        charset = line.substr(startPos, firstMark - startPos);
        StringBuffer encoding = line.substr(firstMark+1, secondMark - (firstMark + 1));
        StringBuffer text = line.substr(secondMark+1, endPos - (secondMark + 1));

        if (encoding.icmp("Q")) {
            // quoted-printable
            text.replaceAll("_", " ");
            char* dec = qp_decode(text);
            if (startPos >= 2 &&  ret.length() == 0) {
                ret += line.substr(0, startPos - 2);
            }

            ret += dec;
            delete [] dec;
        }
        else if (encoding.icmp("B")){
        // base64
            char* dec = new char[text.length()];
            int len = b64_decode((void *)dec, text);
            dec[len]=0;

            if (startPos >= 2 &&  ret.length() == 0) {
                ret += line.substr(0, startPos - 2);
            }
            ret += dec;
            delete [] dec;
        }

        startPos = endPos;
    }

    if (ret.length() == 0) {
        ret += line;
    }

    WCHAR* wret = toWideChar(ret, charset);    
    ret.set(NULL);
    char* t = toMultibyte(wret);
    ret.set(t);
    if (wret) {delete [] wret;}
    if (t) {delete [] t;}
    return ret;
}
/**
 * Get the next bodypart from the message body string.
 *
 * @param rfcBody  (in)  - message content
 * @param boundary (in)  - mime boundary string
 * @param ret      (out) - parsed BodyPart
 * @param next     (i/o) - offset of the new boundary
 * @param isAttach (in)  - says if the current body part is an attachment or not
 */
static bool getBodyPart(StringBuffer &rfcBody, StringBuffer &boundary,
                       BodyPart &ret, size_t &next, bool isAttach)
{
    LOG.debug("getBodyPart START");
    StringBuffer newline;

    // The part starts on the next line
    size_t begin = findNewLine(rfcBody, next);
    if (begin == StringBuffer::npos)
       return false;
    // find the end of the part
    next = rfcBody.find(boundary, begin);
    if (next == StringBuffer::npos)
       return false;
    // get the part
    StringBuffer part = rfcBody.substr(begin, next-begin);
    // If it is a multipart alternative part, get the text part only.
    // check only until the first new line not on all the message (it could be
    // a message inside another message)
    size_t headers_len = getHeadersLen(part, newline);
    StringBuffer headers_part = part.substr(0, headers_len);
    if (headers_part.ifind("Content-Type: multipart/alternative") != StringBuffer::npos) {
        if(part.ifind("Content-Type: multipart/alternative") != StringBuffer::npos) {
            size_t b_pos = part.ifind("boundary=");
            if( b_pos != StringBuffer::npos ) {
                size_t begin = part.find("=\"", b_pos) + 2 ;
                size_t end = part.find("\"", begin) ;

                StringBuffer inner_boundary("\n--");
                inner_boundary += part.substr( begin, end-begin );

                begin = part.find(inner_boundary, end);
                begin += inner_boundary.length();
                end = part.find(inner_boundary, begin);
                if (begin != StringBuffer::npos && end != StringBuffer::npos) {
                    part = part.substr(begin, end-begin);
                    LOG.debug("Bodypart is multipart/alternative: "
                        "getting first alternative only: \n%s\n", part.c_str() );
                }
            }
        }
    }

    // Split headers and body
    size_t hdrlen = getHeadersLen(part, newline);

    // Get headers
    StringBuffer headers = part.substr(0, hdrlen);

    // Join header parts using \t or 8 blank
    StringBuffer joinlinetab("\t");
    headers.replaceAll(joinlinetab, " ");
    StringBuffer joinlinespaces(newline);
    joinlinespaces+=" ";  // 8 blanks
    headers.replaceAll(joinlinespaces, " ");

    ArrayList lines;
    const StringBuffer *line;

    // parse the bodypart headers
    headers.split(lines, newline);

    for ( line=(StringBuffer *)lines.front();
          line;
          line=(StringBuffer *)lines.next() ) {
        if( *line == "\r" )
            continue;
        // The first empty line marks the end of the header section
        //if( line->empty() ){
        //    break;
        //}
        // Process the headers
        if( line->ifind(MIMETYPE) == 0 ) {  // it must at the beginning
            ret.setMimeType(getTokenValue(line, MIMETYPE));
            if (line->ifind(CT_NAME) != StringBuffer::npos) {
                ret.setName(MailMessage::decodeHeader(getTokenValue(line, CT_NAME,false)));
            }
            if (line->ifind(CT_CHARSET) != StringBuffer::npos ) {
                ret.setCharset(getTokenValue(line, CT_CHARSET));
            }
        }   
        else if( line->ifind(DISPOSITION) == 0 ) {
            ret.setDisposition( getTokenValue(line, DISPOSITION));
            if (line->ifind(CD_FILENAME) != StringBuffer::npos ) {
                ret.setFilename( MailMessage::decodeHeader(  getTokenValue(line, CD_FILENAME, false) ) );
            }
        }

        else if( line->ifind(ENCODING) == 0 ) {
            ret.setEncoding( getTokenValue(line, ENCODING));
        }

    }
    // move to the beginning of the content
    hdrlen += strlen(newline) + strlen(newline); // added 2 new line that separate the bodyparts
    // get bodypart content
    if( isAttach == false) { // || !ret.getFilename() ) {
        // this is not an attachment
        if(ret.getEncoding() && strcmp(ret.getEncoding(), "quoted-printable") == 0 ) {
            char *decoded = qp_decode( part.substr(hdrlen) );
            ret.setContent ( decoded );
            delete [] decoded;
        }
        else if (ret.getEncoding() && strcmp(ret.getEncoding(), "base64") == 0 ) {
            char *decoded = "";
            size_t len = 0;
            if( uudecode( part.substr(hdrlen), &decoded, &len ) ) {
                LOG.error("Error decoding content");
            }
            ret.setContent ( decoded );
            delete [] decoded;
        }
        else {
            bool found = true;
            if (part.substr(hdrlen).length() < 6) {
                StringBuffer s(part.substr(hdrlen));
                for (unsigned int i = 0; i < s.length(); i++) {
                    if (s.c_str()[i] != '\r' && s.c_str()[i] != '\n') {
                        found = true;
                        break;
                    } else {
                        found = false;
                    }
                }
            }
            if (found) {
                ret.setContent ( part.substr(hdrlen) );
            }
        }
    }
    else {
        LOG.debug("Attachment");
        ret.setContent( mkTempFileName( ret.getFilename() ) );
        LOG.debug("%s", ret.getContent());
        StringBuffer p = part.substr(hdrlen);
        if (p.length()) {
            LOG.debug("Saving...");
            if( convertAndSave(ret.getContent(), p.c_str(), ret.getEncoding()) ) {
                LOG.error("Error in convertAndSave");
            }
            else {
                LOG.debug("convertAndSave success");
            }
        }
    }
    LOG.debug("getBodyPart END");

    // return true if there are more parts
    return (next != StringBuffer::npos);
}

static void generateBoundary(StringBuffer& boundary)
{
    char buf[40];
    int i;

    *buf = '=';
    memset(buf+1, '-', 9*sizeof(char));
    for(i=10; i<36; i++) {
        buf[i] = '0' + rand() % 10;
    }
    buf[i]=0;
    boundary = buf;
}

static bool isAscii(const char *str){
    if(!str)
        return true;

    for(size_t i = 0; i < strlen(str); i++) {
        if (str[i] < 32 || str[i] > 126 ){
            return false;
        }
    }
    return true;
}

/**
* return the Importance tag given the importance. For MS pocket outlook
* the xPriority is 1-high, 3-normal, 5-low. At the moment it return
* the same value because also the client set the same values
*/
static StringBuffer convertForXPriority(StringBuffer importance) {
       
    return importance;
}

/**
* return the X-Priority tag given the importance. For MS pocket outlook
* the importance is 1-high, 3-normal, 5-low. It convert the long value
* into the text.
*/
static StringBuffer convertForImportance(StringBuffer importance) {
    
    StringBuffer ret(IMP_NORMAL);
    if (importance == "1") {
        ret = IMP_HIGH;
    } else if (importance == "5") {
        ret = IMP_LOW;
    } 
    return ret;
}

/*
* It encodes if needed and folds
*/
StringBuffer encodeHeader(StringBuffer line){

    if(isAscii(line))
        return line;

    StringBuffer ret;
    StringBuffer tmp;
    StringBuffer startPattern("=?utf-8?Q?");
    StringBuffer endPattern("?=");
    StringBuffer foldingPattern("\r\n ");
    int foldingLen = 64;

    char* qp = 0;
    qp = qp_encode(line);

    tmp += startPattern;
    tmp += qp;
    delete [] qp;

    // folding action
    unsigned long p = 0;
    while(p + foldingLen < tmp.length()) {
        ret.append(tmp.substr(p, foldingLen));
        ret.append(foldingPattern);
        ret.append(startPattern);
        p += foldingLen;
    }

    if (ret.length() > 0)
        tmp.append(tmp.substr(p, tmp.length() - p));

    ret = tmp;
    ret.append(endPattern);

    return ret;

}




//----------------------------------------------------------- Public Methods

/**
 * Format a mailmessage in a RFC2822 string
 */
char * MailMessage::format() {

    StringBuffer ret;
    // If the message is empty, return null
    if ( empty() ) {
        if(messageId){
            ret += MESSAGEID; ret += messageId; ret += NL;
            return stringdup(ret.c_str());
        }
        LOG.debug("MailMessage::format: empty message.");
        return 0;
    }

    LOG.debug("MailMessage::format START");

    if ( contentType.empty() ) {
        if ( attachments.size() ) {
            contentType = "multipart/mixed";
        }
        else {
            contentType = body.getMimeType();

            if (headers.size() > 0) {
                StringBuffer *line; int j = 0;
                for (line=(StringBuffer *)headers.front(); line; line=(StringBuffer *)headers.next() ) {
                    if (strstr(line->c_str(), "format=") != 0
                        || strstr(line->c_str(),"reply-type=") != 0 ) {
                            contentType.append("; ");
                            line->replaceAll(";", " ");
                            contentType.append(line->c_str());
                            headers.removeElementAt(j);
                            j--;
                         }
                     j++;
                }
            }

        }
    }
    if ( mimeVersion.empty() ) {
        mimeVersion = "1.0";
    }

    // Add generics headers
    ret.join((ArrayList &)headers, NL);
    // Add parsed headers
    ret += MIMEVERS; ret += mimeVersion; ret += NL;
    ret += MESSAGEID; ret += messageId; ret += NL;
    LOG.debug("MailMessage: From: %s\n", from.c_str());
    ret += FROM; ret += from; ret += NL;
    ret += TO; ret += to; ret += NL;
    if (cc.length() ) {
        ret += CC; ret += cc; ret += NL;
    }
    if (bcc.length() ) {
        ret += BCC; ret += bcc; ret += NL;
    }
    ret += DATE; ret += date.formatRfc822(); ret += NL;
    ret += SUBJECT;

    ret += encodeHeader(subject);

    ret += NL;

    // add priority
    ret += IMPORTANCE; ret += convertForImportance(importance); ret += NL;
    ret += X_PRIORITY; ret += convertForXPriority(importance); ret += NL;


    ret += MIMETYPE; ret += contentType; ret+= "; ";
    if (contentType.ifind(MULTIPART) != StringBuffer::npos ){
        if ( boundary.empty() ) {
            generateBoundary(boundary);
        }
        ret += "\n boundary=\""; ret += boundary;
        ret += "\"\n\nThis is a multi-part message in MIME format.\n";
        // Prepare a string with the boundary on a line alone
        StringBuffer bound = "\n--"; bound += boundary;
        // Body
        ret += bound; ret += NL;
        ret += formatBodyPart(body);
        ret += bound;
        // Attachments
        const BodyPart *part;
        for ( part=(const BodyPart *)attachments.front();
              part;
              part=(BodyPart *)attachments.next() ) {
            ret += NL;
            ret += formatBodyPart(*part);
            ret += bound;
        }
        ret += "--\n";
    }
    else {
        // Body
        if(body.getCharset())
            ret += CT_CHARSET; ret += body.getCharset(); ret += NL;
        if( body.getEncoding() )
            ret += ENCODING; ret += body.getEncoding();
        // end of headers
        ret += NL;
        ret += NL;
        ret += body.getContent(); ret += NL;
    }
    LOG.debug("MailMessage::format END");
    return stringdup(ret.c_str());
}


int MailMessage::parse(const char *rfc2822, size_t len) {
    StringBuffer s(rfc2822, len);
    int rc;

    LOG.debug("MailMessage::parse START");

    size_t hdrlen = getHeadersLen(s, newline);

    StringBuffer headers = s.substr(0, hdrlen);    
    StringBuffer rfcbody;

    rc = parseHeaders(headers);
    if(rc)
        return rc;

    if(contentType.ifind(MULTIPART) != StringBuffer::npos) {
        // Multipart message
        rfcbody = s.substr(hdrlen);
        rc= parseBodyParts(rfcbody);
    }
    else {
        // go to the beginning of the body
        hdrlen = hdrlen + strlen(newline) + strlen(newline);
        rfcbody = s.substr(hdrlen);
        body.setMimeType(contentType);
        // FIXME: handle all encodings, not only quoted-printable
        if( strcmp(body.getEncoding(), "quoted-printable") == 0 ) {
            char *decoded = qp_decode( rfcbody );
            body.setContent ( decoded );
            delete [] decoded;
        }
        else if ( strcmp(body.getEncoding(), "base64") == 0 ) {
            char *decoded = NULL;
            size_t len = 0;
            rc = uudecode( rfcbody, &decoded, &len ) ;
            if( !rc ) {
                body.setContent ( decoded );
                delete [] decoded;
            }
        }
        else body.setContent(rfcbody);
    }

    LOG.debug("MailMessage::parse END");
    return rc;
}




//---------------------------------------------------------- Private Methods

StringBuffer convertImportance(StringBuffer data) {
    
    StringBuffer ret("3");
    if (data == IMP_HIGH) {
        ret = "1";
    } else if (data == IMP_LOW) {
        ret = "5";
    }
    return ret;
}

StringBuffer convertXPriority(StringBuffer data) {
    
    StringBuffer ret("3");
    if (data.ifind("1") == 0 || data.ifind("2") == 0) {
        ret = "1";
    } else if (data.ifind("4") == 0 || data.ifind("5") == 0) {
        ret = "5";
    }
    return ret;
}

int MailMessage::parseHeaders(StringBuffer &rfcHeaders) {

    ArrayList lines;
    const StringBuffer *line;
    StringBuffer strReceived;
    bool receivedExtracted = false;
    LOG.debug("parseHeaders START");

    // Join header parts using \t or 8 blank
    StringBuffer joinlinetab("\t");
    rfcHeaders.replaceAll(joinlinetab, " ");

    StringBuffer joinlinespaces(newline);
    joinlinespaces+=" ";  // 8 blanks

    rfcHeaders.replaceAll(joinlinespaces, " ");

    rfcHeaders.split(lines, newline);

    // importance is set to "0" by default. Then there isn't anything modification
    // in the header, at the end of the loop the importance will be set to "3", default normal
    importance = "0";

    for ( line=(StringBuffer *)lines.front();
          line;
          line=(StringBuffer *)lines.next() ) {

        if( *line == "\r" )
            break;
        // The first empty line marks the end of the header section
        if( line->empty() ){
            break;
        }
        // Process the headers

        if( line->ifind(TO) == 0 ){
            to = MailMessage::decodeHeader(line->substr(TO_LEN));
        }
        else if( line->ifind(FROM) == 0 ) {
            from = MailMessage::decodeHeader(line->substr(FROM_LEN));
        }
        else if( line->ifind(CC) == 0 ) {
            cc = MailMessage::decodeHeader(line->substr(CC_LEN));
        }
        else if( line->ifind(BCC) == 0 ) {
            bcc = MailMessage::decodeHeader(line->substr(BCC_LEN));
        }
        else if ( line->ifind(DATE) == 0 ) {
            //subjectParsing = false;
            if( date.parseRfc822(line->substr(DATE_LEN)) ) {
                LOG.error("Error parsing date");
                return 500;
            }
        }
        else if( line->ifind(SUBJECT) == 0 ) {

            subject = MailMessage::decodeHeader(line->substr(SUBJECT_LEN));
            LOG.debug("SUBJECT: %s", subject.c_str());
        }
        else if( line->ifind(ENCODING) == 0 ) {  // it is here for single part only
            body.setEncoding(line->substr(ENCODING_LEN));
        }
        else if(line->ifind(MIMEVERS) == 0 ) {
            mimeVersion = line->substr(MIMEVERS_LEN);
        }
        else if(line->ifind(MESSAGEID) == 0 ) {
            messageId = line->substr(MESSAGEID_LEN);
        }
        else if(line->ifind(IMPORTANCE) == 0 ) {
            StringBuffer data = line->substr(IMPORTANCE_LEN);
            data.lowerCase();
            importance = convertImportance(data);
        }
        else if(line->ifind(X_PRIORITY) == 0 ) {
            if (importance == "0") {
                StringBuffer data = line->substr(X_PRIORITY_LEN);
                data.lowerCase();
                importance = convertXPriority(data);
            }            
        }
        else if( line->ifind(MIMETYPE) == 0 ) {

            StringBuffer sb = getTokenValue(line, MIMETYPE);

            if (sb.length() > 0)
                contentType = sb;

            sb.reset();
            sb = getTokenValue(line, "boundary=", false);

            if (sb.length() > 0) {
                boundary = sb;
            } else {
                body.setCharset(getTokenValue(line, CT_CHARSET));
            }
            /*

            size_t len = line->find(";") - MIMETYPE_LEN ;
            contentType = line->substr(MIMETYPE_LEN, len);

            // Save boundary for multipart
            size_t begin = line->ifind("boundary=");
            size_t end = StringBuffer::npos;

            if( begin != StringBuffer::npos ) {
                begin += strlen("boundary=\"");
                end = line->find("\"", begin) ;
                boundary = line->substr( begin, end-begin );
            }
            else {
                    begin=line->ifind(CT_CHARSET);
                if( begin != StringBuffer::npos ) {
                    begin += strlen(CT_CHARSET);
                    size_t end = begin;
                    size_t quote = line->find("\"", begin);
                    if (quote != StringBuffer::npos){
                        begin = quote + 1;
                        end = line->find("\"", begin) ;
                    }
                    else {
                        end = line->find(";", begin) ;
                        if (end == StringBuffer::npos) {
                            end = line->find(" ", begin);
                        }
                    }
                    body.setCharset( line->substr( begin, end-begin ) );
                }
            }
            */
        }
        else if(line->ifind(RECEIVED) == 0) {
            if (!receivedExtracted) {
                strReceived = line->substr(line->rfind(";") );

                if (!strReceived.empty()) {
                    received.parseRfc822(strReceived.substr(2));
                    receivedExtracted = true;
                }
                /*
                while (!strReceived.empty()) {
                    if (received.parseRfc822(strReceived.substr(2)) == 0) {
                        receivedExtracted = true;
                        break;
                    } else {
                        StringBuffer s(line->substr(line->rfind(strReceived.c_str())));
                        strReceived = line->substr(s.rfind(";"));
                    }
                }
                */

            }
        }
        else {
            headers.add(*(StringBuffer *)line);
        }

    }
    // If received was not found, copy send date
    if( received == BasicTime() ){
        received = date;
    }

    // if the importance is never set we put the importance to 3, normal
    if (importance == "0") {
        importance = "3";
    }

    LOG.debug("parseHeaders END");

    // FIXME: should check for mandatory headers before return 0
    return 0;
}


int MailMessage::parseBodyParts(StringBuffer &rfcBody) {

    BodyPart part;
    // The boundary is the one defined in the headers preceded by
    // a newline and two hypens
    StringBuffer bound("\n--");
    bound += boundary;

    LOG.debug("parseBodyParts START");

    size_t nextBoundary = rfcBody.find(bound);
    getBodyPart(rfcBody, bound, body, nextBoundary, false);

    if (contentType.ifind("multipart/alternative") == StringBuffer::npos) {
        // If it's not multipart/alternative, get the other parts
        while( getBodyPart(rfcBody, bound, part, nextBoundary, true) ) {
            // some problem in the attachment?
            if( part.getContent() ) {
                attachments.add(part);
            }
            else LOG.error("Empty content in attachment.");
            part = BodyPart();
        }
    }

    LOG.debug("parseBodyParts END");
    return 0;
}

// Return true if the instance is empty (a valid MailMessage must have a valid date that is not
// the default 1/1/1970)
bool MailMessage::empty() {
    return ( this->getDate() == BasicTime() );
}

ArrayElement* MailMessage::clone() {
    return new MailMessage(*this);
}


void MailMessage::setHeaders(const char* chExtraHeaders)
{
    if(chExtraHeaders){
        StringBuffer extraHeaders(chExtraHeaders);
        ArrayList lines;
        extraHeaders.split(headers, "\n");
    }
}

/**
 * The result must be deleted by caller
 */
const char* MailMessage::getHeaders()
{
    if( headers.size() ) {
        StringBuffer buff;
        buff.join(headers, "\n");
        char* strHeaders = stringdup(buff.c_str(), buff.length() -1);
        return strHeaders;
    }
    else return 0;
}

bool MailMessage::operator==(MailMessage& that){
    return (
        this->to == that.to &&
        this->from == that.from &&
        this->cc == that.cc &&
        this->bcc == that.bcc &&
        this->subject == that.subject &&

        this->date == that.date &&
        this->received == that.received &&

        this->contentType == that.contentType &&
        this->boundary == that.boundary &&
        this->mimeVersion == that.mimeVersion &&
        this->messageId == that.messageId
        );
}
