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

#include "base/Log.h"
#include "base/base64.h"
#include "base/util/utils.h"
#include "base/util/XMLProcessor.h"
#include "syncml/core/Constants.h"

#include "inputStream/FileDataInputStream.h"
#include "inputStream/BufferInputStream.h"
#include "inputStream/FileInputStream.h"

USE_NAMESPACE


FileDataInputStream::FileDataInputStream (const StringBuffer& filePath) : MultipleInputStream(), 
                                                                          encodingHelper(FORMAT_B64, NULL, NULL), 
                                                                          path(filePath) {

    // Opens the file.
    // NOTE: MUST open in binary mode, in oder to have a correct position indicator
    // of the stream after each read() call.
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) {
        // File not existing
        // LOG.error("FileDataInputStream error: cannot read the file '%s'", path.c_str());
        return;
    }

    //
    // Fill the FileData object (OMA file obj format)
    //
    FileData fileData;
    int fileSize = fgetsize(f);
    fseek(f, 0, SEEK_SET);          // Resets the position indicator of the stream
    fileData.setSize(fileSize);
    fclose(f); 
    f = NULL;

    StringBuffer fileName(getFileNameFromPath(path));
    WCHAR* wFileName = toWideChar(fileName.c_str());
    fileData.setName(wFileName);
    delete [] wFileName;
    wFileName = NULL;

    // The body is fake: we just write the file name.
    // Will be replaced reading the real file content from the stream (see read())
    StringBuffer& fakeBody = fileName;
    fileData.setBody(fakeBody.c_str(), fakeBody.length());

    // Sets the file modification time (UTC string like "20091028T192200Z")
    unsigned long tstamp = getFileModTime(path);
    StringBuffer modTime = unixTimeToString(tstamp, true);  // file's mod time is already in UTC
    if (!modTime.empty()) {
        WString wmodTime;
        wmodTime = modTime;
        fileData.setModified(wmodTime);
    }

    const char* buf = fileData.format();
    if (!buf) {
        LOG.error("FileDataInputStream error: cannot format output data");
        return;
    }
    formattedFileData = buf;
    delete [] buf;

    //
    // Save the 3 InputStream sections.
    //
    setSections();


    // Calculate the projected total data size
    long b64FileSize = encodingHelper.getDataSizeAfterEncoding(fileSize);
    totalSize = prologue.length() + b64FileSize + epilogue.length();
}


void FileDataInputStream::setSections() {

    unsigned int pos=0, startPos=0, endPos=0;

    // Get the prologue and epilogue XML.
    XMLProcessor::getElementContent(formattedFileData.c_str(), FILE_BODY, &pos, &startPos, &endPos);
    if (!startPos || !endPos || (endPos < startPos)) {
        LOG.error("FileDataInputStream error: cannot find '%s' tag in output string", FILE_BODY);
        return;
    }
    
    prologue = formattedFileData.substr(0, startPos);
    epilogue = formattedFileData.substr(endPos);

    // Create the 3 InputStream to read from.
    BufferInputStream stream1(prologue);
    FileInputStream   stream2(path);
    BufferInputStream stream3(epilogue);

    sections.clear();
    sections.add(stream1);
    sections.add(stream2);
    sections.add(stream3);
}



int FileDataInputStream::readFromStream(InputStream* stream, void* buffer, const unsigned int size) {

    if (currentSection != 1) {
        //
        // 1st and 3rd streams: read data from a string buffer
        //
        return stream->read(buffer, size);
    }

    //
    // 2nd stream: read data from file and encode in Base64
    //
    int rawSize = encodingHelper.getMaxDataSizeToEncode(size);      //int(size / 4) * 3;  // must be 3/4 in size, and multiple of 3.
    if (rawSize <= 0) {
        return 0;
    }

    char* rawBuffer = new char[rawSize];
    int rawBytesRead = stream->read(rawBuffer, rawSize);

    int b64Size = b64_encode((char*)buffer, rawBuffer, rawBytesRead);
    
    delete [] rawBuffer;
    return b64Size;
}


ArrayElement* FileDataInputStream::clone() {
    return new FileDataInputStream(this->path);
}
