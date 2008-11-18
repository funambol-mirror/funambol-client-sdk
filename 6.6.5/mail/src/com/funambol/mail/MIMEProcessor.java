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

import java.io.UnsupportedEncodingException;

import java.util.Date;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import com.funambol.util.Base64;
import com.funambol.util.Log;
import com.funambol.util.MailDateFormatter;
import com.funambol.util.QuotedPrintable;
import com.funambol.util.StringUtil;
import com.funambol.util.ChunkedString;

/**
 * An object of this class is a parser that reads a string representing an email
 * message formatted as per RFC 2822 and MIME (RFC 2045, 2046 etc.) and returns
 * a <code>Message</code> object ready to be stored in the device store (the
 * latter represented by implementations of the <code>Store</code> interface)
 */
public class MIMEProcessor {

    // -------------------------------------------------------------- Constants

    /** RFC 2822 */
    private static final String BCC = "bcc";

    /** RFC 2046 */
    private static final String BOUNDARY = "boundary";

    /** RFC 2822 */
    private static final String CC = "cc";

    /** RFC 2822 */
    private static final String FROM = "from";

    /** RFC 2822 */
    private static final String TO = "to";

    /** RFC 2822 header added by the gateway */
    private static final String RECEIVED = "received";

    /** RFC 2822 */
    private static final String REPLY_TO = "reply-to";
    
    /** RFC 2822 */
    private static final String DATE = "date";

    /** RFC 2822 */
    private static final String MESSAGE_ID = "message-id";

    /** RFC 2822 */
    private static final String SUBJECT = "subject";

    /** RFC 2822 */
    private static final String UA = "user-agent";

    /**
     * An optional header field. In its absence, the mail user agent may use
     * whatever presentation method it deems suitable [RFC 2183 par. 2]
     */
    private static final String CONTENT_DISPOSITION = "content-disposition";
    
    private static final String FILENAME = "filename";

    /** RFC 2822 */
    private static final String CONTENT_TRANSFER_ENCODING =
                                            "content-transfer-encoding";

    /**
     * RFC 2822 header <p>
     * 
     * Here it contains only the first value, and not the parameters that
     * normally follow (e.g. <code>charset</code> etc.)
     */
    private static final String CONTENT_TYPE = "content-type";

    /** RFC 2822 */
    private static final String MIME_VERSION = "mime-version";

    /**
     * Carriage return + Line feed (0x0D 0x0A). In the RFC 2822 this is used as
     * line separator between different headers with relative values
     */
    private static final String EOL = "\r\n";

    // ---------------------------------------------------------- Public Method

    
    /**
     * This method parses a text email message formatted as per RFC 2822 and MIME
     * specifications in order to build a <code>Message</code> object ready to be
     * processed by the mail library.<p>
     * 
     * @param rfc2822
     *            A String containing the entire email message coming from
     *            a server and formatted as per RFC 2822 and MIME specifications
     * @return A <code>Message</code> object build with the information
     *         retrieved from the original text representation of the email
     *         message
     */
    public Message parseMailMessage(String rfc2822) throws MailException {
        return parseMailMessage(new ChunkedString(rfc2822));
    }

