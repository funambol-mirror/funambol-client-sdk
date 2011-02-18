/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2011 Funambol, Inc.
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

package com.funambol.util;

import java.io.*;
import javax.microedition.io.*;
import java.util.*;
import net.rim.device.api.ui.*;
import net.rim.device.api.ui.component.*;
import net.rim.device.api.ui.container.*;
import net.rim.device.api.system.*;
import net.rim.device.api.util.*;
import java.util.Date;

/**
 */ 
public class UsbAppender implements Appender {

    //private OutputStream os = null;
    private static final String CHANNEL = "funambol";

    private static final int MAX_TX_SIZE = 4096;
    private static final int MAX_RX_SIZE = 4096;


    private String msg = "";
    private LowLevelUsbThread usbThread = new LowLevelUsbThread();

    /**
     * Default constructor
     */
    public UsbAppender() {
    }
    
    //----------------------------------------------------------- Public Methods
    /**
     * ConsoleAppender writes one message on the standard output
     */
    public void writeLogMessage(String level, String msg) throws IOException {
        Date now = new Date();
        StringBuffer str = new StringBuffer(now.toString());
        str.append(" [").append(level).append("] " )
            .append(msg).append("\n");

        usbThread.writeMsg(str.toString());

        //this.msg = str.toString();
        //synchronized(sem) {
        //    sem.notify();
        //}
    }
    
    public void initLogFile() {
        usbThread.start();
    }

    public void openLogFile() {
    }

    public void closeLogFile() {
    }

    public void deleteLogFile() {
    }

    public void setLogLevel(int level) {
    }


    public LogContent getLogContent() throws IOException {
        throw new IOException("Cannot get log content");
    }


    private final class LowLevelUsbThread extends Thread implements USBPortListener
    {
        private boolean _dataAvailable;
        private Vector _readQueue;
        private int _channel;
        private USBPort _port;
        private Vector msgQueue = new Vector();
        private boolean isWaiting = false;

        public void run()
        {
            try 
            {
                init();

                while(true) {
                    synchronized(msgQueue) {
                        try {
                            msgQueue.wait();
                        } catch (Exception e) {
                        }
                    }
                    try {
                        for(int i=0;i<msgQueue.size();++i) {
                            String msg = (String)msgQueue.elementAt(i);
                            write(msg);
                        }
                        synchronized(msgQueue) {
                            msgQueue.removeAllElements();
                        }
                    } catch (IOException ioe) {
                    }
                }
                
                //close();
            
            } 
            catch (IOException e) 
            {
            }
        }

        public void writeMsg(String msg) throws IOException {

            if (_port != null) {
                if (msg.length() > MAX_TX_SIZE) {
                    boolean done = false;
                    int first = 0;
                    do {
                        int last;
                        // Write in blocks
                        if (first + MAX_TX_SIZE < msg.length()) {
                            last = first + MAX_TX_SIZE;
                        } else {
                            last = msg.length();
                            done = true;
                        }
                        String block = msg.substring(first, first + MAX_TX_SIZE);
                        write(block);
                        first += MAX_TX_SIZE;
                    } while(!done);
                } else {
                    write(msg);
                }
            }
        }

        
        /**
         * @see UsbThread#init()
         */
        protected void init() throws IOException
        {
            _readQueue = new Vector();

            // Register the app for callbacks.
            UiApplication app = UiApplication.getUiApplication();
            app.addIOPortListener(this);
            
            // Register the channel.
            _channel = USBPort.registerChannel(CHANNEL, MAX_RX_SIZE, MAX_TX_SIZE);
            
            synchronized(this)
            {
                try 
                {
                    // Wait for a channel.
                    if ( _port == null )
                    {
                        this.wait();
                    }                        
                } 
                catch (InterruptedException e) 
                {
                }
            }

            // Remove the listener
            app.removeIOPortListener(this);
        }
        
        /**
         * @see UsbThread#write(String)
         */
        protected void write(String s) throws IOException
        {
             _port.write(s.getBytes());
        }
        
        /**
         * @see UsbThread#close()
         */
        public void close()
        {
            try 
            {
                //message("closing connections...");
                
                if ( _port != null) 
                {
                    _port.close();
                }
                
                // Deregister the channel.
                USBPort.deregisterChannel(_channel); 
                
                //message("connections closed");
                
            } 
            catch (IOException e) 
            {
                //message(e.toString());
            }
        }
        
        // USBPortListener methods --------------------------------------------------
        /**
         * @see net.rim.device.api.system.USBPortListener#getChannel()
         */
        public int getChannel() 
        {
            return _channel;
        }
    
        /**
         * @see net.rim.device.api.system.USBPortListener#connectionRequested()
         */
        public void connectionRequested() 
        {
            try 
            {
                _port = new USBPort(_channel);
                
                synchronized(this) 
                {
                    this.notify();
                }
            } 
            catch (IOException e) 
            {
            }
        }
    
        /**
         * @see net.rim.device.api.system.USBPortListener#dataNotSent()
         */
        public void dataNotSent() 
        {
        }
       
        /**
         * @see net.rim.device.api.system.IOPortListener#connected()
         */
        public void connected()
        {
            //message( "lowlevelusb: connected" );
        }
        
        /**
         * @see net.rim.device.api.system.IOPortListener#disconnected()
         */
        public void disconnected()
        {
            //message( "lowlevelusb: disconnected" );
        }
        
        /**
         * @see net.rim.device.api.system.IOPortListener#receiveError(int)
         */
        public void receiveError(int error)
        {
            //message( "lowlevelusb: Got rxError: " + error );
        }
        
        /**
         * @see net.rim.device.api.system.IOPortListener#dataReceived(int)
         */
        public void dataReceived( int length )
        {            
        }
        
        /**
         * @see net.rim.device.api.system.IOPortListener#dataSent()
         */
        public void dataSent()
        {
        }
        
        /**
         * @see net.rim.device.api.system.IOPortListener#patternReceived(byte[])
         */
        public void patternReceived( byte[] pattern )
        {
            //message( "lowlevelusb: Got pattern " + new String(pattern)); //+ pattern[0] + " " + pattern[1] + " " + pattern[2] + " " + pattern[3] );
        }
    }
 

}
