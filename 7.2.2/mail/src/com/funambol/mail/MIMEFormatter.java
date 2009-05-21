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

    private static final String CC = "Cc: ";
    
    private static final String CONTENT_DISPOSITION = "Content-Disposition: ";
    
    private static final String CONTENT_TYPE = "Content-Type: ";
    
    private static final String CHARSET = " charset=";

    private static final String CONTENT_TRANSFER_ENCODING =
        "Content-Transfer-Encoding: ";
    
    private static final String BCC = "Bcc: ";

    private static final String DATE = "Date: ";

    private static final String FILENAME = "filename=";
    
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
        StringBuffer sb = new StringBuffer();
        format(mailmessage, sb);        
        return sb.toString();
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
        // The boundary of a multi-part Message.
        String boundary = "";
        // "--boundary"
        String startboundary = "";
        // "--boundary--" 
        String endboundary = "";

        /* The textual content of a 'single-part' <code>Message</code> */
        String textContent = null;
        /* The content of a <code>BodyPart</code> */
        String bodyPartContent = null;
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

            Multipart multipart = (Multipart)mailmessage.getContent();
            partString = new StringBuffer();

            int size = multipart.getCount();
           
            for (int i = 0; i < size; i++) {
                BodyPart bodypart = multipart.getBodyPart(i);

                // TODO: handle recursive multiparts (use 'isMultipart()')
                
                /*
                 * TODO: this is the original content type, because it is added
                 * by the MIMEParser to the headers of the body part. But in
                 * BodyPart the method setContent provides default media types
                 * (like "application/octet-stream"), that are stored in the
                 * 'contentType' field but not used here. The implementation of
                 * setContent in BodyPart has to be changed
                 */
                String ct = bodypart.getContentType();
                String cte = bodypart.getHeader(BodyPart.CONTENT_TRANSFER_ENCODING);
                String cd = bodypart.getDisposition();
                String fn = bodypart.getFileName();
        
                // adding headers to part (CONTENT_TYPE is "Content-Type: ")
                partString.append(CONTENT_TYPE + ct +";");
                
                if(bodypart.getContent() instanceof String) {
                    partString.append(CHARSET+"UTF-8");
                }
                        
             
                // adding content to part and fixing content transfer encoding
                // if a b64 encoding was necessary
                bodyPartContent = encodeIfRequired(bodypart.getContent(), cte);
                if (bodypart.getContent() instanceof String) {
                    if (bodyPartContent.length() != ((String)bodypart.getContent()).length()) {
                        cte = "base64";
                    }
                } else if (bodypart.getContent() instanceof byte[]) {
                    if (bodyPartContent.length() != ((byte[])bodypart.getContent()).length) {
                        cte = "base64";
                    }
                }

                partString.append(CRLF).append(
                        CONTENT_TRANSFER_ENCODING 
                            + cte + CRLF)
                    .append(CONTENT_DISPOSITION + cd+';');
                
                if (fn != null) {
                    partString.append(" "+FILENAME + fn + CRLF + CRLF);
                } else {
                    partString.append(CRLF + CRLF);
                }

                // TODO: check if we have to add a blank (probably not)
                partString.append(bodyPartContent).append(CRLF);
                if (i != size - 1) {
                    partString.append("--" + boundary + CRLF);
                }
            }
        } else {
            // TODO: handle multipart content too
            textContent = mailmessage.getTextContent();

            if (textContent == null) {
                textContent =
                    "Error: the content of the message isn't text!";
                Log.error("The content of the message isn't text!");
            } else {
                String textContentEnc = encodeIfRequired(textContent, contentTransferEncoding);
                if (textContentEnc.length() != textContent.length()) {
                    textContent = textContentEnc;
                    contentTransferEncoding = "base64";
                }
            }
        }

        String content =
            partString != null ? partString.toString() : textContent;

        out.append(DATE + MailDateFormatter.dateToRfc2822(date) + CRLF);

        // The FROM field can be omitted. The email connector will set the 
        // from using the data on the server.
        if(from != null && !"".equals(from)) {
            out.append(FROM + from + CRLF);
        }

        if(useragent != null) {
            out.append(UA + useragent + CRLF);
        }

        // Append MIME-Version implemented by this MIME Processor.
        out.append(MIME_VERSION + CRLF);

        String to = mailmessage.getHeader(Message.TO);
        if (to != null)
            out.append(TO + StringUtil.fold(to));

        String cc = mailmessage.getHeader(Message.CC);
        if (cc != null && !"".equals(cc))
            out.append(CC + StringUtil.fold(cc));

        String replyto = mailmessage.getHeader(Message.REPLYTO);
        if (replyto != null && !"".equals(replyto))
            out.append(REPLY_TO + StringUtil.fold(replyto));

        String bcc = mailmessage.getHeader(Message.BCC);
        if (bcc != null && !"".equals(bcc))
            out.append(BCC + StringUtil.fold(bcc));


        String qpSubject;

        try {
            qpSubject = QuotedPrintable.encode(subject, "UTF-8");
            // If the string got quoted printable we must mark the word as
            // encoded per MIME spec
            if (qpSubject.length() != subject.length()) {
                StringBuffer newSubject = new StringBuffer();
                newSubject.append("=?").append(UTF8).append("?Q?")
                    .append(qpSubject).append("?=");
                qpSubject = newSubject.toString();
            }
        } catch (Exception e) {
            Log.error("Cannot encode subject");
            qpSubject = "";
        }


        out.append(SUBJECT).append(qpSubject).append(CRLF).
            append(MESSAGE_ID).append(messageID).append(CRLF).
            append(CONTENT_TRANSFER_ENCODING).append(contentTransferEncoding).append(CRLF).
            append(CONTENT_TYPE).append(contentType);

        //append(CRLF);

        if(mailmessage.isMultipart()){
            //tag boundary is present only in Multipart messages
            out.append(" boundary="+'"'+boundary+'"'+ CRLF).
                append(CRLF).
                append(startboundary).append(CRLF).
                append(content).append(CRLF).
                append(endboundary).append(CRLF);
        }else{
            out.append(CHARSET+ "UTF-8").append(CRLF+CRLF).
                append(content).append(CRLF);
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
            Log.error(bodyPartContent);
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


}
