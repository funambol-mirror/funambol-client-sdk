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

 #ifndef INCL_SYNC_ITEM_KEYS
#define INCL_SYNC_ITEM_KEYS
/** @cond DEV */

#include "base/fscapi.h"
#include "base/constants.h"
#include "base/util/ArrayElement.h"
#include "spds/constants.h"
#include "spds/SyncStatus.h"
#include <string.h>
#include "base/globalsdef.h"

BEGIN_FUNAMBOL_NAMESPACE

   

class SyncItemKeys {

private:

    ArrayList addKeys;
    ArrayList modKeys;
    ArrayList delKeys;

    ArrayList& getAddKeys();
    ArrayList& getModKeys();
    ArrayList& getDelKeys();
    

public:
    /*
     * Default constructor
     */
    SyncItemKeys();

    ~SyncItemKeys(){}

    void insertAddKey(const char* key);

    void insertModKey(const char* key);

    void insertDelKey(const char* key);

    /**
    * Clear the list of keys related to the command argument
    *
    * @param command - the ADD, REPLACE, DELETE command used to clear the related list
    */
    void clearKeys(const char* command);

    ArrayList& getListKeys(const char* command);
   
};

END_FUNAMBOL_NAMESPACE

/** @endcond */
#endif
