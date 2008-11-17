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

package com.funambol.syncclient.spap;

/**
 * This exception represents an error for the asset management.
 *
 * @version $Id: AssetManagementException.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 *
 */
public class AssetManagementException extends Exception {


    private Asset asset = null;
    private String idAsset = null;
    private Throwable cause = null;

    // ------------------------------------------------------------ Constructors

    /**
     * Constructs an instance of <code>AssetManagementException</code> with the
     * specified detail message.
     *
     * @param message the detail message.
     */
    public AssetManagementException(String message) {
        super(message);
    }

    /**
     * Constructs an instance of <code>AssetManagementException</code> with the
     * specified detail message.
     *
     * and the cause.
     * @param message the detail message.
     * @param cause the cause of this error.
     */
    public AssetManagementException(String message, Throwable cause) {
        this(message);
        this.cause = cause;
    }

    /**
     * Constructs an instance of <code>AssetManagementException</code> with the
     * specified <code>Asset</code> and the specified message.
     *
     * @param asset the asset involved in this error.
     * @param message the detail message.
     */
    public AssetManagementException(Asset asset, String message) {
        super(message);
        this.asset = asset;
    }

    /**
     * Constructs an instance of <code>AssetManagementException</code> with
     * the specified <code>Asset</code> and the specified message and the cause.
     * @param asset the asset involved in this error.
     * @param message the detail message.
     * @param cause the cause of this error.
     */
    public AssetManagementException(Asset asset, String message, Throwable cause) {
        this(asset,message);
        this.cause = cause;
    }

    /**
     * Constructs an instance of <code>AssetManagementException</code> with
     * the specified idAsset and the specified message
     * @param idAsset the idAsset involved in this error.
     * @param message the detail message.
     */
    public AssetManagementException(String idAsset, String message) {
        super(message);
        this.idAsset = idAsset;
    }

    /**
     * Constructs an instance of <code>AssetManagementException</code> with the specified
     * idAsset and the specified message and the cause.
     * @param idAsset the idAsset involved in this error.
     * @param message the detail message.
     * @param cause the cause of this error.
     */
    public AssetManagementException(String idAsset, String message, Throwable cause) {
        this(idAsset,message);
        this.cause = cause;
    }

    //---------------------------------------------------
    //  Overrides java.lang.Object method
    //---------------------------------------------------

    public String toString() {
        StringBuffer message = new StringBuffer();
        if (asset!=null) {
            message.append(super.toString());
            message.append(" (id: ");
            message.append(asset.getId());
            message.append(", manufacturer: ");
            message.append(asset.getManufacturer());
            message.append(", name: ");
            message.append(asset.getName());
            message.append(")");
        } else if (idAsset != null) {
            message.append(super.toString());
            message.append(" (id: ");
            message.append(idAsset);
            message.append(")");
        } else {
            message.append(super.toString());
        }

        return message.toString();
    }

    /**
     * Prints the exception and its cause (if any) stack trace
     */
    public void printStackTrace() {
        super.printStackTrace();
        if (cause != null) {
            System.err.println("Caused by:");
            cause.printStackTrace();
        }
    }

}