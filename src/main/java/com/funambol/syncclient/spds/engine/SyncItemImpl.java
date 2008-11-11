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

package com.funambol.syncclient.spds.engine;

import java.util.Hashtable;
import java.util.Enumeration;

import com.funambol.syncclient.spds.engine.*;

/**
 * <i>SyncItemImpl</i> is a basic implemenation of a <i>SyncItem</i>.
 *
 *
 *
 * @version $Id: SyncItemImpl.java,v 1.3 2007-12-22 18:09:18 nichele Exp $
 */
public class SyncItemImpl implements SyncItem {

    // -------------------------------------------------------------- Properties

    /**
     * The SyncItem's uique identifier - read only
     */
    protected SyncItemKey key = null;
    public SyncItemKey getKey() {
        return this.key;
    }

    /**
     * The state of this <i>SyncItem</i>
     *
     */
    protected char state = SyncItemState.UNKNOWN;

    public char getState(){
        return state;
    }

    public void setState(char state){
        this.state = state;
    }

    /**
     * The <i>SyncItem</i>'s properties - read and write
     */
    protected Hashtable properties = new Hashtable();

    /**
     * Returns the <i>properties</i> property. A cloned copy of the internal map
     * is returned.
     *
     * @return the <i>properties</i> property.
     */
    public Hashtable getProperties() {
        return this.properties;
    }

    /**
     * Sets the <i>properties</i> property. All items in the given map are added
     * to the internal map.
     *
     * @param properties the new values
     */
    public void setProperties(Hashtable properties){
        this.properties.clear();

        String name = null;

        Enumeration i = properties.keys();
        while(i.hasMoreElements()) {
            name = (String)i.nextElement();
            this.properties.put(
                name,
                new SyncItemProperty(name, properties.get(name))
            );
        }
    }

    /** Sets/adds the given property to this <i>SyncItem</i>
     *
     * @param property The property to set/add
     */
    public void setProperty(SyncItemProperty property) {
        properties.put(property.getName(), property);
    }

    /** Returns the property with the given name
     *
     * @param propertyName The property name
     *
     * @return the property with the given name if exists or null if not
     */
    public SyncItemProperty getProperty(String propertyName) {
        return (SyncItemProperty)properties.get(propertyName);
    }

        /**
     * The SyncSource this item belongs to
     */
    protected SyncSource syncSource = null;

    /** Getter for property syncSource.
     * @return Value of property syncSource.
     *
     */
    public SyncSource getSyncSource() {
        return syncSource;
    }

    /** Setter for property syncSource.
     * @param syncSource New value of property syncSource. NOT NULL
     *
     */
    public void setSyncSource(SyncSource syncSource) {
        if (syncSource == null) {
            throw new NullPointerException("syncSource cannot be null");
        }

        this.syncSource = syncSource;
    }

    // ------------------------------------------------------------ Constructors

    public SyncItemImpl(SyncSource syncSource, Object key) {
        this(syncSource, key, SyncItemState.UNKNOWN);
    }

    /**
     * Creates a new <i>SyncItem</i> belonging to the given source. After
     * creating a new item, usually <i>setProperties()</i> should be invoked.
     *
     * @param syncSource the source this item belongs to
     * @param key the item identifier
     * @param state one of the state value defined in <i>SyncItemState</i>
     */
    public SyncItemImpl(SyncSource syncSource, Object key, char state) {
        this.syncSource = syncSource          ;
        this.key        = new SyncItemKey(key);
        this.state      = state               ;
    }

    // ---------------------------------------------------------- Public methods


    /** Sets the value of the property with the given name.
     *
     * @param propertyName The property's name
     * @param propertyValue The new value
     */
    public void setPropertyValue(String propertyName, String propertyValue) {
        SyncItemProperty property = (SyncItemProperty)properties.get(propertyName);

        if (property != null) {
            property.setValue(propertyValue);
        }
    }

    /** Returns the value of the property with the given name.
     *
     * @param propertyName The property's name
     *
     * @return the property value if this <i>SyncItem</i> has the given
     *         property or null otherwise.
     */
    public Object getPropertyValue(String propertyName) {
        SyncItemProperty property = (SyncItemProperty)properties.get(propertyName);

        return (property == null) ? null
                                  : property.getValue()
                                  ;
    }

    /**
     * Two <i>SyncItem</i>s are equal if their keys are equal.
     *
     * @param o the object this instance must be compared to.
     *
     * @return the given object is equal to this object
     *
     */
    public boolean equals(Object o) {
        if (!(o instanceof SyncItem)) return false;

        return ((SyncItem)o).getKey().equals(key);
    }

    /**
     * Creates and returns a "not-existing" <i>SyncItem</i>. It is used internally to
     * represent an item which has not a physical correspondance in a source.
     *
     * @param syncSource the <i>SyncSource</i> the not existing item belongs to
     * @return the a "not-exisiting" <i>SyncItem</i>
     */
    public static SyncItem getNotExistingSyncItem(SyncSource syncSource) {
        SyncItem notExisting = new SyncItemImpl(syncSource, "NOT_EXISTING");

        notExisting.setState(SyncItemState.NOT_EXISTING);

        return notExisting;
    }

    /**
     * Constructs a string of the SyncItem
     * @return a string representation of this SyncItem for debugging purposes
     */
    public String toString() {
        StringBuffer buf = new StringBuffer(256);
        buf.append("{ syncSource: '")
           .append(syncSource)
           .append("' key: '")
           .append(key)
           .append("' state: '")
           .append(state)
           .append("'}");
        return buf.toString();
    }
}
