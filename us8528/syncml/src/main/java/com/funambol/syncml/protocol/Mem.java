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
 * This class represents the &lt;Mem&gt; tag as defined by the SyncML r
 * epresentation specifications.
 */
public class Mem {

    // ------------------------------------------------------------ Private data

    private Boolean sharedMem;
    private long    freeMem  ;
    private long    freeID   ;

    // ------------------------------------------------------------ Constructors

    /**
     * For serialization purposes
     */
    protected Mem(){}

    /**
     * Creates a new Mem object from memory characteristics.
     *
     * @param sharedMem is the datastore memory shared
     * @param freeMem free memory size in bytes (>= 0)
     * @param freeID number of available item IDs (>= 0)
     *
     */
    public Mem(final boolean sharedMem, final long freeMem, final long freeID) {
        setFreeMem(freeMem);
        setFreeID(freeID);
        this.sharedMem = (sharedMem) ? new Boolean(sharedMem) : null;
    }

    // ---------------------------------------------------------- Public methods

    /**
     * Returns the memoryShared status
     *
     * @return <i>true</i> if the datastore memory is shared, <i>false</i> otherwise
     *
     */
    public boolean isSharedMem() {
        return (sharedMem != null);
    }

    /**
     * Sets the memoryShared status
     *
     * @param sharedMem the new memoryShared status
     */
    public void setSharedMem(Boolean sharedMem) {
        this.sharedMem = (sharedMem.booleanValue()) ? sharedMem : null;
    }

    /**
     * Gets the Boolean shared memory property
     *
     * @return sharedMem the Boolean shared memory property
     */
    public Boolean getSharedMem() {
        if (sharedMem == null || !sharedMem.booleanValue()) {
            return null;
        }
        return sharedMem;
    }

    /**
     * Returns the freeMem property (in bytes)
     *
     * @return the freeMem property
     *
     */
    public long getFreeMem() {
        return freeMem;
    }

    /**
     * Sets the freeMem property.
     *
     * @param freeMem the freeMem value (>= 0)
     *
     * @throws IllegalArgumentException if freeMEmory is < 0
     */
    public void setFreeMem(long freeMem) {
        if (freeMem < 0) {
            throw new IllegalArgumentException("freeMem cannot be < 0)");
        }
        this.freeMem = freeMem;
    }

    /**
     * Returns the number of available item IDs (>= 0)
     *
     * @return the number of available item IDs (>= 0)
     *
     */
    public long getFreeID() {
        return freeID;
    }

    /**
     * Sets the freeID property.
     *
     * @param freeID the freeIDCount value (>= 0)
     *
     * @throws IllegalArgumentException if freeIDCount is < 0
     */
    public void setFreeID(long freeID) {
        if (freeID < 0) {
            throw new IllegalArgumentException("freeID cannot be < 0)");
        }
        this.freeID = freeID;
    }
}
