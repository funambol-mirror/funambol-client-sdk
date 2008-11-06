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
#include "syncml/parser/parser.h"
#include "syncml/formatter/Formatter.h"
#include "base/util/StringBuffer.h"
#include "syncml/core/SyncML.h"

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

