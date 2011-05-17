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

import java.util.*;

/**
 * Thic class corresponds to the &lt;Sync&gt; command in the SyncML represent DTD
 */
public class Sync extends AbstractCommand {

    // --------------------------------------------------------------- Constants
    public static String COMMAND_NAME = "Sync";

    // ------------------------------------------------------------ Private data
    private Target    target;
    private Source    source;
    private Vector    commands = new Vector();
    private Long      numberOfChanges;

    // ------------------------------------------------------------ Constructors

    public Sync(){}

    /**
     * Creates a new Sync object
     *
     * @param cmdID the command identifier - NOT NULL
     * @param noResp is <b>true</b> if no response is required
     * @param cred the authentication credential
     * @param target the target object
     * @param source the source object
     * @param meta the meta object
     * @param numberOfChanges the number of changes
     * @param commands an array of elements that must be of one of the
     *                 following types: {@link Add}, {@link Atomic},
     *                 {@link Copy}, {@link Delete}, {@link Move},
     *                 {@link Replace}, {@link Sequence} - NOT NULL
     *
     * @throws java.lang.IllegalArgumentException
     */
    public Sync(final String  cmdID,
                final boolean noResp,
                final Cred cred,
                final Target target,
                final Source source,
                final Meta meta,
                final Long numberOfChanges,
                final Vector commands) {
        super(cmdID, noResp, meta);

        setCommands(commands);
        setCred(cred);

        this.noResp  = (noResp) ? new Boolean(noResp) : null;
        this.target = target;
        this.source = source;
        this.numberOfChanges  = numberOfChanges;
    }

    // ---------------------------------------------------------- Public methods

    /**
     * Gets the Target object property
     *
     * @return target the Target object property
     */
    public Target getTarget() {
        return target;
    }

    /**
     * Sets the Target object property
     *
     * @param target the Target object property
     *
     */
    public void setTarget(Target target) {
        this.target = target;
    }

    /**
     * Gets the Source object property
     *
     * @return source the Source object property
     */
    public Source getSource() {
        return source;
    }

    /**
     * Gets the Source object property
     *
     * @param source the Source object property
     */
    public void setSource(Source source) {
        this.source = source;
    }

    /**
     *
     * @return The return value is guaranteed to be non-null.
     *          The array elements are guaranteed to be non-null.
     *
     */
    public Vector getCommands() {
        return this.commands;
    }

    /**
     * Sets the sequenced commands. The given commands must be of the allowed
     * types.
     *
     * @param commands the commands - NOT NULL and o the allawed types
     *
     * @throws IllegalArgumentException if the constraints are not met
     */
    public void setCommands(Vector commands) {
        this.commands = commands;
    }

    /**
     * Gets the total number of changes
     *
     * @return the total number of changes
     */
    public Long getNumberOfChanges() {
        return numberOfChanges;
    }

    /**
     * Sets the numberOfChanges property
     *
     * @param numberOfChanges the total number of changes
     */
    public void setNumberOfChanges(long numberOfChanges) {
        this.numberOfChanges = new Long(numberOfChanges);
    }

    /**
     * Sets the numberOfChanges property
     *
     * @param numberOfChanges the total number of changes
     */
    public void setNumberOfChanges(Long numberOfChanges) {
        this.numberOfChanges = numberOfChanges;
    }

    public String getName() {
        return Sync.COMMAND_NAME;
    }
}
