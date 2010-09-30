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

package com.funambol.mail;

import java.util.Date;
import java.io.IOException;
import java.io.OutputStream;
import java.io.InputStream;
import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;

import com.funambol.util.Log;
import com.funambol.util.MailDateFormatter;
import com.funambol.util.QuotedPrintable;
import com.funambol.util.StringUtil;
import com.funambol.util.Base64;

import java.io.UnsupportedEncodingException;

/**
 * Provides the functionality to transform a <code>Message</code> object into
 * a string formatted as per RFC 2822/MIME ready to be passed to the transport
 * layer of the email application
 */
public class MIMEFormatter {

    // -------------------------------------------------------------- Constants

    private static final String TAG_LOG = "MIMEFormatter";

    private static final String CC = "Cc: ";
    
    private static final String CONTENT_DISPOSITION = "Content-Disposition: ";
    
    private static final String CONTENT_TYPE = "Content-Type: ";
    
    private static final String CHARSET = " charset=";

    private static final String CONTENT_TRANSFER_ENCODING =
        "Content-Transfer-Encoding: ";
    
    private static final String BCC = "Bcc: ";

    private static final String DATE = "Date: ";

    private static final String FILENAME = "filename";
    
    private static final String FROM = "From: ";
    
    private static final String MESSAGE_ID = "Message-ID: ";
    /** The MIME version implemented by this processor */
    private static final String MIME_VERSION = "MIME-Version: 1.0";
    
    private static final String REPLY_TO = "Reply-To: ";

    private static final String SUBJECT = "Subject: ";

    private static final String TO = "To: ";

    private static final String UA = "User-Agent: ";

    private static final String CRLF = "\r\n";
    
    private static final String UTF8        = "UTF-8";
    private static final String ISO_8859_15 = "ISO-8859-15";

    // --------------------------------------------------------- Private Fields

    /**
     * Just an integer to increase the randomness of the boundary string created
     * with #createUniqueBoundaryValue()
     */
    private int part;
        
    // ----------------------------------------------------------- Constructors

    /**
     * Initializes the fields containing a reference to the <code>Message</code>
     * object and to the name of the <code>Folder</code> containing this
     * <code>Message</code> object
     * 
     */
    public MIMEFormatter() {
        part = 0;
    }

    // --------------------------------------------------------- Public Methods

    /**
     * The values of the fields of the <code>Message</code> object referenced
     * by 'mailmessage' are analyzed to be estracted and formatted into a string
     * in the form foreseen by the RFC 2822/MIME specifications
     * 
     */
    public String format(Message mailmessage) {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        try {
            format(mailmessage, baos);
            return baos.toString();
        } catch (IOException ioe) {
            Log.error(TAG_LOG, "Cannot format message to output stream: " + ioe.toString());
            return null;
        }
    }

    /**
     * Writes the Message in RFC2822 format into the StringBuffer out.
     * Message content is encoded if required and according to the specified
     * headers. If the content transfer encoding is set to "base64" then the
     * content is encoded in base64. If the transfer encoding is
     * not specified, but the content contains non printable
     * characters, then it is encoded base64.
     * The subject is encoded only if required (non printable characters) and it
     * is encoded in QuotedPrintable.
     * 
     */
    public void format(Message mailmessage, StringBuffer out) {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        try {
            format(mailmessage, baos);
            out.append(baos.toString());
        } catch (IOException ioe) {
            Log.error(TAG_LOG, "Cannot format message to output stream: " + ioe.toString());
        }
    }

