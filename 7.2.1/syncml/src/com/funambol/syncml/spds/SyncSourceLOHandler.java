/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2008 Funambol, Inc.
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
package com.funambol.syncml.spds;

import java.io.UnsupportedEncodingException;
import java.io.IOException;
import java.io.ByteArrayOutputStream;

import com.funambol.util.Base64;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;
import com.funambol.util.XmlUtil;
import com.funambol.syncml.protocol.SyncMLStatus;
import com.funambol.util.SyncListener;

/**
 */
public class SyncSourceLOHandler {

    private SyncSource             source;
    private int                    maxMsgSize;
    private SyncItem               nextAddItem     = null;
    private SyncItem               nextReplaceItem = null;
    private SyncItem               nextDeleteItem  = null;
    private SyncItem               nextItem        = null;
    private ByteArrayOutputStream  os              = null;
    private SyncItem               lo              = null;

    public SyncSourceLOHandler(SyncSource source, int maxMsgSize) {
        this.source     = source;
        this.maxMsgSize = maxMsgSize;
    }

    public int addItem(SyncItem item) throws SyncException {
        Log.trace("[SyncSourceLOHanlder.addItem] " + item.getKey());
        if (source.getConfig().getSupportsLO()) {
            return source.addItem(item);
        } else {
            // The sync source does not support LO
            if (item.hasMoreData()) {
                // This is a chunk of data, append it in memory
                if (os == null) {
                    os = new ByteArrayOutputStream();
                    lo = item;
                }
                try {
                    updateItemContent(item);
                } catch (Exception e) {
                    Log.error("Cannot write LO buffer");
                    throw new SyncException(SyncException.CLIENT_ERROR,
                            "Cannot write LO buffer " + e.toString());
                }
                return SyncMLStatus.SUCCESS;
            } else {
                // This may be the last chunk of a lo, or a small object
                if (os != null) {
                    try {
                        updateItemContent(item);
                    } catch (Exception e) {
                        Log.error("Cannot write LO buffer");
                        throw new SyncException(SyncException.CLIENT_ERROR,
                                "Cannot write LO buffer " + e.toString());
                    }
                    // Now build the re-assembled item
                    item = new SyncItem(lo);
                    byte data[] = os.toByteArray();
                    if (source.ENCODING_B64.equals(source.getEncoding())) {
                        data = Base64.encode(data);
                    }
                    item.setContent(data);
                    try {
                        os.close();
                    } catch (IOException ioe) {
                        // Even in case of error we continue
                        Log.error("Error closing output stream " + ioe.toString());
                    }
                    os = null;
                }
                return source.addItem(item);
            }
        }
    }

    public int updateItem(SyncItem item) {
        Log.trace("[SyncSourceLOHanlder.updateItem] " + item.getKey());
        if (source.getConfig().getSupportsLO()) {
            return source.updateItem(item);
        } else {
            // The sync source does not support LO
            if (item.hasMoreData()) {
                // This is a chunk of data, append it in memory
                if (os == null) {
                    os = new ByteArrayOutputStream();
                    lo = item;
                }

                try {
                    updateItemContent(item);
                } catch (Exception e) {
                    Log.error("Cannot write LO buffer");
                    throw new SyncException(SyncException.CLIENT_ERROR,
                                       "Cannot write LO buffer " + e.toString());
                }
                return SyncMLStatus.SUCCESS;
            } else {
                // This may be the last chunk of a lo, or a small object
                if (os != null) {
                    try {
                        updateItemContent(item);
                    } catch (Exception e) {
                        Log.error("Cannot write LO buffer");
                        throw new SyncException(SyncException.CLIENT_ERROR,
                                "Cannot write LO buffer " + e.toString());
                    }
                    // Now build the re-assembled item
                    item = new SyncItem(lo);
                    byte data[] = os.toByteArray();
                    if (source.ENCODING_B64.equals(source.getEncoding())) {
                        data = Base64.encode(data);
                    }
                    item.setContent(data);
                    try {
                        os.close();
                    } catch (IOException ioe) {
                        // Even in case of error we continue
                        Log.error("Error closing output stream " + ioe.toString());
                    }
                    os = null;
                }
                return source.updateItem(item);
            }
        }
    }


    /**
     * This method returns the Add command tag.
     */
    public boolean getAddCommand(int size, SyncListener listener,
                                 StringBuffer cmdTag, CmdId cmdId) throws SyncException {

        Log.trace("SyncSourceHandler.getAddCommand]");
        SyncItem item = null;
        if (nextAddItem == null) {
            item = source.getNextNewItem();
            // No item for this source
            if (item == null) {
                return true;
            }
        } else {
            item = nextAddItem;
            nextAddItem = null;
        }

        String itemContent = getItemTag(item);

        if (size + itemContent.length() < maxMsgSize) {
            // Build Add command
            cmdTag.append("<Add>\n").append("<CmdID>" + cmdId.next() + "</CmdID>\n");
        } else {
            // If the item does not fit in the max msg size then we shall drop
            // it but for backward compatibility we let it go through

            // Build Add command
            cmdTag.append("<Add>\n").append("<CmdID>" + cmdId.next() + "</CmdID>\n");

            Log.error(source.getName() + 
                   " returned an item that exceeds max msg size and should be dropped");
        }

        boolean done = false;
        do {
            cmdTag.append(itemContent);
            // Notify the listener
            listener.itemAddSent(item);

            // Ask the source for next item
            item = source.getNextNewItem();

            // Last new item found
            if (item == null) {
                done = true; 
                break;
            }

            itemContent = getItemTag(item);

        } while (size + cmdTag.length() + itemContent.length() < maxMsgSize);

        if (item != null) {
            // If we get here then we reached the max msg size, so
            // we store the next msg for the next message
            nextAddItem = item;
        }

        cmdTag.append("</Add>\n");

        return done;
    }


