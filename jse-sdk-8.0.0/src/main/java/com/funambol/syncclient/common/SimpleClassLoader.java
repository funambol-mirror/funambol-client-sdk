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

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import com.funambol.syncclient.common.logging.Logger;

/**
 * Simple classLoader for the loading of java class.
 * Searches the classes in a given directory, in its subdirectory and
 * in all jar files contained in the given directory and, recursively,
 * in its subdirectory.
 *
 * @version  $Id: SimpleClassLoader.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 *
 *
 */

public class SimpleClassLoader extends ClassLoader {

    // -------------------------------------------------------------Private data

    /** This is the directory in which searching the java class */
    private String workingDirectory = null;

    private Logger logger = new Logger();

    // -------------------------------------------------------------Constructors

    /**
     * Creates a new SimpleClassLoader with the given
     * <code>workingDirectory</code>.
     * @param workingDirectory directory in which the class
     * loader searches the java class.
     */
    public SimpleClassLoader(String workingDirectory) {
        File file = new File(workingDirectory);
        this.workingDirectory = workingDirectory;

        if (logger.isLoggable(Logger.INFO)) {
                logger.info("WorkingDirectory classloader: " +
                             file.getAbsolutePath()           );

        }

    }

    // -----------------------------------------------------------Public methods


    public String getWorkingDirectory() {
        return this.workingDirectory;
    }

    //---------------------------------------
    //Overrides java.lang.ClassLoader method
    //---------------------------------------

    public Class loadClass(String name, boolean resolve) throws ClassNotFoundException {

       byte[] b = null;

       Class classLoaded = null;

       try {
           b = loadClassData(name);
       } catch (Exception ex) {
           throw new ClassNotFoundException("Class " + name + " not found: " + ex);
       }

       if (b==null) {
           classLoaded = findSystemClass(name);
       } else {
           classLoaded = defineClass(name, b, 0, b.length);
       }

        return classLoaded;
    }

    // ----------------------------------------------------------Private methods

    /**
     * Returns the byte array of the java class searched.
     * Searches for the class in <i>workingDirectory</i>
     *
     * @param name the name of the class.
     * @return the byte array of the class. <code>null</code> if
     * the class could not be found.
     * @throw Exception if the <i>name</i> does not represent a
     * valid java class name.
     */
    private byte[] loadClassData(String name) throws Exception {
        byte[] content = null;

        int index = name.lastIndexOf(".");

        if (index == -1) {
            throw new Exception("Java class name not recognized");
        }

        String sPackage = name.substring(0, index);   // com.funambol.sl
        name = name.substring(index+1, name.length());   // Setup

        // Searches the class in the directory
        content = loadInDirectory(workingDirectory, sPackage, name);

        return content;
    }

    /**
     * Searches the java class in the given directory
     * @param dirName the directory name in which searching the java class
     * @param packageClass package of the class to search
     * @param javaClassName name of the class to search
     * @return the byte array of the class. <code>null</code> if
     * the class could not be found.
     *
     * @throws IOException
     */
    private byte[] loadInDirectory(String dirName,
                                   String packageName, String javaClassName)
    throws IOException {
        byte[] content = null;

        // decomposes the package in directory
        StringTokenizer stPackage = new StringTokenizer(packageName, ".");

        while (stPackage.hasMoreTokens()) {
            dirName = dirName + File.separator + stPackage.nextToken();
        }

        // verify if the .class file exists
        File fileClass = new File(dirName + File.separator
                                  + javaClassName + ".class");
        if (fileClass.isFile()) {

            if (logger.isLoggable(Logger.INFO)) {
                logger.info("Class found in: " + dirName);
            }

            int size = (int)fileClass.length();
            content = new byte[size];

            FileInputStream fIstream = new FileInputStream(fileClass);
            fIstream.read(content);
            fIstream.close();
        }

        return content;
    }

}
