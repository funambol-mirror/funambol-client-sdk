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

package com.funambol.syncclient.spds.event;

import java.util.Date;

/**
 * Is used to notify the listeners
 * for the principal states.
 *
 *
 *
 * @version $Id: SyncEvent.java,v 1.3 2007-12-22 18:09:18 nichele Exp $
 */
public class SyncEvent {

    // -------------------------------------------------------- Constants

    /** The synchronization process start */
    public static final int SYNC_BEGIN           = 0  ;

    /** The synchronization process end */
    public static final int SYNC_END             = 1  ;

    /** A no blocking error occours */
    public static final int SYNC_ERROR           = 2  ;

    /** Send initialization message */
    public static final int SEND_INITIALIZATION  = 3  ;

    /** Send modification message   */
    public static final int SEND_MODIFICATION    = 4  ;

    /** Send finalization message   */
    public static final int SEND_FINALIZATION    = 5  ;

    // -------------------------------------------------------- Private data

    private Throwable  cause    = null ;
    private Date       date     = null ;
    private String     message  = null ;
    private int        type     = 0    ;

    // -------------------------------------------------------- Constructors

    /**
     * Creates a SyncEvent
     *
     * @param type the event type
     * @param date the date and time when the event occours
     */
    public SyncEvent (int type, long date) {

        this.type = type;
        this.date = new Date(date);

    }

    /**
     * Creates a SyncEvent
     *
     * @param type the event type
     * @param date the date and time when the event occours
     * @param message only if the event is a SYNC_ERROR, <p>null</p> otherwise
     * @param cause only if the event is a SYNC_ERROR, <p>null</p> otherwise
     */
    public SyncEvent (int        type     ,
                      long       date     ,
                      String     message  ,
                      Throwable  cause    ) {

            this.type    = type           ;
            this.date    = new Date(date) ;
            this.message = message        ;
            this.cause   = cause          ;

    }

    // -------------------------------------------------------- Public methods

    public Throwable getCause () {
        return cause;
    }

    public Date getDate       () {
        return date;
    }

    public String getMessage  () {
        return message;
    }

    public int getType        () {
        return type;
    }

    public void setCause    (Throwable cause  ) {
        this.cause   = cause   ;
    }

    public void setDate     (Date      date   ) {
        this.date    = date    ;
    }

    public void setMessage  (String    message) {
        this.message = message ;
    }

    public void setType     (int       type   ) {
        this.type    = type    ;
    }

    // -------------------------------------------------------- Private methods

}
