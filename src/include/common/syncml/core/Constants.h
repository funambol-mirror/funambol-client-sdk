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


#ifndef INCL_CORE_CONSTANTS
#define INCL_CORE_CONSTANTS
/** @cond DEV */

#include "base/fscapi.h"

#define DIM_64    64
#define DIM_512  512

#define MIMETYPE_SYNCMLDS_XML               "application/vnd.syncml+xml"
#define MIMETYPE_SYNCMLDS_WBXML             "application/vnd.syncml+wbxml"
#define MIMETYPE_SYNCML_DEVICEINFO_XML      "application/vnd.syncml-devinf+xml"
#define MIMETYPE_SYNCML_DEVICEINFO_WBXML    "application/vnd.syncml-devinf+wbxml"

#define NAMESPACE_METINF    "syncml:metinf"
#define NAMESPACE_DEVINF    "syncml:devinf"
#define FORMAT_B64          "b64"

#define AUTH_TYPE_MD5    "syncml:auth-md5"
#define AUTH_TYPE_BASIC  "syncml:auth-basic"
#define AUTH_NONE        "none"
#define AUTH_SUPPORTED_TYPES  "syncml:auth-md5,syncml:auth-basic"


/** @endcond */
#endif
