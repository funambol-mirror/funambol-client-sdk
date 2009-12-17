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
#include "base/constants.h"
#include "base/util/utils.h"
#include "spdm/constants.h"
#include "client/MailSourceManagementNode.h"
#include "base/globalsdef.h"
#include "mail/MailAccount.h"

USE_NAMESPACE

MailSourceManagementNode::MailSourceManagementNode(const char*  context,
                                                   const char*  name   )
    : DeviceManagementNode(context, name) {}

MailSourceManagementNode::MailSourceManagementNode(const char*         context,
                                                   const char*         name   ,
                                                   MailSyncSourceConfig& c      )
    : DeviceManagementNode(context, name) 
{
    setMailSourceConfig(c);
}

MailSourceManagementNode::~MailSourceManagementNode() {
}

void MailSourceManagementNode::setMailAccounts(MailSyncSourceConfig& c){
    char t[512];
    ArrayList mailAccounts = config.getMailAccounts();
    int accountNum = mailAccounts.size();

	if (accountNum) {
        char* fullcontext = toMultibyte(getFullContext());
        DeviceManagementNode* mn = new DeviceManagementNode(fullcontext, PROPERTY_MAIL_ACCOUNT_ROOT);
 
        for (int i = 0; i < accountNum; i ++) { 
            if (((MailAccount*)mailAccounts[i])->getDeleted()){
                WCHAR* wname = toWideChar(((MailAccount*)mailAccounts[i])->getName());
                WCHAR* node = new WCHAR[wcslen(getFullContext()) + wcslen(wname) + wcslen(TEXT("\\mailAccounts\\")) +1];
                wsprintf(node, TEXT("%s\\mailAccounts\\%s"), getFullContext(), wname);
                deletePropertyNode(node);
                config.delMailAccount(((MailAccount*)mailAccounts[i])->getName());

                delete [] wname;
                delete [] node;
            }
        }

        delete mn;
        mailAccounts = config.getMailAccounts();
	    c.setMailAccounts(mailAccounts);
        accountNum = mailAccounts.size();
	    for (int i = 0; i < accountNum; i++) {
	        MailAccount* account = static_cast<MailAccount*>(mailAccounts[i]);
		    char valname[512];
            const char* name = stringdup(((StringBuffer)account->getName()).c_str());
            char fullname[512];
            sprintf(fullname, "%s/%s", fullcontext, PROPERTY_MAIL_ACCOUNT_ROOT);
            
	        DeviceManagementNode* mn = new DeviceManagementNode(fullname,name);
			
	        sprintf(valname, PROPERTY_MAIL_ACCOUNT_VISIBLE_NAME, name);
		    sprintf(t, "%s", account->getVisibleName());
		    mn->setPropertyValue(valname, t);

	        sprintf(valname, PROPERTY_MAIL_ACCOUNT_EMAILADDRESS, name);
		    sprintf(t, "%s", account->getEmailAddress());
		    mn->setPropertyValue(valname, t);

		    sprintf(valname, PROPERTY_MAIL_ACCOUNT_PROTOCOL, name);
		    sprintf(t, "%s", account->getProtocol());
		    mn->setPropertyValue(valname, t);

		    sprintf(valname, PROPERTY_MAIL_ACCOUNT_USERNAME, name);
		    sprintf(t, "%s", account->getUsername());
		    mn->setPropertyValue(valname, t);

		    sprintf(valname, PROPERTY_MAIL_ACCOUNT_PASSWORD, name);
		    sprintf(t, "%s", account->getPassword());
		    mn->setPropertyValue(valname, t);

		    sprintf(valname, PROPERTY_MAIL_ACCOUNT_IN_SERVER, name);
		    sprintf(t, "%s", account->getInServer());
		    mn->setPropertyValue(valname, t);

		    sprintf(valname, PROPERTY_MAIL_ACCOUNT_OUT_SERVER, name);
		    sprintf(t, "%s", account->getOutServer());
		    mn->setPropertyValue(valname, t);

		    sprintf(valname, PROPERTY_MAIL_ACCOUNT_IN_PORT, name);
		    sprintf(t, "%s", account->getInPort());
		    mn->setPropertyValue(valname, t);

		    sprintf(valname, PROPERTY_MAIL_ACCOUNT_OUT_PORT, name);
		    sprintf(t, "%s", account->getOutPort());
		    mn->setPropertyValue(valname, t);

		    sprintf(valname, PROPERTY_MAIL_ACCOUNT_IN_SSL, name);
		    sprintf(t, "%s", account->getInSSL());
		    mn->setPropertyValue(valname, t);

	    	sprintf(valname, PROPERTY_MAIL_ACCOUNT_OUT_SSL, name);
		    sprintf(t, "%s", account->getOutSSL());
		    mn->setPropertyValue(valname, t);

		    sprintf(valname, PROPERTY_MAIL_ACCOUNT_SIGNATURE, name);
		    sprintf(t, "%s", account->getSignature());
		    mn->setPropertyValue(valname, t);

	    	sprintf(valname, PROPERTY_MAIL_ACCOUNT_DOMAINNAME, name);
		    sprintf(t, "%s", account->getDomainName());
		    mn->setPropertyValue(valname, t);

            sprintf(valname, PROPERTY_MAIL_ACCOUNT_TO_BE_CLEANED, name);
            
            int toBeCleaned = 0;
            if (account->getToBeCleaned()){
                toBeCleaned = 1;
            }
            sprintf(t, "%d", toBeCleaned);
            mn->setPropertyValue(valname, t);

            sprintf(valname, PROPERTY_MAIL_ACCOUNT_ID, name);			
		    const WCHAR* accountIdw = account->getID();
	        const char *accountId = toMultibyte(accountIdw);
		    sprintf(t, "%s", accountId);
		    delete [] accountId;

		    mn->setPropertyValue(valname, t);
            delete mn;
            delete [] name;
		}

        delete [] fullcontext;
	}
}

