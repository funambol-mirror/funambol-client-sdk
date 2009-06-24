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
package com.funambol.syncclient.spds.source;

import java.io.*;

import java.security.Principal;

import java.util.Date;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Properties;
import java.util.Vector;

import sync4j.framework.tools.Base64;

import com.funambol.common.pim.calendar.*;
import com.funambol.common.pim.contact.*;
import com.funambol.common.pim.converter.*;
import com.funambol.common.pim.model.VCalendar;
import com.funambol.common.pim.sif.*;
import com.funambol.common.pim.vcard.VcardParser;
import com.funambol.common.pim.xvcalendar.XVCalendarParser;

import com.funambol.syncclient.common.SourceUtils;
import com.funambol.syncclient.common.logging.Logger;
import com.funambol.syncclient.spds.SyncException;
import com.funambol.syncclient.spds.engine.*;
import com.funambol.syncclient.spds.source.FileSystemSyncSource;

/**
 * This class implements a <i>SyncSource</i> that extends a FileSystemSyncSource
 * and it's able to receive and send items in the following formats:
 * <ul>
 *  <li>SIF-C
 *  <li>SIF-E
 *  <li>text
 *  <li>vCard
 *  <li>vCalendar
 * </ul>
 *
 * The clientContentType and the serverContentType specify the format of
 * which the client and the server send the items and would receive the returned.
 *
 * @version $Id: SIFFileSystemSyncSource.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 *
 */
