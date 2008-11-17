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


#include <stdio.h>
#include <ctype.h>
#include <f32file.h>
#include <utf.h>

#include "base/util/utils.h"
#include "base/util/stringUtils.h"
#include "base/util/symbianUtils.h"
#include "base/util/StringBuffer.h"
#include "base/fscapi.h"
#include "base/SymbianLog.h"
#include "spdm/spdmutils.h"
#include "spdm/constants.h"
#include "spdm/ManagementNode.h"
#include "spdm/DeviceManagementNode.h"
#include "base/globalsdef.h"
#include "base/cppdef.h"

USE_NAMESPACE

#define CONFIG_DIR      ".config"
#define SYNC4J_DIR      ".sync4j"

StringBuffer DeviceManagementNode::configPath; 
StringBuffer DeviceManagementNode::configFile = "config.ini";

class line : public ArrayElement {
    char *str;

    public:
        line(const char *newStr = NULL) { str = NULL; setLine(newStr); }
        ~line() { delete str; }
        ArrayElement *clone() { return new line(str); }

        const char *getLine() { return str; }
        void setLine(const char *newStr) { delete str; str = stringdup(newStr ? newStr : ""); }
};


DeviceManagementNode::DeviceManagementNode(const char* parent,
                                           const char *leafName)
                            : ManagementNode(parent, leafName),
                              lines(new ArrayList),
                              modified(false)
{
    initCurrentDir();
    update(true);
}

DeviceManagementNode::DeviceManagementNode(const char *node)
    : ManagementNode(node),
      lines(new ArrayList),
      modified(false)
{
    initCurrentDir();
    update(true);
}

DeviceManagementNode::DeviceManagementNode(const DeviceManagementNode &other)
    : ManagementNode(other) {

    lines = other.lines->clone();
    currentDir = other.currentDir;
    modified = other.modified;
}

DeviceManagementNode::~DeviceManagementNode() {
    if (modified) {
        update(false);
    }
    delete lines;
}


void DeviceManagementNode::update(bool read) 
{
    if (!read && !modified) {
        // no work to be done
        return;
    }

    StringBuffer fileName(currentDir);
    concatDirs(fileName, configFile.c_str());
    const char* fname = fileName.c_str();
    FILE* file = fopen(fname, "r");
    
    if (!file) {
        // Maybe the file or directory does not exist: create it and try again.
        LOG.debug("Could not open file, create it empty: '%s'", fileName.c_str());
        mkdirAll(currentDir);
        file = fopen(fname, "w+");  // "w+" to create the file and be able to read/write
        
        // Anyway, set the last error so Clients can know the file was not found.
        setErrorF(ERR_DM_TREE_NOT_AVAILABLE, "File not found: '%s'", fileName.c_str());
    }

    if (file) {
        // Open a temp file in writing mode if we must update the config
        if (!read) {
            fclose(file);
            fileName.append(".tmp");
            file = fopen(fileName, "w");
            if (!file) {
                setErrorF(ERR_INVALID_CONTEXT, "Error opening file: '%s'", fileName.c_str());
                LOG.error("%s", getLastErrorMsg());
                return;
            }
        }

        if (read) {
            // Note: don't create local buffers too big, the Symbian stack size is limited!
            char buffer[128];

            lines->clear();
            while (fgets(buffer, sizeof(buffer), file)) {
                char *eol = strchr(buffer, '\r');
                if (!eol) {
                    eol = strchr(buffer, '\n');
                }
                if (eol) {
                    *eol = 0;
                }
                line newline(buffer);
                lines->add(newline);
            }
            fclose(file);
        } 
        else {
            int i = 0;
            while (true) {
                line *curr = (line *)lines->get(i);
                if (!curr) {
                    break;
                }
                fprintf(file, "%s\n", curr->getLine());

                i++;
            }
            fflush(file);
            if (!ferror(file)) {
                StringBuffer tmpConfig = configFile;
                tmpConfig += ".tmp";

                // Both files must be closed for the rename.
                int ret = fclose(file);
                if (ret) {
                    setErrorF(ret, "Error (%d) closing file: '%s'", ret, fileName.c_str());
                    return;
                }

                renameFileInCwd( tmpConfig.c_str(), configFile.c_str());
            }
            else {
                fclose(file);
            }
        }
    }
    else {
        setErrorF(ERR_DM_TREE_NOT_AVAILABLE, "Error opening file: '%s'", fileName.c_str());
        LOG.error("%s", getLastErrorMsg());
        return;
    }
}