    /**
     * Writes the Message in RFC2822 format into the output stream.
     * Message content is encoded if required and according to the specified
     * headers. If the content transfer encoding is set to "base64" then the
     * content is encoded in base64. If the transfer encoding is
     * not specified, but the content contains non printable
     * characters, then it is encoded base64.
     * The subject is encoded only if required (non printable characters) and it
     * is encoded in QuotedPrintable.
     * 
     */
    public void format(Message mailmessage, OutputStream os) throws IOException {
        // The boundary of a multi-part Message.
        String boundary = "";
        // "--boundary"
        String startboundary = "";
        // "--boundary--" 
        String endboundary = "";

        /* The BodyParts of this multi-part Message */
        StringBuffer partString = null;
        // message ID created in Message()
        String messageID = mailmessage.getMessageId();
        // the subject
        String subject = mailmessage.getSubject();
        if (subject == null){
            subject = "";// we explicitly don't want any "(no subject)" warn
        }
  
        String from = mailmessage.getHeader(Message.FROM);
        
        // origination date field: see RFC 2822, par. 3.6.1
        Date date = mailmessage.getSentDate();
        
        // User-Agent
        String useragent = mailmessage.getHeader(UA);

        String contentTransferEncoding =
            mailmessage.getHeader(Message.CONTENT_TRANSFER_ENCODING);
        
        if(contentTransferEncoding == null) {
            contentTransferEncoding = "8bit";
        }

        // this is only [meta type]/[subtype], e.g. "multipart/mixed" or
        // "text/plain"
        String contentType = mailmessage.getContentType()+';';
       
        // handling multipart messages
        if (mailmessage.getContent() instanceof Multipart) {

            if (mailmessage.isMultipart()) {
                boundary = createUniqueBoundaryValue();
                startboundary = "--" + boundary;
                endboundary = "--" + boundary + "--";
            }
        }

        print(os, DATE + MailDateFormatter.dateToRfc2822(date) + CRLF);

        // The FROM field can be omitted. The email connector will set the 
        // from using the data on the server.
        if(from != null && !"".equals(from)) {
            print(os, FROM + from + CRLF);
        }

        if(useragent != null) {
            print(os, UA + useragent + CRLF);
        }

        // Append MIME-Version implemented by this MIME Processor.
        print(os, MIME_VERSION + CRLF);

        String to = mailmessage.getHeader(Message.TO);
        if (to != null) {
            print(os, TO + StringUtil.fold(to));
        }

        String cc = mailmessage.getHeader(Message.CC);
        if (cc != null && !"".equals(cc)) {
            print(os, CC + StringUtil.fold(cc));
        }

        String replyto = mailmessage.getHeader(Message.REPLYTO);
        if (replyto != null && !"".equals(replyto)) {
            print(os, REPLY_TO + StringUtil.fold(replyto));
        }

        String bcc = mailmessage.getHeader(Message.BCC);
        if (bcc != null && !"".equals(bcc)) {
            print(os, BCC + StringUtil.fold(bcc));
        }

        String qpSubject = encodeQP(subject);

        // Recompute the content transfer encoding for non multipart messages
        if (!mailmessage.isMultipart()) {
            String origTextContent = mailmessage.getTextContent();
            if (origTextContent != null) {
                String textContentEnc = encodeIfRequired(origTextContent, contentTransferEncoding);
                if (textContentEnc.length() != origTextContent.length()) {
                    contentTransferEncoding = "base64";
                }
            }
        }

        StringBuffer chunk = new StringBuffer();
        chunk.append(SUBJECT).append(qpSubject).append(CRLF)
             .append(MESSAGE_ID).append(messageID).append(CRLF)
             .append(CONTENT_TRANSFER_ENCODING).append(contentTransferEncoding).append(CRLF)
             .append(CONTENT_TYPE).append(contentType);
        print(os, chunk.toString());

        chunk = new StringBuffer();
        if(mailmessage.isMultipart()){
            //tag boundary is present only in Multipart messages
            chunk.append(" boundary="+'"'+boundary+'"'+ CRLF)
                 .append(CRLF)
                 .append(startboundary).append(CRLF);
        } else {
            chunk.append(CHARSET+ "UTF-8").append(CRLF+CRLF);
        }
        print(os, chunk.toString());

        formatContent(mailmessage, boundary, contentTransferEncoding, os);
        print(os, CRLF);

        if (mailmessage.isMultipart()) {
            print(os, endboundary + CRLF);
        }
    }

    
    /**
     * 
     * Writes the MessageID in RFC2822 format into the StringBuffer out, e.g.
     * Message-ID: &lt;12345678&gt;
     * note the &lt; and &gt; surrounding the messageid
     * 
     */
    public void formatOnlyMsgID(Message mailmessage, StringBuffer out) {
      
        // message ID created in Message()
        String messageID = mailmessage.getMessageId();
        out.append(MESSAGE_ID + "<" +messageID + ">" + CRLF);
    }
    

    // -------------------------------------------------------- Private Methods