    /**
     * This method parses a text email message in RFC2822 format and builds
     * <code>Message</code> object ready to be processed by the mail library.<p>
     * The message is passed in a ChunkedString, to limit the memory used by the 
     * processing of the message. See ChunkedString javadoc for more info.
     * 
     * @param rfc2822
     *            A ChunkedString containing the entire email message coming from
     *            a server and formatted as per RFC 2822 and MIME specifications
     * @return A <code>Message</code> object build with the information
     *         retrieved from the original text representation of the email
     *         message
     */
    public Message parseMailMessage(ChunkedString msgbuf) throws MailException {

        // The message to be returned
        Message mailmessage = null;

        /*
         * The hash table containing a mapping between headers
         * for the email message to be parsed
         */
        Hashtable headers = parseHeaders(msgbuf);

        /*
         * The parameters following the [media type]/[subtype] value of the
         * <code>Content-Type</code> header attribute in the form
         * [parameter]=[value] as pairs key/value in a hash table.
         *
         * now all values in the Content-Type header attribute are saved in a
         * hash table. The value corresponding to the "content-type" key is the
         * 'real' content type, i.e. only the first value after the name of this
         * attribute, the MIME media type with subtype ([media type]/[subtype]).
         * All other parameters following this first one are stored in the hash
         * table, each parameter name used as key having its value
         */
        Hashtable contentTypeParameters = 
                            headerToParameters(CONTENT_TYPE, headers);

        /*
         * Building the native Message object to be returned
         */
        mailmessage = new Message();
        String subject = decodeHeader((String)headers.get(SUBJECT));
        mailmessage.setSubject("".equals(subject) ? "(no subject)" : subject);
       
        /* Use StringUtil.removeBackslashes() method 
         * for headers FROM - TO - REPLYTO - CC -BCC
         * to fix visible names coming from Web mailers like
         * libero.it in the form
  	 *     name\.surname\@libero\.it
         */
        parseRecipients(mailmessage, Address.FROM, StringUtil.removeBackslashes((String)headers.get(FROM)));
        
        parseRecipients(mailmessage, Address.TO, StringUtil.removeBackslashes((String)headers.get(TO)));
        
        // Needs to catch the mailexception for mails containing
        // replyto header with value "<>". (bug #3724)
        try{
            parseRecipients(mailmessage,
                        Address.REPLYTO,
                        StringUtil.removeBackslashes((String)headers.get(REPLY_TO)));
        }catch (MailException mex){
             Log.error(this,"error parsing replyto header");
             mex.printStackTrace();
        }
        
        parseRecipients(mailmessage, Address.CC, StringUtil.removeBackslashes((String)headers.get(CC)));
        parseRecipients(mailmessage, Address.BCC, StringUtil.removeBackslashes((String)headers.get(BCC)));
        Date sentGMT = MailDateFormatter.parseRfc2822Date(
                                        (String)headers.get(DATE));
        Date sent = MailDateFormatter.getDeviceLocalDate(sentGMT);
        Date receivedGMT = MailDateFormatter.parseRfc2822Date(
                                    (String)headers.get(RECEIVED));
        Date received = MailDateFormatter.getDeviceLocalDate(receivedGMT);
        if (sent == null && received == null) {
            // No valid dates: should it be better to give up?
            sent = received = new Date();
        }
        // Handle wrong cases
        if (sent != null) {        
            mailmessage.setSentDate(sent);
        } else {
            mailmessage.setSentDate(received);
        }
        if (received != null) {
            mailmessage.setReceivedDate(received);            
        } else {
            mailmessage.setReceivedDate(sent);
        }

        String msgId = (String)headers.get(MESSAGE_ID);
        int pos1 = msgId.indexOf("<");
        int pos2 = msgId.indexOf(">");
        if( pos1 != -1 && pos2 != -1) {
            // remove the '<>'
            msgId = msgId.substring(pos1+1, pos2);
            Log.debug("Message-Id: "+msgId);
        }
        mailmessage.setMessageId(msgId);

        //String userAgent = (String)headers.get(UA);
        //if(userAgent != null) {
        //    mailmessage.setHeader(UA, userAgent);
        //}

        String content_type = (String)contentTypeParameters.get(CONTENT_TYPE);

        // If not set, the default is text plain (set by Part constructor).
        if (content_type != null) {
            // The mime type arrives uppercase from server 6.0, convert it to
            // lowercase.
            mailmessage.setContentType(content_type.toLowerCase());
        }

        if (mailmessage.isText()) {// non multi-part message
            String content = msgbuf.toString();

            String charset = (String)contentTypeParameters.get("charset");
            if (charset == null) {
                charset = "ascii";
            }

            // Check the encoding
            String encoding = (String)headers.get(CONTENT_TRANSFER_ENCODING);
            if (encoding != null) {
                // decode the content if necessary
                if (encoding.equals(Part.ENC_QP) || encoding.equals(Part.ENC_B64)) {
                    byte [] bytes = content.getBytes();
                    int len = bytes.length;

                    content = null; // free memory

                    // Decode the bytes
                    if (encoding.equals(Part.ENC_QP)) {
                        len = QuotedPrintable.decode(bytes);
                    } else if (encoding.equals(Part.ENC_B64)) {
                        // TODO: The same byte[] can be used
                        bytes = Base64.decode(bytes);
                        len = bytes.length;
                    }
                    // Convert back to string
                    try {
                        content = new String(bytes, 0, len, charset);
                    } catch (UnsupportedEncodingException e) {
                        Log.error("MIMEProcessor: "+ charset +
                                  " not supported. " + e.toString());
                        content = new String(bytes);
                    }
                    bytes = null;
                }
            }
            else {
                // Set 7bit as default encoding.
                encoding = Part.ENC_7BIT;
            }

            mailmessage.setHeader(Message.CONTENT_TRANSFER_ENCODING, encoding);
            mailmessage.setContent(content);
            content = null;
        }
        else if (mailmessage.isMultipart()) {// multi-part message
            Multipart multipart=buildMultipart(msgbuf, contentTypeParameters);
            
            /**
             * Check to skip the attachment "funambol file.txt" signature.
             * If the signature is the only attachment present in the original 
             * message, the multipart contains only one part, the body text.
             * Will be removed when the attachments will be managed in different way
             */
            if(multipart.getCount()>1){
                mailmessage.setContent(multipart);
                mailmessage.setLaziness(Message.LAZY_CONTENT);
            }else{
                mailmessage.setContent(multipart.getBodyPart(0).getTextContent());
            }
        }

        headers.clear();
        headers = null;
        contentTypeParameters.clear(); 
        contentTypeParameters = null;

        return mailmessage;
    }


