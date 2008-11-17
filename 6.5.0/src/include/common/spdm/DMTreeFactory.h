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
#ifndef INCL_DEVICE_MANAGER
#define INCL_DEVICE_MANAGER
/** @cond DEV */

#include "spdm/DMTree.h"

/**
 * DeviceManager is an abstract class that must be implemented on a platform
 * specific basis. The current interface allows a client to retrieve a platform
 * specific DMTreeManager instance. Since this class has access to the
 * management properties repository, it is bound to the underlying system. In
 * this use case, the DeviceManager can be seen as a factory for DMTreeManager
 * objects. Note that the DMTreeManager instance returned is created with the
 * standard C++ new operator and must be deleted by the caller with the
 * standard C++ delete operator
*/
class DMTreeFactory {
    public:

        /*
         * Constructor
         */
        DMTreeFactory();

        /*
         * Creates and returns the DMTreeManager that represents the management
         * tree under the root specified. In case of error, NULL is returned.
         *
         * The DMTreeManager is created with the 'new' C++ operator and must be
         * discarded by the caller with the operator 'delete'.
         */
        static DMTree* getDMTree(const char*  node);
};

/** @endcond */
#endif
