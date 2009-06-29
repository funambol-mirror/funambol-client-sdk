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
#ifndef INCL_SPDM_ERRORS
#define INCL_SPDM_ERRORS
/** @cond DEV */

#define ERR_SPDM_BASE                   10000
#define ERR_INVALID_CONTEXT             ERR_SPDM_BASE
#define ERR_SOURCE_DEFINITION_NOT_FOUND ERR_SPDM_BASE + 1
#define ERR_DM_TREE_NOT_AVAILABLE       ERR_SPDM_BASE + 2
#define ERR_NO_SOURCE_TO_SYNC           ERR_SPDM_BASE + 3
#define ERR_ITEM_ERROR                  ERR_SPDM_BASE + 4

#define ERRMSG_INVALID_CONTEXT       "Invalid context: %s"
#define ERRMSG_DM_TREE_NOT_AVAILABLE "Unable to access the DM Tree"
#define ERRMSG_SOURCE_DEFINITION_NOT_FOUND "Configuration not found for source %s"
#define ERRMSG_NO_SOURCE_TO_SYNC "No sources to synchronize"
#define ERRMSG_ITEM_ERROR           "An error occurred on one or more items."

/** @endcond */
#endif
