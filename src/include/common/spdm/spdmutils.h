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

 #ifndef INCL_DEVICE_MANAGER_UTIL_SPDM
    #define INCL_DEVICE_MANAGER_UTIL_SPDM
/** @cond DEV */

    #include "base/fscapi.h"

    /*
     * Extracts the node name from the node path
     *
     * @param node - the node path
     * @param name - the buffer that will contain the node name
     * @param size - buffer size
     */
    void getNodeName(const char*  node, const char* name, int size);

    /*
     * Extracts the node context from the node path
     *
     * @param node - the node path
     * @param context - the buffer that will contain the node context
     * @param size - buffer size
     */
    void getNodeConT(const char*  node, const char* context, int size);

/** @endcond */
#endif
