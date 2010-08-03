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

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/utils.h"
#include "spdm/constants.h"
#include "spdm/ManagementNode.h"
#include "spdm/DeviceManagementNode.h"
#include "base/globalsdef.h"
#include "base/util/WString.h"

USE_NAMESPACE

#ifdef _WIN32_WCE
#define HKEY_DM_ROOT   HKEY_LOCAL_MACHINE
#else
#define HKEY_DM_ROOT   HKEY_CURRENT_USER        // Win32 clients are multi-user, settings go to HKCU.
#include <shlwapi.h>                            // To use SHDeleteKey (defined in shlwapi.lib)
#endif

// default = true
bool DeviceManagementNode::autoFlushRegistry = true;


/**
 * Convert the path in Windows format, changing the slashes in back-slashes
 * and converting it to wide char.
 * If a slash char must be kept, it must be escaped with a double slash "//"
 * so it will be converted into a single slash.
 *
 * @param str - the string to convert
 * @return  a new-allocated string with the converted path
 */
static WCHAR *toWindows(const char* str) {
    
    int i, len;
    len = strlen(str);
    char *buf = new char[len+1];

    int j=0;
    for (i=0; i<len; i++) {

        if (str[i] == '/') {
            if ((i+1 < len) && str[i+1] == '/') {
                // it's a '/' escaped: keep it
                i++;
                buf[j] = '/';
            }
            else {
                buf[j] = '\\';
            }
        }
        else {
            buf[j] = str[i]; 
        }

        j++;
    }

    buf[j] = 0;

    WCHAR* ret = toWideChar(buf);
    delete [] buf;
    return ret;
}


DeviceManagementNode::DeviceManagementNode(const char* parent, const char* name)
    : ManagementNode(parent, name), fullContext(0)
{
    setFullContext();
}

DeviceManagementNode::DeviceManagementNode(const char *node)
    : ManagementNode(node), fullContext(0)
{
    setFullContext();
}

DeviceManagementNode::~DeviceManagementNode()
{
    if (fullContext)
        delete [] fullContext;
}

/*
 * Returns the value of the given property
 * the value is returned as a new char array and must be free'd by the user
 *
 * @param property - the property name
 */
char* DeviceManagementNode::readPropertyValue(const char* prop) {
    HKEY key = NULL;
    DWORD res;
    long err = 0;
    char *ret=NULL;
    TCHAR *p = toWideChar(prop);
    ULONG dim = 0;

    RegCreateKeyEx(
            HKEY_DM_ROOT,
            fullContext,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            NULL,
            &key,
            &res
            );

    if (key == 0) {
        //lastErrorCode = ERR_INVALID_CONTEXT;
        //sprintf(lastErrorMsg, "Invalid context path: %ls", fullContext);
        setErrorF(ERR_INVALID_CONTEXT, "Invalid context path: %ls", fullContext);
        goto finally;
    }

    // Get value length
    err = RegQueryValueEx(
            key,
            p,
            NULL,
            NULL,  // we currently support only strings
            NULL,
            &dim
            );

    if (err == ERROR_SUCCESS) {
		if (dim > 0) {
            TCHAR *buf = new TCHAR[dim + 1];

			err = RegQueryValueEx(
					key,
					p,
					NULL,
					NULL,  // we currently support only strings
					(UCHAR*)buf,
					&dim
					);
            if (err == ERROR_SUCCESS)
                ret = toMultibyte(buf);
            delete [] buf;
		}
    }
    //else MessageBox(NULL,  "Error", "getConfigParameter", MB_OK);

    if (!ret)
        ret = stringdup("");

finally:

    if (p)
        delete [] p;

    if (key != 0) {
        RegCloseKey(key);
    }

    return ret;
}

int DeviceManagementNode::deletePropertyNode(const char* nodeName) { 

    int err = 0;

    if (nodeName) {
        StringBuffer nodeNameToDelete;
        nodeNameToDelete.convert(getFullContext());
        nodeNameToDelete.append("/");
        nodeNameToDelete.append(nodeName);
        const WCHAR* wname = toWindows(nodeNameToDelete.c_str());

#ifdef _WIN32_WCE
        // 'RegDeleteKey' deletes all subkeys recursively in WinCE
        err = RegDeleteKey(HKEY_DM_ROOT, wname);
#else
        // RegDeleteKey() does NOT recurse and will fail on keys with subkeys, on Win32.
        // To delete a key and all its subkeys recursively we have to use SHDeleteKey()
        // RegDeleteTree() is preferred, but is supported only by WinVista or above.
        err = SHDeleteKey(HKEY_DM_ROOT, wname);
#endif

        if (err != ERROR_SUCCESS) {
            setErrorF(ERR_DM_TREE_NOT_AVAILABLE, "Could not delete node: %ls", wname);
        }
        delete [] wname;
    }
    return err;
}

int DeviceManagementNode::getChildrenMaxCount() {
    HKEY key;
    ULONG howMany = 0;

    RegOpenKeyEx(
            HKEY_DM_ROOT,
            fullContext,
            0,
            KEY_READ,
            &key
            );

    if (key == 0) {
        //lastErrorCode = ERR_INVALID_CONTEXT;
        //sprintf(lastErrorMsg, "Invalid context path: %s", fullContext);
        setErrorF(ERR_INVALID_CONTEXT, "Invalid context path: %s", fullContext);

        goto finally;
    }

    RegQueryInfoKey(
            key, NULL, NULL, NULL, &howMany, NULL, NULL, NULL, NULL, NULL, NULL, NULL
            );


finally:

    if (key != 0) {
        RegCloseKey(key);
    }

    return howMany;
}


