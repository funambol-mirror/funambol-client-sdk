/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2008 Funambol, Inc.
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

package com.funambol.common.pim.vcard;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Calendar;
import java.io.ByteArrayInputStream;
import java.io.InputStreamReader;
import java.io.ByteArrayOutputStream;
import java.io.UnsupportedEncodingException;

import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.PIMException;

import javax.microedition.pim.Contact;
import javax.microedition.pim.ContactList;

import com.funambol.util.XmlUtil;
import com.funambol.util.QuotedPrintable;
import com.funambol.util.Log;

import com.funambol.common.pim.vcard.VCardSyntaxParser;
import com.funambol.common.pim.vcard.VCardSyntaxParserListener;
import com.funambol.common.pim.ParamList;
import com.funambol.common.pim.FieldsList;
import com.funambol.common.pim.Utils;
import com.funambol.common.pim.vcard.Token;
import com.funambol.common.pim.vcard.ParseException;

/**
 * This class implements a VCard parser listener that generates a Contact (JSR75
 * definition). This class provides a basic implementation for this task, but it
 * is designed to be extended so that client can customize several things.
 * One major aspect that may require specialization is the mapping of the
 * multiple fields. Today this mapping is hardcoded, but the client can derive
 * the class e specialize few methods to change the behavior. In the future we
 * may decide to have a table to force a particular mapping.
 * The mapping is the following:
 *
 * Contact field  | VCard field
 * ------------------------------
 * first email    | INTERNET EMAIL
 * second email   | HOME EMAIL
 * third email    | WORK EMAIL
 * tel, fax       | BUSINESS FAX
 * tel, work      | TEL VOICE WORK
 * tel, home      | TEL VOICE HOME
 * tel, other     | TEL VOICE
 *
 * Warning: the current implementation is not finished yet and not all fields
 * are mapped. This implementation is used in the BlackBerry so it handles the
 * BB fields only. It will be extended when needed.
 *
 */

public class ContactParserListener implements VCardSyntaxParserListener
{
    private static final String defaultCharset = "UTF-8";

    private boolean faxSet    = false;
    private boolean pagerSet  = false;
    private boolean mobileSet = false;
    private boolean workSet   = false;
    private boolean homeSet   = false;
    private boolean otherSet  = false;

    /**
     * Emails addresses. The current implementation supports 3 different emails
     * emails[0] is INTERNET EMAIL
     * emails[1] is HOME EMAIL
     * emails[2] is WORK EMAIL
     * Other emails are simply discarded
     */
    private String emails[]   = new String[3];

    private Contact contact;

    protected int getUrlMaxValues() {
        return 1;
    }

    protected int getTitleMaxValues() {
        return 1;
    }

    public ContactParserListener(Contact contact) {
        this.contact = contact;
    }

    public void start() {
        Log.info("Starting vcard parsing");
        for (int i=0;i<emails.length;++i) {
            emails[i] = null;
        }
    }

    public void end() {

        Log.debug("Vcard finalizing emails");

        boolean emailDefined = false;
        for(int i=0;i<emails.length;++i) {
            if (emails[i] != null && emails[i].length() != 0) {
                emailDefined = true;
            } else {
                emails[i] = "";
            }
        }

        if (emailDefined) {
            PIMList list = contact.getPIMList();
            if (list.isSupportedField(Contact.EMAIL)) {
                for(int i=0;i<emails.length;++i) {
                    contact.addString(Contact.EMAIL, Contact.ATTR_NONE, emails[i]);
                }
            }
        }

        Log.info("Vcard parsing ended");
    }


    public void setCategories(String content, ParamList plist,
                              Token group) throws ParseException
    {
        String text=unfold(content);
        text=decode(text,plist.getEncoding(), plist.getCharset());
        text=unescape(text);

        PIMList list = contact.getPIMList();
        try {
            Utils pimUtils = new Utils();
            pimUtils.addCategories(text,list,contact);
        } catch (Exception e) {
            Log.error("[ContactParserListener] adding categories failed");
        }
    }

    public void addExtension(String tagName, String content, ParamList plist,
                             Token group) throws ParseException
    {
        // At the moment no extensions are supported
    }

    public void setVersion(String ver, ParamList plist,
                           Token group) throws ParseException
    {
        if (!(ver.equals("2.1")) && !(ver.equals("3.0"))) {
            throw new ParseException("Encountered a vCard version other than 2.1 or 3.0 ("+ver+")");
        }
    }

