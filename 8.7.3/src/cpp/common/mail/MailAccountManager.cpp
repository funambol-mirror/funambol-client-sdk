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
#include "base/Log.h"
#include "mail/MailAccountManager.h"
#include "base/globalsdef.h"
#include "client/MailSourceManagementNode.h"

USE_NAMESPACE



MailAccountManager::MailAccountManager(MailSyncSourceConfig& ssc) : config(ssc) {}

MailAccountManager::~MailAccountManager() {}


int MailAccountManager::createAccount(MailAccount& account) {

    // Create the account on Client
    int ret = createClientAccount(account);

    if (ret) {
        LOG.error("Error creating email account, code %i", ret);
        return ret;
    }

    // Set account settings in config
	if (config.addMailAccount(account) == false) {
		LOG.error("error saving account in configuration");
		return 1;
	}
    return ret;
}

int MailAccountManager::updateAccount(const MailAccount& account) {

    // Update the account on Client
    int ret = updateClientAccount(account);
    if (ret) {
        LOG.error("Error updating email account, code %i", ret);
        return ret;
    }

    // Update account settings in config
    if (config.modifyMailAccount(account) == false) {
		LOG.error("error saving account in configuration");
		return 1;
    }
    return ret;
}

int MailAccountManager::deleteAccount(const WCHAR* accountID) {

    // Deletes the account on Client
    int ret = deleteClientAccount(accountID);
    if (ret) {
        LOG.error("Error deleting email account, code %i", ret);
        return ret;
    }

    // mark account to be deleted on client config 
    ret = markDeleteAccountOnConfig(accountID);

    return ret;
}

int MailAccountManager::markDeleteAccountOnConfig(const WCHAR* accountID)
{
    int ret = 0;

    // Remove account settings in config
    ArrayList mailAccounts = config.getMailAccounts();
	MailAccount* account = NULL;
	for (int i = 0; i<mailAccounts.size(); i++){
		if (wcscmp(((MailAccount*)mailAccounts[i])->getID(), accountID) == 0){
			account = (MailAccount*)mailAccounts[i];
			break;
		}
	}
	if (account) {
		// mark deleted
		config.setDeletedMailAccount( account->getName() );
	} else {
		ret = 1;
	}

    return ret;
}


int MailAccountManager::createFolder(FolderData& folder) {

    // Safe check
    if (folder.getName().empty() || folder.getParent().empty()) {
        return -2;
    }

    if ( !accountExists(folder.getParent()) ) {
        LOG.error("createFolder error: parent account '%s' not found", folder.getParent().c_str());
        return -1;
    }

    int ret = createClientFolder(folder);

    if (ret) {
        LOG.error("Error creating email folder, code %i", ret);
        return ret;
    }

    // folder's ID and settings are not stored in config (TODO?)
    return ret;
}

int MailAccountManager::updateFolder(const FolderData& folder) {

    // Safe check
    if (folder.getName().empty() || folder.getParent().empty()) {
        return -2;
    }

    if ( !accountExists(folder.getParent()) ) {
        LOG.error("updateFolder error: parent account '%s' not found", folder.getParent().c_str());
        return -1;
    }

    int ret = updateClientFolder(folder);
    
    if (ret) {
        LOG.error("Error updating email folder, code %i", ret);
        return ret;
    }

    // folder's ID and settings are not stored in config (TODO?)
    return ret;
}

int MailAccountManager::deleteFolder(const WCHAR* folderID) {

    // Safe check
    if (folderID == NULL) {
        return -2;
    }
    
    // Delete the folder on the Client.
    int ret = deleteClientFolder(folderID);
    if (ret) {
        LOG.error("Error deleting email folder, code %i", ret);
        return ret;
    }

    // folder's ID and settings are not stored in config (TODO?)
    return ret;
}



int MailAccountManager::getAccountNumber() {
    
    return config.getMailAccounts().size();
}