public class SIFFileSystemSyncSource extends FileSystemSyncSource
implements SyncSource {

    // --------------------------------------------------------------- Constants
    //
    // Types of client and server content
    //
    private static final String CONTENT_TYPE_VCARD = "vCard"    ;
    private static final String CONTENT_TYPE_VCAL  = "vCalendar";
    private static final String CONTENT_TYPE_TEXT  = "text"     ;
    private static final String CONTENT_TYPE_SIFC  = "SIF-C"    ;
    private static final String CONTENT_TYPE_SIFE  = "SIF-E"    ;

    //
    // Types of content item
    //
    private static final int TYPE_VCARD = 0;
    private static final int TYPE_VCAL  = 1;
    private static final int TYPE_TEXT  = 2;
    private static final int TYPE_SIFC  = 3;
    private static final int TYPE_SIFE  = 4;

    // -------------------------------------------------------------- Properties
    /**
     * The drive where to read the sourceDirectory
     */
    private String sourceDrive;
    public void setSourceDrive(String sourceDrive) {
        this.sourceDrive = sourceDrive;
        if (getSourceDirectory() != null) {
            super.setSourceDirectory(sourceDrive + getSourceDirectory());
        }
    }
    public String getSourceDrive() {
        return this.sourceDrive;
    }

    public void setSourceDirectory(String sourceDirectory) {
        if (sourceDrive != null) {
            sourceDirectory = sourceDrive + sourceDirectory;
        }
        super.setSourceDirectory(sourceDirectory);
    }

    /**
     * The charset to use in object conversion
     */
    private String charset = "PLAIN";
    public void setCharset(String charset) {
        this.charset = charset;
    }
    public String getCharset() {
        return this.charset;
    }

    /**
     * The items type to read/save from/on the sourceDirectory
     */
    private String clientContentType;
    public String getClientContentType() {
        return this.clientContentType;
    }
    public void setClientContentType(String clientContentType) {
        this.clientContentType = clientContentType;
        setClientItemType();
    }

    /**
     * Sets an int that define the type of the client content
     */
    private int CLIENT_TYPE = TYPE_TEXT;
    private void setClientItemType() {
        if (this.clientContentType.equals(CONTENT_TYPE_VCARD)) {
            CLIENT_TYPE = TYPE_VCARD;
        } else if (this.clientContentType.equals(CONTENT_TYPE_VCAL)) {
            CLIENT_TYPE = TYPE_VCAL;
        } else if (this.clientContentType.equals(CONTENT_TYPE_TEXT)) {
            CLIENT_TYPE = TYPE_TEXT;
        } else if (this.clientContentType.equals(CONTENT_TYPE_SIFC)) {
            CLIENT_TYPE = TYPE_SIFC;
        } else if (this.clientContentType.equals(CONTENT_TYPE_SIFE)) {
            CLIENT_TYPE = TYPE_SIFE;
        }
    }

    /**
     * The type to use on received items and to use to send items to the server
     */
    private String serverContentType;
    public String getServerContentType() {
        return this.serverContentType;
    }
    public void setServerContentType(String serverContentType) {
        this.serverContentType = serverContentType;
        setServerItemType();
    }

    /**
     * Sets an int that define the type of the server content
     */
    private int SERVER_TYPE = TYPE_TEXT;
    private void setServerItemType() {
        if (this.serverContentType.equals(CONTENT_TYPE_VCARD)) {
            SERVER_TYPE = TYPE_VCARD;
        } else if (this.serverContentType.equals(CONTENT_TYPE_VCAL)) {
            SERVER_TYPE = TYPE_VCAL;
        } else if (this.serverContentType.equals(CONTENT_TYPE_TEXT)) {
            SERVER_TYPE = TYPE_TEXT;
        } else if (this.serverContentType.equals(CONTENT_TYPE_SIFC)) {
            SERVER_TYPE = TYPE_SIFC;
        } else if (this.serverContentType.equals(CONTENT_TYPE_SIFE)) {
            SERVER_TYPE = TYPE_SIFE;
        }
    }

    // ------------------------------------------------------------ Constructors
    /** Creates a new instance of AbstractSyncSource */
    public SIFFileSystemSyncSource() {
    }

    // ---------------------------------------------------------- Public methods
    public SyncItem setSyncItem(Principal principal, SyncItem syncItem)
    throws SyncException {

        char itemState = syncItem.getState();

        try {
            String fileName = null;
            if (itemState == SyncItemState.NEW) {
                fileName = getUniqueFileName();
            } else {
                fileName = syncItem.getKey().getKeyAsString();
            }

            byte[] fileContent =
                    (byte[]) syncItem.getPropertyValue(SyncItem.PROPERTY_BINARY_CONTENT);

            if (fileContent == null) {
                fileContent = new byte[0];
            }

            if (isEncode()) {
                fileContent = Base64.decode(fileContent);
            }

            //
            // Verify client item type
            //
            switch (CLIENT_TYPE) {
                // To VCard
                case TYPE_VCARD:
                    //
                    // Verify server item type
                    //
                    switch (SERVER_TYPE) {
                        // From SIF-C
                        case TYPE_SIFC:
                            fileContent = handleSIFCAsVCard(fileContent, syncItem);
                            break;
                        // From VCard or Text
                        case TYPE_VCARD:
                        case TYPE_TEXT:
                            //
                            // No convertion is required
                            //
                            break;
                        default:
                            String msg = "Content types not compatible: "
                                       + " Client content type = "
                                       + clientContentType
                                       + " Server content type = "
                                       + serverContentType
                                       ;
                            throw new SyncException(msg);
                    }
                    break;
                // To vCalendar
                case TYPE_VCAL:
                    //
                    // Verify server item type
                    //
                    switch (SERVER_TYPE) {
                        // From SIF-E
                        case TYPE_SIFE:
                            fileContent = handleSIFEAsVCal(fileContent, syncItem);
                            break;
                        // From vCalendar or Text
                        case TYPE_VCAL:
                        case TYPE_TEXT:
                            //
                            // No convertion is required
                            //
                            break;
                        default:
                            String msg = "Content types not compatible: "
                                       + " Client content type = "
                                       + clientContentType
                                       + " Server content type = "
                                       + serverContentType
                                       ;
                            throw new SyncException(msg);
                    }
                    break;
                // To Text
                case TYPE_TEXT:
                    //
                    // Is not need to check the server item type because no
                    // conversion is required
                    //
                    break;
                // To SIF-C
                case TYPE_SIFC:
                    //
                    // Verify server item type
                    //
                    switch (SERVER_TYPE) {
                        // From VCard
                        case TYPE_VCARD:
                            fileContent = handleVCardAsSIFC(fileContent, syncItem);
                            fileContent = handleSIFContent(fileContent, fileName, syncItem);
                            break;
                        // From SIF-C
                        case TYPE_SIFC:
                            fileContent = handleSIFContent(fileContent, fileName, syncItem);
                            break;
                        // From Text
                        case TYPE_TEXT:
                            //
                            // No conversion is required
                            //
                            break;
                        default:
                            String msg = "Content types not compatible: "
                                       + " Client content type = "
                                       + clientContentType
                                       + " Server content type = "
                                       + serverContentType
                                       ;
                            throw new SyncException(msg);
                    }
                    break;
                // To SIF-E
                case TYPE_SIFE:
                    //
                    // Verify server item type
                    //
                    switch (SERVER_TYPE) {
                        // From vCal
                        case TYPE_VCAL:
                            fileContent = handleVCalAsSIFE(fileContent, syncItem);
                            fileContent = handleSIFContent(fileContent, fileName, syncItem);
                            break;
                        // From SIF-E
                        case TYPE_SIFE:
                            fileContent = handleSIFContent(fileContent, fileName, syncItem);
                            break;
                        // From Text
                        case TYPE_TEXT:
                            //
                            // No conversion is required
                            //
                            break;
                        default:
                            String msg = "Content types not compatible: "
                                       + " Client content type = "
                                       + clientContentType
                                       + " Server content type = "
                                       + serverContentType
                                       ;
                            throw new SyncException(msg);
                    }
                    break;
            }

            File f = new File(getSourceDirectory(), fileName);
            FileOutputStream fos = new FileOutputStream(f);
            fos.write(fileContent);
            fos.close();

            Date t = (Date) syncItem.getPropertyValue(SyncItem.PROPERTY_TIMESTAMP);
            f.setLastModified(roundTime(t.getTime()));

            setState(principal,
                     fileName,
                     SyncItemState.SYNCHRONIZED,
                     roundTime(t.getTime()));

            SyncItem newSyncItem =
                new SyncItemImpl(this, fileName, SyncItemState.NEW);

            newSyncItem.setProperties(syncItem.getProperties());

            return newSyncItem;
        } catch (Exception e) {
            throw new SyncException( "Error setting the item "
                                   + syncItem.getKey().getKeyAsString()
                                   , e
                                   );
        }
    }

    // ------------------------------------------------------------ Private data
    /**
     * Convert item from SIF-C format into VCard format
     *
     * @param fileContent a byte[] with the content of item
     * @param syncItem the SyncItem object
     *
     * @return byte[] the file content converted into VCard format
     *
     * @throws SyncException
     */
    private byte[] handleSIFCAsVCard(byte[] fileContent, SyncItem syncItem)
    throws SyncException {

        try {

            //
            // Converting the xmlStream into a Contact object
            //
            ByteArrayInputStream xmlStream = new ByteArrayInputStream(fileContent);
            SIFCParser parser = new SIFCParser(xmlStream);
            Contact contact = (Contact)parser.parse();

            //
            // Converting the Contact object into a vcard string
            //
            ContactToVcard c2vcard = new ContactToVcard(null, charset);
            String vcard = c2vcard.convert(contact);

            return vcard.getBytes();

        } catch (Exception e) {
            String msg = "Error converting item "
                       + syncItem.getKey().getKeyAsString()
                       + " from SIF-C to VCard format"
                       ;
            throw new SyncException(msg, e);
        }

    }

    /**
     * Convert item from SIF-E format into vCalendar format
     *
     * @param fileContent a byte[] with the content of item
     * @param syncItem the SyncItem object
     *
     * @return byte[] the file content converted into vCalendar format
     * @throws SyncException
     */
    private byte[] handleSIFEAsVCal(byte[] fileContent, SyncItem syncItem)
    throws SyncException {

        try {

            //
            // Converting the xmlStream into a Calendar object
            //
            ByteArrayInputStream xmlStream = new ByteArrayInputStream(fileContent);
            SIFCalendarParser parser = new SIFCalendarParser(xmlStream);
            Calendar calendar = (Calendar)parser.parse();
            
            //
            // Converting the Calendar object into a vcalendar string
            //
            VCalendarConverter vcf = new VCalendarConverter(null, charset);
            VCalendar vcal = vcf.calendar2vcalendar(calendar, true);
            VComponentWriter writer = 
                    new VComponentWriter(VComponentWriter.NO_FOLDING);
            
            return writer.toString(vcal).getBytes();

        } catch (Exception e) {
            String msg = "Error converting item "
                       + syncItem.getKey().getKeyAsString()
                       + " from SIF-E to ICal format"
                       ;
            throw new SyncException(msg, e);
        }

    }

    /**
     * Convert item from VCard format into SIF-C format
     *
     * @param fileContent a byte[] with the content of item
     * @param syncItem the SyncItem object
     *
     * @return byte[] the file content converted into SIF-C format
     *
     * @throws SyncException
     */
    private byte[] handleVCardAsSIFC(byte[] fileContent, SyncItem syncItem)
    throws SyncException {

        try {

            //
            // Handle the long line
            //
            String fc = SourceUtils.handleLineDelimiting(new String(fileContent));

            ByteArrayInputStream buffer = new ByteArrayInputStream(fc.getBytes());
            //
            //Converting the vCard item into a Contact object
            //
            VcardParser parser = new VcardParser(buffer);
            Contact contact = (Contact)parser.vCard();

            //
            //Converting the Contact object into an XML file
            //
            ContactToSIFC c2xml = new ContactToSIFC(null, charset);            
            String xmlStream = c2xml.convert(contact);

            return xmlStream.getBytes();

        } catch(com.funambol.common.pim.vcard.TokenMgrError e) {
            throw new SyncException("Lexical error to parse item " +
                                    syncItem.getKey().getKeyAsString(), e);
        } catch (com.funambol.common.pim.vcard.ParseException e) {
            throw new SyncException("Error parsing the item " +
                                    syncItem.getKey().getKeyAsString()
                                    + " from vCard to SIF-C", e);
        } catch (Exception e) {
            throw new SyncException( "Error to handle convert from vCard to SIF-C"
                                   + " of the item "
                                   + syncItem.getKey().getKeyAsString(), e);
        }
    }

    /**
     * Convert item from SIF-E format into vCalendar format
     *
     * @param fileContent a byte[] with the content of item
     * @param syncItem the SyncItem object
     *
     * @return byte[] the file content converted into SIF-E format
     *
     * @throws SyncException
     */
    private byte[] handleVCalAsSIFE(byte[] fileContent, SyncItem syncItem)
    throws SyncException {

        try {

            //
            // Handle the long line
            //
            String fc = SourceUtils.handleLineDelimiting(new String(fileContent));

            ByteArrayInputStream buffer = new ByteArrayInputStream(fc.getBytes());
            //
            //Converting the vCalendar item into a Calendar object
            //            
            XVCalendarParser parser = new XVCalendarParser(buffer);
            VCalendar vcal = (VCalendar)parser.XVCalendar();

            VCalendarConverter vcf = new VCalendarConverter(null, charset);
            Calendar c = vcf.vcalendar2calendar(vcal);
            
            //
            //Converting the Calendar object into an XML file
            //
            CalendarToSIFE c2xml = new CalendarToSIFE(null, charset);            
            String xmlStream = c2xml.convert(c);

            return xmlStream.getBytes();

        } catch(com.funambol.common.pim.xvcalendar.TokenMgrError e) {
            throw new SyncException("Lexical error to parse item " +
                                    syncItem.getKey().getKeyAsString(), e);
        } catch (com.funambol.common.pim.xvcalendar.ParseException e) {
            throw new SyncException("Error parsing the item "
                                    + syncItem.getKey().getKeyAsString()
                                    + " from vCalendar to SIF-E (" 
                                    + e.getMessage() + ").", e);
        } catch (Exception e) {
            throw new SyncException( "Error to handle convert from vCalendar to"
                                   + " SIF-E of the item "
                                   + syncItem.getKey().getKeyAsString(), e);
        }
    }

    /**
     * Il client content type is SIF-C or SIF-E and server are the same
     * content type, than check if the file exist
     *
     * @param fileContent a byte[] with the content of item
     * @param fileName the name of file that content the item
     * @param syncItem the SyncItem object
     *
     * @return byte[] the file content merged
     *
     * @throws SyncException
     */
    private byte[] handleSIFContent(byte[]   fileContent, 
                                    String   fileName   , 
                                    SyncItem syncItem   )
    throws SyncException {

        try {

            HashMap hashMap         = null;
            HashMap hashMapFromFile = null;
            String xmlStream = new String(fileContent);

            File f = new File(getSourceDirectory(), fileName);
            if (f.exists()) {
                hashMapFromFile = SourceUtils.xmlToHashMap(readFileString(f));
                hashMap = SourceUtils.xmlToHashMap(xmlStream);
                hashMapFromFile.putAll(hashMap);
                fileContent = SourceUtils.hashMapToXml(hashMapFromFile).getBytes();
            }
            else {
                fileContent = xmlStream.getBytes();
            }

            return fileContent;

        } catch (IOException e) {
            throw new SyncException("Error setting the item " +
                                    syncItem.getKey().getKeyAsString(), e);
        } catch (Exception e) {
            throw new SyncException("Error setting the hashmap in item " +
                                    syncItem.getKey().getKeyAsString(), e);
        }
    }

    /**
     * Reads a file into a String given its filename
     *
     * @param file the filename (as java.io.File)
     *
     * @return the content of the file as a string
     *
     * @throws java.io.IOException;
     */
    private String readFileString(File file) throws IOException {
        FileInputStream fis = null;

        byte[] buf = new byte[(int)file.length()];
        try {
            fis = new FileInputStream(file);
            fis.read(buf);
            fis.close();
        } finally {
            if (fis != null) {
                fis.close();
            }
        }
        return new String(buf);
    }

    /**
     * Filters the SyncItems in the synchronization database (after a refresh)
     * based on the given principal, last sync timestamp and state (see
     * SyncItemState). If state is equals to UNKNOWN all items are returned.<br>
     * Note that the current implementation ignores the principal: data do not
     * depend on users.
     *
     * @param principal principal. null means any
     * @param since     last sync timestamp. null neans since ever
     * @param state     the state to use as filter
     *
     * @return an array of SyncItem objects whose state is equal to the given
     *         state.
     */
    protected SyncItem[] filterSyncItems(Principal principal, Date since, char state)
    throws SyncException {

        Vector syncItems = new Vector();

        long fileTimestamp,
             sinceTimestamp = (since == null) ? -1 : roundTime(since.getTime());
        
        Properties syncDatabase = updateSyncDatabase(principal, sinceTimestamp);

        SyncItem  syncItem      = null;
        String    fileName      = null;
        String    stateString   = null;
        char      fileState           ;        
        byte[] fileContent = new byte[0];

        for (Enumeration en = syncDatabase.keys(); en.hasMoreElements(); ) {
            fileName  = (String)en.nextElement();
            stateString = (String)syncDatabase.get(fileName);
            fileState = stateFromStateString(stateString);
            if ((state == SyncItemState.UNKNOWN) || (fileState == state)) {
                fileTimestamp = lastModifiedFromStateString(stateString);
                if (fileTimestamp > sinceTimestamp ) {
                    syncItem = new SyncItemImpl(this, fileName, fileState);
                    fileContent = readFileContent(fileName);

                    if (fileContent.length != 0) {
                        //
                        // Check the content type of client items and the
                        // content type required from server item: if different
                        // than change the item format in the server item format
                        //
                        try {
                            fileContent = handleItemContentType(fileContent, fileName, syncItem);
                        } catch(Exception e) {
                            e.printStackTrace();
                            if(Logger.isLoggable(Logger.INFO)) {
                                Logger.info("Error reading item: " + e.getMessage());
                            }
                            throw new SyncException("Error reading item: " + e.getMessage(), e);
                        }
                    }

                    if (isEncode()){
                        syncItem.setProperty(
                            new SyncItemProperty(
                                SyncItem.PROPERTY_BINARY_CONTENT,
                                Base64.encode(fileContent)
                            )
                        );
                        syncItem.setProperty(
                            new SyncItemProperty(
                                SyncItem.PROPERTY_FORMAT,
                                FileSystemSyncSource.FORMAT_BASE64)
                        );
                    } else {
                        syncItem.setProperty(
                            new SyncItemProperty(
                                SyncItem.PROPERTY_BINARY_CONTENT,
                                fileContent
                            )
                        );
                    }
                    syncItem.setProperty(
                        new SyncItemProperty(
                            SyncItem.PROPERTY_TYPE,
                            this.getType())
                    );
                    syncItems.addElement(syncItem);
                }
            }
        }  // next en

        SyncItem[] ret = new SyncItem[syncItems.size()];
        for (int i=0; i<ret.length; ++i) {
            ret[i] = (SyncItem)syncItems.elementAt(i);
        }

        return ret;
    }

    /**
     * Check the content type of client items and the content type required
     * from server: if different than change the item format into server item
     * format
     *
     * @param fileContent a byte[] with the content of item
     * @param fileName the name of file that content the item
     * @param syncItem the SyncItem object
     *
     * @return byte[] the file content
     *
     * @throws SyncException
     */
    private byte[] handleItemContentType(byte[] fileContent,
                                         String fileName   ,
                                         SyncItem syncItem )
    throws SyncException {

        //
        // Verify server item type
        //
        switch (SERVER_TYPE) {
            // To VCard
            case TYPE_VCARD:
                //
                // Verify client item type
                //
                switch (CLIENT_TYPE) {
                    // From SIF-C
                    case TYPE_SIFC:
                        fileContent = handleSIFCAsVCard(fileContent, syncItem);
                        break;
                    // From VCard or Text
                    case TYPE_VCARD:
                    case TYPE_TEXT:
                        //
                        // No convertion is required
                        //
                        break;
                    default:
                        String msg = "Content types not compatible: "
                                   + " Client content type = "
                                   + clientContentType
                                   + " Server content type = "
                                   + serverContentType
                                   ;
                        throw new SyncException(msg);
                }
                break;
            // To vCalendar
            case TYPE_VCAL:
                //
                // Verify client item type
                //
                switch (CLIENT_TYPE) {
                    // From SIF-E
                    case TYPE_SIFE:
                        fileContent = handleSIFEAsVCal(fileContent, syncItem);
                        break;
                    // From vCalendar or Text
                    case TYPE_VCAL:
                    case TYPE_TEXT:
                        //
                        // No convertion is required
                        //
                        break;
                    default:
                        String msg = "Content types not compatible: "
                                   + " Client content type = "
                                   + clientContentType
                                   + " Server content type = "
                                   + serverContentType
                                   ;
                        throw new SyncException(msg);
                }
                break;
            // To Text
            case TYPE_TEXT:
                //
                // Is not need to check the client item type because no
                // conversion is required
                //
                break;
            // To SIF-C
            case TYPE_SIFC:
                //
                // Verify client item type
                //
                switch (CLIENT_TYPE) {
                    // From VCard
                    case TYPE_VCARD:
                        fileContent = handleVCardAsSIFC(fileContent, syncItem);
                        break;
                    // From SIF-C
                    case TYPE_SIFC:
                        fileContent = handleSIFContent(fileContent, fileName, syncItem);
                        break;
                    // From Text
                    case TYPE_TEXT:
                        //
                        // No conversion is required
                        //
                        break;
                    default:
                        String msg = "Content types not compatible: "
                                   + " Client content type = "
                                   + clientContentType
                                   + " Server content type = "
                                   + serverContentType
                                   ;
                        throw new SyncException(msg);
                }
                break;
            // To SIF-E
            case TYPE_SIFE:
                //
                // Verify client item type
                //
                switch (CLIENT_TYPE) {
                    // From vCalendar
                    case TYPE_VCAL:
                        fileContent = handleVCalAsSIFE(fileContent, syncItem);
                        break;
                    // From SIF-E
                    case TYPE_SIFE:
                        fileContent = handleSIFContent(fileContent, fileName, syncItem);
                        break;
                    // From Text
                    case TYPE_TEXT:
                        //
                        // No conversion is required
                        //
                        break;
                    default:
                        String msg = "Content types not compatible: "
                                   + " Client content type = "
                                   + clientContentType
                                   + " Server content type = "
                                   + serverContentType
                                   ;
                        throw new SyncException(msg);
                }
                break;
        } //end switch server type
        return fileContent;
    }
}
