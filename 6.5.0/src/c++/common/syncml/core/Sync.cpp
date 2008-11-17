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

#include "syncml/core/Sync.h"

Sync::Sync() {

    initialize();

    COMMAND_NAME = new char[strlen(SYNC_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, SYNC_COMMAND_NAME);
}

Sync::~Sync() {
    if (COMMAND_NAME)   { delete [] COMMAND_NAME; COMMAND_NAME = NULL; }
    if (target)         { delete target; target = NULL; }
    if (source)         { delete source; source = NULL; }
    if (commands)       { commands->clear(); } //delete commands; commands = NULL;         }
    numberOfChanges = 0;
}

/**
* Creates a new Sync object
*
* @param cmdID the command identifier - NOT NULL
* @param noResp is <b>true</b> if no response is required
* @param cred the authentication credential
* @param target the target object
* @param source the source object
* @param meta the meta object
* @param numberOfChanges the number of changes
* @param commands an array of elements that must be of one of the
*                 following types: {@link Add}, {@link Atomic},
*                 {@link Copy}, {@link Delete}, {@link Replace},
*                 {@link Sequence}
*
*
*/
Sync::Sync(CmdID* cmdID,
        BOOL noResp,
        Cred* cred,
        Target* target,
        Source* source,
        Meta* meta,
        long numberOfChanges,
        ArrayList* commands) : AbstractCommand (cmdID, noResp, meta) {

    initialize();

    COMMAND_NAME = new char[strlen(SYNC_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, SYNC_COMMAND_NAME);


    setCommands(commands);
    setCred(cred);

    setNoResp(noResp);
    setTarget(target);
    setSource(source);
    this->numberOfChanges  = numberOfChanges;

}

void Sync::initialize() {
    target = NULL;
    source = NULL;
    commands = new ArrayList();
    numberOfChanges = 0;
}

/**
* Gets the Target object property
*
* @return target the Target object property
*/
Target* Sync::getTarget() {
    return target;
}

/**
* Sets the Target object property
*
* @param target the Target object property
*
*/
void Sync::setTarget(Target* target) {
    if (this->target) {
        delete this->target; this->target = NULL;
    }
    if (target) {
        this->target = target->clone();
    }
}
/**
* Gets the Source object property
*
* @return source the Source object property
*/

Source* Sync::getSource() {
    return source;
}


/**
* Gets the Source object property
*
* @param source the Source object property
*/
void Sync::setSource(Source* source) {
    if (this->source) {
        delete this->source; this->source = NULL;
    }
    if (source) {
        this->source = source->clone();
    }
}

/**
*
* @return The return value is guaranteed to be non-null.
*          The array elements are guaranteed to be non-null.
*
*/
ArrayList* Sync::getCommands() {
    return commands;
}

/**
* Sets the sequenced commands. The given commands must be of the allowed
* types.
*
* @param commands the commands - NOT NULL and o the allawed types
*
*/
void Sync::setCommands(ArrayList* commands) {
    BOOL err = FALSE;
    if (commands == NULL) {
        // TBD
        err = TRUE;
    }
    for (int i = 0; i < commands->size(); i++) {
        if (commands->get(i) == NULL) {
            // TBD
            err = TRUE;
        }
    }
    if (err == FALSE) {
        this->commands->clear();
        this->commands = commands->clone();
    }
}

/**
* Gets the total number of changes
*
* @return the total number of changes
*/
long Sync::getNumberOfChanges() {
    return numberOfChanges;
}


/**
* Sets the numberOfChanges property
*
* @param numberOfChanges the total number of changes
*/
void Sync::setNumberOfChanges(long numberOfChanges) {
    this->numberOfChanges = numberOfChanges;
}

const char* Sync::getName() {
    return COMMAND_NAME;
}

ArrayElement* Sync::clone() {
    Sync* ret = new Sync(getCmdID(), getNoResp(), getCred(), target, source, getMeta(), numberOfChanges, commands);
    return ret;
}

