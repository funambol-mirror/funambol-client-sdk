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

#ifndef __APPLE_INPUT_STREAM_H__
#define __APPLE_INPUT_STREAM_H__

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFStream.h>
#include "base/fscapi.h"
#include "base/util/StringBuffer.h"
#include "inputStream/InputStream.h"

BEGIN_FUNAMBOL_NAMESPACE

class AppleInputStream : public InputStream
{
    protected:
        // Apple read stream: its data source to read is passed in the constructor and EXTERNALLY OWNED.     
        CFReadStreamRef readStream;        
        int eofbit;
        unsigned int position;
        bool streamOpened;

    public:
        /**
         * constructor. 
         */
        AppleInputStream();

        virtual ~AppleInputStream();

        /**
         * Reads 'size' bytes from the CFReadStreamRef
         * Returns the number of bytes effectively read.
         * @param buffer    [IN/OUT] the buffer of data read, allocated by the caller
         * @param size      the size of the chunk to be read [in bytes]
         * @return          the number of bytes effectively read (<= size)
         */
        virtual int read(void* buffer, const unsigned int size);


        /**
         * Call this method to start again reading from the beginning of the stream.
         * Resets the position indicator of the stream.
         */
        virtual void reset();

        /**
         * The function returns a non-zero value  if the eofbit stream's error flag has been 
         * set by a previous i/o operation. 
         * This flag is set by all standard input operations when the End Of File 
         * is reached in the sequence associated with the stream.
         */
        virtual int eof();

        /**
         * Returns the absolute position of the 'position' pointer.
         * The 'position' pointer determines the next location in the input 
         * sequence to be read by the next input operation.
         */
        virtual int getPosition();

        /**
         * Returns the CFReadStreamRef 'attached' to
         * input stream buffer
         */
        virtual CFReadStreamRef getStream() { return readStream; }

        /// From ArrayElement
        virtual ArrayElement* clone();
}; 

END_FUNAMBOL_NAMESPACE

#endif
