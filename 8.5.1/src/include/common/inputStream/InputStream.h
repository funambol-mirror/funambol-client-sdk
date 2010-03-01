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

#ifndef INCL_INPUT_STREAM
#define INCL_INPUT_STREAM
/** @cond DEV */

#include "base/fscapi.h"
#include "base/constants.h"
#include "base/globalsdef.h"
#include "base/util/ArrayElement.h"


BEGIN_NAMESPACE


/**
 * Abstract class that represents a generic Input Stream. 
 * The method read() is inteded to return a buffer of data (a chunk)
 * of a given size, until the end of data.
 * Specific implementations of InputStream can read data directly from a stream 
 * in order to avoid loading a large object in memory.
 */
class InputStream : public ArrayElement {

protected:

    /**
     * The total size of data.
     * It should be set in the constructor, this information is needed by the sync 
     * engine to know how much data to send.
     * Note: not all the stream implementation can get this information. If not available
     *       this value can be left = 0.
     */
    unsigned int totalSize;


public:

    /// Constructor
    InputStream();

    virtual ~InputStream();


    /**
     * Returns the total size of the input stream data.
     * Note: not all the stream implementation can get this information. If not available
     * it will return 0.
     */
    int getTotalSize();


    /** 
     * Reads 'size' bytes of data from the stream and returns them ('buffer').
     * Returns the number of bytes effectively read.
     * @param buffer    [IN/OUT] the buffer of data read, allocated by the caller
     * @param size      the size of the chunk to be read [in bytes]
     * @return          the number of bytes effectively read (<= size)
     */
    virtual int read(void* buffer, const unsigned int size) = 0;

    /**
     * Call this method to start again reading from the beginning of the stream.
     * Resets the position indicator of the stream.
     */
    virtual void reset() = 0;

    /**
     * Closes the current input stream.
     * Derived classes SHOULD override this method in order to execute 
     * all all necessary operations to close the stream.
     * @return 0 if no errors
     */
    virtual int close();

    /**
     * The function returns a non-zero value  if the eofbit stream's error flag has been 
     * set by a previous i/o operation. 
     * This flag is set by all standard input operations when the End Of File 
     * is reached in the sequence associated with the stream.
     */
    virtual int eof() = 0;

    /**
     * Returns the absolute position of the 'position' pointer.
     * The 'position' pointer determines the next location in the input 
     * sequence to be read by the next input operation.
     */
    virtual int getPosition() = 0;

    /// From ArrayElement
    virtual ArrayElement* clone() = 0;

};


END_NAMESPACE

/** @endcond */
#endif
