/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2010 Funambol, Inc.
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

#include "ioStream/AppleInputStream.h"
#include "base/util/utils.h"

BEGIN_FUNAMBOL_NAMESPACE

AppleInputStream::AppleInputStream() : InputStream() 
{
    this->position  = 0;
    this->eofbit    = 0;
    streamOpened    = false;
    readStream      = NULL;
}


AppleInputStream::~AppleInputStream() 
{
    close();
}

int AppleInputStream::open()
{
    close();
    
    return 0;
}

int AppleInputStream::close()
{
    if (readStream) {
        if (streamOpened) {
            CFReadStreamClose(readStream);
        }

        CFRelease(readStream);
        
        this->position  = 0;
        this->eofbit    = 0;
        streamOpened    = false;
        readStream      = NULL;
    }

    return 0;
}

int AppleInputStream::read(void* buffer, const unsigned int size) 
{
    CFIndex bytesRead = 0;

    if ((bytesRead = CFReadStreamRead(readStream, static_cast<UInt8 *>(buffer), 
            static_cast<CFIndex>(size))) < 0) {

        return 0;
    }

    position += bytesRead;

    if (bytesRead == 0) {
        eofbit = 1;
    }

    return bytesRead;
}


void AppleInputStream::reset() { 
    position = 0;
    eofbit   = 0;
}

int AppleInputStream::eof() {
    return eofbit;
}

int AppleInputStream::setPosition(unsigned int offset)
{
    int ret = 1;
    
    if (streamOpened) {
        CFNumberRef offsetNum = CFNumberCreate(kCFAllocatorDefault,  kCFNumberLongType, static_cast<void *>(&offset));
        
        if (offsetNum) {
            if (CFReadStreamSetProperty(readStream, kCFStreamPropertyFileCurrentOffset, offsetNum)) {
                position = offset;
                ret = 0; // stream property set succeeded: set success return code
            }
            
            CFRelease(offsetNum);
        }
    }
    
    return ret;
}

int AppleInputStream::getPosition() {
    return position;
}

ArrayElement* AppleInputStream::clone() {
    // The 'data' buffer is always externally owned.
    return new AppleInputStream(); 
}

END_FUNAMBOL_NAMESPACE

