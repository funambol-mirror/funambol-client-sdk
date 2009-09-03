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


#include "spds/CredentialHandler.h"
#include "base/globalsdef.h"

USE_NAMESPACE

/*
 * Default constructor
 */
CredentialHandler::CredentialHandler() {
    initialize();
}

/**
 * Initializes private members
 */
void CredentialHandler::initialize() {
    username       = NULL;
    password       = NULL;
    clientAuthType = NULL;
    clientNonce    = NULL;

    serverID       = NULL;
    serverPWD      = NULL;
    serverAuthType = NULL;
    serverNonce    = NULL;
}

/*
 * Destructor. Free the allocated memory (if any)
 */
CredentialHandler::~CredentialHandler() {
    safeDel(&username       );
    safeDel(&password       );
    safeDel(&clientAuthType );
    safeDel(&clientNonce    );

    safeDel(&serverID       );
    safeDel(&serverPWD      );
    safeDel(&serverAuthType );
    safeDel(&serverNonce    );

}

void CredentialHandler::setUsername(const char* t) {
    safeDel(&username);
    username = stringdup(t);
}

const char *CredentialHandler::getUsername() {
    return username;
}

void CredentialHandler::setPassword(const char* t) {
    safeDel(&password);
    password = stringdup(t);
}

const char *CredentialHandler::getPassword() {
    return password;
}

void CredentialHandler::setClientAuthType(const char* t){
    safeDel(&clientAuthType);
    clientAuthType = stringdup(t);
}

const char* CredentialHandler::getClientAuthType(){
    return clientAuthType;
}


void CredentialHandler::setClientNonce(const char* t){
    safeDel(&clientNonce);
    clientNonce = stringdup(t);
}

const char* CredentialHandler::getClientNonce() {
    return clientNonce;
}

void CredentialHandler::setServerID(const char* t) {
    safeDel(&serverID);
    serverID = stringdup(t);
}

void CredentialHandler::setServerPWD(const char* t) {
    safeDel(&serverPWD);
    serverPWD = stringdup(t);
}

void CredentialHandler::setServerAuthType(const char* t) {
    safeDel(&serverAuthType);
    serverAuthType = stringdup(t);
}

const char* CredentialHandler::getServerAuthType() {
    return serverAuthType;
}

void CredentialHandler::setServerNonce(const char* t) {
    safeDel(&serverNonce);
    serverNonce = stringdup(t);
}

const char* CredentialHandler::getServerNonce() {
    return serverNonce;
}

void CredentialHandler::setServerAuthRequired(bool t) {
    isServerAuthRequired = t;
}

bool CredentialHandler::getServerAuthRequired() {
    return isServerAuthRequired;
}

Cred* CredentialHandler::getClientCredential() {

    Authentication* auth = NULL;
    char* credential  = NULL;

    if (strcmp(clientAuthType, AUTH_TYPE_MD5) == 0) {
        credential = MD5CredentialData(username, password, clientNonce);
        auth = new Authentication(AUTH_TYPE_MD5, credential);
        // overwrite the username that for MD5 auth is the same as data
        auth->setUsername(username);
        auth->setPassword(password);
        if (credential) { delete [] credential; credential = NULL; }
    } else {
        auth = new Authentication(AUTH_TYPE_BASIC, username, password);
    }

    Cred* cred           = new Cred(auth);

    deleteAuthentication(&auth);
    return cred;

}

/*
* it returns the server credential. The method is used both during the request of authentication
* and the creation of the status as response of server authentication.
* Therefore, if the server is arleady authenticated, no credential are sent back.
*/


Cred* CredentialHandler::getServerCredential() {

    Authentication* auth = NULL;
    Cred* cred           = NULL;
    char* credential  = NULL;
    if (strcmp(serverAuthType, AUTH_TYPE_BASIC) == 0) {
        auth = new Authentication(AUTH_TYPE_BASIC, serverID, serverPWD);
    } else {
        credential = MD5CredentialData(serverID, serverPWD, serverNonce);
        auth = new Authentication(AUTH_TYPE_MD5, credential);
    }

    cred = new Cred(auth);

    deleteAuthentication(&auth);
    return cred;

}

bool CredentialHandler::performServerAuth(Cred* cred) {

    bool ret = false;
    Cred* currentCred = getServerCredential();

    if (cred == NULL || currentCred == NULL) {
        goto finally;
    }

    if (strcmp(cred->getData(), currentCred->getData()) == 0) {
        ret = true;
    }
finally:

    return ret;
}

Chal* CredentialHandler::getServerChal(bool isServerAuthenticated) {

    Chal* chal = NULL;

    if (strcmp(serverAuthType, AUTH_TYPE_BASIC) == 0 && isServerAuthenticated == false) {
        chal = Chal::getBasicChal();

    } else if (strcmp(serverAuthType, AUTH_TYPE_MD5) == 0) { // MD5
        chal = Chal::getMD5Chal();
        char nonce[16];
        generateNonce(nonce);
        NextNonce* nextNonce = new NextNonce(nonce, 16);
        chal->setNextNonce(nextNonce);
        setServerNonce(nextNonce->getValueAsBase64());
    }

    return chal;
}


// private
void CredentialHandler::generateNonce(char nonce[16]) {
    srand((unsigned int)time(NULL));
    for (unsigned int i = 0; i < 16; i++) {
        nonce[i] = ((rand()%100) * (rand()%100))%100;

        if (nonce[i] < 32) {
            nonce[i] +=96;
        }
    }
}