    public void setTitle(String content, ParamList plist,
                         Token group) throws ParseException
    {
        Log.debug("Setting title");

        int maxTitleValues = getTitleMaxValues();

        if (contact.countValues(Contact.TITLE) == maxTitleValues) {
            Log.error("Dropping title");
            return;
        }

        String text=unfold(content);
        text=decode(text, plist.getEncoding(), plist.getCharset());
        text = unescape(text);

        PIMList list = contact.getPIMList();
        if (list.isSupportedField(Contact.TITLE))
        {
            contact.addString(Contact.TITLE, Contact.ATTR_NONE, text);
        }
    }

    public void setMail(String content, ParamList plist,
                        Token group) throws ParseException
    {
        Log.debug("Setting Email");

        if (plist.containsKey("INTERNET") && plist.getSize() == 1) {
            emails[0] = content;
        } else if (plist.containsKey("HOME")) {
            emails[1] = content;
        } else if (plist.containsKey("WORK")) {
            emails[2] = content;
        }
    }

    public void setUrl(String content, ParamList plist,
                       Token group) throws ParseException
    {
        Log.debug("Setting Url");

        int maxUrlValues = getUrlMaxValues();

        if (contact.countValues(Contact.URL) == maxUrlValues) {
            Log.error("Dropping telephone number");
            return;
        }

        // We do not distinguish the different url addresses and
        // fill the only available one
        PIMList list = contact.getPIMList();
        if (list.isSupportedField(Contact.URL))
        {
            contact.addString(Contact.URL, Contact.ATTR_NONE, content);
        }
    }

    public void setTelephone(String content, ParamList plist,
                             Token group) throws ParseException
    {
        Log.debug("Setting Telephone");

        content=unfold(content);
        content=decode(content,plist.getEncoding(), plist.getCharset());
        content=unescape(content);


//            field = "";
//            if (PIMItemHelper.isSupportedAttributedField(list,Contact.TEL, Contact.ATTR_HOME2))
//            {
//                contact.addString(Contact.TEL, Contact.ATTR_HOME2, field);
//            }


//            field = "";
//            if (PIMItemHelper.isSupportedAttributedField(list,Contact.TEL, Contact.ATTR_WORK2))
//            {
//                contact.addString(Contact.TEL, Contact.ATTR_WORK2, "");
//            }
                                

        PIMList list = contact.getPIMList();
        if (plist.containsKey("FAX") && plist.containsKey("WORK")) {
            // BB does not distinguish between home and work fax, so we
            // simply store the business one and discard others
            if (!faxSet && isSupportedAttributedField(list, Contact.TEL, Contact.ATTR_FAX))
            {
                Log.debug("Setting FAX telephone to: " + content);
                contact.addString(Contact.TEL, Contact.ATTR_FAX, content);
                faxSet = true;
            }
        } else if (plist.containsKey("PAGER")) {
            // BB does not distinguish between home and work pager, so we
            // simply store the first one and discard others
            if (!pagerSet && isSupportedAttributedField(list, Contact.TEL, Contact.ATTR_PAGER))
            {
                Log.debug("Setting PAGER telephone to: " + content);
                contact.addString(Contact.TEL, Contact.ATTR_PAGER, content);
                pagerSet = true;
            }
        } else if (plist.containsKey("CELL")) {
            if (!mobileSet && isSupportedAttributedField(list, Contact.TEL,Contact.ATTR_MOBILE))
            {
                Log.debug("Setting MOBILE telephone to: " + content);
                contact.addString(Contact.TEL, Contact.ATTR_MOBILE, content);
                mobileSet = true;
            }
        } else if (plist.containsKey("WORK") && plist.containsKey("VOICE") && plist.getSize() == 2) {
            if (!workSet && isSupportedAttributedField(list, Contact.TEL, Contact.ATTR_WORK))
            {
                Log.debug("Setting WORK telephone to: " + content);
                contact.addString(Contact.TEL, Contact.ATTR_WORK, content);
                workSet = true;
            }

        } else if (plist.containsKey("HOME") && plist.containsKey("VOICE") && plist.getSize() == 2) {
            if (!homeSet && isSupportedAttributedField(list, Contact.TEL, Contact.ATTR_HOME))
            {
                Log.debug("Setting HOME telephone to: " + content);
                contact.addString(Contact.TEL, Contact.ATTR_HOME, content);
                homeSet = true;
            }
        } else if (plist.containsKey("VOICE") && plist.getSize() == 1) {
            if (!otherSet && isSupportedAttributedField(list, Contact.TEL, Contact.ATTR_OTHER))
            {
                Log.debug("Setting OTHER telephone to: " + content);
                contact.addString(Contact.TEL, Contact.ATTR_OTHER, content);
                otherSet = true;
            }
        }
    }

