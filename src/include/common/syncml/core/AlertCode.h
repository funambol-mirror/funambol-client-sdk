/*
 * Copyright (C) 2003-2007 Funambol, Inc
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

#ifndef INCL_ALERT_CODE
#define INCL_ALERT_CODE
/** @cond DEV */

#include "base/fscapi.h"


#define DISPLAY                        100
#define TWO_WAY                        200
#define SLOW                           201
#define ONE_WAY_FROM_CLIENT            202
#define REFRESH_FROM_CLIENT            203
#define ONE_WAY_FROM_SERVER            204
#define REFRESH_FROM_SERVER            205
#define TWO_WAY_BY_SERVER              206
#define ONE_WAY_FROM_CLIENT_BY_SERVER  207
#define REFRESH_FROM_CLIENT_BY_SERVER  208
#define ONE_WAY_FROM_SERVER_BY_SERVER  209
#define REFRESH_FROM_SERVER_BY_SERVER  210
#define RESULT_ALERT                   221
#define NEXT_MESSAGE                   222
#define NO_END_OF_DATA                 223



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
* </ul>
*
* @param code the code to be checked
*
* @return true if the code is an initialization code, false otherwise
*/
BOOL isInitializationCode(int code) {
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
         );
}


/**
* Determines if the given code represents a client only action, such as is
* one of:
* <ul>
*   <li>ONE_WAY_FROM_CLIENT</li>
*   <li>REFRESH_FROM_CLIENT</li>
* </ul>
*
* @param code the code to be checked
*
* @return true if the code represents a client only action, false otherwise
*/
BOOL isClientOnlyCode(int code) {
  return (  (code == ONE_WAY_FROM_CLIENT)
         || (code == REFRESH_FROM_CLIENT)
         );
}

/** @endcond */
#endif // INCL_ALERT_CODE