int DeviceManagementNode::strnicmp( const char *a, const char *b, int len ) {
    while (--len >= 0) {
        if (toupper(*a) != toupper(*b)) {
            return 1;
        }
        a++;
        b++;
    }
    return 0;
}


/*
 * Returns the value of the given property
 * the value is returned as a new char array and must be fred by the user
 *
 * @param property - the property name
 */
char* DeviceManagementNode::readPropertyValue(const char* property) {
    int i = 0;

    while (true) {
        line *curr = (line *)lines->get(i);
        if (!curr) {
            break;
        }

        const char *value = curr->getLine();
        while (*value && isspace(*value)) {
            value++;
        }
        if (!strnicmp(value, property, strlen(property))) {
            value = strchr(value, '=');
            if (value) {
                value++;
                while (*value && isspace(*value)) {
                    value++;
                }
                char *res = stringdup(value);   // FOUND :)

                // remove trailing white space: usually it is
                // added accidentally by users
                char *tmp = res + strlen(res) - 1;
                while (tmp > res) {
                    if (!isspace(*tmp)) {
                        break;
                    }
                    tmp--;
                }
                tmp[1] = 0;

                return res;
            }
        }
        i++;
    }
    // Not found, return an empty string
    return stringdup("");
}

#include "base/util/symbianUtils.h"

int DeviceManagementNode::getChildrenMaxCount() {
    int count = 0;

    RFs fileSession;
    RFile file;
    int cleanupStackSize = 0;

    StringBuffer fileSpecSb(currentDir);
    concatDirs(fileSpecSb, "*.*");

    // TODO use utility function for string conversion
    TBuf8<DIM_MANAGEMENT_PATH> buf8((const unsigned char*)fileSpecSb.c_str());
    HBufC* fileSpec = CnvUtfConverter::ConvertToUnicodeFromUtf8L(buf8);
    CleanupStack::PushL(fileSpec);
    ++cleanupStackSize;

    //
    // Connect to the file server
    //
    fileSession.Connect();
    CleanupClosePushL(fileSession);
    ++cleanupStackSize;

    StringBuffer buf;

    //
    // Get the directories list, sorted by name
    // (Leave if an error occurs)
    //
    CDir* dirList;
    TRAPD(err, fileSession.GetDir(*fileSpec, KEntryAttDir|KEntryAttMatchExclusive,
                                  ESortByName, dirList));
    if (err != KErrNone || dirList == NULL) {
        goto finally;
    }
    CleanupStack::PushL(dirList);
    ++cleanupStackSize;

    count = dirList->Count();

finally:
    //
    // Close the connection with the file server
    // and destroy dirList
    //
    fileSession.Close();
    CleanupStack::PopAndDestroy(cleanupStackSize);
    return count;
}



char **DeviceManagementNode::getChildrenNames() {
    char **childrenName = 0;
    int cleanupStackSize = 0;
    RFs fileSession;
    RFile file;

    StringBuffer fileSpecSb(currentDir);
    concatDirs(fileSpecSb, "*.*");

    // TODO use utility function for string conversion
    TBuf8<DIM_MANAGEMENT_PATH> buf8((const unsigned char*)fileSpecSb.c_str());
    HBufC* fileSpec = CnvUtfConverter::ConvertToUnicodeFromUtf8L(buf8);
    CleanupStack::PushL(fileSpec);
    ++cleanupStackSize;

    //
    // Connect to the file server
    //
    fileSession.Connect();
    CleanupClosePushL(fileSession);
    ++cleanupStackSize;

    //
    // Get the directories list, sorted by name
    // (Leave if an error occurs)
    //
    CDir* dirList;
    TRAPD(err, fileSession.GetDir(*fileSpec, KEntryAttDir|KEntryAttMatchExclusive,
                                  ESortByName, dirList));
    if (err != KErrNone || dirList == NULL) {
        goto finally;
    }
    CleanupStack::PushL(dirList);
    ++cleanupStackSize;

    //
    // Process each entry
    //
    childrenName = new char*[dirList->Count()];
    TInt i;
    for (i=0;i<dirList->Count();i++)
    {
        TBuf<DIM_MANAGEMENT_PATH> fileName = (*dirList)[i].iName;

#if 0
        childrenName[i] = bufToNewChar(buf8);
#else
        // TODO use string utils
        HBufC8* buf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(fileName);
        childrenName[i] = stringdup((const char*)buf8->Ptr(), buf8->Length());
        *(childrenName[i] + buf8->Length()) = (char)0;
        delete buf8;
#endif
    }
    fileSession.Close();

finally:
    //
    // Close the connection with the file server
    // and destroy dirList
    //
    CleanupStack::PopAndDestroy(cleanupStackSize);
    return childrenName;
}

