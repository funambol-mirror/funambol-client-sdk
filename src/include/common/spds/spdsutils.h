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

#ifndef INCL_SPDS_UTIL
#define INCL_SPDS_UTIL
/** @cond DEV */

#include "base/util/ArrayList.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncItemStatus.h"

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

int uudecode(const char *msg, char **binmsg, size_t *binlen);
char *uuencode(const char *msg, int len);

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

/** @endcond */
#endif
