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


#include "syncml/core/Results.h"
#include "base/globalsdef.h"

USE_NAMESPACE


Results::Results() {
    COMMAND_NAME = new char[strlen(RESULTS_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, RESULTS_COMMAND_NAME);
}
Results::~Results() {
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
}

/**
* Creates a new Results object.
*
* @param cmdID command identifier - NOT NULL
* @param msgRef message reference
* @param cmdRef command reference - NOT NULL
* @param meta meta information
* @param targetRef target reference
* @param sourceRef source reference
* @param items command items
*
*
*/
Results::Results(CmdID*      cmdID,
                 const char*    msgRef,
                 const char*    cmdRef,
                 Meta*       meta,
                 ArrayList*  targetRef,
                 ArrayList*  sourceRef,
                 ArrayList*  items) : ResponseCommand (cmdID, msgRef, cmdRef,
                                      targetRef ,
                                      sourceRef ,
                                      items ) {


    COMMAND_NAME = new char[strlen(RESULTS_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, RESULTS_COMMAND_NAME);

    setMeta(meta);
}

/**
* Returns the command name.
*
* @return the command name
*/
const char* Results::getName() {
    return COMMAND_NAME;
}

ArrayElement* Results::clone() {
    Results* ret = new Results(getCmdID(), getMsgRef(), getCmdRef(),
                               getMeta(), getTargetRef(), getSourceRef(), getItems());
    ret->setMeta(getMeta());
    return ret;
}