    /**
     * This method returns the Replace command tag.
     */
    public boolean getReplaceCommand(int size, SyncListener listener,
                                     StringBuffer cmdTag, CmdId cmdId) throws SyncException {

        Log.trace("SyncSourceHandler.getReplaceCommand]");
        SyncItem item = null;
        if (nextReplaceItem == null) {
            item = source.getNextUpdatedItem();
            // No item for this source
            if (item == null) {
                return true;
            }
        } else {
            item = nextReplaceItem;
            nextReplaceItem = null;
        }

        String itemContent = getItemTag(item);
        if (size + itemContent.length() < maxMsgSize) {
            // Build Replace command
            cmdTag.append("<Replace>\n").append("<CmdID>").append(cmdId.next()).append("</CmdID>\n");
        } else {
            // If the item does not fit in the max msg size then we shall drop
            // it but for backward compatibility we let it go through

            // Build Replace command
            cmdTag.append("<Replace>\n").append("<CmdID>").append(cmdId.next()).append("</CmdID>\n");

            Log.error(source.getName() + 
                   " returned an item that exceeds max msg size and should be dropped");
        }

        boolean done = false;
        do {
            cmdTag.append(itemContent);
            // Notify the listener
            listener.itemReplaceSent(item);

            // Ask the source for next item
            item = source.getNextUpdatedItem();

            // Last item found
            if (item == null) {
                done = true;
                break;
            }
            itemContent = getItemTag(item);
        } while (size + cmdTag.length() + itemContent.length() < maxMsgSize);

        if (item != null) {
            // If we get here then we reached the max msg size, so
            // we store the next msg for the next message
            nextReplaceItem = item;
        }

        cmdTag.append("</Replace>\n");

        return done;
    }

    /**
     * This method returns the Delete command tag.
     */
    public boolean getDeleteCommand(int size, SyncListener listener,
                                    StringBuffer cmdTag, CmdId cmdId)
    throws SyncException {

        Log.trace("SyncSourceHandler.getDeleteCommand]");
        SyncItem item = null;
        if (nextDeleteItem == null) {
            item = source.getNextDeletedItem();
            // No item for this source
            if (item == null) {
                return true;
            }
        } else {
            item = nextDeleteItem;
            nextDeleteItem = null;
        }

        String itemContent = getItemTag(item);
        if (size + itemContent.length() < maxMsgSize) {
            // Build Replace command
            cmdTag.append("<Delete>\n").append("<CmdID>").append(cmdId.next()).append("</CmdID>\n");
        } else {
            // If the item does not fit in the max msg size then we shall drop
            // it but for backward compatibility we let it go through

            // Build Replace command
            cmdTag.append("<Delete>\n").append("<CmdID>").append(cmdId.next()).append("</CmdID>\n");

            Log.error(source.getName() + 
                   " returned an item that exceeds max msg size and should be dropped");
        }

        // Build Delete command
        boolean done = false;
        do {
            cmdTag.append(itemContent);
            // Notify the listener
            listener.itemDeleteSent(item);

            // Ask the source for next item
            item = source.getNextDeletedItem();

            // Last item found
            if (item == null) {
                done = true;
                break;
            }
            itemContent = getItemTag(item);
        } while (size + cmdTag.length() + itemContent.length() < maxMsgSize);

        if (item != null) {
            // If we get here then we reached the max msg size, so
            // we store the next msg for the next message
            nextDeleteItem = item;
        }

        cmdTag.append("</Delete>\n");

        return done;
    }

