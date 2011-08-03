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


#include "syncml/core/Get.h"
#include "base/globalsdef.h"

USE_NAMESPACE


Get::Get() {
    lang = NULL;
    COMMAND_NAME = new char[strlen(GET_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, GET_COMMAND_NAME);
}

Get::~Get() {
    if (lang) {
        delete [] lang; lang = NULL;
    }
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
}
/**
* Creates a new Get object with the given command identifier,
* noResponse, language, credential, meta and an array of item
*
* @param cmdID the command identifier - NOT NULL
* @param noResp true if no response is required
* @param lang the preferred language for results data
* @param cred the authentication credential
* @param meta the meta information
* @param items the array of item - NOT NULL
*
*/
Get::Get(CmdID* cmdID,
         bool noResp,
         const char* lang,
         Cred* cred,
         Meta* meta,
         ArrayList* items) : ItemizedCommand(cmdID, meta, items) {

    this->lang = NULL;
    COMMAND_NAME = new char[strlen(GET_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, GET_COMMAND_NAME);

    setCred(cred);
    setNoResp(noResp);
    setLang(lang);
}


/**
* Returns the preferred language
*
* @return the preferred language
*
*/
const char* Get::getLang() {
    return lang;
}

/**
* Sets the preferred language
*
* @param lang new preferred language
*/
void Get::setLang(const char*lang) {
    if (this->lang) {
        delete [] this->lang; this->lang = NULL;
    }
    this->lang = stringdup(lang);
}

/**
* Gets the command name property
*
* @return the command name property
*/
const char* Get::getName() {
    return COMMAND_NAME;
}

ArrayElement* Get::clone() {
    Get* ret = new Get(getCmdID(), getNoResp(), lang, getCred(), getMeta(), getItems());
    return ret;
}
