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
#include "inputStream/MultipleInputStream.h"

USE_NAMESPACE


MultipleInputStream::MultipleInputStream () : InputStream() {

    currentSection = 0;
    position       = 0;
    eofbit         = 0;
    totalSize      = 0;
}



MultipleInputStream::~MultipleInputStream() {
    close();
}


int MultipleInputStream::read(void* buffer, const unsigned int size) {

    LOG.debug("MultipleInputStream::read - section #%i, size requested = %i", currentSection, size);    

    InputStream* stream = (InputStream*)sections.get(currentSection);
    if (!stream) {
        LOG.error("FileDataInputStream: error reading stream #%i", currentSection+1);
    }

    // Reads data from the current stream
    int bytesRead = readFromStream(stream, buffer, size);
    position += bytesRead;

    if (stream->eof()) {
        // Current section has ended
        if (isLastSection()) {
            eofbit = 1;
            return bytesRead;
        }
        else {
            // Move to next section and read remaining bytes.
            // Note: recursive call!
            currentSection ++;
            int ret = read((char*)buffer + bytesRead, size - bytesRead);
            return (bytesRead + ret);
        }
    }

    return bytesRead;
}


int MultipleInputStream::readFromStream(InputStream* stream, void* buffer, const unsigned int size) {

    return stream->read(buffer, size);
}


bool MultipleInputStream::isLastSection() {
    return currentSection == sections.size() - 1;
}

InputStream* MultipleInputStream::getSection(const int index) {
    if (index >= sections.size()) {
        LOG.error("No insput stream defined with index %i", index);
        return NULL;
    }
    return (InputStream*)sections.get(index);
}


void MultipleInputStream::reset() {

    for (int i=0; i<sections.size(); i++) {
        InputStream* stream = (InputStream*)sections[i];
        if (stream) {
            stream->reset();
        }
    }

    eofbit         = 0;
    position       = 0;
    currentSection = 0;
}


int MultipleInputStream::close() {

    int ret = 0;
    for (int i=0; i<sections.size(); i++) {
        InputStream* stream = (InputStream*)sections[i];
        if (stream) {
            ret |= stream->close();
        }
    }
    return ret;
}

int MultipleInputStream::eof() {
    return eofbit;
}

int MultipleInputStream::getPosition() {
    return position;
}