/*
 * Sets a property value.
 *
 * @param property - the property name
 * @param value - the property value (zero terminated string)
 */
void DeviceManagementNode::setPropertyValue(const char* property, const char* newvalue) {
    
    if (!property || !newvalue) {
        LOG.debug("DeviceManagementNode::setPropertyValue - Warning, NULL char*!");
        return;
    }
    
    int i = 0;

    while (true) {
        line *curr = (line *)lines->get(i);
        if (!curr) {
            break;
        }

        const char *start = curr->getLine();
        const char *value = start;

        while (*value && isspace(*value)) {
            value++;
        }
        if (!strnicmp(value, property, strlen(property))) {
            value = strchr(value, '=');
            if (value) {
                value++;
                while (*value && isspace(*value)) {
                    value++;
                }
                if (strcmp(value, newvalue)) {
                    // preserve indention and property name from original config
                    char *newstr = new char[(value - start) + strlen(newvalue) + 1];
                    strncpy(newstr, start, value - start);
                    strcpy(newstr + (value - start), newvalue);
                    curr->setLine(newstr);
                    delete [] newstr;
                    modified = true;
                }
                goto finally;
            }
        }
        i++;
    }

    {
        // This is a new property
        char *newstr = new char[strlen(property) + 3 + strlen(newvalue) + 1];
        sprintf(newstr, "%s = %s", property, newvalue);
        line newline(newstr);
        lines->add(newline);
        delete [] newstr;
        modified = true;
    }

finally:
    i = 0;
}

ArrayElement* DeviceManagementNode::clone()
{
    DeviceManagementNode* ret = new DeviceManagementNode(context, name);

    int n = children.size();

    for (int i = 0; i<n; ++i) {
        ret->addChild(*((ManagementNode*)children[i]));
    }

    return ret;
}



void DeviceManagementNode::initCurrentDir() {

    if (configPath.empty()) {
        currentDir = ".\\";
    } else {
        currentDir = configPath;
    }
    if (context) {
        StringBuffer translatedContext = contextToPath(context);
        const char* tc = translatedContext.c_str();
        concatDirs(currentDir, tc);
    }
    if (name) {
        concatDirs(currentDir, name);
    }
}


int DeviceManagementNode::renameFileInCwd(const char* src, const char* dst)
{
    RFs fileSession;
    RFile file;

    StringBuffer srcSb(currentDir);
    concatDirs(srcSb, src);
    StringBuffer dstSb(currentDir);
    concatDirs(dstSb, dst);

    RBuf srcDes, dstDes;
    srcDes.Assign(stringBufferToNewBuf(srcSb));
    dstDes.Assign(stringBufferToNewBuf(dstSb));

    // Connect to the file server
    fileSession.Connect();
    CleanupClosePushL(fileSession);

    // Replace 'config.ini' file with 'config.ini.tmp'
    TInt err = fileSession.Replace(srcDes, dstDes);

    CleanupStack::PopAndDestroy(&fileSession);
    srcDes.Close();
    dstDes.Close();

    if (err == KErrNone) {
        return 0;
    }
    else {
        LOG.error("Error (code %d) replacing file '%s'", err, dstSb.c_str());
        if (err == KErrAccessDenied) {
            LOG.error("Access denied");
        }
        else if (err == KErrPathNotFound) {
            LOG.error("Unable to find the specified folder");
        }
        return -1;
    }
}