void MailSourceManagementNode::getMailAccounts()
{
    char nname[512];
    char* tmp;
    char* fullcontext = toMultibyte(getFullContext());
    sprintf(nname, "%s/%s", fullcontext , PROPERTY_MAIL_ACCOUNT_ROOT);
    DeviceManagementNode* dmn = new DeviceManagementNode(nname);
    int numchild = dmn->getChildrenMaxCount();
    char** accountNames = dmn->getChildrenNames();

    for ( int p = 0; p<numchild; p++){
        MailAccount ma;
        char valname[512];
        char fullname[512];
        sprintf(fullname, "%s/%s",  fullcontext, PROPERTY_MAIL_ACCOUNT_ROOT);
        const char* name = stringdup(accountNames[p]);
        DeviceManagementNode* mn = new DeviceManagementNode(fullname,name);

        ma.setName(name);

        sprintf(valname,PROPERTY_MAIL_ACCOUNT_VISIBLE_NAME,name);
        tmp = mn->readPropertyValue(valname);
        ma.setVisibleName(tmp); safeDel(&tmp);
        sprintf(valname,PROPERTY_MAIL_ACCOUNT_EMAILADDRESS,name);
        tmp = mn->readPropertyValue(valname);
        ma.setEmailAddress(tmp); safeDel(&tmp);
        sprintf(valname,PROPERTY_MAIL_ACCOUNT_PROTOCOL,name);
        tmp = mn->readPropertyValue(valname);
        ma.setProtocol(tmp); safeDel(&tmp);
        sprintf(valname,PROPERTY_MAIL_ACCOUNT_USERNAME,name);
        tmp = mn->readPropertyValue(valname);
        ma.setUsername(tmp); safeDel(&tmp);
        sprintf(valname,PROPERTY_MAIL_ACCOUNT_PASSWORD,name);
        tmp = mn->readPropertyValue(valname);
        ma.setPassword(tmp); safeDel(&tmp);
        sprintf(valname,PROPERTY_MAIL_ACCOUNT_IN_SERVER,name);
        tmp = mn->readPropertyValue(valname);
        ma.setInServer(tmp); safeDel(&tmp);
        sprintf(valname,PROPERTY_MAIL_ACCOUNT_OUT_SERVER,name);
        tmp = mn->readPropertyValue(valname);
        ma.setOutServer(tmp); safeDel(&tmp);
        sprintf(valname,PROPERTY_MAIL_ACCOUNT_IN_PORT,name);
        tmp = mn->readPropertyValue(valname);
        ma.setInPort(tmp); safeDel(&tmp);
        sprintf(valname,PROPERTY_MAIL_ACCOUNT_OUT_PORT,name);
        tmp = mn->readPropertyValue(valname);
        ma.setOutPort(tmp); safeDel(&tmp);
        sprintf(valname,PROPERTY_MAIL_ACCOUNT_IN_SSL,name);
        tmp = mn->readPropertyValue(valname);
        ma.setInSSL(tmp); safeDel(&tmp);
        sprintf(valname,PROPERTY_MAIL_ACCOUNT_OUT_SSL,name);
        tmp = mn->readPropertyValue(valname);
        ma.setOutSSL(tmp); safeDel(&tmp);
        sprintf(valname,PROPERTY_MAIL_ACCOUNT_SIGNATURE,name);
        tmp = mn->readPropertyValue(valname);
        ma.setSignature(tmp); safeDel(&tmp);
        sprintf(valname,PROPERTY_MAIL_ACCOUNT_DOMAINNAME,name);
        tmp = mn->readPropertyValue(valname);
        ma.setDomainName(tmp); safeDel(&tmp);

        sprintf(valname,PROPERTY_MAIL_ACCOUNT_TO_BE_CLEANED,name);
        tmp = mn->readPropertyValue(valname);
        if(strcmp(tmp, "1") == 0){
            ma.setToBeCleaned(true);
        }

        sprintf(valname,PROPERTY_MAIL_ACCOUNT_ID,name);
        tmp = mn->readPropertyValue(valname);
        WCHAR* idW = toWideChar(tmp); safeDel(&tmp);
        ma.setID(idW);
        if (idW) { delete [] idW; }

        config.addMailAccount(ma);

        delete [] name;
    }

    delete [] fullcontext;
}


