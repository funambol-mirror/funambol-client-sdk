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

#ifndef INCL_SYMBIAN_DEVICE_MANAGEMENT_NODE
#define INCL_SYMBIAN_DEVICE_MANAGEMENT_NODE
/** @cond DEV */

#include <syncmldef.h>
#include <string.h>

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "spdm/ManagementNode.h"
#include "base/util/StringBuffer.h"


BEGIN_NAMESPACE

/*
 * File-based implementation of ManagementNode.
 * Each node is mapped to one file, located in
 *    $HOME/.sync4j/<node>
 * with entries of the type
 * <property>\s*=\s*<value>\n
 *
 * Comments look like:
 * \s*# <comment>
 */
class DeviceManagementNode : public ManagementNode {

protected:
    ArrayList *lines;
    bool modified;
    char *prefix;
    static StringBuffer configFile;

    class line : public ArrayElement {
        char *str;

        public:
            line(const char *newStr = NULL) { str = NULL; setLine(newStr); }
            ~line() { free(str); }
            ArrayElement *clone() { return new line(str); }

            const char *getLine() { return str; }
            void setLine(const char *newStr) { if (str) { free(str); } str = strdup(newStr ? newStr : ""); }
    };

    // the application's working directory
    StringBuffer currentDir;

    // change into directory which holds config file,
    // creating directories if necessary for writing
    //
    // @return true for success, false for error - call returnFromDir() in both cases
    bool gotoDir(bool read);

    // return to original directory after a gotoDir()
    void returnFromDir();

    // copy content of "lines" to or from file
    void update(bool read);

    // String compare case insensitive
    int strnicmp( const char *a, const char *b, int len );

    // Initialize current dir. Current dir is initialized as configPath +
    // context + name
    void initCurrentDir();

    // Rename a file in the current working directory (currentDir)
    int renameFileInCwd(const char* src, const char* dst);

#if defined(UPDATE_NATIVE_CONFIG)
    // Push configuration parameters into the native configuration (only for
    // paramters that apply)
    void pushSymbianSyncMLConfigParameter(const char* property,
                                          const char* value);
#endif

    private:

    static StringBuffer  configPath;
    static StringBuffer  server;
    static StringBuffer  profileName;
    static StringBuffer  cardURI;
    static StringBuffer  calURI;
    static StringBuffer  imapServer;
    static StringBuffer  smtpServer;
    static unsigned int  imapPort;
    static unsigned int  smtpPort;
    static TSmlCreatorId uid;

    public:

        // ------------------------------------------ Constructors & destructors

        /**
         * Constructor.
         *
         * @param parent - a ManagementNode is usually under the context of a
         *                 parent node.
         * @param name - the node name
         *
         */
        DeviceManagementNode(const char* parent, const char *leafName);
        DeviceManagementNode(const char* fullName);

        DeviceManagementNode(const DeviceManagementNode &other);
        virtual ~DeviceManagementNode();

        static void setConfigPath(const StringBuffer &p)        { configPath = p;       }
        static const StringBuffer& getConfigPath()              { return configPath;    }
        void lookupDir();
        
        // ----------------------------------------------------- Virtual methods

        /*
         * Returns the value of the given property
         *
         * @param property - the property name
         */
        virtual char* readPropertyValue(const char* property);


        /*
         * Sets a property value.
         *
         * @param property - the property name
         * @param value - the property value (zero terminated string)
         */
        virtual void setPropertyValue(const char* property, const char* value);

        /*
         * Returns the children's name of the parent node.
         */
        char **getChildrenNames();

        /*
         * Find how many children are defined for this node in the underlying
         * config system.
         */
        virtual int getChildrenMaxCount();

        /*
         * Creates a new ManagementNode with the exact content of this object.
         * The new instance MUST be created with the C++ new opertator.
         */
        virtual ArrayElement* clone();

        static void  setServerURI(const StringBuffer& server);
        static const StringBuffer& getServerURI();

        static void  setProfileName(const StringBuffer& name);
        static const StringBuffer& getProfileName();

        static void  setUID(TSmlCreatorId uid);
        static TSmlCreatorId getUID();

        static void setCardURI(const StringBuffer& cardURI);
        static const StringBuffer& getCardURI();

        static void setCalURI(const StringBuffer& calURI);
        static const StringBuffer& getCalURI();

        static void setImapServer(const StringBuffer& imapServer);
        static const StringBuffer& getImapServer();

        static void setImapPort(unsigned int imapPort);
        static unsigned int getImapPort();

        static void setSmtpServer(const StringBuffer& smtpServer);
        static const StringBuffer& getSmtpServer();

        static void setSmtpPort(unsigned int smtpPort);
        static unsigned int getSmtpPort();
};


END_NAMESPACE

/** @endcond */
#endif
