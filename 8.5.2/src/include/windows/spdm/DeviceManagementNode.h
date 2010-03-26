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
#ifndef INCL_WIN_DEVICE_MANAGEMENT_NODE
#define INCL_WIN_DEVICE_MANAGEMENT_NODE
/** @cond DEV */



#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "spdm/ManagementNode.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/*
 * Windows implementation of ManagementNode.
 */
class DeviceManagementNode: public ManagementNode {

    private:
            TCHAR *fullContext;

            /**
             * If true the registry will be flushed after setting EACH property.
             * Its default value is true, since on WinCE it is important to call 
             * RegFlushKey to save important registry information.
             *
             * Clients may set it to false when setting a large amount of data and
             * then call flushRegistry() just once: frequent calling of 
             * RegFlushKey can degrade performance
             *
             * @note: only for WinCE. On Win32 the registry flush is not necessary
             * because registry changes are flushed to disk by the registry using its lazy flusher
             */
            static bool autoFlushRegistry;

    protected:

		const TCHAR* getFullContext() const { return fullContext; }
    
        void setFullContext();

    public:

        // ------------------------------------------ Constructors & destructors

        /**
         * Constructor.
         *
         * @param parent - a ManagementNode is usually under the context of a
         *                 parent node.
         * @param name - the node name
         *
         */
        DeviceManagementNode(const char*  parent, const char*  name);
        DeviceManagementNode(const char*  fullName);
        ~DeviceManagementNode();

        // --------------------------------------------------- Public methods
        /*
         * Returns the value of the given property
         *
         * @param property - the property name
         */
        char* readPropertyValue(const char*  property);

        /*
         * Sets a property value.
         *
         * @param property - the property name
         * @param value - the property value (zero terminated string)
         */
        void setPropertyValue(const char*  property, const char*  value);

        /*
         * Returns the children's name of the parent node.
         */
        char **getChildrenNames();

        /*
         * Find how many children are defined for this node in the underlying
		 * config system.
         */
        int getChildrenMaxCount();

        /*
         * Creates a new ManagementNode with the exact content of this object.
         * The new instance MUST be created with the C++ new opertator.
         */
        ArrayElement* clone();

        /**
         * Deletes a ManagementNode, given the node name.
         *
         * @param nodeName  the name of the subnode to delete
         * @return          0 if no errors, otherwise the error code
         */
        int deletePropertyNode(const char* nodeName);


        static void setAutoFlushRegistry(const bool v);
        static const bool getAutoFlushRegistry();

        /**
         * Flushes the entire registry to the platform-supported storage.
         * By default, Windows CE does not support a lazy flush of the registry; 
         * therefore, it is important to call RegFlushKey to save important registry information. 
         * @note  call flushRegistry after a group of changes have been made, but not for every 
         * individual registry change. Frequent calling of RegFlushKey can degrade performance.
         * @note  under Win32 it is not necessary to call this method. 
         */
        static void flushRegistry();
};


END_NAMESPACE

/** @endcond */
#endif
