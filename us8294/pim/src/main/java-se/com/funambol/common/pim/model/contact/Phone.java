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
package com.funambol.common.pim.model.contact;

import com.funambol.common.pim.model.common.TypifiedPluralProperty;

/**
 * An object representing a phone number. This class holds informations about the
 * type of the telephone number (businness, home etc.) and the number itself
 */
public class Phone extends TypifiedPluralProperty {

    public static final String MAIN_PHONE_NUMBER            = "MainTelephoneNumber";
    public static final String MOBILE_PHONE_NUMBER          = "MobileTelephoneNumber";
    public static final String MOBILE_HOME_PHONE_NUMBER     = "MobileHomeTelephoneNumber";
    public static final String MOBILE_BUSINESS_PHONE_NUMBER = "MobileBusinessTelephoneNumber";
    public static final String OTHER_PHONE_NUMBER           = "OtherTelephoneNumber";
    public static final String HOME_PHONE_NUMBER            = "HomeTelephoneNumber";
    public static final String BUSINESS_PHONE_NUMBER        = "BusinessTelephoneNumber";
    public static final String OTHER_FAX_NUMBER             = "OtherFaxNumber";
    public static final String HOME_FAX_NUMBER              = "HomeFaxNumber";
    public static final String BUSINESS_FAX_NUMBER          = "BusinessFaxNumber";
    public static final String PAGER_NUMBER                 = "PagerNumber";
    public static final String CAR_PHONE_NUMBER             = "CarTelephoneNumber";
    public static final String COMPANY_PHONE_NUMBER         = "CompanyMainTelephoneNumber";
    public static final String PRIMARY_PHONE_NUMBER         = "PrimaryTelephoneNumber";
    public static final String CALLBACK_PHONE_NUMBER        = "CallbackTelephoneNumber";
    public static final String RADIO_PHONE_NUMBER           = "RadioTelephoneNumber";
    public static final String TELEX_NUMBER                 = "TelexNumber";
    public static final String DCONLY_PHONE_NUMBER          = "DCOnlyTelephoneNumber";
    public static final String MOBILEDC_PHONE_NUMBER        = "MobileDCTelephoneNumber";

    /**
     * Creates an empty telephone number
     */
    public Phone() {
        super();
    }
    /**
     * Creates a telehpone number (Property) with the value.
     */
    public Phone(String value) {
        super(value);
    }

    /**
     * Returns the phone type for this phone
     *
     * @return the phone type for this phone
     */
    public String getPhoneType() {
       return propertyType;
    }

    /**
     * Sets the phone type for this phone
     *
     * @param phoneType the phone type to set
     */
    public void setPhoneType(String phoneType) {
        this.propertyType = phoneType;
    }

    private static String getId(int idx) {
        if (idx == 1) {
            return "";
        } else {
            return "" + idx;
        }
    }
}
