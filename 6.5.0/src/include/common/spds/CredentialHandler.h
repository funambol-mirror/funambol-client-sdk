/*
 * Copyright (C) 2003-2007 Funambol, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
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

        BOOL isServerAuthRequired;

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

        void setServerAuthRequired(BOOL t);
        BOOL getServerAuthRequired();

        Cred* getClientCredential();
        Cred* getServerCredential();
        Chal* getServerChal(BOOL isServerAuthenticated);
        BOOL  performServerAuth(Cred* cred);

    };

/** @endcond */
#endif
