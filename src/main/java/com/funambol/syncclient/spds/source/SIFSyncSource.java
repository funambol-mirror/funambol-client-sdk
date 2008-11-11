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
import java.util.HashMap;
import java.util.Vector;
import java.util.Enumeration;
import java.util.Properties;

import sync4j.framework.tools.Base64;

import com.funambol.syncclient.common.FileSystemTools;
import com.funambol.syncclient.common.SourceUtils;
import com.funambol.syncclient.spds.engine.*;
import com.funambol.syncclient.spds.SyncException;



/**
 * This class implements a dummy <i>SyncSource</i> that just displays the calls
 * to its methods
 *
 * @version $Id: SIFSyncSource.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 *
 */
public class SIFSyncSource extends FileSystemSyncSource implements SyncSource  {

    // --------------------------------------------------------------- Constants

    // -------------------------------------------------------------- Properties

    // ------------------------------------------------------------ Constructors

    /** Creates a new instance of AbstractSyncSource */
    public SIFSyncSource() {
        super();
    }

    // ---------------------------------------------------------- Public methods

    /*
    * @see SyncSource
    */
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
                (byte[])syncItem.getPropertyValue(SyncItem.PROPERTY_BINARY_CONTENT);

            if (fileContent == null) {
                fileContent = new byte[0];
            }

            HashMap hashMap         = null;
            HashMap hashMapFromFile = null;

            File f = new File (getSourceDirectory(), fileName);

            if (isEncode() && fileContent.length > 0) {


                if (f.exists()) {
                    hashMapFromFile = SourceUtils.
                        xmlToHashMap(FileSystemTools.readFileString(f));

                    hashMap = SourceUtils.xmlToHashMap
                        (new String(Base64.decode(fileContent)));

                    hashMapFromFile.putAll(hashMap);

                    FileSystemTools.writeFile(
                        SourceUtils.hashMapToXml(hashMapFromFile), f);


                }
                else {

                    hashMapFromFile = new HashMap();

                    hashMap = SourceUtils.xmlToHashMap
                        (new String(Base64.decode(fileContent)));

                    hashMapFromFile.putAll(hashMap);

                    FileSystemTools.writeFile(
                        SourceUtils.hashMapToXml(hashMapFromFile), f);
                }

            } else {

                if (f.exists()) {

                    hashMapFromFile = SourceUtils.
                        xmlToHashMap(FileSystemTools.readFileString(f));

                    hashMap = SourceUtils.
                        xmlToHashMap(new String(fileContent));

                    hashMapFromFile.putAll(hashMap);

                    FileSystemTools.writeFile(
                        SourceUtils.hashMapToXml(hashMapFromFile), f);

                } else {

                    hashMapFromFile = new HashMap();

                    hashMap = SourceUtils.
                        xmlToHashMap(new String(fileContent));

                    hashMapFromFile.putAll(hashMap);

                    FileSystemTools.writeFile(
                        SourceUtils.hashMapToXml(hashMapFromFile), f);

                }

            }

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
        } catch (IOException e) {
            throw new SyncException( "Error setting the item "
                                      + syncItem
                                      , e
                                      );
        }
        catch (Exception e) {
            throw new SyncException( "Error setting the hashmap in item "
                                     + syncItem
                                     , e
                                     );
        }
    }

    // --------------------------------------------------------- Private methods

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
    protected SyncItem[] filterSyncItems(Principal principal,
                                         Date      since    ,
                                         char      state    ) {

        Vector syncItems = new Vector();

        long fileTimestamp,
             sinceTimestamp = (since == null) ? -1 : roundTime(since.getTime());

        Properties syncDatabase = updateSyncDatabase(principal, sinceTimestamp);

        SyncItem  syncItem      = null;
        String    fileName      = null;
        String    stateString   = null;
        char      fileState           ;
        for (Enumeration e = syncDatabase.keys(); e.hasMoreElements(); ) {
            fileName  = (String)e.nextElement();
            stateString = (String)syncDatabase.get(fileName);
            fileState = stateFromStateString(stateString);
            if ((state == SyncItemState.UNKNOWN) || (fileState == state)) {
                fileTimestamp = lastModifiedFromStateString(stateString);
                if (fileTimestamp > sinceTimestamp ) {
                    syncItem = new SyncItemImpl(this, fileName, fileState);
                    if (isEncode()){
                        syncItem.setProperty(
                            new SyncItemProperty(SyncItem.PROPERTY_BINARY_CONTENT         ,
                                                 Base64.encode(readFileContent(fileName)))
                        );
                        syncItem.setProperty(
                            new SyncItemProperty(SyncItem.PROPERTY_FORMAT,
                                                 SIFSyncSource.FORMAT_BASE64)
                        );
                    } else {
                        syncItem.setProperty(
                            new SyncItemProperty(SyncItem.PROPERTY_BINARY_CONTENT,
                                                 readFileContent(fileName)       )
                        );
                    }
                    syncItem.setProperty(
                        new SyncItemProperty(SyncItem.PROPERTY_TYPE,this.getType())
                    );
                    syncItems.addElement(syncItem);
                }
            }
        }  // next e

        SyncItem[] ret = new SyncItem[syncItems.size()];
        for (int i=0; i<ret.length; ++i) {
            ret[i] = (SyncItem)syncItems.elementAt(i);
        }

        return ret;
    }
}
