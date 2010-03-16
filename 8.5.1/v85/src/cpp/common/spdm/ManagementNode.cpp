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
#include "base/Log.h"
#include "base/util/utils.h"
#include "spdm/ManagementNode.h"
#include "base/globalsdef.h"

USE_NAMESPACE


/*
 * Constructor.
 *
 * @param parent - a ManagementNode is usually under the context of a
 *                 parent node.
 * @param name - the node name
 *
 */
ManagementNode::ManagementNode(const char* parent, const char* name) {
    context = stringdup(parent);
    this->name = stringdup(name);
}

ManagementNode::ManagementNode(const char* fullname) {
    if(setFullName(fullname)){
        char msg[512];

        sprintf(msg, "Invalid context: %s", fullname);
        //TODO call ErrorHandler XXX
        LOG.error("%s", msg);
    }
}

ManagementNode::~ManagementNode() {
    if (context)
        delete [] context;
    if (name)
        delete [] name;
}

/*
 * Returns the full node name (in a new-allocated buffer)
 */
char* ManagementNode::createFullName(){
    char*ret = new char[strlen(context)+strlen(name)+2];

    sprintf(ret, "%s/%s", context, name);
    return ret;
}

int ManagementNode::setFullName(const char *fullname) {
    char* p;
	int len;

    p = strrchr((char*)fullname, '/');

    if ( !p )
        return -1;

	len = p-fullname;
    context = stringdup(fullname, len);
	p++; len=strlen(fullname)-len;
	name = stringdup(p, len);

	return 0;
}

const char *ManagementNode::getName() {
    return name;
}

/*
 * Returns how many children belong to this node.
 *
 */
int ManagementNode::getChildrenCount() {
	return children.size();
}

/*
 * Get a child from the list
 */
ManagementNode * ManagementNode::getChild(int index) {
    return (ManagementNode *)children[index];
}

ManagementNode * ManagementNode::getChild(const char* name) {
    for (int index = 0; index < children.size(); index++) {
        if (!strcmp(((ManagementNode *)children[index])->getName(), name)) {
            return (ManagementNode *)children[index];
        }
    }
    return NULL;
}

/*
 * Adds a new child. A clone of the given node is created internally, so that
 * the caller is free to release node as soon as it wants.
 *
 * @parent node the new node
 */
void ManagementNode::addChild(ManagementNode& node) {
	children.add(node);
}