    protected void formatContent(Message mailmessage, String boundary,
                                 String contentTransferEncoding,
                                 OutputStream os)
    throws IOException
    {
        /* The content of a <code>BodyPart</code> */
        String content = null;
        String bodyPartContent = null;

        if (mailmessage.getContent() instanceof Multipart) {

            Multipart multipart = (Multipart)mailmessage.getContent();
            int size = multipart.getCount();
 
            for (int i = 0; i < size; i++) {

                BodyPart bodypart = multipart.getBodyPart(i);
                StringBuffer partString = new StringBuffer();

                /*
                 * TODO: this is the original content type, because it is added
                 * by the MIMEParser to the headers of the body part. But in
                 * BodyPart the method setContent provides default media types
                 * (like "application/octet-stream"), that are stored in the
                 * 'contentType' field but not used here. The implementation of
                 * setContent in BodyPart has to be changed
                 */
                String ct  = bodypart.getContentType();
                String cte = bodypart.getHeader(BodyPart.CONTENT_TRANSFER_ENCODING);
                String cd  = bodypart.getDisposition();
                String fn  = bodypart.getFileName();
                String au  = bodypart.getAttachUrl();

                // adding headers to part (CONTENT_TYPE is "Content-Type: ")
                partString.append(CONTENT_TYPE + ct +";");

                if(bodypart.getContent() instanceof String) {
                    partString.append(CHARSET+"UTF-8");
                }

                // If the content is null and this is a file and we need to
                // explode attachments, then we shall do it
                InputStream contentIS = null;
                boolean encodingRequired = false;

                if (bodypart.getContent() != null) {
                    // adding content to part and fixing content transfer encoding
                    // if a b64 encoding was necessary
                    encodingRequired = isBase64EncodingRequired(bodypart.getContent(), cte);
                    if (encodingRequired) {
                        cte = "base64";
                    }
                }

                partString.append(CRLF)
                          .append(CONTENT_TRANSFER_ENCODING).append(cte).append(CRLF)
                          .append(CONTENT_DISPOSITION).append(cd).append(';');

                if (fn != null) {
                    String encodedFn = encodeQP(fn);
                    partString.append(" ").append(FILENAME).append("=")
                              .append("\"").append(encodedFn).append("\"");
                }

                partString.append(CRLF + CRLF);
                print(os, partString.toString());

                try {                
                    formatBodyPartContent(bodypart, os, encodingRequired);
                } catch (IOException ioe) {
                    Log.error(TAG_LOG, "Cannot copy body part content into output stream");
                    Log.error(TAG_LOG, "part will be truncated");
                }

                // TODO: check if we have to add a blank (probably not)
                print(os, CRLF);
                if (i != size - 1) {
                    print(os, "--" + boundary + CRLF);
                }
            }
        } else {
            // this is a single part, so we expect it to be a string content
            String textContent = mailmessage.getTextContent();

            if (textContent == null) {
                textContent =
                    "Error: the content of the message isn't text!";
                Log.error(TAG_LOG, "The content of the message isn't text!");
            } else {
                if ("base64".equals(contentTransferEncoding)) {
                    byte contentBytes[];
                    try {
                        contentBytes = textContent.getBytes("UTF-8");
                    } catch (UnsupportedEncodingException e) {
                        contentBytes = textContent.getBytes();
                    }
                    byte b64Content[] = Base64.encode(contentBytes);
                    // Base64 content must be formatted properly
                    textContent = new String(b64Content, "UTF-8");
                    textContent = formatBase64Value(textContent);
                }
            }
            print(os, textContent);
        }
    }

    protected void formatBodyPartContent(BodyPart bodypart, OutputStream os, boolean encode)
    throws IOException
    {

        ByteArrayInputStream is = null;
        if (bodypart.getContent() instanceof String) {
            String bodyPartContent = (String) bodypart.getContent();
            try {
                is = new ByteArrayInputStream(bodyPartContent.getBytes("UTF-8"));
            } catch (UnsupportedEncodingException uee) {
                Log.error(TAG_LOG, "Unsupported encoding " + uee.toString());
                is = new ByteArrayInputStream(bodyPartContent.getBytes());
            } 
        } else if (bodypart.getContent() instanceof byte[]) {
            byte contentBytes[] = (byte[])bodypart.getContent();
            is = new ByteArrayInputStream(contentBytes);
        }

        if (is != null) {
            if (encode) {
                // We must perform b64 encoding and formatting on 76 columns
                Base64.encode(is, os, 76, CRLF);
            } else {
                // Copy all the input stream into the output one
                int ch = is.read();
                while (ch != -1) {
                    os.write(ch);
                    ch = is.read();
                }
            }
            is.close();
        }
    }

    private String encodeQP(String value) {
        String res = value;
        try {
            String qp = QuotedPrintable.encode(value, "UTF-8");
            // If the string got quoted printable we must mark the word as
            // encoded per MIME spec
            if (qp.length() != value.length()) {
                StringBuffer newValue = new StringBuffer();

                // QP does not encode question marks, but they are used as
                // separators here, so we need to encode them here
                qp = quotedPrintableEncodeQuestionMarks(qp);

                newValue.append("=?").append(UTF8).append("?Q?")
                          .append(qp).append("?=");
                res = newValue.toString();
            }
        } catch (Exception e) {
            Log.error(TAG_LOG, "Cannot encode string " + value, e);
        }
        return res;
    }


