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
package com.funambol.syncml.protocol;

/**
 * This class represents the &lt;Item&gt; tag as defined by the SyncML
 * representation specifications
 */
public class Item implements ReusableObject {

    // ------------------------------------------------------------ Private data
    private Target       target      ;
    private Source       source      ;
    private SourceParent sourceParent;
    private TargetParent targetParent;
    private Meta         meta        ;
    private Data         data        ;
    private Boolean      moreData    ;

    private boolean      incompleteInfo = false;;

    // ------------------------------------------------------------ Constructors

    Item() {}

    // ---------------------------------------------------------- Public methods

    public static Item newInstance() {
        return ObjectsPool.createItem();
    }

    public void init() {
        target       = null;
        source       = null;
        sourceParent = null;
        targetParent = null;
        meta         = null;
        data         = null;
        moreData     = null;
        incompleteInfo = false;;
    }

    /**
     * Returns the item target
     *
     * @return the item target
     */
    public Target getTarget() {
        return target;
    }

    /**
     * Sets the item target
     *
     * @param target the target
     *
     */
    public void setTarget(Target target) {
        this.target = target;
    }

    /**
     * Returns the item source
     *
     * @return the item source
     */
    public Source getSource() {
        return source;
    }

    /**
     * Sets the item source
     *
     * @param source the source
     *
     */
    public void setSource(Source source) {
        this.source = source;
    }

    /**
     * Returns the item source parent
     *
     * @return the item source parent
     */
    public SourceParent getSourceParent() {
        return sourceParent;
    }

    /**
     * Sets the parent information of the item
     *
     * @param sourceParent the parent information of the item
     *
     */
    public void setSourceParent(SourceParent sourceParent) {
        this.sourceParent = sourceParent;
    }

    /**
     * Returns the item target parent
     *
     * @return the item target parent
     */
    public TargetParent getTargetParent() {
        return targetParent;
    }

    /**
     * Sets the parent information of the item
     *
     * @param targetParent the parent information of the item
     *
     */
    public void setTargetParent(TargetParent targetParent) {
        this.targetParent = targetParent;
    }

    /**
     * Returns the item meta element
     *
     * @return the item meta element
     */
    public Meta getMeta() {
        return meta;
    }

    /**
     * Sets the meta item
     *
     * @param meta the item meta element
     *
     */
    public void setMeta(Meta meta) {
        this.meta = meta;
    }

    /**
     * Returns the item data
     *
     * @return the item data
     *
     */
    public Data getData() {
        return data;
    }
    
    /**
     * Returns the hidden data. This is used in the bindingHiddenData.xml in order
     * to avoid to show sensitive data.
     *
     * @return <i>*****</i>
     */
    public Data getHiddenData() {
        return new Data("*****");
    }

    /**
     * Sets the item data
     *
     * @param data the item data
     *
     */
    public void setData(Data data) {
        this.data = data;
    }

    /**
     * Gets moreData property
     *
     * @return true if the data item is incomplete and has further chunks
     *         to come, false otherwise
     */
    public boolean isMoreData() {
        return (moreData != null);
    }

    /**
     * Gets the Boolean value of moreData
     *
     * @return true if the data item is incomplete and has further chunks
     *         to come, false otherwise
     */
    public Boolean getMoreData() {
        if (moreData == null || !moreData.booleanValue()) {
            return null;
        }
        return moreData;
    }

    /**
     * Sets the moreData property
     *
     * @param moreData the moreData property
     */
    public void setMoreData(Boolean moreData) {
        this.moreData = (moreData.booleanValue()) ? moreData : null;
    }

    /**
     * Returns incompleteInfo property
     * @return the property incompleteInfo
     */
    public boolean isWithIncompleteInfo() {
        return this.incompleteInfo;
    }

    /**
     * Sets the propert incompleteInfo
     * @param incompleteInfo boolean
     */
    public void setIncompleteInfo(boolean incompleteInfo) {
        this.incompleteInfo = incompleteInfo;
    }
}