    // --------------------------------------------------------- Helper methods

    /**
     * Parse and validate the recipients in the given header, before setting 
     * them in the Message.
     *
     * @param msg the message to fill
     * @param type the type of address
     * @param header the header from the input message
     */
    private void parseRecipients(Message msg, int type, String header)
    throws MailException {
        if(header != null) {
            Address[] recipients = Address.parse(type, decodeHeader(header));
            msg.addRecipients(recipients);
        }
    }

    /*
     * A helper method to build the <code>Multipart</code> container for
     * <code>BodyPart</code> objects in multi-part email messages
     * 
     * TODO: treat all the multi-part types (now just the simpliest ones)
     * 
     * @param content
     *            The content body to be parsed of the <code>Message</code>
     * @return The more external <code>Multipart</code> object to be added to
     *         a multi-part <code>Message</code> object
     */
    private Multipart buildMultipart(ChunkedString content, Hashtable ctParams)
    throws MailException {
   
        Multipart multipart = new Multipart();
        BodyPart part = null;
        ChunkedString partbuf = null;

        String boundary = (String)ctParams.get(BOUNDARY);
     
        if (boundary == null) {
            // Panic! No boundary in the headers.
            // Treat the rest as a single part, at least to show
            // something to the user.
            part = new BodyPart();
            part.setContent(content.toString());
            multipart.addBodyPart(part);
            return multipart;
        }

        /*
         * the actual string used as boundary in the content body (see RFC 2046,
         * 5.1.1)
         */
        boundary = "--" + boundary;
        
        // skip the preamble (see RFC 2046 p. 20-21)
        /**RFC 1341 7.2 Multipart
         * Note that the encapsulation boundary must occur at the beginning of a line,
         * i.e., following a CRLF, and that that initial CRLF is considered to be part 
         * of the encapsulation boundary rather than part of the preceding part. 
         * The boundary must be followed immediately either by another CRLF and the 
         * header fields for the next part, or by two CRLFs, in which case there 
         * are no header fields for the next part (and it is therefore assumed to be 
         * of Content-Type text/plain).
         */
        partbuf = content.getNextChunk(boundary+EOL);
        

        // the boundary before the epilogue (see RFC 2046 p. 20-21)
        int lastboundary = content.indexOf(boundary + "--");
        
        
        // get the new chunk without the epilogue
        //content = new ChunkedString(content, 0, lastboundary);
        content = content.substring(0,lastboundary);
        
        
        partbuf = content.getNextChunk(boundary+EOL);
        

        int partNumber = 1;

        while (!partbuf.isEmpty()) {
            part = buildBodyPart(partbuf, partNumber++);
            
            /**
             * Check to skip the attachment "funambol file.txt" signature.
             * The bodypart won't be added if contains the signature,  
             * Will be removed when the attachments will be managed in different way
             */
            if (!(part.getFileName()).equals("Funambol File.txt")){
                multipart.addBodyPart(part);
            }
            
            partbuf = content.getNextChunk(boundary+EOL);
            

        }

        return multipart;
    }

