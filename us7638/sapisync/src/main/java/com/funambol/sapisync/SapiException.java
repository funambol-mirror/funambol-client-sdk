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

package com.funambol.sapisync;

/**
 * This exception represents the base exception for synchronization
 * related error conditions.
 *
 */
public class SapiException extends RuntimeException {
    //Default 
    private static final long serialVersionUID = 1L;
    

    //SAPI ERROR CODE 
    
    /** */
    public static final String UNKNOWN = "Unknown";
    
    /** */
    public static final String PAPI_0000 = "PAPI-0000";
    /** User not specified when SAPI requires realm SYSTEM or USER */
    public static final String SEC_1001 = "SEC-1001";
    /** */
    public static final String SEC_1002 = "SEC-1002";
    /** */
    public static final String SEC_1004 = "SEC-1004";
    /** */
    public static final String HTTP_400 = "HTTP-400";
    /** */
    public static final String MED_1002 = "MED-1002";
    /** */
    public static final String MED_1007 = "MED-1007";

    /** The code of the exception */
    private final String code;

    /** Server side cause of the exception */
    private final String sapiCause;
    
    public final static SapiException SAPI_EXCEPTION_UNKNOWN =
        new SapiException(UNKNOWN, "Impossible to retrieve SAPI exception from server reply");
    
    /**
     * Constructs an instance of <code>SapiException</code>
     * with the specified detail message.
     *
     * @param code the error code.
     * @param message the detail message.
     * @param sapiCause the server side error cause 
     */
    public SapiException(String code, String message, String sapiCause) {
        super(message);
        this.code = code;
        this.sapiCause = sapiCause;
    }

    /**
     * Constructs an instance of <code>SapiException</code>
     * with the specified detail message.
     *
     * @param code the error code.
     * @param message the detail message.
     */
    public SapiException(String code, String message) {
        this(code, message, "");
    }
    
    /** Returns the code of this exception */
    public String getCode() {
        return code;
    }    

    public String getSapiCause() {
        return sapiCause;
    }
}