    public void setFName(String content, ParamList plist,
                         Token group) throws ParseException {

        // Not supported
        Log.debug("Setting FName skipped");
    }

    public void setRole(String content, ParamList plist,
                        Token group) throws ParseException {

        // Not supported
    }

    public void setRevision(String content, ParamList plist,
                            Token group) throws ParseException {
        // Not supported
    }

    public void setNickname(String content, ParamList plist,
                            Token group) throws ParseException {
        // Not supported
        Log.debug("Setting NickName skipped");
    }

    public void setOrganization(String content, ParamList plist,
                                Token group) throws ParseException {

        Log.debug("Setting Organization");

        String encoding  = null            ;
        FieldsList flist = new FieldsList();
        flist.addValue(content);

        int pos;  // Position in tlist (i.e. position of the current value field)

        // Organization Name
        pos = 0;
        if (flist.size() > pos) {
            String text=unfold(flist.getElementAt(pos));
            text=decode(text,plist.getEncoding(), plist.getCharset());
            contact.addString(Contact.ORG, Contact.ATTR_NONE, text);
        }

        // Organizational Unit
        pos = 1;
        if (flist.size() > pos) {
            String text=unfold(flist.getElementAt(pos));
            text=decode(text,plist.getEncoding(), plist.getCharset());
            // Not supported
        }
    }

    public void setAddress(String content, ParamList plist,
                           Token group)throws ParseException
    {
        FieldsList flist = new FieldsList();
        flist.addValue(content);

        int pos;  // Position in tlist (i.e. position of the current value field)
        String extended_addr, street, locality, region, postalcode, country;

        // Business Address
        String arrayField[] = new String[7];

        //Post Office Address
        pos = 0;
        String text;
        if (flist.size()>pos) {
            text=unfold(flist.getElementAt(pos));
            text=decode(text,plist.getEncoding(), plist.getCharset());
        } else {
            text="";
        }
        // Not supported

        // Extended Address
        pos = 1;
        if (flist.size()>pos) {
            text=unfold(flist.getElementAt(pos));
            extended_addr=decode(text,plist.getEncoding(), plist.getCharset());
        } else {
            extended_addr="";
        }
       
        // Street
        pos = 2;
        if (flist.size()>pos) {
            text=unfold(flist.getElementAt(pos));
            street=decode(text,plist.getEncoding(), plist.getCharset());
        } else {
            street="";
        }

        // Locality
        pos = 3;
        if (flist.size()>pos) {
            text=unfold(flist.getElementAt(pos));
            locality=decode(text,plist.getEncoding(), plist.getCharset());
        } else {
            locality="";
        }

        // Region
        pos = 4;
        if (flist.size()>pos) {
            text=unfold(flist.getElementAt(pos));
            region=decode(text,plist.getEncoding(), plist.getCharset());
        } else {
            region="";
        }

        // Postal Code
        pos = 5;
        if (flist.size()>pos) {
            text=unfold(flist.getElementAt(pos));
            postalcode=decode(text,plist.getEncoding(), plist.getCharset());
        } else {
            postalcode="";
        }

        // Country
        pos = 6;
        if (flist.size()>pos) {
            text=unfold(flist.getElementAt(pos));
            country=decode(text,plist.getEncoding(), plist.getCharset());
        } else {
            country="";
        }
        
        arrayField[Contact.ADDR_EXTRA] = extended_addr;
        arrayField[Contact.ADDR_STREET] = street;
        arrayField[Contact.ADDR_LOCALITY] = locality;
        arrayField[Contact.ADDR_REGION] = region;
        arrayField[Contact.ADDR_POSTALCODE] = postalcode;
        arrayField[Contact.ADDR_COUNTRY] = country;

        if (plist.containsKey("WORK")) {
            contact.addStringArray(Contact.ADDR, Contact.ATTR_WORK,arrayField);
        } else if (plist.containsKey("HOME")) {
            contact.addStringArray(Contact.ADDR, Contact.ATTR_HOME,arrayField);
        } 
    }

