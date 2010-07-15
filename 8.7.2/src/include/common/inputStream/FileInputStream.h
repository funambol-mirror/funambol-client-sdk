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

#ifndef INCL_FILE_INPUT_STREAM
#define INCL_FILE_INPUT_STREAM
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/StringBuffer.h"
#include "inputStream/InputStream.h"

BEGIN_NAMESPACE


/**
 * Extends the InputStream class, implements an input stream to read from a 
 * generic file, given its full path.
 * The file's content read is returned as it is (raw data).
 */
class FileInputStream : public InputStream {

private:

    /// The file location (path & file name)
    StringBuffer path;

    /// The file to be read.
    FILE* f;


public:

    /**
     * Constructor. 
     * Opens the file named by the path 'path' in the file system. The FILE object
     * remains opened for reading until the close() method is called, or this object is destroyed.
     * @param path    the file location (path & file name) to read from.
     */
    FileInputStream(const StringBuffer& path);

    /// Closes the FILE object.
    virtual ~FileInputStream();

    /**
     * Reads 'size' bytes from the file. The file is already opened in the constructor.
     * Returns the number of bytes effectively read.
     * @note            The file's content read is returned as it is (raw data).
     * @param buffer    [IN/OUT] the buffer of data read, allocated by the caller
     * @param size      the size of the chunk to be read [in bytes]
     * @return          the number of bytes effectively read (<= size)
     */
    virtual int read(void* buffer, const unsigned int size);

    /**
     * Call this method to start again reading from the beginning of the file stream.
     * Resets the position indicator of the stream.
     */
    void reset();

    /**
     * Closes the FILE object.
     * @return 0 if no errors
     */
    int close();

    /**
     * The function returns a non-zero value if the eofbit stream's error flag 
     * has been set by a previous i/o operation. 
     * Proxy method to the feof(f).
     */
    int eof();

    /**
     * Returns the absolute position of the 'position' pointer.
     * Proxy method to the ftell(f);
     */
    int getPosition();


    /// From ArrayElement
    ArrayElement* clone();

    /// Copy constructor
    FileInputStream(const FileInputStream& stream);

    /// Operator =
    FileInputStream& operator=(const FileInputStream& stream);
};


END_NAMESPACE

/** @endcond */
#endif
