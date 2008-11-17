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



#include "base/util/utils.h"
#include "base/fscapi.h"
#include "spdm/spdmutils.h"
#include "spdm/constants.h"
#include "spdm/ManagementNode.h"
#include "spdm/DeviceManagementNode.h"

#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>

static inline BOOL isNode(struct dirent *entry) {
    struct stat buf;
    return (!stat(entry->d_name, &buf) && S_ISDIR(buf.st_mode) &&
        strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."));
}

DeviceManagementNode::DeviceManagementNode(const char* parent, const char *leafName) : ManagementNode(parent, leafName)  {
    lines = new ArrayList;
    modified = FALSE;
    cwdfd = -1;
    update(TRUE);
}

DeviceManagementNode::DeviceManagementNode(const char *node)
    : ManagementNode(node)
{
    lines = new ArrayList;
    modified = FALSE;
    cwdfd = -1;
    update(TRUE);
}

DeviceManagementNode::DeviceManagementNode(const DeviceManagementNode &other) : ManagementNode(other) {
    lines = other.lines->clone();
    cwdfd = -1;
    modified = other.modified;
}

DeviceManagementNode::~DeviceManagementNode() {
    if (modified) {
        update(FALSE);
    }
    delete lines;
    if (cwdfd) {
        close(cwdfd);
    }
}

BOOL DeviceManagementNode::gotoDir(BOOL read) {
    BOOL success = TRUE;

    returnFromDir();
    cwdfd = open(".", O_RDONLY);

    char *curr = getenv("HOME");
    if (curr) {
        chdir(curr);
    }
    char *dirs = new char[strlen(context) + strlen(name) + 30];
    sprintf(dirs, ".sync4j/%s/%s", context, name);
    curr = dirs;
    do {
        char *nextdir = strchr(curr, '/');
        if (nextdir) {
            *nextdir = 0;
            nextdir++;
        }
        if (*curr) {
            if (chdir(curr)) {
                if (errno == ENOENT) {
                    if (!read) {
                        mkdir(curr, 0777);
                    } else {
                        // failed
                        success = FALSE;
                        break;
                    }
                }
                chdir(curr);
            }
        }
        curr = nextdir;
    } while (curr);
    delete [] dirs;

    return success;
}

void DeviceManagementNode::returnFromDir() {
    if (cwdfd >= 0) {
        fchdir(cwdfd);
        close(cwdfd);
        cwdfd = -1;
    }
}

void DeviceManagementNode::update(BOOL read) {
    if (!read && !modified) {
        // no work to be done
        return;
    }

    if (gotoDir(read)) {
        FILE *file = read ?
            fopen("config.txt", "r") :
            fopen("config.txt.tmp", "w");
        if (read) {
            char buffer[512];

            lines->clear();
            if (file) {
                while (fgets(buffer, sizeof(buffer), file)) {
                    char *eol = strchr(buffer, '\n');
                    *eol = 0;
                    line newline(buffer);
                    lines->add(newline);
                }
            }
        } else {
            if (file) {
                int i = 0;

                while (TRUE) {
                    line *curr = (line *)lines->get(i);
                    if (!curr) {
                        break;
                    }
                    fprintf(file, "%s\n", curr->getLine());

                    i++;
                }
                fflush(file);
                if (!ferror(file)) {
                    rename("config.txt.tmp", "config.txt");
                }
            }
        }
        if (file) {
            fclose(file);
        }
    }
    returnFromDir();
}

static int strnicmp( const char *a, const char *b, int len ) {
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

    while (TRUE) {
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
                return stringdup(value);   // FOUND :)
            }
        }
        i++;
    }
    // Not found, return an empty string
    return stringdup("");
}

int DeviceManagementNode::getChildrenMaxCount() {
    int count = 0;

    if (gotoDir(TRUE)) {
        DIR *dir = opendir(".");
        if (dir) {
            struct dirent *entry;
            for (entry = readdir(dir); entry; entry = readdir(dir)) {
                if (isNode(entry))
                    count++;
            }
            closedir(dir);
        }
    }
    returnFromDir();

    return count;
}



char **DeviceManagementNode::getChildrenNames() {
    char **childrenName = 0;

    int size = getChildrenMaxCount();
    if (size) {
        if (gotoDir(TRUE)) {
            DIR *dir = opendir(".");
            if (dir) {
                struct dirent *entry;
                int i = 0;
                childrenName = new char*[size];

                // restart reading, but this time copy file names
                rewinddir(dir);
                for (entry = readdir(dir); entry && (i < size) ; entry = readdir(dir)) {
                    if (isNode(entry)) {
                        childrenName[i] = stringdup(entry->d_name);
                        i++;
                    }
                }
                closedir(dir);
            }
        }
        returnFromDir();
    }
    return childrenName;
}

/*
 * Sets a property value.
 *
 * @param property - the property name
 * @param value - the property value (zero terminated string)
 */
void DeviceManagementNode::setPropertyValue(const char* property, const char* newvalue) {
    int i = 0;

    while (TRUE) {
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
                    modified = TRUE;
                }
                return;
            }
        }

        i++;
    }

    char *newstr = new char[strlen(property) + 3 + strlen(newvalue) + 1];
    sprintf(newstr, "%s = %s", property, newvalue);
    line newline(newstr);
    lines->add(newline);
    modified = TRUE;
    delete [] newstr;
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