    /**
     * build a body part
     */
    private BodyPart buildBodyPart(ChunkedString partContent, int partNum)
                                                        throws MailException {
        
        Hashtable headers = parseHeaders(partContent);
        Hashtable conttypeparams =
                    headerToParameters(CONTENT_TYPE, headers);
        Hashtable contdispparams =
                    headerToParameters(CONTENT_DISPOSITION, headers);

        BodyPart ret = new BodyPart();
        
        String ct = ((String)conttypeparams.get(CONTENT_TYPE));//.toLowerCase();
        
        if( ct !=null ) {
            ret.setContentType(ct.toLowerCase());
        }
        
        String cd = (String)contdispparams.get(CONTENT_DISPOSITION);
        
        ret.setDisposition(cd != null ? cd : BodyPart.CD_INLINE);
        
        String fn = (String)contdispparams.get(FILENAME);
        
        ret.setFileName(fn != null ? fn : "Part1."+partNum);
        
        

        Object body = null;

        String cte = (String)headers.get(CONTENT_TRANSFER_ENCODING);
        
        String cs = (String)conttypeparams.get("charset");
        

        // decoding the content if necessary
        if (cte != null) {
            byte[] bytes = partContent.toString().getBytes();

            if (cte.equals(Part.ENC_QP)) {
                body = QuotedPrintable.decode(bytes, cs);

            } else if (cte.equals(Part.ENC_B64) ) {
                if(ret.isText()) {
                    body = Base64.decode(bytes, cs);
                } else {
                   // byte[] encoded = partContent.toString().getBytes();
                    body = Base64.decode(bytes);
                }
            }else{
                body = partContent.toString();
            }
            
        }
        else {
            cte=Part.ENC_7BIT;
            body = partContent.toString();
        }
        ret.addHeader(BodyPart.CONTENT_TRANSFER_ENCODING, cte);

        ret.setContent(body);

        return ret;
    }

    /**
     * Helper method used to check if the content of a header attribute is
     * Quoted Printable encoded. <p>
     * 
     * A signal for this is if the string starts with "Q?". In this case the
     * content is decoded
     * 
     * @param iso
     *            The string representing the content of the header attribute
     * @return The decoded string if the content was encoded, the start string
     *         otherwise
     */
    private String decodeHeader(String iso) {
        int start = 0;
        int end = 0;

        if (iso == null || iso.equals("")) {
            return "";
        }

        StringBuffer ret = new StringBuffer();

        while ( ( start=iso.indexOf("=?", start) ) != -1) {
            Log.debug("start: "+start);
            // Skip the '=?'
            start += 2;

            // Find the first '?'
            int firstMark = iso.indexOf("?", start);
            if (firstMark == -1) {
                Log.error("Invalid encoded header");
                return iso;
            }
            // Find the second '?'
            int secondMark = iso.indexOf("?", firstMark+1);
            if (secondMark == -1) {
                Log.error("Invalid encoded header");
                return iso;
            }
            // Find the final '?='
            end = iso.indexOf("?=", secondMark+1);
            if (end == -1) {
                Log.error("Invalid encoded header");
                return iso;
            }
            
            String charset = iso.substring(start, firstMark);
            String encoding = iso.substring(firstMark+1, secondMark);
            String text = iso.substring(secondMark+1, end);
            
            // Add the initial part if not encoded
            if (start >= 2 &&  ret.length() == 0) {
                ret.append(iso.substring(0, start - 2));
            }
            if (StringUtil.equalsIgnoreCase(encoding, "Q")) {
                // quoted-printable
                String enc = text.replace('_', ' ');
                ret.append(QuotedPrintable.decode(enc.getBytes(), charset));
            }
            else if (StringUtil.equalsIgnoreCase(encoding, "B")){
                // base64
                ret.append(Base64.decode(text, charset));
            }

            start = end;
            end+=2;
        }
        // Append the last part
        if(end < iso.length()){
            ret.append(iso.substring(end));
        }
        return ret.toString();
    }
    
