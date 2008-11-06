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


#ifndef INCL_AUTHENTICATION
#define INCL_AUTHENTICATION
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "base/base64.h"
#include "syncml/core/Meta.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class Authentication {

     // ------------------------------------------------------------ Private data
    private:
        char*  data;
        char*  username;
        char*  password;
        bool encode;
        char*  deviceId;
        char*  syncMLVerProto;
        char*  principalId;
        Meta* meta;

        void initialize();

    // ---------------------------------------------------------- Public data
    public:

    // ---------------------------------------------------------- Constructor
        Authentication();
        ~Authentication();

       /**
        * used for clone action. It clone every value.
        */
        Authentication(Authentication* auth);

        /**
         * Creates a new Authentication object with the given data
         *
         * @param meta the Meta object with authentication type and format
         * @param data the data of authentication
         *
         */
        Authentication(Meta* meta, const char*  data);

        /**
         * Creates a new Authentication object with the given data
         *
         * @param type the authentication type
         * @param data the data of authentication
         *
         */
        Authentication(const char*  type, const char*  data);

        /**
         * Creates a new Authentication object with the given data
         *
         * @param type the authentication type
         * @param data the data of authentication
         * @param encode true if data is encoded, false otherwise
         *
         */
        Authentication(const char*  type,
                       const char*  data,
                       bool encode);

        /**
         * Creates a new Authentication object with the given data
         *
         * @param type the authentication type
         * @param username the username
         * @param password the password
         *
         */
        Authentication(const char*  type,
                       const char*  username,
                       const char*  password);

        // ---------------------------------------------------------- Public methods

        void createAuthentication(const char*  type, const char*  data);

        /**
         * Gets the type property
         *
         * @return the type property
         */
        const char* getType();

        /**
         * Sets the type property
         *
         * @param type the type property
         */
        void setType(const char* type);

        /**
         * Gets the format property
         *
         * @return the format property
         */
        const char* getFormat();

        /**
         * Sets the format property
         *
         * @param format the format property
         */
        void setFormat(const char* format);

        /**
         * Gets the data property
         *
         * @return the data property
         */
        const char* getData();

        /**
         * Sets the data property
         *
         * @param data the data property
         *
         */
        void setData(const char* data);


        /**
         * Gets username property
         *
         * @return the username property
         */
        const char* getUsername();

        /**
         * Sets the username property
         *
         * @param username the username property
         */
        void setUsername(const char* username);

        /**
         * Gets password property
         *
         * @return the password property
         */
        const char* getPassword();

        /**
         * Sets the password property
         *
         * @param password the password property
         */
        void setPassword(const char* password);

        /**
         * Gets the nextNonce property
         *
         * @return nextNonce the nextNonce property
         */
        NextNonce* getNextNonce();

        /**
         * Sets the nextNonce property
         *
         * @param nextNonce the nextNonce property
         *
         */
        void setNextNonce(NextNonce* nextNonce);

        /**
         * Gets the meta property
         *
         * @return meta the meta property
         */
        Meta* getMeta();

        /**
         * Sets the meta property
         *
         * @param meta the meta property
         *
         */
        void setMeta(Meta* meta);

        /**
         * Gets the device id
         *
         * @return deviceId the device identificator
         */
        const char* getDeviceId();

        /**
         * Sets the device identificator
         *
         * @param deviceId the device identificator
         */
        void setDeviceId(const char* deviceId);

        /**
         * Gets the SyncML Protocol version. It is useful to decide how calculate
         * the digest with MD5 authentication.
         *
         * @return syncMLVerProto the SyncML Protocol version.
         */
        const char* getSyncMLVerProto();

        /**
         * Sets the SyncML Protocol version. It is useful to decide how calculate
         * the digest with MD5 authentication.
         *
         * @param syncMLVerProto the SyncML Protocol version.
         *
         */
         void setSyncMLVerProto(const char* syncMLVerProto);

        /**
         * Gets the principal id
         *
         * @return principalId the principal identificator
         */
        const char* getPrincipalId();

        /**
         * Sets the principal identificator
         *
         * @param principalId the principal identificator
         */
        void setPrincipalId(const char* principalId);

        Authentication* clone();

};


END_NAMESPACE

/** @endcond */
#endif
