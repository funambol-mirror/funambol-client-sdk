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


#ifndef INCL_MAIL_ACCOUNT_MANAGER
#define INCL_MAIL_ACCOUNT_MANAGER
/** @cond DEV */

#include "base/util/utils.h"
#include "base/util/ArrayElement.h"
#include "base/util/StringBuffer.h"
#include "mail/MailSyncSourceConfig.h"
#include "mail/MailAccount.h"
#include "spds/FolderData.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


class AccountProvisioner;
      
/**
 * Manager for Email accounts.
 * It's used to add/modify/delete email accounts and folders.
 * All settings for each email account are stored in the config, passed in constructor,
 * in order to be able to check for any local change in the account settings.
 * Clients should extend this class and implement virtual methods to create/update/delete
 * accounts and folders in the specific platform.
 */
class MailAccountManager {

public:

    MailAccountManager(MailSyncSourceConfig& ssc);
    virtual ~MailAccountManager();

    // ------------------------ Email account management ------------------------
    /**
     * Creates a new email account.
     * Calls createClientAccount(), if success then saves the config.
     * The new account ID is set inside the 'account' object.
     * @param account the account settings informations
     * @return 0 if no errors
     */
    int createAccount(MailAccount& account);

    /**
     * Updates an email account.
     * Calls updateClientAccount(), if success then saves the config.
     * @param account the account settings informations
     * @return 0 if no errors, -1 if account not found
     */
    int updateAccount(const MailAccount& account);

    /**
     * Deletes an email account.
     * Calls deleteClientAccount(), if success then saves the config.
     * @param accountID the account ID to be removed
     * @return 0 if no errors, -1 if account not found
     */
    int deleteAccount(const WCHAR* accountID);

    /**
     * marks an account as deleted on config
     * on first config save it will be cleared
     * @param accountID the account id to be removed
     * @return 0 if no errors, -1 if account not found
     */
    int markDeleteAccountOnConfig(const WCHAR* accountID);

    // ------------------------ Email folders management ------------------------
    /**
     * Creates a new folder under the parent account.
     * Calls createClientFolder(), if success then saves the config.
     * The new folder ID is set inside the 'folder' object.
     * Note: the folder name and parent are required.
     * @param folder the FolderData settings informations
     * @return 0 if no errors
     */
    int createFolder(FolderData& folder);

    /**
     * Updates an email folder under the parent account.
     * Calls updateClientFolder(), if success then saves the config.
     * Note: the folder name and parent are required.
     * @param folder the FolderData settings informations
     * @return 0 if no errors, -1 if folder not found
     */
    int updateFolder(const FolderData& folder);

    /**
     * Deletes an email folder under the parent account.
     * Calls deleteClientFolder(), if success then saves the config.
     * @param folderID  the key of folder to delete
     * @return 0 if no errors, -1 if folder not found
     */
    int deleteFolder(const WCHAR* folderID);


    /**
     * Returns the account ID from its index in the MailAccounts array.
     * Scans the mail accounts in the config.
     * @param index  the index [0 ; size-1]
     * @return the id (b64 key) of the account, an empty string if index out of range
     */
    StringBuffer getIdOfAccount(const int index);

    /**
     * Returns the account ID from its name.
     * Scans the mail accounts in the config.
     * @param accountName the account name to search
     * @return the id (b64 key) of the account, an empty string if account not found
     */
    StringBuffer getIdOfAccount(const StringBuffer& accountName);

    /**
    * Returns an account by name
    */
    MailAccount* getAccountByName(const WCHAR* accountName);

    /**
    * Returns a mail account by mail address
    */
    MailAccount* getAccountFromMailAddr(const char* mailAddr);

    /**
    * Returns an account class by id
    */
    MailAccount* getAccountById(const WCHAR* accountId);


    /// Returns the number of existing email accounts.
    int getAccountNumber();


    /**
     * Reads an email account given its ID.
     * Note: the account ID is required in input.
     * @param account    [IN-OUT] the email account to be filled with all settings
     */
    //int readAccount(MailAccount& account);
    
    /**
     * Reads an email folder given its name and the parent account ID.
     * Note: the folder name and parent are required in input.
     * @param folder     [IN-OUT] the email folder to be filled with all settings
     */
    //int readFolder(FolderData& folder);    
   
    /**
     * Returns the internal MailSyncSourceConfig.
     *
     * @return config, internal reference to the MailSyncSourceConfig
     */
    MailSyncSourceConfig& getMailSyncSourceConfig(){return config;}
    void setMailSyncSourceConfig(MailSyncSourceConfig& mssc){config.assign(mssc);}
protected:
    /**
     * Creates the email account on the Client.
	 * Clients must implement this method in order to create the new Email account.
     * The new account ID will be set inside the 'account' object.
     * @param account the account settings informations
     * @return 0 if no errors
     */
    virtual int createClientAccount(MailAccount& account) = 0;

    /**
     * Updates an email account.
     * Clients must implement this method in order to update a given email account.
     * @param account the account settings informations
     * @return 0 if no errors, -1 if account not found
     */
    virtual int updateClientAccount(const MailAccount& account) = 0;

    /**
     * Deletes an email account.
     * Clients must implement this method in order to delete an email account given its ID.
     * @param accountID the account ID to be removed
     * @return 0 if no errors, -1 if account not found
     */
    virtual int deleteClientAccount(const WCHAR* accountID) = 0;

    /**
     * Creates an email folder on the Client.
     * Clients must implement this method in order to create a new folder inside
     * the parent email account. 
     * Note: the new folder's ID must be set inside the folder object.
     * Note: the folder name and parent are required.
     * @param folder the FolderData settings informations
     * @return 0 if no errors
     */
    virtual int createClientFolder(FolderData& folder) = 0;

    /**
     * Updates an email folder on the Client.
     * Clients must implement this method in order to update a folder inside
     * the parent email account.
     * Note: the folder name and parent are required.
     * @param folder the FolderData settings informations
     * @return 0 if no errors
     */
    virtual int updateClientFolder(const FolderData& folder) = 0;

	/**
     * Deletes an email folder on the Client.
     * Clients must implement this method in order to create a new folder inside
     * the parent email account.
     * Note: the folder name and parent are required.
     * @param folder the FolderData settings informations
     * @return 0 if no errors
     */
    virtual int deleteClientFolder(const WCHAR* folderID) = 0;


    /// To synchronize config settings and account settings.
    //virtual int refreshAccounts() = 0;

    /// Checks the config, returns true if the account exists.
    bool accountExists(const StringBuffer& accountID);


    /**
     * Returns the account ID of the first account stored.
     * Scans the mail accounts in the config.
     * @return the id (b64 key) of the account, an empty string if no accounts found
     */
    StringBuffer getIdOfFirstAccount();

    /// Reference to config of MailSyncSource. Used to read and save email accounts settings.
    MailSyncSourceConfig& config;


};


END_NAMESPACE

/** @endcond */
#endif

