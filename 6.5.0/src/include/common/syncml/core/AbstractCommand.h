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



#ifndef INCL_ABSTRACT_COMMAND
#define INCL_ABSTRACT_COMMAND
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "syncml/core/CmdID.h"
#include "syncml/core/Meta.h"
#include "syncml/core/Cred.h"



/**
 * This class implements an abstract command. It must be derived to be used by other classes.
 *
 */

class AbstractCommand : public ArrayElement {

    // ---------------------------------------------------------- Protected data
    protected:

        CmdID*   cmdID ;
        BOOL noResp;
        Meta*    meta;
        Cred*    credential;

        void initialize();

    // ---------------------------------------------------------- Protected data
    public:
    AbstractCommand();

    AbstractCommand(CmdID* cmdID, BOOL noResp);

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
     AbstractCommand(CmdID* cmdID, BOOL noResp, Meta* meta);

     void set(CmdID* cmdID, BOOL noResp);

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
     BOOL isNoResp();

     BOOL getNoResp();

    /**
     * Sets noResp true if no response is required
     *
     * @param noResp is true if no response is required
     *
     */
     void setNoResp(BOOL noResp);

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

/** @endcond */
#endif
