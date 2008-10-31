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

 #ifndef INCL_CREDENTIAL_HANDLER
    #define INCL_CREDENTIAL_HANDLER
/** @cond DEV */

    #include "base/fscapi.h"
    #include "base/constants.h"
    #include "base/util/utils.h"
    #include "spds/constants.h"
    #include "syncml/core/TagNames.h"
    #include "syncml/core/ObjectDel.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

    class CredentialHandler{

    private:

        char*  username;
        char*  password;
        char*  clientAuthType;
        char*  clientNonce;

        char*  serverID;
        char*  serverPWD;
        char*  serverAuthType;
        char*  serverNonce;

        bool isServerAuthRequired;

        /**
         * Initializes private members
         */
        void initialize();
        void  generateNonce(char nonce[16]);

    public:
        /*
         * Default constructor
         */
        CredentialHandler();

        ~CredentialHandler();

        /*
         * Constructs a new SyncItem identified by the given key. The key must
         * not be longer than DIM_KEY (see SPDS Constants).
         *
         * @param key - the key
         */
        CredentialHandler(const char*  key);

        /*
         * Returns the SyncItem's key. If key is NULL, the internal buffer is
         * returned; if key is not NULL, the value is copied in the caller
         * allocated buffer and the given buffer pointer is returned.
         *
         * @param key - buffer where the key will be stored
         */

        void setUsername(const char*  t);
        const char*  getUsername();
        void setPassword(const char*  t);
        const char*  getPassword();
        void setClientAuthType(const char*  t);
        void setClientNonce(const char*  t);
        const char*  getClientAuthType();
        const char*  getClientNonce();

        void setServerID(const char*  t);
        void setServerPWD(const char*  t);
        void setServerAuthType(const char*  t);
        void setServerNonce(const char*  t);
        const char*  getServerAuthType();
        const char*  getServerNonce();

        void setServerAuthRequired(bool t);
        bool getServerAuthRequired();

        Cred* getClientCredential();
        Cred* getServerCredential();
        Chal* getServerChal(bool isServerAuthenticated);
        bool  performServerAuth(Cred* cred);

    };


END_NAMESPACE

/** @endcond */
#endif
