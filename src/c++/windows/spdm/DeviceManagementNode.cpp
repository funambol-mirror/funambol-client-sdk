/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/utils.h"
#include "spdm/spdmutils.h"
#include "spdm/constants.h"
#include "spdm/ManagementNode.h"
#include "spdm/DeviceManagementNode.h"

#ifdef _WIN32_WCE
#define HKEY_DM_ROOT   HKEY_LOCAL_MACHINE
#else
// Win32 clients are multi-user, settings go to HKCU.
#define HKEY_DM_ROOT   HKEY_CURRENT_USER
#endif



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
        lastErrorCode = ERR_INVALID_CONTEXT;
        sprintf(lastErrorMsg, "Invalid context path: %ls", fullContext);
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
        lastErrorCode = ERR_INVALID_CONTEXT;
        sprintf(lastErrorMsg, "Invalid context path: %s", fullContext);

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
        lastErrorCode = ERR_INVALID_CONTEXT;
        sprintf(lastErrorMsg, "Invalid context path: %s", fullContext);
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
                lastErrorCode = GetLastError();
                strcpy(lastErrorMsg, "Error enumerating children nodes");
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
        lastErrorCode = ERR_INVALID_CONTEXT;
        sprintf(lastErrorMsg, "Invalid context path: %s", fullContext);
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
    RegFlushKey(key);
finally:

    if (key != 0) {
        RegCloseKey(key);
    }
}

/*
 * Convert the path in Windows format, changing the slashes in back-slashes
 * and converting it to wide char.
 *
 * @param str - the string to convert
 *
 * @return  a new-allocated string with the converted path
 */
static WCHAR *toWindows(const char* str) {
    int i, len;

    len = strlen(str);
    WCHAR *ret = new WCHAR[len+1];

    for (i=0; i<len; ++i) {
        if (str[i] == L'/') {
            ret[i] = L'\\';
        }
        else {
            ret[i] = str[i];
        }
    }
    ret[len] = 0;
    return ret;
}

void DeviceManagementNode::setFullContext() {
    int len = strlen(context)+strlen(name)+1;
    char *ctx;
    const char swkey[] = "Software";

	if (fullContext)
		delete [] fullContext;

    if (strstr(context, swkey) == NULL) {
        len += strlen(swkey)+1;
        ctx = new char[len+1];
        sprintf(ctx, "%s/%s/%s", swkey, context, name);
    }
    else {
        ctx = new char[len+1];
        sprintf(ctx, "%s/%s", context, name);
    }
    fullContext = toWindows(ctx);
    delete [] ctx;
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

