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
#ifndef INCL_DM_TREE_MANAGER
#define INCL_DM_TREE_MANAGER
/** @cond DEV */

#include "base/constants.h"
#include "spdm/ManagementNode.h"

/**
 * DMTreeManager is an abstract class for which implementors have to provide a
 * concrete implementation. It provides the ability to retrieve and store
 * objects into the DM platform specific repository. Note that the object
 * returned by readManagementNode() is created with the standard C++ new operator
 * and must be deleted by the caller with the standard C++ delete operator.
 */
class DMTreeManager {
    public:
        /*
         * Returns the management node identified by the given node pathname
         * (relative to the root management node). If the node is not found
         * NULL is returned.
         *
         * The ManagementNode is created with the new operator and must be
         * discarded by the caller with the operator delete. Depending on
         * which node is given, the result is either an instance
         * of SourceManagementNode or AccessManagementNode.
         */
        virtual ManagementNode* const readManagementNode(const char*  node)=0;

        /*
         * Stores the content of the node permanently in the DMTree
         */
        virtual void setManagementNode(ManagementNode& n)=0;
};

/** @endcond */
#endif
