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
#include "syncml/parser/parser.h"
#include "syncml/formatter/Formatter.h"
#include "base/util/StringBuffer.h"
#include "syncml/core/SyncML.h"
#include "base/globalsdef.h"

USE_NAMESPACE

/*
* Win32 example client for testing framework
*/


WCHAR* readContentFromFile(WCHAR* path) {

	WCHAR wfilename [256];

    int position = 0;
    int len = 0;
	wsprintf(wfilename, TEXT("%s"),  path);
	HANDLE file;
    DWORD lpFileSizeHigh;
    DWORD dwSize;

	WCHAR line[2048];
	FILE* f;
	WCHAR* ptr = NULL;


    file = CreateFile(wfilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);


    if( file ) {
        dwSize = GetFileSize(file, &lpFileSizeHigh);
        ptr = new WCHAR[dwSize + 1];
        wmemset(ptr, 0, dwSize);
    } else {

        goto finally;
    }

    CloseHandle(file);

    f = _wfopen(path, TEXT("r"));

    if (f == NULL) {
        goto finally;
    }

    while(fgetws(line, 2048, f) != NULL) {

        len = wcslen(line);
        wcsncpy(&ptr[position], line, len);

        position = position + len;

    }

    fflush(f);
    fclose(f);

finally:

    return ptr;

}



void writeTextToTextFile(WCHAR* fName, WCHAR* text) {

    FILE* f;

	f = _wfopen(fName, TEXT("w"));

    if( f != NULL ) {
       fwprintf(f, text);
    } else {
       MessageBox(NULL, TEXT("Error in write file"), TEXT("writeTextToTextFile"), MB_OK);
    }

	fflush(f);
	fclose(f);

}


#ifdef _WIN32_WCE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd ) {
#else
int main(int argc, char** argv) {
#endif

    WCHAR* xml = readContentFromFile(TEXT("sourceSync.txt"));

    SyncML* syncML = NULL;

    syncML = Parser::getSyncML(xml);
    StringBuffer* s = Formatter::getSyncML(syncML);
    writeTextToTextFile(TEXT("sourceSyncReversed.txt"), s->getChars());

    return 0;
 }

