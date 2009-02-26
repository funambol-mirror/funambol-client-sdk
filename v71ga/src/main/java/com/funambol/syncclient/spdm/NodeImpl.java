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



package com.funambol.syncclient.spdm;

import java.util.Hashtable;
import java.util.Properties;
import java.util.Enumeration;
import java.util.StringTokenizer;

import java.io.*;
import java.lang.reflect.Method;

import com.funambol.syncclient.common.logging.Logger;


/**
 * This is an implementation of <i>ManagamentNode</i> that uses file system
 * directories for contexts and properties file for leaf nodes.
 *
 * $Id: NodeImpl.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class NodeImpl implements ManagementNode {
    //----------------------------------------------------------------- Costants
    private static final String PROPERTY_CLASS_NAME = "className";

    // ------------------------------------------------------------ Private data

    private String context        = null;
    private String fullContext    = null;
    private ManagementNode parent = null;

    //
    // save last file properties values
    //
    Hashtable propertiesValues = null;

    //
    // save lastModified about last read file properties
    //
    long lastModified = 0;

    // ------------------------------------------------------------ Constructors

    /**
     * Creates a <i>NodeImpl</i> object for the given management context
     * and parent node.
     *
     * @param parent the parent context; if null, create a root node
     * @param context the context
     *
     */
    public NodeImpl (ManagementNode parent, String context) {
        this.parent      = parent;
        this.context     = context;

        if (parent != null) {
            String fc = parent.getFullContext();
            this.fullContext =  fc
                             + ((fc.endsWith("/")) ? "" : "/")
                             + context;
        } else {
            this.fullContext = context;
        }
    }

    //----------------------------------------------------------- Public methods

    /** Returns the node's context
     *
     * @return the node's context
     *
     */
    public String getContext() {
        return this.context;
    }

    /** Returns the entire node's context path (concatenation of all parent
     * context paths).
     *
     * @return the entire node's context path
     *
     */
    public String getFullContext() {
        return this.fullContext;
    }


    /**
     *
     * @return value
     *
     */
    public Object getValue(String name) throws DMException {
        Hashtable values = this.getValues();
        return values.get((Object) name);
    }

    /**
     *
     * @return values
     *
     */
    public Hashtable getValues()
    throws DMException{
        try {

            File f = (parent != null)
                   ? new File(parent.getFullContext(), context + ".properties")
                   : new File(context + ".properties")
                   ;

            long filePropertiesLastModified = f.lastModified();

            //
            // load file properties only filePropertiesLastModified is changed
            //
            if (lastModified != filePropertiesLastModified) {

                propertiesValues = this.loadProperties(f);

                lastModified = filePropertiesLastModified;

            }

            return propertiesValues;

        } catch (IOException e){
            throw new DMException("Error loading values for the context "
                                 + fullContext
                                 + ": "
                                 + e.getMessage()
                                 , e);
        }
    }

    /**
     * Provides direct access to a management subtree relative to the node's
     * fullContext.
     *
     * @param node the name of the subnode (it may contain directories)
     *
     * @return the node values
     *
     */
    public Hashtable getNodeValues(String node)
    throws DMException {
        try {
            return loadProperties(new File(fullContext, node + ".properties"));
        } catch (IOException e) {
            throw new DMException( "Error reading the management node values in " +
                                   fullContext                                    +
                                   '/'                                            +
                                   node, e);
        }
    }

   /**
     * Retrieves a value from the given management subnode name.
     *
     * @param node the subnode containing the config value specified by name
     * @param name the name of the configuration value to return
     *
     * @return the node value
     *
     * @throws DMException in case of errors.
     */
    public Object getNodeValue(String node, String name)
    throws DMException {
        Hashtable values = getNodeValues(node);

        return values.get(name);
    }

    /**
     * Reads the properties of the given node and creates an object with the
     * values read.
     * @param node the subnode containing the required config values
     * @return the object
     * @throws DMException in case of errors.
     */
     public Object getManagementObject(String node) throws DMException {
         Object objectToReturn = null;

         Hashtable properties = getNodeValues(node);
         String className = (String)properties.get(PROPERTY_CLASS_NAME);

         try {
             objectToReturn = (Class.forName(className)).newInstance();
         } catch (Exception e) {

             String msg = "Error loading class " +
                          className +
                          ": " +
                          e.getMessage();

             throw new DMException(msg);
         }

         // setting all properties
         Enumeration e = properties.keys();

         String key = null;
         while (e.hasMoreElements()) {
             key = (String)e.nextElement();
             if (!PROPERTY_CLASS_NAME.equals(key)) {
                 setProperty(objectToReturn, key, (String)properties.get(key));
             }
         }

         return objectToReturn;
     }

    /**
     * Retrieves the children subnodes of the current node. Subnodes may be
     * directories or property files. In the latter case, the node context is
     * the name of the file without the extension.
     *
     * @return an array of <i>ManagementNode</i> containing this node's children
     *
     * @throws DMException in case of errors.
     */
    public ManagementNode[] getChildren() throws DMException{
        File f = new File(fullContext);

        String[] children = f.list();

        if ((children == null) || (children.length==0)) {
            return new ManagementNode[0];
        }

        ManagementNode[] nodes = new ManagementNode[children.length];
        for (int i=0; i<children.length; ++i){
            nodes[i] = new NodeImpl(this, removeExtension(children[i]));
        }

        return nodes;
    }

    /**
     * Retrieves the subnode with the given name
     *
     * @return subcontext the subnode context (name)
     *
     * @throws DMException in case of errors.
     */
    public ManagementNode getChildNode(String subcontext)
    throws DMException {
        if ((!new File(fullContext, subcontext).exists() && !new File(fullContext, subcontext+".properties").exists())) {
            throw new DMException("The sub context "
                                  + subcontext
                                  + " does not exist in context "
                                  + fullContext
                                  );
        }

        return new NodeImpl(this, subcontext);
    }

    /**
     * Retrieves this node's parent.
     *
     * @return the node's parent
     *
     * @throws DMException in case of errors.
     */
    public ManagementNode getParent(){
        return this.parent;
    }

    /**
     * Sets a node value as a whole object.
     *
     * @param name
     * @param value the value
     *
     * @throws DMException in case of errors.
     */
    public void setValue(String name, Object value) throws DMException {

        File file = new File(fullContext + ".properties");
        setValue(file, "", name, value);
    }

    /**
     * Sets a subnode specific value.
     *
     * @param node subnode to set
     * @param name configuration parameter to set
     * @param value
     *
     * @throws DMException in case of errors.
     */
    public void setValue(String node, String name, Object value)
    throws DMException {

       File file = new File(fullContext, node + ".properties");
       setValue(file, node, name, value);
    }


    /**
     * Remove subnode from configuration three.
     *
     * @param node subnode to reomove
     *
     * @throws DMException in case of errors.
     */
    public void removeNode(String node)
    throws DMException {

        Properties values = null;

        File context = new File(fullContext, node);
        File file    = new File(fullContext, node + ".properties");

        if (file.exists()) {
            file.delete();
        }

        emptyTree(context);
    }

    // ------------------------------------------------------------ Private methods

   /**
    * Sets a subnode specific value
    *
    * @param file file to set
    * @param name configuration parameter to set
    * @param value
    *
    * @throws DMException in case of errors.
    */
   private void setValue(File file, String node, String name, Object value)
        throws DMException {

       Properties values = null;

       //creates a new, empty file named by this abstract pathname
       //if and only if a file with this name does not yet exist
       if(!file.exists()) {
           String dir = file.getParent();

           try {
               if (dir != null) {
                   new File(dir).mkdirs();
               }
           } catch (Exception e) {
               throw new DMException( "Error creating the management node "
                                    +
                                    + '/'
                                    + node + " " + e
                                    , e);
           }
           values = new Properties();
       } else {
           try {
               values = loadProperties(file);
           } catch (IOException e) {
               throw new DMException("Error reading the management node values in " +
                                     fullContext                                    +
                                     '/'                                            +
                                     node, e                                        );
           }
       }

       values.put(name, value);

       try {
           saveProperties(file, values);
       } catch  (IOException e) {
           throw new DMException("Error writing the management node values in " +
                                 fullContext                                    +
                                 '/'                                            +
                                 node, e);
       }
    }

    private Properties loadProperties(File file)
    throws IOException {
        FileInputStream fis = null;
        Properties prop = new Properties();

        try {
            fis = new FileInputStream(file);
            prop.load(fis);
            return prop;
        } finally {
            if (fis != null) try {
                fis.close();
            } catch (IOException e) {}
        }

    }

    private void saveProperties(File file, Properties values)
    throws IOException {
        FileOutputStream fos = null;

        try {
            fos = new FileOutputStream(file);
            values.save(fos, "");
        } finally {
            if (fos != null) try {
                fos.close();
            } catch (IOException e) {}
        }
    }

    private String removeExtension(String context) {
        int p = context.lastIndexOf('.');
        if (p < 0) {
            return context;
        }

        if (p == 0) {
            return "";
        }

        return context.substring(0, p);
    }


    /**
     * Remove files and directories in a folder
     *
     * @param dir folder to empty
     *
     */
    private void emptyTree(File dir) {
        String[] files = dir.list();

        if (files == null) {
            return;
        }

        File f;
        for (int i=0; i < files.length;i++) {
            f = new File(dir, files[i]);
            if (f.isDirectory()) {
                emptyTree(f);
            }
            f.delete();
        }

        dir.delete();
    }

    private void setProperty(Object obj, String key, String value) {
        String methodName = "";

        char firstLetter = key.toUpperCase().charAt(0);
        if (key.length() > 1) {
            methodName = key.substring(1);
        }

        methodName = "set" + firstLetter + methodName;

        Class objClass = obj.getClass();

        try {
            Method m = objClass.getMethod(methodName, new Class[] { String.class });
            m.invoke(obj, new String[] {value});
        } catch (Exception e) {
            String msg = "Property "                 +
                          key                        +
                          " not set to "             +
                          value                      +
                          ". Method "                +
                          methodName                 +
                          "(String s) not found in " +
                          objClass.getName()      ;

            if (Logger.isLoggable(Logger.DEBUG)) {
                Logger.debug(msg);
            }
        }
    }

}
