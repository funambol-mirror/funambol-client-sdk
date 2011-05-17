/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2005 - 2007 Funambol, Inc.
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
package com.funambol.syncml.protocol;

/**
 * This class represents the &lt;Filter&gt; tag as defined by the SyncML
 * representation specifications
 */
public class Filter {

    // ------------------------------------------------------------ Private data
    private Meta   meta      ;
    private Item   field     ;
    private Item   record    ;
    private String filterType;

    // ------------------------------------------------------------ Constructors

    protected Filter() {}

    /**
     * Creates a new Filter object.
     *
     * @param meta       the meta information - NOT NULL
     * @param field      the field item data
     * @param record     the record item data
     * @param filterType the type of filtering
     */
    public Filter(final Meta   meta      ,
                  final Item   field     ,
                  final Item   record    ,
                  final String filterType) {

        this.meta       = meta      ;
        this.field      = field     ;
        this.record     = record    ;
        this.filterType = filterType;
    }

    // ---------------------------------------------------------- Public methods
    /**
     * Returns the meta information
     *
     * @return the meta information
     */
    public Meta getMeta() {
        return meta;
    }

    /**
     * Sets the meta information
     *
     * @param meta the meta information
     */
    public void setMeta(Meta meta) {
        if (meta == null) {
            throw new IllegalArgumentException("meta cannot be null");
        }
        this.meta = meta;
    }

    /**
     * Returns the item field data
     *
     * @return the item field data
     */
    public Item getField() {
        return field;
    }

    /**
     * Sets a field level filter to be performed on the parent element of
     * the filter
     *
     * @param field the item field data
     */
    public void setField(Item field) {
        this.field = field;
    }

    /**
     * Returns the item record data
     *
     * @return the item record data
     */
    public Item getRecord() {
        return record;
    }

    /**
     * Sets a record level filter to be performed on the parent element of
     * the filter
     *
     * @param record the item record data
     */
    public void setRecord(Item record) {
        this.record = record;
    }

    /**
     * Returns the type of filtering
     *
     * @return the type of filtering
     */
    public String getFilterType() {
        return filterType;
    }

    /**
     * Sets the type of filtering
     *
     * @param filterType the type of filtering
     */
    public void setFilterType(String filterType) {
        this.filterType = filterType;
    }

}
