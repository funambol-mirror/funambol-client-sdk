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
#ifndef INCL_HTTP_ERRORS
#define INCL_HTTP_ERRORS
/** @cond DEV */

#define ERR_TRANSPORT_BASE              2000
#define ERR_NETWORK_INIT                ERR_TRANSPORT_BASE
#define ERR_CONNECT                     ERR_TRANSPORT_BASE+ 1
#define ERR_HOST_NOT_FOUND              ERR_TRANSPORT_BASE+ 2
#define ERR_READING_CONTENT             ERR_TRANSPORT_BASE+ 3
#define ERR_WRITING_CONTENT             ERR_TRANSPORT_BASE+ 4
#define ERR_HTTP                        ERR_TRANSPORT_BASE+50
#define ERR_HTTP_MISSING_CONTENT_LENGTH ERR_TRANSPORT_BASE+51
#define ERR_SERVER_ERROR                ERR_TRANSPORT_BASE+52

#define ERR_HTTP_STATUS_NOT_OK          ERR_TRANSPORT_BASE+53
#define ERR_CREDENTIAL                  401
#define ERR_CLIENT_NOT_NOTIFIABLE       420

#define ERR_TRANSPORT_LAST              2999

#define ERRMSG_NETWORK_INIT "Network initialization error"
#define ERRMSG_CONNECT "Connection failure"
#define ERRMSG_HOST_NOT_FOUND "Host not found"
#define ERRMSG_READING_CONTENT "Error reading content"
#define ERRMSG_WRITING_CONTENT "Error writing content"
#define ERRMSG_HTTP_MISSING_CONTENT_LENGTH "Missing Content-Length header"
#define ERRMSG_SERVER_ERROR ("Server error")
/** @endcond */
#endif
