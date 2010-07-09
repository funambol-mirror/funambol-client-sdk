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


#ifndef INCL_VIRTUAL_CARD_CONVERTER
#define INCL_VIRTUAL_CARD_CONVERTER
/** @cond DEV */

#include "base/fscapi.h"
#include "vocl/vCard/Contact.h"
#include "vocl/VConverter.h"

#define VCARD21_PROPERTIES_LIST TEXT("BEGIN,FN,N,PHOTO,BDAY,ADR,LABEL,TEL,EMAIL,MAILER,TZ,GEO,TITLE,ROLE,LOGO,AGENT,ORG,NOTE,REV,SOUND,UID,URL,VERSION,KEY,NICKNAME,END")
#define VCARD30_PROPERTIES_LIST TEXT("BEGIN,FN,N,PHOTO,BDAY,ADR,LABEL,TEL,EMAIL,MAILER,TZ,GEO,TITLE,ROLE,LOGO,AGENT,ORG,NOTE,REV,SOUND,UID,URL,VERSION,KEY,CATEGORIES,CLASS,NICKNAME,PRODID,SORT-STRING,END")
#define PARAMTER21_LIST         TEXT("GROUP,TYPE,type,VALUE,ENCODING,CHARSET,LANGUAGE,QUOTED-PRINTABLE,BASE64,")
#define PARAMTER30_LIST         TEXT("GROUP,TYPE,type,LANGUAGE,ENCODING,VALUE,")
#define PHOTO21_TYPES           TEXT("GIF,CGM,WMF,BMP,MET,PMB,DIB,PICT,TIFF,PS,PDF,JPEG,MPEG,MPEG2,AVI,QTIME")
#define ADDRESS21_TYPES         TEXT("DOM,INTL,POSTAL,PARCEL,HOME,WORK")
#define ADDRESS30_TYPES         TEXT("DOM,INTL,POSTAL,PARCEL,HOME,WORK,PREF")
#define TEL21_TYPES             TEXT("PREF,WORK,HOME,VOICE,FAX,MSG,CELL,PAGER,BBS,MODEM,CAR,ISDN,VIDEO")
#define TEL30_TYPES             TEXT("PREF,WORK,HOME,VOICE,FAX,MSG,CELL,PAGER,BBS,MODEM,CAR,ISDN,VIDEO,PCS")
#define EMAIL21_TYPES           TEXT("AOL,AppleLink,ATTMail,CIS,eWorld,INTERNET,IBMMail,MCIMail,POWERSHARE,PRODIGY,TLX,X400,PREF,WORK,HOME")
#define EMAIL30_TYPES           TEXT("INTERNET,X400,PREF,WORK,HOME")
#define SOUND21_TYPES           TEXT("WAVE,PCM,AIFF")
#define KEY_TYPES               TEXT("X509,PGP")

#define MAX_ERROR_DESCRIPTION        100
#define ERROR_SUCCESS                0L
#define ERROR_KEY_PROPERTY_MISSING   100L
#define ERROR_ILLEGAL_VERSION_NUMBER 101L
#define ERROR_ILLEGAL_PROPERTY_NAME  102L
#define ERROR_UNSUPPORTED_ENCODING   103L
#define ERROR_ILLEGAL_TYPE_PARAMETER 104L
#define ERROR_ILLEGAL_PARAMETER      105L
#define ERROR_INVALID_PROPERTY_VALUE 106L
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class WString;


class vCardConverter {

private:
    WCHAR* vCard;
    Contact* contact;
    bool validateTZ(WCHAR* timeZone);
    bool validateGeo(WCHAR* geo);
    bool checkType(const WCHAR* types, const WCHAR* typesList);
    bool validateProperty21(VProperty* prop, WString& errorDescription, long* errorCode);
    bool validateProperty30(VProperty* prop, WString& errorDescription, long* errorCode);

public:
    vCardConverter();
    ~vCardConverter();
    void setSource(WCHAR* inputCard);
    void setSource(Contact& inputContact);
    void getvCard(WCHAR* vCard);
    void getContact(Contact** outputContact);
    bool convert(WString& error, long* errorCode);
    bool validate(VObject*, WString& error, long* errorCode);

};

END_NAMESPACE

/** @endcond */
#endif
