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
 * This class represents the &lt;Map&gt; tag as defined by the SyncML
 * representation specifications.
 */
public class Map extends AbstractCommand {

    // --------------------------------------------------------------- Constants

    public static String COMMAND_NAME = "Map";

    // ------------------------------------------------------------ Private data

    private Target    target;
    private Source    source;
    private Vector    mapItems = new Vector();

    // ------------------------------------------------------------ Constructors

    public Map() {}

    /**
     * Creates a new Map commands from its constituent information.
     *
     * @param cmdID command identifier - NOT NULL
     * @param target the target - NOT NULL
     * @param source the source - NOT NULL
     * @param cred authentication credential - NULL ALLOWED
     * @param meta the associated meta data - NULL ALLOWED
     * @param mapItems the mapping items - NOT NULL
     *
     */
    public Map(final String cmdID,
               final Target target,
               final Source source,
               final Cred cred,
               final Meta meta,
               final Vector mapItems) {

        super(cmdID);
        setMeta(meta);
        setCred(cred);

        setTarget  (target  );
        setSource  (source  );
        setMapItems(mapItems);
    }

    // ---------------------------------------------------------- Public methods

    /**
     * Returns the target property
     * @return the target property
     *
     */
    public Target getTarget() {
        return target;
    }

    /**
     * Sets the target property
     *
     * @param target the target - NOT NULL
     *
     * @throws IllegalArgumentException if target is null
     */
    public void setTarget(Target target) {
        if (target == null) {
            throw new IllegalArgumentException("target cannot be null");
        }
        this.target = target;
    }

    /**
     * Returns the source property
     * @return the source property
     *
     */
    public Source getSource() {
        return source;
    }

    /**
     * Sets the source property
     *
     * @param source the source - NOT NULL
     *
     * @throws IllegalArgumentException if source is null
     */
    public void setSource(Source source) {
        if (source == null) {
            throw new IllegalArgumentException("source cannot be null");
        }
        this.source = source;
    }

    /**
     * Returns the map items
     *
     * @return the map items
     *
     */
    public Vector getMapItems() {
        return mapItems;
    }

    /**
     * Sets the mapItems property
     *
     * @param mapItems the map items - NOT NULL
     *
     * @throws IllegalArgumentException if mapItems is null
     */
    public void setMapItems(Vector mapItems) {
        this.mapItems = mapItems;
    }

    /**
     * Returns the command name
     *
     * @return the command name
     */
    public String getName() {
        return Map.COMMAND_NAME;
    }
}
