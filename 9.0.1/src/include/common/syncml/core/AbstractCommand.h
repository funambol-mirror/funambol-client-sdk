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



#ifndef INCL_ABSTRACT_COMMAND
#define INCL_ABSTRACT_COMMAND
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "syncml/core/CmdID.h"
#include "syncml/core/Meta.h"
#include "syncml/core/Cred.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE



/**
 * This class implements an abstract command. It must be derived to be used by other classes.
 *
 */

class AbstractCommand : public ArrayElement {

    // ---------------------------------------------------------- Protected data
    protected:

        CmdID*   cmdID ;
        bool noResp;
        Meta*    meta;
        Cred*    credential;

        void initialize();

    // ---------------------------------------------------------- Protected data
    public:
    AbstractCommand();

    AbstractCommand(CmdID* cmdID, bool noResp);

    /**
     * Create a new AbstractCommand object with the given commandIdentifier
     *
     * @param cmdID the command identifier - NOT NULL
     *
     */
     AbstractCommand(CmdID* cmdID);

    /**
     * Create a new AbstractCommand object with the given commandIdentifier
     * and noResponse
     *
     * @param cmdID the command identifier - NOT NULL
     * @param noResponse true if the command doesn't require a response
     * @param meta the Meta object
     */
     AbstractCommand(CmdID* cmdID, bool noResp, Meta* meta);

     void set(CmdID* cmdID, bool noResp);

     virtual ~AbstractCommand();

    /**
     * Get CommandIdentifier property
     *
     * @return the command identifier - NOT NULL
     */
     CmdID* getCmdID();

    /**
     * Sets the CommandIdentifier property
     *
     * @param cmdID the command identifier
     *
     */
     void setCmdID(CmdID* cmdID);

    /**
     * Gets noResp property
     *
     * @return true if the command doesn't require a response, false otherwise
     */
     bool isNoResp();

     bool getNoResp();

    /**
     * Sets noResp true if no response is required
     *
     * @param noResp is true if no response is required
     *
     */
     void setNoResp(bool noResp);

    /**
     * Gets Credential object
     *
     * @return the Credential object
     */
     Cred* getCred();

    /**
     * Sets authentication credential
     *
     * @param cred the authentication credential
     *
     */
     void setCred(Cred* cred);

    /**
     * Gets an Meta object
     *
     * @return an Meta object
     */
     Meta* getMeta();

    /**
     * Sets Meta object
     *
     * @param meta the meta object
     *
     */
     void setMeta(Meta* meta);

    /**
     * Get name property
     *
     * @return the name of the command
     */
    virtual const char* getName() = 0;

    virtual ArrayElement* clone() = 0;

};


END_NAMESPACE

/** @endcond */
#endif
