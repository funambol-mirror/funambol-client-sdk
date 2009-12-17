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
#include "base/util/utils.h"
#include "inputStream/FileInputStream.h"

USE_NAMESPACE

FileInputStream::FileInputStream(const StringBuffer& path) : InputStream() {

    f = NULL;
    totalSize = 0;

    if (path.empty()) {
        LOG.error("FileInputStream error: empty file path");
        return;
    }
    this->path = path;

    // Opens the file.
    // NOTE: MUST open in binary mode, in oder to have a correct position indicator
    // of the stream after each read() call.
    f = fopen(path.c_str(), "rb");
    if (!f) {
        // LOG.error("FileInputStream error: cannot read the file '%s'", path.c_str());
    } else {    
        // Get file size
        totalSize = fgetsize(f);
        fseek(f, 0, SEEK_SET);          // Resets the position indicator of the stream
    }
    // Get file size (alternate way)
    //struct stat st;
    //memset(&st, 0, sizeof(struct stat));
    //if (stat(path.c_str(), &st) < 0) {
    //    LOG.error("FileInputStream error: can't stat file '%s' [%d]", path.c_str(), errno);
    //    return;
    //}
    //totalSize = st.st_size;
}


FileInputStream::~FileInputStream() {

    close();
}


int FileInputStream::read(void* buffer, const unsigned int size) {

    if (!f) {
        LOG.error("FileInputStream::read error: file is not opened");
        buffer = NULL;
        return 0;
    }

    return fread(buffer, sizeof(char), size, f);
}

void FileInputStream::reset() {

    if (f) {
        fseek(f, 0, SEEK_SET);
    }
}


int FileInputStream::close() {

    int ret = 0;
    if (f) {
        ret = fclose(f);
        f = NULL;
    }
    return ret;
}

int FileInputStream::eof() {

    int ret = 0;
    if (f) {
        ret = feof(f);
    }
    return ret;
}


int FileInputStream::getPosition() {
    int ret = 0;
    if (f) {
        ret = (int)ftell(f);
    }
    return ret;
}


ArrayElement* FileInputStream::clone() {
    return new FileInputStream(this->path);
}

FileInputStream::FileInputStream(const FileInputStream& stream) {
    FileInputStream(stream.path);
}

FileInputStream& FileInputStream::operator=(const FileInputStream& stream) {
    FileInputStream(stream.path);
    return *this;
}
