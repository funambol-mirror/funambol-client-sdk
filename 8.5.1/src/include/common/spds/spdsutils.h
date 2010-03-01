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

#ifndef INCL_SPDS_UTIL
#define INCL_SPDS_UTIL
/** @cond DEV */

#include "base/util/ArrayList.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncItemStatus.h"
#include "base/util/StringBuffer.h"

BEGIN_NAMESPACE

/**
 * returns the SyncMode corresponding to the string,
 * SYNC_NONE if string is invalid
 *
 * @param syncMode      one of the keywords valid as config parameter (see documentation of config)
 */
SyncMode syncModeCode(const char*  syncMode);

/**
 * returns the config keyword corresponding to the sync mode,
 * empty string if invalid
 */
const char *syncModeKeyword(SyncMode syncMode);

/// Returns the sync mode given the type, of syncCapsMapping struct.
const SyncMode getSyncCapsMode(const int type);

/// Returns the sync type given the mode, of syncCapsMapping struct.
const int getSyncCapsType(const SyncMode mode);

/**
 * Converts a comma separated string of syncModes into a list of 
 * supported syncmodes, as per the OMA DefInfo specifications.
 * "two-way,slow,one-way-from-client" -> List: {1 2 4}
 * 
 * @param syncModesString  the input string (comma separated values)
 * @return                 a new allocated ArrayList of sync types
 */
ArrayList* syncModesStringToList(const StringBuffer& syncModesString);

/**
 * Converts a list of supported syncmodes, as per the OMA DefInfo specifications,
 * into a comma separated string of syncModes. 
 * List: {1 2 4} -> "two-way,slow,one-way-from-client"
 * 
 * @param syncModesList the input ArrayList of sync types
 * @return              the formatted string (comma separated values)
 */
StringBuffer syncModesListToString(const ArrayList& syncModesList);

/*
 * Translates an ArrayList object into an array of SyncItemStatus*.
 *
 * @param items the item list
 */
SyncItemStatus** toSyncItemStatusArray(ArrayList& items);

/*
 * Translates an ArrayList into an array of SyncItem*.
 *
 * @param items the item list
 */
SyncItem** toSyncItemArray(ArrayList& items);


/**
 * This functions works for standard encoded files with new line every
 * 72 characters. It does not work if the line length is not multiple of 4.
 *
 * @param const char* msg the message to decode
 * @param char** binmsg new allocated decoded message
 * @param size_t* binlen the lenght of the decoded message
 * @return int 0 if successful
 */
int uudecode(const char *msg, char **binmsg, size_t *binlen);

/**
 * Encode the message in base64, splitting the result in lines of 72 columns
 * each.
 * 
 * @param const char* msg the message to encode (can be binary data)
 * @param int len the lenght of the message
 * @return char* new allocated string or NULL in case of error
 */
char *uuencode(const char *msg, int len);

/**
 * Encode the message in base64, splitting the result in lines of 72 columns
 * each.
 *
 * @param msg the const char* to encode
 * @param len the lenght of the const char* to encode
 * @return  a new allocated char* buffer
 */
char* b64EncodeWithSpaces(const char *msg, int len);

/**
* It converts the content using the encoding specified. If all
* is correct it saves the converted content into a file.
* @param filename the file name in which the converted data is saved
* @param s the data that will be converted
* @param encoding the encoding used to convert to data
* @return -1 if the filename is NULL or s is NULL;
*          0 if all is correct
*/
int convertAndSave(const char *filename, const char *str,
                  const char *encoding = "UTF-8");

/**
* Load a file and convert its content according to encoding.
* @param filename the file name in which the converted content is saved
* @param encoding the encoding that will be used
* @return the content of the file
*/
char *loadAndConvert(const char *filename,
                        const char *encoding = "UTF-8");

/**
 * indent a string encoded in bb4
 *
 * @param StringBuffer& the string to indent
 * @param int space how much to indent
 * @return 0 if successful
 */
int indent(StringBuffer& content, int space = 8);

END_NAMESPACE

/** @endcond */
#endif
