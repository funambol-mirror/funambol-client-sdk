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
package com.funambol.syncclient.common;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

/**
 * This class supplies some methods of usefullness for the
 * management of zip file.
 * <p>In particular it supplies methods for the check and for
 * the extraction of zip file
 *
 * @version $Id: ZipTools.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 *
 */
public class ZipTools {


    // ---------------------------------------------------------- Public methods
    /**
     * Extract the zip file rappresented by the given byte array.
     * @param workingDirectory the directory in which the file
     * should be extracted
     * @param zipFile the byte array that represents the zip
     * @throws Exception if error occurs or if the zip file is not valid
     */
    public static void extract(String workingDirectory, byte[] zipFile)
    throws Exception {

        ByteArrayInputStream byteStream = new ByteArrayInputStream(zipFile);
        ByteArrayOutputStream out = new ByteArrayOutputStream();

        ZipInputStream zipStream = new ZipInputStream(byteStream);
        ZipEntry zipEntry = null;

        String nameZipEntry = null;

        byte[] contentZipEntry = null;
        boolean isDirectory = false;

        int indexFileSeparator = -1;
        String directory = null;
        String fileName =  null;

        while ( (zipEntry = zipStream.getNextEntry()) != null ) {
            nameZipEntry = workingDirectory + File.separator + zipEntry.getName();

            isDirectory = zipEntry.isDirectory();

            if (isDirectory) {
                File file = new File(nameZipEntry);
                file.mkdirs();
            } else {

                // read zipEntry
                byte[] buf = new byte[1024];
                int c = 0;

                while ( (c = zipStream.read(buf)) != -1) {
                    out.write(buf, 0, c);
                }

                indexFileSeparator = nameZipEntry.lastIndexOf(File.separator);
                directory = nameZipEntry.substring(0, indexFileSeparator);
                fileName =  nameZipEntry.substring(indexFileSeparator+1,nameZipEntry.length());

                FileSystemTools.createFile(directory, fileName, out);

                out.reset();
                zipStream.closeEntry();
            }
        }
        zipStream.close();
        byteStream.close();
    }

    /**
     * Verify if the given byte array represents a valid zip file.
     * Reads all the content of the zip file but it does not save it
     * @param zipFile the byte array to verify
     * @throws IOException if the byte array does not represent a valid zip file
     */
    public static void verifyZip(byte[] zipFile) throws IOException {

        boolean isValidZipFile = true;

        ByteArrayInputStream bStream = new ByteArrayInputStream(zipFile);

        ZipInputStream zipStream = new ZipInputStream(bStream);

        ZipEntry zipEntry = null;

        String nameZipEntry = null;
        int dimZipEntry = 0;
        byte[] contentZipEntry = null;
        boolean isDirectory = false;

        while ( (zipEntry = zipStream.getNextEntry()) != null ) {

            isDirectory = zipEntry.isDirectory();

            if (!isDirectory) {
                // read zipEntry
                int c = -1;
                byte[] buf = new byte[1024];
                while ( (c = zipStream.read(buf)) != -1) {
                    dimZipEntry = dimZipEntry + c;
                }
                dimZipEntry = 0;
                zipStream.closeEntry();
            }
        }
        zipStream.close();
        bStream.close();
    }
}
