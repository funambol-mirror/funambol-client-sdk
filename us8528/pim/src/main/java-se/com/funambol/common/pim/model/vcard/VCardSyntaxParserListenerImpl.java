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
package com.funambol.common.pim.model.vcard;

import java.util.List;
import java.util.ArrayList;
import java.io.UnsupportedEncodingException;

import com.funambol.common.pim.FieldsList;
import com.funambol.common.pim.ParamList;
import com.funambol.common.pim.vcard.Token;
import com.funambol.common.pim.vcard.ParseException;
import com.funambol.common.pim.vcard.AbstractVCardSyntaxParserListener;
import com.funambol.common.pim.model.common.Property;
import com.funambol.common.pim.model.common.TypifiedPluralProperty;
import com.funambol.common.pim.model.common.XTag;
import com.funambol.common.pim.model.contact.Address;
import com.funambol.common.pim.model.contact.Contact;
import com.funambol.common.pim.model.contact.Email;
import com.funambol.common.pim.model.contact.Note;
import com.funambol.common.pim.model.contact.Phone;
import com.funambol.common.pim.model.contact.Title;
import com.funambol.common.pim.model.contact.WebPage;
import com.funambol.util.QuotedPrintable;
import com.funambol.util.Log;

/**
 * Represents a VCardSyntaxParserListener used in the pim-framework
 */
public class VCardSyntaxParserListenerImpl extends AbstractVCardSyntaxParserListener {

    private static final String TAG = "VCardSyntaxParserListenerImpl";

    private String defaultCharset;

    private Contact contact;

    public VCardSyntaxParserListenerImpl(Contact contact, String defaultCharset) {
        this.contact = contact;
        this.defaultCharset = defaultCharset;
    }

    public void start() {
    }

    public void end() {
    }

    @Override
    public void setCategories(String content, ParamList plist,
                              Token group) throws ParseException
    {
        contact.getCategories().setPropertyValue(unfoldDecodeUnescape(content, plist));
        setParameters(contact.getCategories(), plist, group);
    }

    @Override
    public void addExtension(String tagName, String content, ParamList plist,
                             Token group) throws ParseException
    {
        XTag tmpxTag = new XTag();
        tmpxTag.getXTag().setPropertyValue(unfoldDecodeUnescape(content, plist));
        setPreferredIfNeeded(tmpxTag.getXTag(), plist);

        if (plist.containsKey("HOME")) {
            tmpxTag.getXTag().setPropertyType(XTag.HOME_XTAG);
        } else if (plist.containsKey("WORK")) {
            tmpxTag.getXTag().setPropertyType(XTag.WORK_XTAG);
        } else {
            tmpxTag.getXTag().setPropertyType(XTag.OTHER_XTAG);
        }

        setParameters(tmpxTag.getXTag(), plist, group);

        tmpxTag.setXTagValue(tagName);
        contact.addXTag(tmpxTag);
    }

