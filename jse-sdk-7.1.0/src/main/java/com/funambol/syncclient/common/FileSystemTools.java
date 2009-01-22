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

import java.io.*;
import java.util.Vector;

import com.funambol.syncclient.common.VectorTools;

/**
 * This class supplies some methods of usefullness for the
 * management of files and directories.
 * <p>In particular it supplies methods for the creation and the reading of
 * file and for the removal of a directory and of its everything content
 *
 * @version $Id: FileSystemTools.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class FileSystemTools {

    // -----------------------------------------------------------Public methods

    /**
     * Delete a directory, his subdirectory and all files.
     * @param directoryName the name of the directory to remove.
     * @throws Exception if an error occurs.
     */
    public static void removeDirectoryTree(String directoryName)
            throws Exception {

        File directory = new File(directoryName);

        if (!directory.exists()) {
            return;
        }

        if (!directory.isDirectory()) {
            throw new Exception("'" + directory + "' is not a directory");
        }

        String[] fileList = directory.list();
        int numFile = fileList.length;
        boolean fileDeleted = false;
        File f = null;
        for (int i=0; i<numFile; i++) {
            f = new File(directoryName + File.separator +  fileList[i]);
            if (f.isDirectory()) {
                removeDirectoryTree(f.getPath());
            } else {
                fileDeleted = f.delete();
            }
        }
        fileDeleted = directory.delete();
    }

    /**
     * Create a new file in the specified directory using
     * the given file name and content.
     * <p>If the directory it does not exist it will be created.</p>
     * <p>If already exists a file with the same name, this has deleted before
     * create the new file.</p>
     * @param directoryName the directory in which the file must be created.
     * @param fileName the name of the file
     * @param content the content of the file.
     * @throws Exception if an error occurs during creation.
     */
    public static void createFile(String directoryName,
                                  String fileName, byte[] content)
            throws Exception {

        int size = content.length;

        File fileOut = new File(directoryName +
                File.separator + fileName);

        fileOut.mkdirs();

        if (fileOut.exists()) {
            fileOut.delete();
        }

        FileOutputStream fout = new FileOutputStream(fileOut);

        fout.write(content);
        fout.close();

    }


    /**
     * Create a new file in the specified directory using
     * the given file name and byteStream.
     * <p>If the directory it does not exist it will be created.</p>
     * <p>If already exists a file with the same name, this has deleted before
     * create the new file.</p>
     * @param directoryName the directory in which the file must be created.
     * @param fileName the name of the file
     * @param byteStream the content of the file.
     * @throws Exception if an error occurs during creation.
     */
    public static void createFile(String directoryName,
                                  String fileName, ByteArrayOutputStream byteStream)
            throws Exception {

        File fileOut = new File(directoryName +
                File.separator + fileName);

        fileOut.mkdirs();

        if (fileOut.exists()) {
            fileOut.delete();
        }

        FileOutputStream fout = new FileOutputStream(fileOut);
        byteStream.writeTo(fout);
        fout.close();

    }

    /**
     * Write the given text in the given file appending to or rewriting it
     * accordingly to <i>append</i>.
     *
     * @param file the file to write to - NOT NULL
     * @param text the text to write - NULL, equivalent to blank
     * @param append if true the text is appended to the file, if false
     *               the file is overwritten
     *
     * @throws IOException if an error occurs
     */
    public static void writeTextFile(File file, String text, boolean append)
    throws IOException {
        FileOutputStream fos = null;

        try {
            fos = new FileOutputStream(file.getAbsolutePath(), append);

            if (text != null) {
                fos.write(text.getBytes());
            }

        } finally {
            if (fos != null) {
                fos.close();
            }
        }
    }


    /**
     * Returns the content of the file specified.
     * @param fileName the name of the file.
     * @return the content of the file.
     * @throws IOException if the file not exits or if an I/O error occurs.
     */
    public static byte[] getFile(String fileName) throws IOException {
        File file = new File(fileName);
        int dim = (int)file.length();
        byte[] content = new byte[dim];

        BufferedInputStream stream =
                new BufferedInputStream(new FileInputStream(fileName));
        stream.read(content);
        stream.close();
        return content;
    }

    /**
     * Returns all files in the given directory filtering on the given extension.
     * Pathnames are returned relative to <i>directory</i>
     *
     * @param directory the directory to list
     * @param extension select only the files with this extension
     *
     * @return String[] with the selected filenames. An zero-length array if
     *                  no files are selected
     */
    public static String[] getAllFiles(String  directory,
                                       String  extension) {

        ExtensionFilter filter = new ExtensionFilter(extension);

        Vector fileList = new Vector();

        String[] files = new File(directory).list(filter);

        if ((files == null) || (files.length == 0)) {
            return new String[0];
        }

        File f;
        String[] children;
        for (int i=0; i<files.length; ++i) {
            f = new File(directory, files[i]);
            if (f.isDirectory()) {
                children = getAllFiles(f.getPath(),  extension);
                VectorTools.add(fileList, children);
            }
            //
            // we have to exclude directories
            //
            if (f.isFile()){
                fileList.addElement(f.getPath());
            }
        }

        return VectorTools.toStringArray(fileList);
    }

    /**
     * Writes the given string to the file with the given name
     *
     * @param str the string to write
     * @param file the file name as a java.io.File
     *
     * @throws java.io.IOException
     */
    static public void writeFile(String str, File file)
    throws IOException {
        writeFile(str.getBytes(), file);
    }

    /**
     * Writes the given string to the file with the given name
     *
     * @param str the string to write
     * @param filename the file name as a java.lang.String
     *
     * @throws java.io.IOException
     */
    static public void writeFile(String str, String filename)
    throws IOException {
        writeFile(str.getBytes(), new File(filename));
    }

    /**
     * Writes the given bytes to the file with the given name
     *
     * @param buf the bytes to write
     * @param filename the file name as a java.lang.String
     *
     * @throws java.io.IOException
     */
    static public void writeFile(byte[] buf, String filename)
    throws IOException {
        writeFile(buf, new File(filename));
    }

    /**
     * Writes the given bytes to the file with the given name
     *
     * @param buf the bytes to write
     * @param file the file name as a java.io.File
     *
     * @throws java.io.IOException
     */
    static public void writeFile(byte[] buf, File file)
    throws IOException {
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(file);
            fos.write(buf);
            fos.close();
        } finally {
            if (fos != null) {
                fos.close();
            }
        }
    }

    /**
     * Reads a file into a byte array given its filename
     *
     * @param file the filename (as java.io.File)
     *
     * @return the content of the file as a byte array
     *
     * @throws java.io.IOException;
     */
    static public byte[] readFileBytes(File file)
    throws IOException {
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

        return buf;
    }


    /**
     * Reads a file into a byte array given its filename
     *
     * @param filename the filename (as java.lang.String)
     *
     * @return the content of the file as a byte array
     *
     * @throws java.io.IOException;
     */
    static public byte[] readFileBytes(String filename)
    throws IOException {
        return readFileBytes(new File(filename));
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
    static public String readFileString(File file)
    throws IOException {
        return new String(readFileBytes(file));
    }

    /**
     * Reads a file into a String given its filename
     *
     * @param filename the filename (as java.lang.String)
     *
     * @return the content of the file as a string
     *
     * @throws java.io.IOException;
     */
    static public String readFileString(String filename)
    throws IOException {
        return readFileString(new File(filename));
    }

    /**
     * Reads a file into a String given an input stream. It is responsibility
     * of the caller to close the stream when not used anymore.
     *
     * @param is the InputStream to read
     *
     * @return the content of the stream
     *
     * @throws java.io.IOException;
     */
    static public String readFileString(InputStream is)
    throws IOException {
        StringBuffer sb = new StringBuffer();

        byte[] buf = new byte[512]; int n = 0;
        while ((n = is.read(buf)) >= 0) {
            sb.append(new String(buf, 0, n));
        }
        return sb.toString();
    }


}
