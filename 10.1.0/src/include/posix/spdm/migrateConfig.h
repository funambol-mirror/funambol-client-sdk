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

#include "base/util/StringBuffer.h"
#include "base/util/utils.h"
#include "base/fscapi.h"
#include "spdm/constants.h"

#define CONFTXT "config.txt"
#define CONFINI "config.ini"

USE_FUNAMBOL_NAMESPACE

/**
 * This function migrate the configuration tree from the old system to the new one
 *
 * @param const char* oldPath the old path to move e.g. /home/pippo/.sync4j/appName
 * @param const char* newPath the new path e.g. /home/pippo/.config/appName
 * @param const char* oldName the old config file name: "config.txt" for default
 * @patam const char* newName the new config file name: "config.ini" for default
 *
 * @return bool true if everything is moved and the migration process is successfull
 */
bool migrateConfig(const char* oldPath, const char* newPath, 
                   const char* oldName = 0, const char* newName = 0){
    bool ret = false;
    if (!oldName){
        oldName = CONFTXT;
    }
    if (!newName){
        newName = CONFINI;
    }
    StringBuffer command;
    command = "mv ";
    command += oldPath; 
    command += " " ;
    command += newPath;
    if (system(command.c_str()) == 0 ){
        ret = true;
    }else{
        ret = false;
    }
    chdir(newPath);
    //for file in `find -name config.txt`; do dirs=`dirname $file`; mv $file $dirs/config.ini; done
    StringBuffer val;
    val ="for file in `find -name ";
    val += oldName;
    val += "`; do dirs=`dirname $file`; mv $file $dirs/";
    val += newName;
    val += "; done";
    if (system(val.c_str()) == 0 && ret){
        ret = true;
    }else{
        ret = false;
    }
    return ret; 
}

