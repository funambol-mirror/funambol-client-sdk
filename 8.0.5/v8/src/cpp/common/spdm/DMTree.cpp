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



#include "base/fscapi.h"
#include "base/util/utils.h"
#include "base/Log.h"

#include "spdm/ManagementNode.h"
#include "spdm/DeviceManagementNode.h"
#include "spdm/DMTree.h"
#include "base/globalsdef.h"

USE_NAMESPACE


/*
 * Basic implementation of DMTree, can be re-defined if platform specific
 * variant is needed.
 */

/*
 * Constructor
 */
DMTree::DMTree(const char *root) : root(root) {
}

/*
 * Destructor
 */
DMTree::~DMTree() {
}

bool DMTree::isLeaf(const char *node) {
    DeviceManagementNode dmn(node);

    return (dmn.getChildrenMaxCount() == 0);
}

ManagementNode* DMTree::getNode(const char* node) {

    StringBuffer completeNodeName;
    if (root.endsWith('/')) {
        completeNodeName.sprintf("%s%s", root.c_str(), node);
    } else {
        completeNodeName.sprintf("%s/%s", root.c_str(), node);
    }

    ManagementNode *n = new DeviceManagementNode(completeNodeName.c_str());
    return n;
}

/*
 * Returns the management node identified by the given node pathname
 * (relative to the root management node). If the node is not found
 * NULL is returned; additional info on the error condition can be
 * retrieved calling getLastError() and getLastErrorMessage()
 *
 * The ManagementNode is created with the new operator and must be
 * discarded by the caller with the operator delete.
 */
ManagementNode* DMTree::readManagementNode(const char* node) {


    ManagementNode *n = new DeviceManagementNode(node);

    int childrenCount = n->getChildrenMaxCount();

    if (childrenCount) {
        char** childrenNames = n->getChildrenNames();

        if (!childrenNames){
            LOG.error("Error in getChildrenNames");
            return NULL;
        }
        int i = 0;
        for (i = 0; i < childrenCount; i++) {
            DeviceManagementNode s(node, childrenNames[i]);
            n->addChild(s);
        }
        for (i = 0; i < childrenCount; i++) {
            delete [] childrenNames[i]; childrenNames[i] = NULL;
        }
        delete [] childrenNames;
    }

    return n;
}