    /**
     * Performs an additional step of quoted printable encoding to encode also
     * question marks as they are special chars in MIME
     */
    private String quotedPrintableEncodeQuestionMarks(String qpSubject) {
        StringBuffer res = new StringBuffer();

        for(int i=0;i<qpSubject.length();++i) {
            char ch = qpSubject.charAt(i);
            if (ch == '?') {
                res.append("=3F");
            } else {
                res.append(ch);
            }
        }
        return res.toString();
    }


    /**
     * Creates a unique boundary as described in the RFC 2046
     * 
     * @return A unique boundary as described in the RFC 2046
     */
    private String createUniqueBoundaryValue() {
        StringBuffer boundary = new StringBuffer();
        boundary.append("--=_Part_").append(part++).append("_").append( 
            boundary.hashCode()).append('.').append(System.currentTimeMillis());
        return boundary.toString();
    }

    private String formatBase64Value(String value) {
        //each line of the base64 block hasn't to exceed 76 chars
        int len = value.length();
        if (len > 76) {
            StringBuffer res = new StringBuffer();
            for (int j = 0, k = j + 76; j < len; j += 76, k = j + 76) {
                if (len - j < 76){
                    k = len;
                }
                res.append(value.substring(j, k));
                res.append(CRLF);
            }
            value = res.toString();
        }
        return value;
    }

    private boolean isB64Required(byte contentBytes[]) {
        // This is not very accurate, but we allow anything between 31 and 127
        for(int i=0;i<contentBytes.length;++i) {
            if (contentBytes[i] < 32  || contentBytes[i] > 127 &&
                contentBytes[i] != 10 && contentBytes[i] != 13) {
                return true;
            }
        }
        return false;
    }

    private String encodeIfRequired(Object content, String cte) {

        byte contentBytes[];
        String bodyPartContent;

        if (content instanceof String) {
            bodyPartContent = (String)content;
            try {
                contentBytes = (byte[]) bodyPartContent.getBytes("UTF-8");
            } catch (Exception e) {
                contentBytes = bodyPartContent.getBytes();
            }
        } else if (content instanceof byte[]) {
            contentBytes = (byte[]) content;
        } else {// TODO: handle recursive multiparts
            bodyPartContent =
                "Warning! The content of this part isn't of a known type!";
            Log.error(TAG_LOG, bodyPartContent);
            try {
                contentBytes = (byte[]) bodyPartContent.getBytes("UTF-8");
            } catch (Exception e) {
                contentBytes = (byte[]) bodyPartContent.getBytes();
            }
        }

        // Check if the user asked for b64 encoding, or b64 encoding is
        // required
        boolean b64 = false;
        if ("base64".equals(cte)) {
            b64 = true;
        } else if (isB64Required(contentBytes)) {
            b64 = true;
        }

        // If b64 is necessary, then we do it
        if (b64) {
            byte b64Content[] = Base64.encode(contentBytes);
            // Base64 content must be formatted properly
            bodyPartContent = new String(b64Content);
            bodyPartContent = formatBase64Value(bodyPartContent);
        } else {
            try {
                bodyPartContent = new String(contentBytes, "UTF-8");
            } catch (Exception e) {
                bodyPartContent = new String(contentBytes);
            }
        }

        return bodyPartContent;
    }

    private boolean isBase64EncodingRequired(Object content, String cte) {

        // Check if the user asked for b64 encoding, or b64 encoding is
        // required
        boolean b64 = false;
        if ("base64".equals(cte)) {
            return true;
        }

        byte contentBytes[];
        if (content instanceof String) {
            String bodyPartContent = (String)content;
            try {
                contentBytes = (byte[]) bodyPartContent.getBytes("UTF-8");
            } catch (Exception e) {
                contentBytes = bodyPartContent.getBytes();
            }
        } else if (content instanceof byte[]) {
            contentBytes = (byte[]) content;
        } else {// TODO: handle recursive multiparts
            String bodyPartContent =
                "Warning! The content of this part isn't of a known type!";
            Log.error(TAG_LOG, bodyPartContent);
            try {
                contentBytes = (byte[]) bodyPartContent.getBytes("UTF-8");
            } catch (Exception e) {
                contentBytes = (byte[]) bodyPartContent.getBytes();
            }
        }

        return isB64Required(contentBytes);
    }


    private void print(OutputStream os, String msg) throws IOException {
        os.write(msg.getBytes());
    }
}
