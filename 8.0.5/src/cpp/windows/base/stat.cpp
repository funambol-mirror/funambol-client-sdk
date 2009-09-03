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


// Only used by WinMobile. Do not include in other project
#ifdef _WIN32_WCE

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/stat.h"
#include "base/util/utils.h"



BEGIN_NAMESPACE

/**
 * Implementation of the stat structure with Windows Mobile function
 *
 */

/**
 * Convert FILETIME to Unix time_t
 */
time_t FileTimeToTime_t(FILETIME &ft) {
    
     __int64 llTmp;
     memcpy (&llTmp, &ft, sizeof (__int64));
     llTmp = (llTmp - 116444736000000000) / 10000000;
     return (time_t) llTmp;
}

/**
 * Convert Unix time_t to FILETIME
 */
void Time_tToFileTime(time_t t, FILETIME &ft) {

     // Note that LONGLONG is a 64-bit value. From MS article 
     // http://support.microsoft.com/default.aspx?scid=KB;en-us;q167296
     LONGLONG ll;
     ll = Int32x32To64(t, 10000000) + 116444736000000000;
     ft.dwLowDateTime =  (DWORD)ll;
     ft.dwHighDateTime = (DWORD)(ll >> 32);    
     
}

int stat(const char* filename, struct stat* Stat) {
    
    int ret = -1;
    DWORD lpFileSizeHigh    = 0;
    DWORD dwSize            = 0;
    WCHAR* wfile            = NULL;

    if (filename == NULL) {
        LOG.error("wm stat: the filename is null");
        goto finally;
    }    
    WIN32_FILE_ATTRIBUTE_DATA wdata;
    wfile = toWideChar(filename);    
    
    BOOL res = GetFileAttributesEx(wfile, GetFileExInfoStandard, &wdata);
    
    if (res == 0) {
        LOG.error("wm error in GetFileAttributesEx: %i", GetLastError());
        goto finally;
    }

    Stat->st_mode =  (unsigned short)wdata.dwFileAttributes;
    Stat->st_mtime = FileTimeToTime_t(wdata.ftLastWriteTime);  // only write: it is the modification
    Stat->st_atime = FileTimeToTime_t(wdata.ftLastAccessTime); // both read/write
    Stat->st_ctime = FileTimeToTime_t(wdata.ftCreationTime); 
    Stat->st_size = wdata.nFileSizeLow;
    if (Stat->st_size == 0xFFFFFFFF) {
        Stat->st_size = 0;
        LOG.info("wm: the filesize low is 0xFFFFFFFF");
    } 

    ret = 0;

finally:
    
    if (ret == -1) {
        Stat->st_atime = 0;
        Stat->st_ctime = 0;
        Stat->st_mode  = 0;
        Stat->st_mtime = 0;
        Stat->st_size = -1;
    }
    
    delete [] wfile;

    return ret;
}

END_NAMESPACE

#endif  // #ifdef _WIN32_WCE