    public void setBirthday(String content, ParamList plist,
                            Token group) throws ParseException {
        Log.debug("Setting Birthday");
        
        PIMList list = contact.getPIMList();
        if (list.isSupportedField(Contact.BIRTHDAY))
        {
            if(content.length()==10){
                String year = content.substring(0, 4);
                String month = content.substring(5, 7);
                String day = content.substring(8, 10);
                Calendar date = Calendar.getInstance();
                date.set(Calendar.YEAR, Integer.parseInt(year));
                date.set(Calendar.MONTH, Integer.parseInt(month)-1);
                date.set(Calendar.DAY_OF_MONTH, Integer.parseInt(day));
                contact.addDate(Contact.BIRTHDAY, Contact.ATTR_NONE, date.getTime().getTime());
            }else if (content.length()>10) {
                Log.error("Parsing a wrong date format for birthday field :" +content);
                throw new ParseException("wrong date format for birthday field :" +content);
            }
        }
        
    }

    public void setLabel(String content, ParamList plist,
                         Token group) throws ParseException {

        // Not supported
    }

    public void setTimezone(String content, ParamList plist,
                            Token group) throws ParseException
    {
        // Not supported
    }

    public void setLogo(String content, ParamList plist,
                        Token group) throws ParseException
    {
        // Not supported
    }

    public void setNote(String content, ParamList plist,
                        Token group) throws ParseException
    {
        Log.debug("Setting Note");

        String text=unfold(content);
        text=decode(text,plist.getEncoding(), plist.getCharset());
        text=unescape(text);

        PIMList list = contact.getPIMList();
        if (list.isSupportedField(Contact.NOTE))
        {
            contact.addString(Contact.NOTE, Contact.ATTR_NONE, text);
        }
    }

    public void setUid(String content, ParamList plist,
                       Token group) throws ParseException
    {
        // Not supported
    }

    public void setPhoto(String content, ParamList plist,
                         Token group) throws ParseException
    {
        Log.debug("Setting Photo");

        String text=unfold(content);
        text=decode(text,plist.getEncoding(), plist.getCharset());
        text=unescape(text);

        if (plist.containsKey("VALUE") && "URL".equals(plist.getValue("VALUE"))) {
            Log.error("Photo with remote url are not supported");
            return;
        }

        try{
            PIMList list = contact.getPIMList();
            if (list.isSupportedField(Contact.PHOTO) && text.length() > 0)
            {
                byte[] byteField = text.getBytes();
                contact.addBinary(Contact.PHOTO, Contact.ATTR_NONE, byteField ,0,byteField.length);
            }
        }
        catch (final Throwable e)
        {
            Log.error("Cannot set photo as the underlying system does not accept the binary data");
        }       
    }
 
    public void setName(String content, ParamList plist,
                        Token group) throws ParseException
    {
        Log.debug("Setting Name to " + content);

        FieldsList flist = new FieldsList();
        flist.addValue(content);

        int pos;  // Position in tlist (i.e. position of the current value field)

        String fieldname = "Name";
        String[] arrayField = new String[5];

        // Last name
        pos=0;
        if (flist.size() > pos) {
            String text=unfold(flist.getElementAt(pos));
            text=decode(text,plist.getEncoding(), plist.getCharset());
            arrayField[Contact.NAME_FAMILY] = text; 
        }

        // First name
        pos=1;
        if (flist.size() > pos) {
            String text=unfold(flist.getElementAt(pos));
            text=decode(text,plist.getEncoding(), plist.getCharset());
            arrayField[Contact.NAME_GIVEN]  = text;
        }

        // Middle name
        pos=2;
        if (flist.size() > pos) {
            String text=unfold(flist.getElementAt(pos));
            text=decode(text,plist.getEncoding(), plist.getCharset());
            // Not supported
        }

        // Prefix
        pos=3;
        if (flist.size() > pos) {
            String text=unfold(flist.getElementAt(pos));
            text=decode(text,plist.getEncoding(), plist.getCharset());
            arrayField[Contact.NAME_PREFIX] = text;
        }

        // Suffix
        pos=4;
        if (flist.size() > pos) {
            String text=unfold(flist.getElementAt(pos));
            text=decode(text,plist.getEncoding(), plist.getCharset());
            // Not supported
        }

        PIMList list = contact.getPIMList();
        if (list.isSupportedField(Contact.NAME)) {
            contact.addStringArray(Contact.NAME, PIMItem.ATTR_NONE, arrayField);
        }
    }

    public void setFolder(String content, ParamList plist,
                          Token group) throws ParseException
    {
        // Not supported
    }