    /**
     * Extracts all parameters from the string containing the value of the
     * passed header attribute (e.g. "Content-Type") and put them into a hash
     * table (in the form [key]=[value]), being the element corresponding to the
     * passed header key (e.g. the key "content-type") the 'real' content type
     * 
     * media type/subtype (e.g. "text/plain"), the 'real' content type
     * 
     * 1st parameter (e.g. charset=ISO-8859-1)
     * 
     * 2nd parameter (e.g. format=flowed)
     * 
     * 3rd parameter
     * 
     * etc.
     * 
     * Pay attention: the value of the content-type as field of the Message
     * object is only the 'media type/subtype'; in the header is the combination
     * of this and the list of parameters
     * 
     * @param header
     *            The name of the header attribute (e.g. "Content-Type")
     * @param hashtable
     *            The hash table containing the <code>Message</code>'s data
     * @return A hash table containing all the parameters of the passed header
     *         (in case of "Content-Type" the [media type/subtype] is included)
     */
    private Hashtable headerToParameters(String header, Hashtable hashtable) {
       
        Hashtable parameters = new Hashtable();
        
        String hdrvalue = (String)hashtable.get(header);
        
        if(hdrvalue == null) {
            return parameters;
        }

        /*
         * A String array containing all possible parts of the given header
         */
        String[] elements = StringUtil.split(hdrvalue, ";");

        for (int w = 0; w < elements.length; w++) {
            int index = elements[w].indexOf("=");
            if (index != -1) {
                String key =
                    elements[w].substring(0, index).trim().toLowerCase();
                String val = elements[w].substring(index + 1).trim();
                // If the parameter is enclosed in quotes, remove them
                if(val.charAt(0) == '"') {
                    val = val.substring(1, val.indexOf("\"", 1));
                }
                parameters.put(key, val);
            }
        }
        // contentTypeElements[0] is always the value of the header
        parameters.put(header, elements[0]);
        return parameters;
    }

    /**
     * Get the next header in the current message chunk. If the headers
     * span over multiple lines, it is unfolded by this method and returned
     * in one line only.
     */
    private String getNextHeader(ChunkedString chunk) {
        
        int end = 0;
        StringBuffer ret = new StringBuffer();
        String line = null;

        // The new chunk starts with CRLF: this is the end 
        // of the headers.
        // Return null also if the end of the chunk is reached
        if(chunk.isEmpty() || chunk.startsWith(EOL)) {
            return null;
        }

        boolean folded;

        do {
            line = chunk.getNextString(EOL);
            if (line == null) break;

            ret.append(line);

            if(chunk.isEmpty()) break;
            
            folded = false;
            // If the next chunk starts with tab or blank,
            // it's a folded header: skip the blanks and add it to
            // the current header
            char next = chunk.charAt(0);
            while ( next == '\t' || next == ' ' ) {
                if ( !chunk.moveStart(1) ) {
                    // If the end of the chunk has been reached
                    // quit the loop (unlikely happens)
                    break;
                }
                folded = true;
                next = chunk.charAt(0);
            }
            
            // patch to avoid date string containing no spaces beetween its parts
            if (folded) {
                ret.append(' ');
            }
                
                
            
        } while (folded);
        
        return ret.toString();
    }

    /**
     * Parse the message or bodypart headers and store them in an hashtable.
     */
    private Hashtable parseHeaders(ChunkedString chunk) {
        
        String line = null;
        String header = null;
        String value = null;
        Hashtable ret = new Hashtable();

        String received = null;

        while ( (line = getNextHeader(chunk)) != null ) {
            int index = line.indexOf(":");
            if (index != -1) {
                header = line.substring(0, index).toLowerCase();
 
                value = line.substring(index + 1).trim();
                
                
                /*
                 * Get the first received field (last in time)
                 * see RFC 2822, par. 3.6.7
                 */
                if (header.equals(RECEIVED)) {
                    if (received == null) {
                        int start = value.lastIndexOf(';');// date is after the last ";"
                        if (start == -1) {
                            Log.info("Invalid Received:");
                            continue;
                        }

                        // drop e.g. "(PST)"
                        int end = value.indexOf("(", start);
                        
                        if (end == -1) {
                            end = value.length();
                        }

                        received = (value.substring(start + 1, end)).trim();
                        ret.put(header, received);
                    }
                }
                else {
                    ret.put(header, value);
                }
            }
        }

        return ret;
    }

}