MailSyncSourceConfig& MailSourceManagementNode::getMailSourceConfig(bool refresh) {
    if (refresh) {
        char*  c = NULL;
        char* tmp;
 
        config.setName((tmp = readPropertyValue(PROPERTY_SOURCE_NAME)));
        safeDel(&tmp);
        config.setURI((tmp = readPropertyValue(PROPERTY_SOURCE_URI)));
        safeDel(&tmp);
        config.setSyncModes((tmp = readPropertyValue(PROPERTY_SOURCE_SYNC_MODES)));
        safeDel(&tmp);
        config.setSync((tmp = readPropertyValue(PROPERTY_SOURCE_SYNC)));
        safeDel(&tmp);
        config.setType((tmp = readPropertyValue(PROPERTY_SOURCE_TYPE)));
        safeDel(&tmp);

        config.setVersion((tmp = readPropertyValue(PROPERTY_SOURCE_VERSION)));
        safeDel(&tmp);
        config.setEncoding((tmp = readPropertyValue(PROPERTY_SOURCE_ENCODING)));
        safeDel(&tmp);
        config.setSupportedTypes((tmp = readPropertyValue(PROPERTY_SOURCE_SUPP_TYPES)));
        safeDel(&tmp);

        config.setLast(strtol((tmp = readPropertyValue(PROPERTY_SOURCE_LAST_SYNC)), &c, 10));
        safeDel(&tmp);
        config.setDownloadAge((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_DOWNLOAD_AGE)), &c, 10));
        safeDel(&tmp);
        config.setBodySize((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_BODY_SIZE)), &c, 10));
        safeDel(&tmp);
        config.setAttachSize((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_ATTACH_SIZE)), &c, 10));
        safeDel(&tmp);

        config.setInbox((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_INBOX)), &c, 10));
        safeDel(&tmp);
        config.setDraft((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_DRAFT)), &c, 10));
        safeDel(&tmp);
        config.setTrash((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_TRASH)), &c, 10));
        safeDel(&tmp);
        config.setOutbox((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_OUTBOX)), &c, 10));
        safeDel(&tmp);
        config.setSent((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_SENT)), &c, 10));
        safeDel(&tmp);
        config.setSchedule((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_SCHEDULE)), &c, 10));
        safeDel(&tmp);
        config.setEncryption((tmp = readPropertyValue(PROPERTY_SOURCE_ENCRYPTION)));
        safeDel(&tmp);

        getMailAccounts();
    }

    return config;
}

void MailSourceManagementNode::setMailSourceConfig(MailSyncSourceConfig& c) {
    config.assign(c);
    char t[512];
    setMailAccounts(c);

    setPropertyValue(PROPERTY_SOURCE_NAME,       (char* )c.getName());
    setPropertyValue(PROPERTY_SOURCE_URI,        (char* )c.getURI());
    setPropertyValue(PROPERTY_SOURCE_SYNC_MODES, (char* )c.getSyncModes());
    setPropertyValue(PROPERTY_SOURCE_SYNC,       (char* )c.getSync());
    setPropertyValue(PROPERTY_SOURCE_TYPE,       (char* )c.getType());

    setPropertyValue(PROPERTY_SOURCE_VERSION,    (char* )c.getVersion());
    setPropertyValue(PROPERTY_SOURCE_ENCODING,   (char* )c.getEncoding());
    setPropertyValue(PROPERTY_SOURCE_SUPP_TYPES, (char* )c.getSupportedTypes());

    sprintf(t, "%ld", c.getLast());
    setPropertyValue(PROPERTY_SOURCE_LAST_SYNC, t);
    sprintf(t, "%d", c.getDownloadAge());
    setPropertyValue(PROPERTY_SOURCE_DOWNLOAD_AGE, t);
    sprintf(t, "%d", c.getBodySize());
    setPropertyValue(PROPERTY_SOURCE_BODY_SIZE, t);
    sprintf(t, "%d", c.getAttachSize());
    setPropertyValue(PROPERTY_SOURCE_ATTACH_SIZE, t);

    sprintf(t, "%d", c.getInbox());
    setPropertyValue(PROPERTY_SOURCE_INBOX, t);
    sprintf(t, "%d", c.getOutbox());
    setPropertyValue(PROPERTY_SOURCE_OUTBOX, t);
    sprintf(t, "%d", c.getTrash());
    setPropertyValue(PROPERTY_SOURCE_TRASH, t);
    sprintf(t, "%d", c.getSent());
    setPropertyValue(PROPERTY_SOURCE_SENT, t);
    sprintf(t, "%d", c.getDraft());
    setPropertyValue(PROPERTY_SOURCE_DRAFT, t);
    sprintf(t, "%d", c.getSchedule());
    setPropertyValue(PROPERTY_SOURCE_SCHEDULE, t);

    setPropertyValue(PROPERTY_SOURCE_ENCRYPTION,       (char* )c.getEncryption());
	

}


ArrayElement* MailSourceManagementNode::clone()  {
    return new MailSourceManagementNode(context, name, config);
}