    public void setFreebusy(String content, ParamList plist,
                            Token group) throws ParseException
    {
        // Not supported
    }

    public void setAnniversary(String content, ParamList plist,
                               Token group) throws ParseException
    {
        // Not supported
    }

    public void setChildren(String content, ParamList plist,
                            Token group) throws ParseException
    {
        // Not supported
    }

    public void setCompanies(String content, ParamList plist,
                             Token group) throws ParseException
    {
        // Not supported
    }

    public void setLanguages(String content, ParamList plist,
                             Token group) throws ParseException
    {
        // Not supported
    }

    public void setManager(String content, ParamList plist,
                           Token group) throws ParseException
    {
        // Not supported
    }

    public void setMileage(String content, ParamList plist,
                           Token group) throws ParseException
    {
        // Not supported
    }

    public void setSpouse(String content, ParamList plist,
                          Token group) throws ParseException
    {
        // Not supported
    }

    public void setSubject(String content, ParamList plist,
                           Token group) throws ParseException
    {
        // Not supported
    }




    /**
     * Unfolds a string (i.e. removes all the CRLF characters)
     */
    private String unfold (String str) {
        int ind = str.indexOf("\r\n");
        if (ind == -1) {
            return unfoldNewline(str);
        }
        else {
            String tmpString1 = str.substring(0,ind);
            String tmpString2 = str.substring(ind+2);
            return unfoldNewline(unfold(tmpString1+tmpString2));
        }
    }

    /**
     * Unfolds a string (i.e. removes all the line break characters).
     * This function is meant to ensure compatibility with vCard documents
     * that adhere loosely to the specification
     */
    private String unfoldNewline (String str) {
        int ind = str.indexOf("\n");
        if (ind == -1) {
            return str;
        }
        else {
            String tmpString1 = str.substring(0,ind);
            String tmpString2 = str.substring(ind+1);
            return unfoldNewline(tmpString1+tmpString2);
        }
    }

    /**
     * Decode the given text according to the given encoding and charset
     *
     * @param text the text to decode
     * @param encoding the encoding
     * @param propertyCharset the charset
     *
     * @return the text decoded
     */
    private String decode(String text, String encoding, String propertyCharset)
    {
        if (text == null) {
            return null;
        }

        //
        // If input charset is null then set it with default charset
        //
        if (propertyCharset == null) {
            propertyCharset = defaultCharset; // we use the default charset
        }
        if (encoding != null) {
            if ("QUOTED-PRINTABLE".equals(encoding)) {
                try {
                    byte textBytes[] = text.getBytes(propertyCharset);
                    int len = QuotedPrintable.decode(textBytes);
                    String res = new String(textBytes, 0, len, propertyCharset);
                    return res;
                } catch (UnsupportedEncodingException ue) {
                    Log.error("Cannot decode quoted printable: " + text);
                    // In this case we keep this value
                    return text;
                }
            }
        } else {
            try {
                return new String(text.getBytes(propertyCharset), propertyCharset);
            } catch (UnsupportedEncodingException ue) {
                // In this case we keep this value
                return text;
            }
        }
        return text;
    }

    /**
     * Removes the last equals from the end of the given String
     */
    private String removeLastEquals(String data) {
        if (data == null) {
            return data;
        }
        data = data.trim();
        while (data.endsWith("=")) {
            data = data.substring(0, data.length() - 1);
        }
        return data;
    }

    /**
     * Unescape backslash and semicolon.
     *
     * @param text the text to unescape
     * @return String the unescaped text
     */
    private String unescape(String text) {

        if (text == null) {
            return text;
        }

        StringBuffer value = new StringBuffer();
        int length = text.length();
        boolean foundSlash = false;
        for (int i=0; i<length; i++) {
            char ch = text.charAt(i);
            switch (ch) {
                case '\\':
                    if (foundSlash) {
                        foundSlash = false;
                        value.append('\\');
                    } else {
                        foundSlash = true;
                    }
                    break;
                case ';':
                    value.append(';');
                    foundSlash = false;
                    break;
                default:
                    if (foundSlash) {
                        foundSlash = false;
                        value.append('\\');
                    }
                    value.append(ch);
                    break;
            }
        }
        return value.toString();
    }

    private boolean isSupportedAttributedField(PIMList list, int field, int attribute) {
        return list.isSupportedField(field) &&
               list.isSupportedAttribute(field, attribute);
    }


}


