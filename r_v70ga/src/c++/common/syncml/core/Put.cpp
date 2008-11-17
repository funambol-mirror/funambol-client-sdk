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

#include "syncml/core/Put.h"
#include "base/globalsdef.h"

USE_NAMESPACE

Put::Put() {
    lang = NULL;
    COMMAND_NAME = new char[strlen(PUT_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, PUT_COMMAND_NAME);
}

Put::~Put() {
    if (lang) {
        delete [] lang; lang = NULL;
    }
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
}
/**
* Creates a new Put object given its elements.
*
* @param cmdID the command identifier - NOT NULL
* @param noResp is &lt;NoResponse/&gt; required?
* @param lang Preferred language
* @param cred authentication credentials
* @param meta meta information
* @param items Item elements - NOT NULL
*
*/
Put::Put(CmdID* cmdID,
         bool noResp,
         char* lang,
         Cred* cred,
         Meta* meta,
         ArrayList* items ) : ItemizedCommand(cmdID, meta, items) {

    this->lang = NULL;

    setCred(cred);
    setNoResp(noResp);
    setLang(lang);

    COMMAND_NAME = new char[strlen(PUT_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, PUT_COMMAND_NAME);
}

// ----------------------------------------------------------- Public methods

/**
* Returns the preferred language
*
* @return the preferred language
*
*/
const char* Put::getLang() {
    return lang;
}

/**
* Sets the preferred language
*
* @param lang new preferred language
*/
void Put::setLang(const char*lang) {
    if (this->lang) {
        delete [] this->lang; this->lang = NULL;
    }
    this->lang = stringdup(lang);
}

/**
* Returns the command name
*
* @return the command name
*/
const char* Put::getName() {
    return COMMAND_NAME;
}

ArrayElement* Put::clone() {
    Put* ret = new Put(getCmdID(), getNoResp(), lang, getCred(), getMeta(), getItems());
    return ret;
}
