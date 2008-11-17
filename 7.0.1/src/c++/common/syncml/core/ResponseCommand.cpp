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


#include "syncml/core/ResponseCommand.h"
#include "base/globalsdef.h"

USE_NAMESPACE

ResponseCommand::ResponseCommand() {
    msgRef     = NULL;
    cmdRef     = NULL;
    targetRef  = new ArrayList(); // TargetRef[]
    sourceRef  = new ArrayList(); // SourceRef[]

}
ResponseCommand::~ResponseCommand()  {
    if (msgRef)     { delete [] msgRef;     msgRef    = NULL; }
    if (cmdRef)     { delete [] cmdRef;     cmdRef    = NULL; }
    if (targetRef)  { /*targetRef->clear();   }*/ delete targetRef; targetRef = NULL; }
    if (sourceRef)  { /*sourceRef->clear();   }*/ delete sourceRef; sourceRef = NULL; }

}

/**
 * Creates a new ResponseCommand object.
 *
 * @param cmdID the command idendifier  - NOT NULL
 * @param msgRef message reference
 * @param cmdRef command reference - NOT NULL
 * @param targetRefs target references
 * @param sourceRefs source references
 * @param items command items
 * if any of the NOT NULL parameter is null
 */
ResponseCommand::ResponseCommand(CmdID*         cmdID     ,
                                 const char*   msgRef    ,
                                 const char*   cmdRef    ,
                                 ArrayList*     targetRefs,
                                 ArrayList*     sourceRefs,
                                 ArrayList*     items      )
: ItemizedCommand (cmdID, items)
{
    this->msgRef     = stringdup(msgRef);
    this->cmdRef     = stringdup(cmdRef);
    this->targetRef  = targetRefs->clone();
    this->sourceRef  = sourceRefs->clone();
}


/**
 * Returns the message reference
 *
 * @return the message reference
 *
 */
const char* ResponseCommand::getMsgRef() {
    return msgRef;
}

/**
 * Sets the message reference
 *
 * @param msgRef message reference
 */
void ResponseCommand::setMsgRef(const char* msgRef) {
     if (this->msgRef) {
        delete [] this->msgRef; this->msgRef = NULL;
    }
    this->msgRef = stringdup(msgRef);
}

/**
 * Returns the command reference
 *
 * @return the command reference
 *
 */
const char* ResponseCommand::getCmdRef() {
    return cmdRef;
}

/**
 * Sets the command reference
 *
 * @param cmdRef commandreference - NOT NULL
 *
 */
void ResponseCommand::setCmdRef(const char* cmdRef) {
     if (this->cmdRef) {
         delete [] this->cmdRef; this->cmdRef = NULL;
     }
     this->cmdRef = stringdup(cmdRef);
}

/**
 * Returns the target references
 *
 * @return the target references
 *
 */
ArrayList* ResponseCommand::getTargetRef() {
    return targetRef;
}

/**
 * Sets the target references
 *
 * @param targetRefs target refrences
 */
void ResponseCommand::setTargetRef(ArrayList* targetRefs) {
    if (this->targetRef) {
        this->targetRef->clear();
    }
    if (targetRefs != NULL) {
        this->targetRef = targetRefs->clone();
    }
}

/**
 * Returns the source references
 *
 * @return the source references
 *
 */
ArrayList* ResponseCommand::getSourceRef(){
    return sourceRef;
}

/**
 * Sets the source references
 *
 * @param sourceRefs source refrences
 */
void ResponseCommand::setSourceRef(ArrayList* sourceRefs) {
    if (this->sourceRef) {
        this->sourceRef->clear();
    }
    if (sourceRefs != NULL) {
        this->sourceRef = sourceRefs->clone();
    }
}

