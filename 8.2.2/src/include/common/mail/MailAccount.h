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


#ifndef INCL_MAIL_ACCOUNT
#define INCL_MAIL_ACCOUNT

#include "base/globalsdef.h"
#include "base/util/ArrayList.h"
#include "spds/FolderData.h"
#include "base/Log.h"

#define VISIBLENAME     "VisibleName"
#define EMAILADDRESS    "EmailAddress"
#define PROTOCOL        "Protocol"
#define USERNAME        "Username"
#define PASSWORD        "Password"
#define IN_SERVER       "IncomingServer"
#define OUT_SERVER      "OutgoingServer"
#define IN_PORT         "PortIn"
#define OUT_PORT        "PortOut"
#define IN_SSL          "IncomingSSL"
#define OUT_SSL         "OutcomingSSL"
#define SIGNATURE       "Signature"
#define DOMAINNAME      "DomainName"
#define ID              "ID"

/**
* Definition of constants to be used to retrieve the id of the folders/account
*/
#define ROLE_ACCOUNT    "account"
#define ROLE_INBOX      "inbox"
#define ROLE_OUTBOX     "outbox"
#define ROLE_DRAFT      "draft"
#define ROLE_SENT       "sent"
#define ROLE_TRASH      "trash"
#define ROLE_DELETED    "deleted"

BEGIN_NAMESPACE

class MailAccount : public FolderData {


    // ------------------------------------------------------- Private data
    private:
        bool deleted;
        bool toBeCleaned;

    public:
    // ------------------------------------------------------- Constructors
        MailAccount() : deleted(false), toBeCleaned(false) {};
        MailAccount(const MailAccount& ma);
        MailAccount(const FolderData& ma);
        ~MailAccount(){};


    // ---------------------------------------------------------- Accessors
        const char* getVisibleName() const { return getValueByName(VISIBLENAME); }
        void setVisibleName(const char* xval){ setValueByName(VISIBLENAME, xval); }
        
        const char* getEmailAddress() const { return getValueByName(EMAILADDRESS); }
        void setEmailAddress(const char* xval){ setValueByName(EMAILADDRESS, xval); }

        const char* getProtocol() const { return getValueByName(PROTOCOL); }
        void setProtocol(const char* xval){ setValueByName(PROTOCOL, xval); }

        const char* getUsername() const { return getValueByName(USERNAME); }
        void setUsername(const char* xval){ setValueByName(USERNAME, xval); }

        const char* getPassword() const { return getValueByName(PASSWORD); }
        void setPassword(const char* xval){ setValueByName(PASSWORD, xval); }

        const char* getInServer() const { return getValueByName(IN_SERVER); }
        void setInServer(const char* xval){ setValueByName(IN_SERVER, xval); }

        const char* getOutServer() const { return getValueByName(OUT_SERVER); }
        void setOutServer(const char* xval){ setValueByName(OUT_SERVER, xval); }

        const char* getInPort() const { return getValueByName(IN_PORT); }
        void setInPort(const char* xval){ setValueByName(IN_PORT, xval); }

        const char* getOutPort() const { return getValueByName(OUT_PORT); }
        void setOutPort(const char* xval){ setValueByName(OUT_PORT, xval); }

        const char* getInSSL() const { return getValueByName(IN_SSL); }
        void setInSSL(const char* xval){ setValueByName(IN_SSL, xval); }  

        const char* getOutSSL() const { return getValueByName(OUT_SSL); }
        void setOutSSL(const char* xval){ setValueByName(OUT_SSL, xval); } 

        const char* getSignature() const { return getValueByName(SIGNATURE); }
        void setSignature(const char* xval){ setValueByName(SIGNATURE, xval); }  

        const char* getDomainName() const { return getValueByName(DOMAINNAME); }
        void setDomainName(const char* xval){ setValueByName(DOMAINNAME, xval); }


        bool getDeleted() const { return deleted; }
        void setDeleted(const bool val) { deleted = val; }

        bool getToBeCleaned() const { return toBeCleaned; }
        void setToBeCleaned(bool tobecleaned) { toBeCleaned = tobecleaned; }
        // ----------------------------------------------------- Public Methods

		ArrayElement* clone() { return new MailAccount(*this); }

};


END_NAMESPACE

/** @endcond */
#endif