bool MailAccountManager::accountExists(const StringBuffer& accountID) {
    
    if (getAccountNumber() == 0) {
        return false;
    }
    if (accountID.empty()) {
        return false;
    }

    const ArrayList& accounts = config.getMailAccounts();
    for (int i=0; i<accounts.size(); i++) {
        MailAccount* account = (MailAccount*)accounts[i];
        if (account) {
            StringBuffer currentID;
            currentID.convert(account->getID());
            if (currentID == accountID) {
                // found
                return true;
            }
        }
    }

    // not found
    return false;
}


StringBuffer MailAccountManager::getIdOfAccount(const int index) {

    StringBuffer fid("");

    if (index < 0 || index >= getAccountNumber()) {
        LOG.error("index %d out of range for account list", index);
        return fid;
    }

    const ArrayList& accounts = config.getMailAccounts();
    MailAccount* account = (MailAccount*)accounts[index];
    if (account) {
        fid.convert(account->getID());
    }

    return fid;
}



StringBuffer MailAccountManager::getIdOfAccount(const StringBuffer& accountName) {

    StringBuffer fid("");

    if (accountName.empty()) {
        return fid;
    }

    const ArrayList& accounts = config.getMailAccounts();

    for (int i=0; i<accounts.size(); i++) {
        MailAccount* account = (MailAccount*)accounts[i];
        if (account) {
            if (accountName == account->getName()) {
                // found
                const WCHAR* wid = account->getID();
                const char* tid = toMultibyte(wid);
                fid = tid;
                delete [] tid; tid = NULL;
                break;
            }
        }
    }

    return fid;
}


MailAccount* MailAccountManager::getAccountByName(const WCHAR* name)
{
    MailAccount* account = NULL;

    StringBuffer accountName;
    accountName.convert(name);

    if (accountName.empty()) {
        return account;
    }

    const ArrayList& accounts = config.getMailAccounts();

    for (int i=0; i<accounts.size(); i++) {
        MailAccount* ma = (MailAccount*)accounts[i];
        if (ma) {
            if (accountName == ma->getName()) {
                // found
                account = new MailAccount(*ma);
            }
        }
    }

    return account;
}

MailAccount* MailAccountManager::getAccountById(const WCHAR* fid)
{
    MailAccount* account = NULL;
    
    if (fid == NULL) {
        LOG.error("%s: no account id specified", __FUNCTION__);
        return account;
    }
   
    const ArrayList& accounts = config.getMailAccounts();

    for (int i=0; i<accounts.size(); i++) {
        MailAccount* ma = (MailAccount*)accounts[i];
        if (ma) {
            const WCHAR* storedId = ma->getID();
            if (storedId) { 
                if (wcscmp(fid, storedId) == 0) {
                    // found
                    account = ma;
                }
            } else {
                LOG.error("%s: error getting ID from mail account", __FUNCTION__);
            }
        }
    }

    return account;
}

MailAccount* MailAccountManager::getAccountFromMailAddr(const char* mailAddr)
{
    MailAccount* account = NULL;

    if (mailAddr == NULL) {
        return account;
    }
    
    const ArrayList& accounts = config.getMailAccounts();

    for (int i=0; i<accounts.size(); i++) {
        MailAccount* ma = (MailAccount*)accounts[i];
        if (ma) {
            const char* addr = ma->getEmailAddress();
            if (addr) {
                if (strcmp(mailAddr, addr) == 0) {
                    // found
                    account = ma;
                }
            } else {
                LOG.error("%s: can't get email address from account");
            }
        }
    }

    return account;
}


StringBuffer MailAccountManager::getIdOfFirstAccount() {

    StringBuffer fid("");

    const ArrayList& accounts = config.getMailAccounts();
    for (int i=0; i<accounts.size(); i++) {
        MailAccount* account = (MailAccount*)accounts[i];
        if (account && (account->getDeleted() == false)) {
            fid.convert(account->getID());
            break;
        }
    }
    return fid;
}



/*int MailAccountManager::readAccount(MailAccount& account) {

    // TODO: read from config
    return 0;
}*/

/*int MailAccountManager::readFolder(FolderData& folder) {

    // Safe check
    if (folder.getName().empty() || folder.getParent().empty()) {
        return -2;
    }
    
    // TODO: read from config
    return 0;
}*/