    public boolean getNextCommand(int size, SyncListener listener,
                                  StringBuffer cmdTag, CmdId cmdId)
    throws SyncException {

        SyncItem item = null;
        if (nextItem == null) {
            item = source.getNextItem();
            // No item for this source
            if (item == null) {
                return true;
            }
        } else {
            item = nextItem;
            nextItem = null;
        }

        String itemContent = getItemTag(item);
        if (size + itemContent.length() < maxMsgSize) {
            // Build Replace command
            cmdTag.append("<Replace>\n").append("<CmdID>").append(cmdId.next()).append("</CmdID>\n");
        } else {
            // If the item does not fit in the max msg size then we shall drop
            // it but for backward compatibility we let it go through

            // Build Replace command
            cmdTag.append("<Replace>\n").append("<CmdID>").append(cmdId.next()).append("</CmdID>\n");

            Log.error(source.getName() + 
                   " returned an item that exceeds max msg size and should be dropped");
        }

        boolean done = false;
        do {
            cmdTag.append(itemContent);
            // Notify the listener
            listener.itemReplaceSent(item);

            // Ask the source for next item
            item = source.getNextItem();

            // Last item found
            if (item == null) {
                done = true;
                break;
            }
            itemContent = getItemTag(item);
        } while (size + cmdTag.length() + itemContent.length() < maxMsgSize);

        if (item != null) {
            // If we get here then we reached the max msg size, so
            // we store the next msg for the next message
            nextItem = item;
        }

        cmdTag.append("</Replace>\n");

        return done;
    }

    /**
     * Encode the item data according to the format specified by the SyncSource.
     *
     * @param formats the list of requested encodings (des, 3des, b64)
     * @param data the byte array of data to encode
     * @return the encoded byte array, or <code>null</code> in case of error
     */
    private byte[] encodeItemData(String[] formats, byte[] data) {

        if (formats != null && data != null) {
            // If ecryption types are specified, apply them
            for (int count = formats.length - 1; count >= 0; count--) {

                String encoding = formats[count];

                if (encoding.equals("b64")) {
                    data = Base64.encode(data);
                }
            /*
            else if (encoding.equals("des")) {
            // DES not supported now, ignore SyncSource encoding
            }
            else if (currentDecodeType.equals("3des")) {
            // 3DES not supported now, ignore SyncSource encoding
            }
             */
            }
        }
        return data;
    }

    /**
     * Get the format string to add to the outgoing message.
     *
     * @return the Format string, according to the source encoding
     */
    private String getFormat() {
        // Get the Format tag from the SyncSource encoding.
        if (!source.getEncoding().equals(source.ENCODING_NONE)) {
            return "<Format xmlns=\'syncml:metinf\'>" + source.getEncoding() + "</Format>\n";
        } else {
            return "";
        }
    }

    private void updateItemContent(SyncItem chunk) throws IOException {

        // We must handle b64 encoding/decoding issues related to
        // chunking. The solution implemented is rather simple. For LO
        // we decode and store the plain data and encode at the very
        // end. This is not very efficient as we do redundant b64
        // encoding/decoding, but this is a corner case. A sync source
        // that handles LO shall implement its own mechanism and not
        // rely on the default one.
        byte data[] = chunk.getContent();
        if (source.ENCODING_B64.equals(source.getEncoding())) {
            data = Base64.decode(data);
        }

        os.write(data);
    }

    /**
     * This method formats the Item tag.
     */
    private String getItemTag(SyncItem item) {

        
        StringBuffer ret = new StringBuffer();
        
        ret.append("<Item>\n");
         
        switch (item.getState()) {

            case SyncItem.STATE_DELETED:
                return ret.append("<Source><LocURI>").append(item.getKey())
                          .append("</LocURI></Source>\n").append("</Item>\n").toString();

            case SyncItem.STATE_UPDATED:
            case SyncItem.STATE_NEW:
                Log.info("The encoding method is [" + source.getEncoding() + "]");
                String encodedData = null;

                if (item.getContent() == null) {
                    Log.error("Empty content from SyncSource for item:" +
                            item.getKey());
                    encodedData = "";
                } else if (!source.getEncoding().equals(source.ENCODING_NONE)) {
                    String[] formatList = StringUtil.split(
                            source.getEncoding(), ";");
                    byte[] data = encodeItemData(formatList, item.getContent());

                    encodedData = new String(data);
                } else {
                    // Else, the data is text/plain,
                    // and the XML special chars are escaped.
                    String content = new String(item.getContent());
                    encodedData = XmlUtil.escapeXml(content);
                }

                
                // type
                String theType = item.getType() == null ? source.getType() : item.getType();
                
                ret.append("<Meta><Type xmlns=\"syncml:metinf\">")
                   .append(theType)
                   .append("</Type>")
                   .append(getFormat());

                // If this item has a declared size then we must specify it in
                // the meta data
                long declaredSize = item.getLODeclaredSize();
                if (declaredSize != -1) {
                    ret.append("<Size>").append(declaredSize).append("</Size>");
                }

                ret.append("</Meta>\n");
                
                // source
                ret.append(
                        "<Source><LocURI>" + item.getKey() + "</LocURI></Source>\n");
                
                //parent
                if (item.getParent() != null) {
                    ret.append("<SourceParent><LocURI>").append(item.getParent()).append("</LocURI></SourceParent>\n");
                }
                //item data
                ret.append("<Data>").append(encodedData).append("</Data>\n");

                // More data flag
                if (item.hasMoreData()) {
                    ret.append("<MoreData/>\n");
                }
                ret.append("</Item>\n");

               
                return ret.toString();

            default:
                // Should never happen
                Log.error("[getItemTag] Invalid item state: " + item.getState());
                // Go on without sending this item
                return "";

        }// end switch
    }


}

