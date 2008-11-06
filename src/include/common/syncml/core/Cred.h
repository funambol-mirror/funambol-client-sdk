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


#ifndef INCL_CRED
#define INCL_CRED
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/Authentication.h"
#include "syncml/core/Constants.h"

class Cred {


     // ------------------------------------------------------------ Private data
    private:

        Authentication* authentication;

    // ---------------------------------------------------------- Protected data
    public:

        Cred();
        ~Cred();

        Cred(Authentication* authentication);

        /**
         * Gets type property
         *
         * @return type property
         */
        const char* getType();

        /**
         * Gets format property
         *
         * @return format property
         */
        const char* getFormat();

        /**
         * Gets data property
         *
         * @return data property
         */
        const char* getData();

        /**
         * Gets the username stored in this credential
         *
         * @return the username stored in this credential
         */
        const char* getUsername();


        /**
         * Create and return the Authentication object corresponding to the given
         * type and data.
         *
         * @param type the type of the required Authentication object
         * @param data the data to be interpreted based on the type
         *
         * @return the corresponding Authentication object.
         */
        Authentication* createAuthentication(char*  data, char*  type);

        /**
         * Gets the Authentication object.
         *
         * @return authentication the authentication objects
         */
        Authentication* getAuthentication();

        /**
         * Sets the Authentication object.
         *
         * @param auth the new Authentication object
         *
         */
        void setAuthentication(Authentication* auth);

        Cred* clone();
};

/** @endcond */
#endif