/*
 * Returns the children's name of the parent node.
 *
 * @param children - the buffer where ManagementNode* must be stored
 * @param size - the size of the children buffer (number of ManagementNode*) in
 *               input; the number of children in output
 */
char **DeviceManagementNode::getChildrenNames() {
    ULONG dim = DIM_MANAGEMENT_PATH;
    HKEY key;
	char **childrenName = NULL;
    ULONG i, howMany = 0;

    DWORD res;
    HRESULT ret;

    RegCreateKeyEx(
            HKEY_DM_ROOT,
            fullContext,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            NULL,
            &key,
            &res
            );

    if (key == 0) {
        //lastErrorCode = ERR_INVALID_CONTEXT;
        //sprintf(lastErrorMsg, "Invalid context path: %s", fullContext);
        setErrorF(ERR_INVALID_CONTEXT, "Invalid context path: %s", fullContext);
        goto finally;
    }

    //
    // First of all we need to know how many
    RegQueryInfoKey(
            key, NULL, NULL, NULL, &howMany, NULL, NULL, NULL,
			NULL, NULL, NULL, NULL
            );

	childrenName = new char *[howMany];

    for (i=0; i<howMany; ++i) {
        dim = DIM_MANAGEMENT_PATH*sizeof(char);
        TCHAR child[DIM_MANAGEMENT_PATH] = TEXT("");

        ret = RegEnumKeyEx(key, i, child, &dim, NULL, NULL, NULL, NULL);
        if (ret != ERROR_SUCCESS) {
            if (ret == ERROR_NO_MORE_ITEMS) {
                break;
            }
            else {
                //lastErrorCode = GetLastError();
                //strcpy(lastErrorMsg, "Error enumerating children nodes");
                setError(GetLastError(), "Error enumerating children nodes"); 
                goto finally;
            }
        }
        childrenName[i] = toMultibyte(child);
    }


finally:

    if (key != 0) {
        RegCloseKey(key);
    }

	return childrenName;
}


/*
 * Sets a property value.
 *
 * @param property - the property name
 * @param value - the property value (zero terminated string)
 */
void DeviceManagementNode::setPropertyValue(const char* prop, const char* value) {
    if ((prop == NULL) || (value == NULL)) {
        return;
    }

    HKEY key;
    DWORD res;
    TCHAR *p = NULL;
    TCHAR *v = NULL;

    RegCreateKeyEx(
            HKEY_DM_ROOT,
            fullContext,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            NULL,
            &key,
            &res
            );


    if (key == 0) {
        //lastErrorCode = ERR_INVALID_CONTEXT;
        //sprintf(lastErrorMsg, "Invalid context path: %s", fullContext);
        setErrorF(ERR_INVALID_CONTEXT, "Invalid context path: %s", fullContext);
        goto finally;
    }

    p = toWideChar(prop);
    v = toWideChar(value);

    RegSetValueEx(
            key,
            p,
            NULL,
            REG_SZ,  // we currently support only strings
            (UCHAR*)v,
            (wcslen(v)+1)*sizeof(WCHAR)
            );

    delete [] p;
    delete [] v;


#ifdef _WIN32_WCE
    // we can avoid auto flushing the registry (expensive operation) setting this value to false.
    // in this case the flushRegistry() must then be called to really persist data.
    if (autoFlushRegistry) {
        RegFlushKey(key);
    }
#else
    // Calling RegFlushKey is an expensive operation that significantly affects performance.
    // Under Win32 it is not necessary to call RegFlushKey to make key changes visible to other processes 
    // because registry changes are flushed to disk by the registry using its lazy flusher
#endif

finally:

    if (key != 0) {
        RegCloseKey(key);
    }
}


void DeviceManagementNode::setFullContext() {
    
    StringBuffer ctx;
    const char swkey[] = "Software";
    
    if (fullContext) {
		delete [] fullContext;
    }

    // Escape any slash char inside the node name, with a double slash.
    // Otherwise they will be converted in a backslash and an additional leaf
    // would be created (see toWindows() function, see also bug 9384).
    StringBuffer nodeName(name);
    nodeName.replaceAll("/", "//");

    if (strstr(context, swkey) == NULL) {
        ctx.sprintf("%s/%s/%s", swkey, context, nodeName.c_str());
    }
    else {
        ctx.sprintf("%s/%s", context, nodeName.c_str());
    }

    // Converts all "/" into "\\", for windows.
    fullContext = toWindows(ctx.c_str());
}

/*
 * Creates a new ManagementNode with the exact content of this object.
 * The new instance MUST be created with the C++ new opertator.
 */
ArrayElement* DeviceManagementNode::clone() {
	DeviceManagementNode* ret = new DeviceManagementNode(context, name);

	int n = children.size();

	for (int i = 0; i<n; ++i) {
		ret->addChild(*((ManagementNode*)children[i]));
	}

	return ret;
}

void DeviceManagementNode::setAutoFlushRegistry(const bool v) { 
    autoFlushRegistry = v; 
}
const bool DeviceManagementNode::getAutoFlushRegistry() {
    return autoFlushRegistry; 
}

void DeviceManagementNode::flushRegistry() {
    RegFlushKey(HKEY_DM_ROOT);
}
