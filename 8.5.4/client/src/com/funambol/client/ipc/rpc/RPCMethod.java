/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2009 Funambol, Inc.
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

package com.funambol.client.ipc.rpc;

/**
 * This class represents a method that can be registered into the {@link RPCManager}
 * to be accessible from remote. Such a method has a name that must be unique
 * (no overloading is supported) and the it has an abstract execution method
 * associated. When a request arrives to execute this method, the RPCManager
 * invokes the <i>execute</i> method of the RPCMethod which is responsible for
 * processing the incoming paramters and to produce the expected output
 * parameter (if any).
 *
 */
public abstract class RPCMethod {

    private String methodName;

    /**
     * Build a RPCMethod, a method that can be invoked from remote.
     * @param methodName the method name
     */
    public RPCMethod(String methodName) {
        this.methodName = methodName;
    }

    /**
     * Get the method's name
     * @return the method name
     */
    public String getName() {
        return methodName;
    }

    /**
     * This method is called back on remote execution of this RPCMethod. This
     * method is executed by the RPCManager in its own thread.
     * @param params the parameters given to the method (null if no parameters
     * are supplied)
     * @return the return value (if any, null otherwise)
     */
    public abstract RPCParam execute(RPCParam params[]) throws Exception;
}