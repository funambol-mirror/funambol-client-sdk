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

#ifndef INCL_ALERT_CODE
#define INCL_ALERT_CODE
/** @cond DEV */

#include "base/fscapi.h"


#define DISPLAY                               100
#define TWO_WAY                               200
#define SLOW                                  201
#define ONE_WAY_FROM_CLIENT                   202
#define REFRESH_FROM_CLIENT                   203
#define ONE_WAY_FROM_SERVER                   204
#define REFRESH_FROM_SERVER                   205
#define TWO_WAY_BY_SERVER                     206
#define ONE_WAY_FROM_CLIENT_BY_SERVER         207
#define REFRESH_FROM_CLIENT_BY_SERVER         208
#define ONE_WAY_FROM_SERVER_BY_SERVER         209
#define REFRESH_FROM_SERVER_BY_SERVER         210
#define RESULT_ALERT                          221
#define NEXT_MESSAGE                          222
#define NO_END_OF_DATA                        223
#define SMART_ONE_WAY_FROM_CLIENT             250
#define SMART_ONE_WAY_FROM_SERVER             251
#define INCREMENTAL_SMART_ONE_WAY_FROM_CLIENT 252
#define INCREMENTAL_SMART_ONE_WAY_FROM_SERVER 251


/**
* Determines if the given code is an initialization code, such as one of:
* <ul>
*   <li> TWO_WAY
*   <li> SLOW
*   <li> ONE_WAY_FROM_CLIENT
*   <li> REFRESH_FROM_CLIENT
*   <li> ONE_WAY_FROM_SERVER
*   <li> REFRESH_FROM_SERVER
*   <li> TWO_WAY_BY_SERVER
*   <li> ONE_WAY_FROM_CLIENT_BY_SERVE
*   <li> REFRESH_FROM_CLIENT_BY_SERVER
*   <li> ONE_WAY_FROM_SERVER_BY_SERVER
*   <li> REFRESH_FROM_SERVER_BY_SERVER
*   <li> SMART_ONE_WAY_FROM_CLIENT
*   <li> SMART_ONE_WAY_FROM_SERVER
*   <li> INCREMENTAL_SMART_ONE_WAY_FROM_CLIENT
*   <li> INCREMENTAL_SMART_ONE_WAY_FROM_SERVER
* </ul>
*
* @param code the code to be checked
*
* @return true if the code is an initialization code, false otherwise
*/
bool isInitializationCode(int code) {
  return (  (code == TWO_WAY                      )
         || (code == SLOW                         )
         || (code == ONE_WAY_FROM_CLIENT          )
         || (code == REFRESH_FROM_CLIENT          )
         || (code == ONE_WAY_FROM_SERVER          )
         || (code == REFRESH_FROM_SERVER          )
         || (code == TWO_WAY_BY_SERVER            )
         || (code == ONE_WAY_FROM_CLIENT_BY_SERVER)
         || (code == REFRESH_FROM_CLIENT_BY_SERVER)
         || (code == ONE_WAY_FROM_SERVER_BY_SERVER)
         || (code == REFRESH_FROM_SERVER_BY_SERVER)
         || (code == SMART_ONE_WAY_FROM_CLIENT    )
         || (code == SMART_ONE_WAY_FROM_SERVER    )
         || (code == INCREMENTAL_SMART_ONE_WAY_FROM_CLIENT)
         || (code == INCREMENTAL_SMART_ONE_WAY_FROM_SERVER)
         );
}


/**
* Determines if the given code represents a client only action, such as is
* one of:
* <ul>
*   <li>ONE_WAY_FROM_CLIENT</li>
*   <li>REFRESH_FROM_CLIENT</li>
*   <li>SMART_ONE_WAY_FROM_CLIENT</li>
*   <li>INCREMENTAL_SMART_ONE_WAY_FROM_CLIENT</li>
* </ul>
*
* @param code the code to be checked
*
* @return true if the code represents a client only action, false otherwise
*/
bool isClientOnlyCode(int code) {
  return (  (code == ONE_WAY_FROM_CLIENT)
         || (code == REFRESH_FROM_CLIENT)
         || (code == SMART_ONE_WAY_FROM_CLIENT)
         || (code == INCREMENTAL_SMART_ONE_WAY_FROM_CLIENT)
         );
}

/** @endcond */
#endif // INCL_ALERT_CODE
