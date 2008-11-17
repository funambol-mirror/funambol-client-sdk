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

#ifndef INCL_DMTREE
    #define INCL_DMTREE
/** @cond DEV */

    #include "spdm/constants.h"
    #include "spdm/ManagementNode.h"

    class DMTree {

    private:

        char *root;

    protected:

        virtual bool isLeaf(const char *node);

    public:
        DMTree(const char *root);

        virtual ~DMTree();

        /*
         * Returns the management node identified by the given node pathname
         * (relative to the root management node). If the node is not found
         * NULL is returned; additional info on the error condition can be
         * retrieved calling getLastError() and getLastErrorMessage()
         *
         * The ManagementNode is created with the new operator and must be
         * discarded by the caller with the operator delete.
         */
        virtual ManagementNode* readManagementNode(const char*  node);

        void setManagementNode(ManagementNode& n);

    };

/** @endcond */
#endif

