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

package com.funambol.syncclient.spdm;

import java.util.Hashtable;

/**
 * This interface models a device management node. A management not can be either
 * a configuration context (a tree element that contains other nodes) or a
 * configuration node (which instead contains configuration properties).
 *
 *
 *
 * @version $Id: ManagementNode.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public interface ManagementNode {

    /**
     * Returns the node's context
     *
     * @return the node's context
     */
    public String getContext();

    /**
     * Returns the entire node's context path (concatenation of all parent
     * context paths).
     *
     * @return the entire node's context path
     */
    public String getFullContext();

    /**
     * Returns the given config parameter
     *
     * @param name the configration parameter name
     * @return this node value
     *
     * @throws DMException in case of errors.
     */
    public Object getValue(String name)
    throws DMException;


    /**
     * @return hashtable of values
     *
     * @throws DMException in case of errors.
     */
    public Hashtable getValues()
    throws DMException;


    /**
     * Retrieves a value from the given management subnode name.
     *
     * @param node the subnode containing the config value specified by name
     * @param name the name of the configuration value to return
     *
     * @return the node value
     *
     * @throws DMException in case of errors.
     */
    public Object getNodeValue(String node, String name)
    throws DMException;

    /**
     * Retrieves all values in the given management subnode name.
     *
     * @param node the subnode containing the required config values
     *
     * @return the node values
     *
     * @throws DMException in case of errors.
     */
    public Hashtable getNodeValues(String node)
    throws DMException;

   /**
    * Reads the properties of the given node and creates an object with the
    * values read.
    * @param node the subnode containing the required config values
    * @return the object
    * @throws DMException in case of errors.
    */
    public Object getManagementObject(String node)
    throws DMException;

    /**
     * Retrieves the children subnodes of the current node.
     *
     * @return an array of <i>ManagementNode</i> containing this node's children
     *
     * @throws DMException in case of errors.
     */
    public ManagementNode[] getChildren()
    throws DMException;

    /**
     * Retrieves the subnode with the given name
     *
     * @return context the subnode context (name)
     *
     * @throws DMException in case of errors.
     */
    public ManagementNode getChildNode(String context)
    throws DMException;


    /**
     * Retrieves this node's parent.
     *
     * @return the node's parent
     *
     * @throws DMException in case of errors.
     */
    public ManagementNode getParent()
    throws DMException;


    /**
     * Sets the node value as a whole object.
     *
     * @param name the configuration parameter name
     * @param value the value
     *
     * @throws DMException in case of errors.
     */
    void setValue(String name, Object value) throws DMException;


    /**
     * Sets a subnode specific value.
     *
     * @param node subnode to set
     * @param name configuration parameter to set
     * @param value
     *
     * @throws DMException in case of errors.
     */
    void setValue(String node, String name, Object value) throws DMException;


    /**
     * Remove subnode from configuration three.
     *
     * @param node subnode to reomove
     *
     * @throws DMException in case of errors.
     */
    void removeNode(String node) throws DMException;


}
