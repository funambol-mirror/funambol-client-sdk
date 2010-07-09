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


#ifndef INCL_ATOMIC
#define INCL_ATOMIC
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/AbstractCommand.h"


#define ATOMIC_COMMAND_NAME "Atomic"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class Atomic : public AbstractCommand {
     // ------------------------------------------------------------ Private data
    private:
       char*  COMMAND_NAME;
       ArrayList* commands;

    // ---------------------------------------------------------- Public data
    public:

    Atomic();
    ~Atomic();

    /**
     * Creates a new Atomic object with the given command identifier, noResponse,
     * meta and an array of abstract command
     *
     * @param cmdID the command identifier - NOT NULL
     * @param noResp is true if no response is required
     * @param meta the meta data
     * @param commands an array of abstract command - NOT NULL
     */
    Atomic(CmdID* cmdID,
           bool noResp,
           Meta* meta,
           ArrayList* commands); // AbstractCommand[]

    /**
     * Gets an array of AbstractCommand
     *
     * @return an array of command objects
     */
    ArrayList* getCommands();

    /**
     * Sets an array of AbstractCommand
     *
     * @param commands the array of AbstractCommand
     *
     */
    void setCommands(ArrayList* commands);
    /**
     * Gets the command name property
     *
     * @return the command name property
     */
    const char* getName();

    ArrayElement* clone();

};


END_NAMESPACE

/** @endcond */
#endif
