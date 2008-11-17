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


#include "syncml/core/Sequence.h"


Sequence::Sequence() {

    COMMAND_NAME = new char[strlen(SEQUENCE_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, SEQUENCE_COMMAND_NAME);
    this->commands = new ArrayList();
}

Sequence::~Sequence() {
    if(COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
    if (commands) {
        commands->clear();  //delete commands; commands = NULL;
    }
}

/**
* Create a new Sequence object. The commands in <i>commands</i>
* must be of the allowed types.
*
* @param cmdID command identifier - NOT NULL
* @param noResp is &lt;NoREsponse/&gt; required?
* @param meta meta information
* @param commands the sequenced commands - NOT NULL
*
*/
Sequence::Sequence(CmdID*       cmdID ,
            BOOL         noResp,
            Meta*        meta  ,
            ArrayList*   commands) : AbstractCommand(cmdID, noResp) {

    COMMAND_NAME = new char[strlen(SEQUENCE_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, SEQUENCE_COMMAND_NAME);
    this->commands = new ArrayList();

    setMeta(meta);
    setCommands(commands);
}

/**
* Gets an array of AbstractCommand
*
* @return an array of command objects
*/
ArrayList* Sequence::getCommands() {
    return commands;
}

/**
* Sets the sequenced commands. The given commands must be of the allowed
* types.
*
* @param commands the commands - NOT NULL and o the allawed types
*/
void Sequence::setCommands(ArrayList* commands) {
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
* Returns the command name
*
* @return the command name
*/
const char* Sequence::getName(){
    return COMMAND_NAME;
}

ArrayElement* Sequence::clone() {
    Sequence* ret = new Sequence(getCmdID(), getNoResp(), getMeta(), commands);
    return ret;

}
