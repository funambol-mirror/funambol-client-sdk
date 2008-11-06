/*
 * Copyright (C) 2003-2007 Funambol, Inc.
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


#include "syncml/core/Results.h"


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
