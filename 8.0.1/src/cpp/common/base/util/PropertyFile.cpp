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

#include "base/util/PropertyFile.h"
#include "base/globalsdef.h"

#define REMOVED "__#REM#O#VED#__"

USE_NAMESPACE


StringBuffer escapeString(const char* val) {
    StringBuffer s(val);
    s.trim();
    s.replaceAll("\\", "\\\\");
    s.replaceAll("=", "\\=");
    return s;
}

StringBuffer unescapeString(const char* val) {
    StringBuffer s(val);
    s.trim();
    s.replaceAll("\\=", "=");
    s.replaceAll("\\\\", "\\");
    return s;
}

static bool existsFile(const char* fullname) {
    bool found = false;
    FILE* f = fopen(fullname, "r");
    if (f) {
        found = true;
        fclose(f);
    }
    return found;
}

static bool removeFile(const char* fullname) {
    char* p;
	int len;
    bool ret = false;
    
    // The path separator could be '/' or '\'
    p = strrchr((char*)fullname, '/');
    if (!p) {
        // try with '\'
        p = strrchr((char*)fullname, '\\');
    }

    if (!p) {
        // the file is in the current directory
        ret = removeFileInDir(".", fullname);                
    } else {
	    len = p-fullname;        
        StringBuffer dir(fullname, len);	 
	    p++; len=strlen(fullname)-len;
        StringBuffer filename(p, len);
        ret = removeFileInDir(dir, filename);
    }    	
    return ret;
}


int PropertyFile::read() {
    
    char line[512];
    FILE* f;
    f = fopen(node, "r");
    if (!f) {
        //LOG.debug("PropertyFile: the file '%s' doesn't exist. Try the journal file '%s'", node.c_str(), nodeJour.c_str());        
    } else {
        while(fgets(line, 511, f) != NULL) {
            StringBuffer s(line);
            StringBuffer key;
            StringBuffer value;

            if (separateKeyValue(s, key, value)) {
                KeyValuePair toInsert(key, value);
                data.add(toInsert);
            }            
        }   
        fclose(f);   
    }
    // check if there is the journal file and if any, set every value in memory. After that
    // empty the journal
    f = fopen(nodeJour, "r");
    if (!f) {
        // LOG.debug("PropertyFile: there is no journal file: '%s'", nodeJour.c_str());        
    } else {
        LOG.debug("PropertyFile: journal file found! (%s)", nodeJour.c_str()); 
        while(fgets(line, 511, f) != NULL) {
            StringBuffer s(line);
            StringBuffer key;
            StringBuffer value;
            if (separateKeyValue(s, key, value)) {
                if (value == REMOVED) {
                    LOG.debug("removing cache item (key = %s)", key.c_str());
                    MemoryKeyValueStore::removeProperty(key);
                } else {
                    LOG.debug("setting cache item (key = %s)", key.c_str());
                    MemoryKeyValueStore::setPropertyValue(key, value);
                }
            }                  
        }       
        fclose(f);         
    }
    return 0;
}

int PropertyFile::close() {

    FILE* file;
    file = fopen(node, "w");
    int ret = 0;   
    if (file) {
        KeyValuePair* curr = NULL;         
        for (curr = (KeyValuePair*)data.front(); curr;
             curr = (KeyValuePair*)data.next() ) {
            
             fprintf(file, "%s=%s\n", escapeString(curr->getKey()).c_str(), escapeString(curr->getValue()).c_str());            
        }       
        fclose(file); 
        
        // reset the content of the journal        
        if (existsFile(nodeJour) && !removeFile(nodeJour)) {
            LOG.error("There are problem in removing journal file");
        }
        
        ret = 0;

    } else {
        LOG.error("PropertyFile: it is not possible to save the file: '%s'", node.c_str());
        ret = -1;
    }
    return ret;
}

int PropertyFile::setPropertyValue(const char *prop, const char *value) {
    
    StringBuffer p(prop), v(value);
    p.trim();
    v.trim();

    int ret = MemoryKeyValueStore::setPropertyValue(p.c_str(), v.c_str());
    if (ret) {
        return ret;
    }

    FILE* file = fopen(nodeJour, "a+");    
    if (file) {
        fprintf(file, "%s=%s\n", escapeString(prop).c_str(), escapeString(value).c_str());
        fclose(file);
    } else {        
        LOG.error("PropertyFile setProperty: it is not possible to save the journal file: '%s'", node.c_str());
        ret = -1;
    }

    return ret;

}

int PropertyFile::removeProperty(const char *prop) {
    
    int ret = 0;

    FILE* file = fopen(nodeJour, "a+");    
    if (file) {
        fprintf(file, "%s=%s\n", escapeString(prop).c_str(), escapeString(REMOVED).c_str());
        fclose(file);
    } else {        
        LOG.error("PropertyFile removeProperty: it is not possible to save the journal file: '%s'", node.c_str());        
    }
    
    StringBuffer p(prop); p.trim();
    ret = MemoryKeyValueStore::removeProperty(p);
    if (ret) {
        LOG.debug("PropertyFile: it is not possible to remove from the ArrayList");
    }

    return ret;

}

int PropertyFile::removeAllProperties() {
    
    int ret = MemoryKeyValueStore::removeAllProperties();
    if (ret) {
        return ret;
    }
    // reset the content         
    if (existsFile(node) && !removeFile(node)) {
        LOG.error("There are problem in removing the file %s", node.c_str());
    }            
    return ret;
}


bool PropertyFile::separateKeyValue(StringBuffer& s, StringBuffer& key, StringBuffer& value) {
    bool ret = false;        
    bool foundSlash = false;
    
    for (unsigned int i = 0; i < s.length(); i++) {
        
        if (s.c_str()[i] == '\\') {
            if (foundSlash) {
                foundSlash = false;
            } else {
                foundSlash = true;
            }
            continue;
        }
        if (s.c_str()[i] == '=') {
            if (foundSlash) {
                foundSlash = false;
                continue;
            } else {
                key = unescapeString(s.substr(0, i));
                value = unescapeString(s.substr(i + 1, (s.length() - (i + 2)))); // it remove the \n at the end                                   
                ret = true;
                break;
            }
        }
        
    }
    return ret;
}
