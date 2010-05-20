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

/**
 * This class provides a storage mechanism for BlackBerry devices. Its
 * intent is to circumvent the 64k limit of javax.microedition.rms.RecordStore
 * on BlackBerry devices. It exposes methods of similar name 
 * to provide an interface similar to that of RMS. The BlackBerry model differs 
 * from the RMS model in that it isnt persistent. Changes made to a record are 
 * not automatically reflected in the repository. Consequentially, commits must be 
 * explicitly made after every transaction for the entire datastructure, and not just 
 * the affected record. There may be room for performance gains by using ObjectGroups 
 * and altering the implementation. 
 */

package com.funambol.storage;

import net.rim.device.api.util.Persistable;

/**
 * An interface to avoid unusability of the API with multiple applications
 * This interface must be implemented by the application that make use of the 
 * BlackberryRecordStore class. 
 */
public interface ObjectWrapperHandler {
    /**
     * Use this method to wrap an object to a persistable object
     * @param o is the object to be wrapped
     * @return the wrapped object
     */
    public Persistable createObjectWrapper(Object o);
    
    /**
     * gets the wrapped object content
     * @param p the persistable object
     * @return the object contained into the give Persistable
     */
    public Object getObject(Persistable p);
}

