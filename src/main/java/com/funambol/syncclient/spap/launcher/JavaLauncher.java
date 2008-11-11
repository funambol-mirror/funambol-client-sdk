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
package com.funambol.syncclient.spap.launcher;

import java.io.*;
import java.security.*;
import java.lang.reflect.*;
import java.lang.reflect.Method;

import com.funambol.syncclient.common.logging.Logger;
import com.funambol.syncclient.spap.Asset;
import com.funambol.syncclient.spap.AssetInstallationException;
import com.funambol.syncclient.spap.installer.InstallationContext;

/**
 * This class represents a <code>Laucher</code> for the execution of java class.
 * It uses the <code>SimpleClassLoader</code> in order to load the class and
 * executes the method <i>main(String[] args)</i>
 *
 * @see com.funambol.syncclient.common.SimpleClassLoader
 *
 * @version  $Id: JavaLauncher.java,v 1.3 2007-12-22 18:09:18 nichele Exp $
 */
public class JavaLauncher implements Launcher {

    // ---------------------------------------------------------- Private Data

    // This is the security manager used to prevent the Sistem.exit call
    private SimpleSecurityManager simpleSecurityManager = null;


    Logger logger = new Logger();

    // ---------------------------------------------------------- Constructor
    public JavaLauncher() {
        simpleSecurityManager = new SimpleSecurityManager();
    }

    // ---------------------------------------------------------- Public methods

    /**
     * Executes the class with given name.
     *
     * @param programName the program to execute.
     * @param install <code>true</code> if the program is the installation program,
     *  <code>false</code> if the program is the uninstallation program
     * @param ctx installation context information
     *
     * @return Returns the exit code of the program. Exit code is the value returned of the method
     *  called or, if the method calls System.exit(code), is the code used in the exit method.
     *
     *
     * @throws AssetInstallationException if the java class is not found or an error occurs during execution.
     */
    public int execute(String programName, boolean install, InstallationContext ctx)
    throws AssetInstallationException {
        Integer exitState = null;
        SecurityManager originalSecurityManager = System.getSecurityManager();

        String workingDirectory = ctx.getWorkingDirectory();


        try {
            int indexClass = programName.lastIndexOf(".class");

            programName = programName.substring(0, indexClass);

            String msgLog = "JavaLauncher - Execute " +
                            programName               +
                            " in "                    +
                            workingDirectory          ;

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug(msgLog);
            }

            String methodName = null;

            // use ClassLoader in order to load the class
            ClassLoader cl = this.getClass().getClassLoader();

            Class programClass = cl.loadClass(programName);

            if (install) {
                // launch install method
                methodName = "install";

            } else  {
                // launch uninstall method
                methodName = "uninstall";
            }

            // call method install(InstallationContext)
            Class[] arg = { InstallationContext.class };
            Method method = programClass.getMethod(methodName, arg);
            Object[] objArg = { ctx };

            System.setSecurityManager(simpleSecurityManager);

            exitState = (Integer)method.invoke(null, objArg);
        } catch (InvocationTargetException ex) {

            Throwable cause = ex.getTargetException();

            if (cause instanceof SimpleSecurityManagerException) {
                cause.printStackTrace();
                // the program has called System.exit
                exitState = new Integer( ((SimpleSecurityManagerException)cause).getExitCode() );
            } else if (cause instanceof AssetInstallationException) {
                throw (AssetInstallationException)cause;
            } else {
                // the program has generated a exception
                throw new AssetInstallationException("Error in launcher", cause);
            }

        } catch (Exception e) {
            throw new AssetInstallationException("Error in launcher: " + e, e);
        } finally {
            // re-set the originale security manager
            System.setSecurityManager(originalSecurityManager);
        }

        // the program has returned a null values
        if (exitState == null) {

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("Java class executed with error. Return state null (--> -1)!! ");
            }

            return -1;
        }

        if (logger.isLoggable(Logger.DEBUG)) {
            logger.debug("Java class executed. Return state: " + exitState.intValue());
        }
        return exitState.intValue();
    }
}


/**
 *
 * It's a simple security manager used for catch the <code>System.exit(int)</code> call.
 *
 */
class SimpleSecurityManager extends SecurityManager  {

    private final SecurityManager parent = System.getSecurityManager();

    public void checkPermission(Permission perm)  {
        if (parent != null) {
            parent.checkPermission(perm);
        }
    }

    public void checkExit(int code) {
        throw new SimpleSecurityManagerException("System.exit not allowed", code);
    }

}

/**
 *
 * It's a simple exception throw from SimpleSecurityManager if the
 * method System.exit is called
 *
 */
class SimpleSecurityManagerException extends SecurityException  {

    private int exitCode = -1;

    public SimpleSecurityManagerException(String message, int code) {
        super(message);
        exitCode = code;
    }

    public int getExitCode() {
        return exitCode;
    }

}