    @Override
    public void setVersion(String ver, ParamList plist,
                           Token group) throws ParseException
    {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG, "setVersion");
        }
        if (!(ver.equals("2.1")) && !(ver.equals("3.0"))) {
            throw new ParseException("Encountered a vCard version other than 2.1 or 3.0 ("+ver+")");
        }
    }

    @Override
    public void setTitle(String content, ParamList plist,
                         Token group) throws ParseException
    {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG, "setTitle");
        }

        Title tmptitle = new Title(unfoldDecodeUnescape(content, plist));
        setParameters(tmptitle, plist, group);

        tmptitle.setTitleType("JobTitle");

        contact.getBusinessDetail().addTitle(tmptitle);
    }

    @Override
    public void setMail(String content, ParamList plist,
                        Token group) throws ParseException
    {
        if (plist.getXParams().containsKey("X-CELL")) {
            Email tmpmail = new Email(unfoldDecodeUnescape(content, plist));
            setParameters(tmpmail,plist,group);
            tmpmail.setEmailType(Email.MOBILE_EMAIL);
            contact.getPersonalDetail().addEmail(
                    (Email)setPreferredIfNeeded(tmpmail, plist));
        } else if (plist.getSize() == 0 ||
                  (plist.getSize() == 1 && plist.containsKey("INTERNET")) ||
                  (plist.getSize() == 2 && (plist.containsKey("PREF")
                  && plist.containsKey("INTERNET"))))
        {
            Email tmpmail = new Email(unfoldDecodeUnescape(content, plist));
            setParameters(tmpmail,plist,group);
            tmpmail.setEmailType(Email.OTHER_EMAIL);
            contact.getPersonalDetail().addEmail(
                    (Email)setPreferredIfNeeded(tmpmail, plist));
        } else if (plist.containsKey("HOME")) {
            Email tmpmail = new Email(unfoldDecodeUnescape(content, plist));
            setParameters(tmpmail,plist,group);
            if (plist.getXParams().containsKey("X-FUNAMBOL-INSTANTMESSENGER")) {
                tmpmail.setEmailType(Email.IM_ADDRESS);
                contact.getPersonalDetail().addEmail(
                        (Email)setPreferredIfNeeded(tmpmail, plist));
            } else {
                tmpmail.setEmailType(Email.HOME_EMAIL);
                contact.getPersonalDetail().addEmail(
                        (Email)setPreferredIfNeeded(tmpmail, plist));
            }
        } else if (plist.containsKey("WORK")) {
            Email tmpmail = new Email(unfoldDecodeUnescape(content, plist));
            setParameters(tmpmail,plist,group);
            tmpmail.setEmailType(Email.WORK_EMAIL);
            contact.getBusinessDetail().addEmail(
                    (Email)setPreferredIfNeeded(tmpmail, plist));
        } else {
            Email tmpmail = new Email(unfoldDecodeUnescape(content, plist));
            setParameters(tmpmail,plist,group);
            tmpmail.setEmailType(Email.OTHER_EMAIL);
            contact.getPersonalDetail().addEmail(
                    (Email)setPreferredIfNeeded(tmpmail, plist));
        }
    }

    @Override
    public void setUrl(String content, ParamList plist,
                       Token group) throws ParseException
    {
        if (!plist.containsKey("HOME") && !plist.containsKey("WORK")) {
            WebPage tmppage = new WebPage();
            tmppage.setPropertyValue(unfoldDecodeUnescape(content, plist));
            setParameters(tmppage,plist,group);
            tmppage.setWebPageType(WebPage.OTHER_WEBPAGE);
            contact.getPersonalDetail().addWebPage(
                    (WebPage)setPreferredIfNeeded(tmppage, plist));
        }

        if (plist.containsKey("HOME")) {
            WebPage tmppage = new WebPage();
            tmppage.setPropertyValue(unfoldDecodeUnescape(content, plist));
            setParameters(tmppage,plist,group);
            tmppage.setWebPageType(WebPage.HOME_WEBPAGE);
            contact.getPersonalDetail().addWebPage(
                    (WebPage)setPreferredIfNeeded(tmppage, plist));
        }

        if (plist.containsKey("WORK")) {
            WebPage tmppage = new WebPage();
            tmppage.setPropertyValue(unfoldDecodeUnescape(content, plist));
            setParameters(tmppage,plist,group);
            tmppage.setWebPageType(WebPage.WORK_WEBPAGE);
            contact.getBusinessDetail().addWebPage(
                    (WebPage)setPreferredIfNeeded(tmppage, plist));
        }
    }

    @Override
    public void setTelephone(String content, ParamList plist,
                             Token group) throws ParseException
    {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG, "setTelephone");
        }

        content = unfoldDecodeUnescape(content, plist);
        List<String> telPlist=new ArrayList<String>();
        String[] telParameters =
        {
            "PREF",
            "WORK",
            "HOME",
            "VOICE",
            "FAX",
            "MSG",
            "CELL",
            "PAGER",
            "BBS",
            "MODEM",
            "CAR",
            "ISDN",
            "VIDEO",
            "X-FUNAMBOL-RADIO",
            "X-FUNAMBOL-CALLBACK",
            "X-FUNAMBOL-TELEX",
            "X-DC"
        };
        for (String parameter : telParameters) {
            if (plist.containsKey(parameter) ||
                plist.getXParams().containsKey(parameter)) {
                if(!"PREF".equals(parameter)) {
                    telPlist.add(parameter);
                } else if(plist.getSize() == 2 && (plist.containsKey("WORK") ||
                        plist.containsKey("VOICE"))) {
                    telPlist.add(parameter);
                }
            }
        }

        Phone tmphone = new Phone(content);
        setParameters(tmphone,plist,group);

        if (telPlist.contains("WORK")) {
            if (telPlist.contains("CELL")) {
                tmphone.setPhoneType(Phone.MOBILE_BUSINESS_PHONE_NUMBER);
                contact.getBusinessDetail().addPhone(
                        (Phone)setPreferredIfNeeded(tmphone, plist));
            }
            if (telPlist.contains("VOICE") || (telPlist.size() == 1)) {
                tmphone.setPhoneType(Phone.BUSINESS_PHONE_NUMBER);
                contact.getBusinessDetail().addPhone(
                        (Phone)setPreferredIfNeeded(tmphone, plist));
            }
            if (telPlist.contains("FAX")) {
                tmphone.setPhoneType(Phone.BUSINESS_FAX_NUMBER);
                contact.getBusinessDetail().addPhone(
                        (Phone)setPreferredIfNeeded(tmphone, plist));
            }
            if (telPlist.contains("PREF")) {
                tmphone.setPhoneType(Phone.COMPANY_PHONE_NUMBER);
                contact.getBusinessDetail().addPhone(
                        (Phone)setPreferredIfNeeded(tmphone, plist));
            }
        } else if ((telPlist.contains("CELL") && telPlist.size() == 1) ||
            (telPlist.contains("CELL") && telPlist.contains("VOICE"))) {
            tmphone.setPhoneType(Phone.MOBILE_PHONE_NUMBER);
            contact.getPersonalDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if (telPlist.contains("HOME") && telPlist.contains("CELL")) {
            tmphone.setPhoneType(Phone.MOBILE_HOME_PHONE_NUMBER);
            contact.getPersonalDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if ((telPlist.size() == 1 && telPlist.contains("VOICE"))) {
            tmphone.setPhoneType(Phone.OTHER_PHONE_NUMBER);
            contact.getPersonalDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if (telPlist.isEmpty()) {
            tmphone.setPhoneType(Phone.MAIN_PHONE_NUMBER);
            contact.getPersonalDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if ((telPlist.contains("VOICE") && telPlist.contains("HOME"))  ||
            (telPlist.size() == 1 && telPlist.contains("HOME"))  )       {
            tmphone.setPhoneType(Phone.HOME_PHONE_NUMBER);
            contact.getPersonalDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if (telPlist.size() == 1 && telPlist.contains("FAX")) {
            tmphone.setPhoneType(Phone.OTHER_FAX_NUMBER);
            contact.getPersonalDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if (telPlist.contains("HOME") && telPlist.contains("FAX")) {
            tmphone.setPhoneType(Phone.HOME_FAX_NUMBER);
            contact.getPersonalDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if (telPlist.contains("CAR")) {
            tmphone.setPhoneType(Phone.CAR_PHONE_NUMBER);
            contact.getPersonalDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if (telPlist.contains("PAGER")) {
            tmphone.setPhoneType(Phone.PAGER_NUMBER);
            contact.getBusinessDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if (((telPlist.contains("PREF") && telPlist.contains("VOICE")) ||
                    (telPlist.contains("PREF") && telPlist.size() == 1))) {
            tmphone.setPhoneType(Phone.PRIMARY_PHONE_NUMBER);
            contact.getPersonalDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if (telPlist.contains("X-FUNAMBOL-CALLBACK")) {
            tmphone.setPhoneType(Phone.CALLBACK_PHONE_NUMBER);
            contact.getBusinessDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if (telPlist.contains("X-FUNAMBOL-RADIO")) {
            tmphone.setPhoneType(Phone.RADIO_PHONE_NUMBER);
            contact.getPersonalDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if (telPlist.contains("X-FUNAMBOL-TELEX")) {
            tmphone.setPhoneType(Phone.TELEX_NUMBER);
            contact.getBusinessDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        } else if (telPlist.contains("X-DC")) {
            if (telPlist.contains("CELL")) {
                tmphone.setPhoneType(Phone.MOBILEDC_PHONE_NUMBER);
            } else {
                tmphone.setPhoneType(Phone.DCONLY_PHONE_NUMBER);
            }
            contact.getBusinessDetail().addPhone(
                    (Phone)setPreferredIfNeeded(tmphone, plist));
        }
    }

    @Override
    public void setFName(String content, ParamList plist,
                         Token group) throws ParseException {

        contact.getName().getDisplayName().setPropertyValue(unfoldDecodeUnescape(content, plist));
        setParameters(contact.getName().getDisplayName(), plist, group);
    }

    @Override
    public void setRole(String content, ParamList plist,
                        Token group) throws ParseException {

        contact.getBusinessDetail().getRole().setPropertyValue(unfoldDecodeUnescape(content, plist));
        setParameters(contact.getBusinessDetail().getRole(), plist, group);
    }

    @Override
    public void setRevision(String content, ParamList plist,
                            Token group) throws ParseException {
        contact.setRevision(unfoldDecodeUnescape(content, plist));
    }

    @Override
    public void setNickname(String content, ParamList plist,
                            Token group) throws ParseException {
        contact.getName().getNickname().setPropertyValue(unfoldDecodeUnescape(content, plist));
        setParameters(contact.getName().getNickname(),plist,group);
    }

    @Override
    public void setOrganization(String content, ParamList plist,
                                Token group) throws ParseException {

        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG, "setOrganization");
        }

        FieldsList flist = new FieldsList();
        flist.addValue(content);

        int pos;  // Position in tlist (i.e. position of the current value field)

        // Organization Name
        pos = 0;
        if (flist.size() > pos) {
            String text = unfoldDecode(flist.getElementAt(pos), plist);
            contact.getBusinessDetail().getCompany().setPropertyValue(text);
            setParameters(contact.getBusinessDetail().getCompany(), plist, group);
        }

        // Organizational Unit
        pos = 1;
        if (flist.size() > pos) {
            String text = unfoldDecode(flist.getElementAt(pos), plist);
            contact.getBusinessDetail().getDepartment().setPropertyValue(text);
            setParameters(contact.getBusinessDetail().getDepartment(), plist, group);
        }
    }

    @Override
    public void setAddress(String content, ParamList plist,
                           Token group)throws ParseException
    {
        FieldsList flist = new FieldsList();
        flist.addValue(content);

        int pos;  // Position in tlist (i.e. position of the current value field)
        String text;

        Address tmpAddress = new Address();

        //Post Office Address
        pos = 0;
        if (flist.size() > pos) {
            text = unfoldDecode(flist.getElementAt(pos), plist);
        } else {
            text = "";
        }
        tmpAddress.getPostOfficeAddress().setPropertyValue(text);
        setParameters(tmpAddress.getPostOfficeAddress(), plist, group);

        // Extended Address
        pos = 1;
        if (flist.size() > pos) {
            text = unfoldDecode(flist.getElementAt(pos), plist);
        } else {
            text = "";
        }
        tmpAddress.getExtendedAddress().setPropertyValue(text);
        setParameters(tmpAddress.getExtendedAddress(), plist, group);

        // Street
        pos = 2;
        if (flist.size() > pos) {
            text = unfoldDecode(flist.getElementAt(pos), plist);
        } else {
            text = "";
        }
        tmpAddress.getStreet().setPropertyValue(text);
        setParameters(tmpAddress.getStreet(), plist, group);

        // Locality
        pos = 3;
        if (flist.size() > pos) {
            text = unfoldDecode(flist.getElementAt(pos), plist);
        } else {
            text = "";
        }
        tmpAddress.getCity().setPropertyValue(text);
        setParameters(tmpAddress.getCity(), plist, group);

        // Region
        pos = 4;
        if (flist.size() > pos) {
            text = unfoldDecode(flist.getElementAt(pos), plist);
        } else {
            text = "";
        }
        tmpAddress.getState().setPropertyValue(text);
        setParameters(tmpAddress.getState(), plist, group);

        // Postal Code
        pos = 5;
        if (flist.size() > pos) {
            text = unfoldDecode(flist.getElementAt(pos), plist);
        } else {
            text = "";
        }
        tmpAddress.getPostalCode().setPropertyValue(text);
        setParameters(tmpAddress.getPostalCode(), plist, group);

        // Country
        pos = 6;
        if (flist.size() > pos) {
            text = unfoldDecode(flist.getElementAt(pos), plist);
        } else {
            text = "";
        }
        tmpAddress.getCountry().setPropertyValue(text);
        setParameters(tmpAddress.getCountry(), plist, group);
        
        if (plist.containsKey("WORK")) {
            tmpAddress.setAddressType(Address.WORK_ADDRESS);
            contact.getBusinessDetail().addAddress(
                    (Address)setPreferredIfNeeded(tmpAddress, plist));
        } else if (plist.containsKey("HOME")) {
            tmpAddress.setAddressType(Address.HOME_ADDRESS);
            contact.getPersonalDetail().addAddress(
                    (Address)setPreferredIfNeeded(tmpAddress, plist));
        } else if (!plist.containsKey("HOME") && !plist.containsKey("WORK")) {
            tmpAddress.setAddressType(Address.OTHER_ADDRESS);
            contact.getPersonalDetail().addAddress(
                    (Address)setPreferredIfNeeded(tmpAddress, plist));
        }
    }

    @Override
    public void setBirthday(String content, ParamList plist,
                            Token group) throws ParseException {

        String birthday = unfoldDecodeUnescape(content, plist);

        try {
            // TODO FIXME
            //birthday = TimeUtils.normalizeToISO8601(birthday, defaultTimeZone);
            contact.getPersonalDetail().setBirthday(birthday);
        } catch (Exception e) {
            //
            // If the birthday isn't in a valid format
            // (see TimeUtils.normalizeToISO8601), ignore it
            //
        }
    }

    @Override
    public void setLabel(String content, ParamList plist,
                         Token group) throws ParseException {
        Log.error(TAG, "setLabel not supported");
    }

    @Override
    public void setTimezone(String content, ParamList plist,
                            Token group) throws ParseException
    {
        contact.setTimezone(unfoldDecodeUnescape(content, plist));
    }

    @Override
    public void setGeo(String content, ParamList plist,
                       Token group) throws ParseException
    {
        // Does not unescape because it contains a pair of values
        Property geo = new Property(unfoldDecodeUnescape(content, plist));
        contact.getPersonalDetail().setGeo(geo);
    }

    @Override
    public void setMailer(String content, ParamList plist,
                       Token group) throws ParseException
    {
        contact.setMailer(unfoldDecodeUnescape(content, plist));
    }

    @Override
    public void setLogo(String content, ParamList plist,
                        Token group) throws ParseException
    {
        contact.getBusinessDetail().getLogo().setPropertyValue(unfoldDecodeUnescape(content, plist));
        setParameters(contact.getBusinessDetail().getLogo(), plist, group);
    }

    @Override
    public void setNote(String content, ParamList plist,
                        Token group) throws ParseException
    {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG, "setNote");
        }
        Note tmpnote = new Note();
        tmpnote.setPropertyValue(unfoldDecodeUnescape(content, plist));
        setParameters(tmpnote, plist, group);
        tmpnote.setNoteType("Body");
        contact.addNote(tmpnote);
    }

    @Override
    public void setUid(String content, ParamList plist,
                       Token group) throws ParseException
    {
        contact.setUid(unfoldDecodeUnescape(content, plist));
    }

    public void setPhoto(String content, ParamList plist,
                         Token group) throws ParseException
    {
        TypifiedPluralProperty photo = new TypifiedPluralProperty();
        photo.setPropertyValue(unfoldDecodeUnescape(content, plist));
        setParameters(photo, plist, group);
        if (plist != null) {
            photo.setType(plist.getValue("TYPE"));
        }
        contact.getPersonalDetail().addPhoto(setPreferredIfNeeded(photo, plist));
    }

    @Override
    public void setName(String content, ParamList plist,
                        Token group) throws ParseException
    {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG, "setName");
        }
        FieldsList flist = new FieldsList();
        flist.addValue(content);

        int pos;  // Position in tlist (i.e. position of the current value field)

        // Last name
        pos=0;
        if (flist.size() > pos) {
            String text = unfoldDecode(flist.getElementAt(pos), plist);
            contact.getName().getLastName().setPropertyValue(text);
            setParameters(contact.getName().getLastName(), plist, group);
        }
        // First name
        pos=1;
        if (flist.size() > pos) {
            String text = unfoldDecode(flist.getElementAt(pos), plist);
            contact.getName().getFirstName().setPropertyValue(text);
            setParameters(contact.getName().getFirstName(), plist, group);
        }
        // Middle name
        pos=2;
        if (flist.size() > pos) {
            String text = unfoldDecode(flist.getElementAt(pos), plist);
            contact.getName().getMiddleName().setPropertyValue(text);
            setParameters(contact.getName().getMiddleName(), plist, group);
        }
        // Prefix
        pos=3;
        if (flist.size() > pos) {
            String text = unfoldDecode(flist.getElementAt(pos), plist);
            contact.getName().getSalutation().setPropertyValue(text);
            setParameters(contact.getName().getSalutation(), plist, group);
        }
        // Suffix
        pos=4;
        if (flist.size() > pos) {
            String text = unfoldDecode(flist.getElementAt(pos), plist);
            contact.getName().getSuffix().setPropertyValue(text);
            setParameters(contact.getName().getSuffix(), plist, group);
        }
    }

    @Override
    public void setFolder(String content, ParamList plist,
                          Token group) throws ParseException
    {
        contact.setFolder(unfoldDecodeUnescape(content, plist));
    }

    @Override
    public void setFreebusy(String content, ParamList plist,
                            Token group) throws ParseException
    {
        contact.setFreeBusy(unfoldDecodeUnescape(content, plist));
    }

    @Override
    public void setAnniversary(String content, ParamList plist,
                          Token group) throws ParseException
    {
        contact.getPersonalDetail().setAnniversary(unfoldDecodeUnescape(content, plist));
    }

    @Override
    public void setChildren(String content, ParamList plist,
                          Token group) throws ParseException
    {
        // Does not unescape because it could contain a list of values
        contact.getPersonalDetail().setChildren(unfoldDecode(content, plist));
    }

    @Override
    public void setCompanies(String content, ParamList plist,
                          Token group) throws ParseException
    {
        // Does not unescape because it could contain a list of values
        contact.getBusinessDetail().setCompanies(unfoldDecode(content, plist));
    }

    @Override
    public void setLanguages(String content, ParamList plist,
                          Token group) throws ParseException
    {
        // Does not unescape because it could contain a list of values
        contact.setLanguages(unfoldDecode(content, plist));
    }

    @Override
    public void setManager(String content, ParamList plist,
                          Token group) throws ParseException
    {
        contact.getBusinessDetail().setManager(unfoldDecodeUnescape(content, plist));
    }

    @Override
    public void setMileage(String content, ParamList plist,
                          Token group) throws ParseException
    {
        contact.setMileage(unfoldDecodeUnescape(content, plist));
    }

    @Override
    public void setSpouse(String content, ParamList plist,
                          Token group) throws ParseException
    {
        contact.getPersonalDetail().setSpouse(unfoldDecodeUnescape(content, plist));
    }

    @Override
    public void setSubject(String content, ParamList plist,
                          Token group) throws ParseException
    {
        contact.setSubject(unfoldDecodeUnescape(content, plist));
    }

    @Override
    public void setAccessClass(String content, ParamList plist,
                                Token group) throws ParseException
    {
        String accessClass = unfoldDecodeUnescape(content, plist);
        Short sensitivity;
        if (Contact.CLASS_PUBLIC.equalsIgnoreCase(accessClass)) {
            sensitivity = Contact.SENSITIVITY_NORMAL;
        } else if (Contact.CLASS_CONFIDENTIAL.equalsIgnoreCase(accessClass)) {
            sensitivity = Contact.SENSITIVITY_CONFIDENTIAL;
        } else if (Contact.CLASS_PRIVATE.equalsIgnoreCase(accessClass)) {
            sensitivity = Contact.SENSITIVITY_PRIVATE;
        } else {
            sensitivity = Contact.SENSITIVITY_PERSONAL;
        }
        contact.setSensitivity(sensitivity);
    }

    private TypifiedPluralProperty setPreferredIfNeeded(TypifiedPluralProperty prop, ParamList plist) {
        prop.setPreferred(plist.containsKey("PREF"));
        return prop;
    }

    /**
     * Sets the parameters encoding, charset, language, value for a given property
     * fetching them from the given ParamList.
     * Notice that if the items are not set (i.e. are null) in the ParamList, they
     * will be set to null in the property too (this is to avoid inconsistency when
     * the same vCard property is encountered more than one time, and thus overwritten
     * in the Contact object model).
     */
    private void setParameters(Property property, ParamList plist) {
        if (plist != null) {
            property.setEncoding(plist.getEncoding());
            property.setCharset (plist.getCharset());
            property.setLanguage(plist.getLanguage());
            property.setValue   (plist.getValue   ());
            property.setXParams (plist.getXParams ());
        }
    }

    /**
     * Sets the parameters encoding, charset, language, value and group for a given property
     * fetching them from the given ParamList and the group Token.
     */
    private void setParameters(Property property, ParamList plist, Token group) {
        if (!(group==null)) {
            property.setGroup(group.image);
        }
        else {
            property.setGroup(null);
        }
        setParameters(property,plist);
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
    throws ParseException {
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
                    //
                    // Some phone, like the Sony Ericsson k750i can send something
                    // like that:
                    //
                    // BEGIN:VCARD
                    // VERSION:2.1
                    // N;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;=C3=85=C3=A5=C3=A6
                    // TITLE;CHARSET=UTF-8:Title
                    // ORG;CHARSET=UTF-8:Compan
                    // TEL;CELL:0788554422
                    // EMAIL;INTERNET;PREF;CHARSET=UTF-8:ac0@dagk.com
                    // ADR;HOME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;;S=CE=A6;City;Stat;6;Peru=
                    //
                    // X-IRMC-LUID:000200000102
                    // END:VCARD
                    //
                    // At the end of the address there is a '=\r\n\r\n'. This is replaced
                    // with '=\r\n' by SourceUtils.handleDelimiting so here the vcard is:
                    //
                    // BEGIN:VCARD
                    // VERSION:2.1
                    // N;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;=C3=85=C3=A5=C3=A6
                    // TITLE;CHARSET=UTF-8:Title
                    // ORG;CHARSET=UTF-8:Compan
                    // TEL;CELL:0788554422
                    // EMAIL;INTERNET;PREF;CHARSET=UTF-8:ac0@dagk.com
                    // ADR;HOME;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:;;S=CE=A6;City;Stat;6;Peru=
                    // X-IRMC-LUID:000200000102
                    // END:VCARD
                    //
                    // The problem is with the address becasue the value is in QP but
                    // it finishes with a '=' so this is not a valid QP
                    // (Invalid quoted-printable encoding)
                    // To fix the problem, before to decode the string, we remove the
                    // '=' at the end of the string
                    //
                    text = removeLastEquals(text);

                    byte t[] = text.getBytes(propertyCharset);
                    int len = QuotedPrintable.decode(t);
                    String value = new String(t, 0, len, propertyCharset);
                    return value;
                } catch (Exception e) {
                    throw new ParseException(e.getMessage());
                }
            }
        } else {
            try {
                return new String(text.getBytes(), propertyCharset);
            } catch (UnsupportedEncodingException ue) {
                throw new ParseException(ue.getMessage());
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

    private String unfoldDecodeUnescape(String content, ParamList plist)
    throws ParseException {
        String text = unfold(content);
        text = decode(text, plist.getEncoding(), plist.getCharset());
        text = unescape(text);
        return text;
    }

    private String unfoldDecode(String content, ParamList plist)
    throws ParseException {
        String text = unfold(content);
        text = decode(text, plist.getEncoding(), plist.getCharset());
        return text;
    }

}
