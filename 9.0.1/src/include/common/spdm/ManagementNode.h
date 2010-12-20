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

#ifndef INCL_MANAGEMENT_NODE
#define INCL_MANAGEMENT_NODE
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "base/util/ArrayList.h"
#include "spdm/constants.h"
#include "base/util/StringBuffer.h"
#include "base/util/StringMap.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/*
 * This class represents a management node, so that a configuration
 * object under the device manager control.
 * This is an abstract class that defines the interface of platform
 * specific concrete implementations.
 *
 * See the design documents for more information.
 */
class ManagementNode : public ArrayElement {

    protected:
        char *name;
        char *context;

        //
        // Children are dinamically allocated inside this class and given to
        // the list. The list will delete all created objects at descruction
        // time.
        //
        ArrayList children;

        /*
         * Set node attributes (name, context, fullcontext) from a Full Name string
         *
         */
        int setFullName(const char *name);

    public:

        // -------------------------------------------- Constructors & Destructors

        /*
         * Constructor.
         *
         * @param parent - a ManagementNode is usually under the context of a
         *                 parent node.
         * @param name - the node name
         *
         */
        ManagementNode(const char*  parent, const char*  name);

        /*
         * Constructor.
         *
         * @param fullcontext - the complete path to the node. The last
         *                      component is used as name, the rest as context
         *                      NOTE: the fullcontext MUST contain at least one "/" char
         *                            to correctly split context and name.
         *
         */
        ManagementNode(const char*  fullcontext);

        /* Base class destructor */
        virtual ~ManagementNode();

        // ----------------------------------------------------- Virtual methods

        /*
         * Returns this node's child, at index specified
         *
         * @param index - the index of the child to get
         *
         * @return the node or NULL on failure.
         *         Caller MUST NOT delete the object
         */
        virtual ManagementNode * getChild(int index);

        /**
         * Returns the node's child with the given name, NULL if not found.
         */
        virtual ManagementNode * getChild(const char* name);

        /**
         * Add a new child to this node.
         *
         * @param child - the ManagementNode to add
         */
        virtual void addChild(ManagementNode &child);

        /*
         * Returns how many children belong to this node (how many have been added)
         */
        virtual int getChildrenCount();

        /*
         * Returns the full node name in a newly allocated buffer,
         * caller must free it with delete [].
         *
         */
        virtual char* createFullName();

        /**
         * Returns the node name itself without the context.
         */
        virtual const char *getName();

        // ---------------------------------------------------- Abstract methods

        /*
         * Find how many children are defined for this node in the underlying
         * config system.
         */
        virtual int getChildrenMaxCount() = 0;

        /* Returns the names of the children nodes, in a new-allocated
         * string array
         *
         * @return NULL on failure
         */
        virtual char **getChildrenNames() = 0;

        /*
         * Returns the value of the given property
         *
         * @param property - the property name
         *
         * @return - the property value. MUST be deleted by the caller with delete [];
         *           never NULL, for non-existant properties an empty string is returned
         */
        virtual char* readPropertyValue(const char*  property) = 0;

        /*
         * Sets a property value.
         *
         * @param property - the property name
         * @param value - the property value (zero terminated string)
         */
        virtual void setPropertyValue(const char*  property, const char*  value) = 0;

        /**
         * Deletes a ManagementNode, given the node name.
         * Needs a specific implementation for each platform.
         *
         * @param nodeName  the name of the subnode to delete
         * @return          0 if no errors, otherwise the error code
         */
        virtual int deletePropertyNode(const char* nodeName) = 0;
        
        /**
         * Reads all the properties of the node and return a StringMap filled
         * by keys and values. The default implementation of the ManagementNode
         * return a new StringMap object filled with a dummy key (PROPERTY_DUMMY_KEY).
         * the DMTClientConfig automatically purge it from the count
         *
         * @return a new allocated StringMap with all keys/values 
         */
        virtual StringMap* readAllProperties();
        /*
         * Creates a new ManagementNode with the exact content of this object.
         * The new instance MUST be created with the C++ new opertator.
         */
        virtual ArrayElement* clone() = 0;

};


END_NAMESPACE

/** @endcond */
#endif

