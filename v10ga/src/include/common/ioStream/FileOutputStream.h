/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2003 - 2010 Funambol, Inc.
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

#ifndef INCL_FILE_OUTPUT_STREAM
#define INCL_FILE_OUTPUT_STREAM
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/StringBuffer.h"
#include "ioStream/OutputStream.h"


BEGIN_FUNAMBOL_NAMESPACE


/**
 * Extends the OutputStream class: implements an input stream to write into a 
 * generic file, given its full path.
 */
class FileOutputStream : public OutputStream {

private:

    StringBuffer path;
    
    /// The file to write on.
    FILE* f;
    
    //size_t offset;
    
    
public:

    /**
     * Constructor
     * It creates a new OutputStream on the file with the given path. By default
     * the stream is created and it is ready to be written by the beginning of the file itself, both
     * if the file exists or not (in this case a new one is genereated)
     * With appendMode = true, if the file already exists, the internal offset of the stream
     * is set at the end of the file and the getOffset will return the number of byte already written 
     * (the size of the current file). Is up to the caller of the stream to check if the offset 
     * has been already initializated and decide if want to append from the offset or reset from the beginning.
     * @param - the path of the file
     * @param - represents if it has to append or to write by the begninning
     */
    FileOutputStream(const char* path, bool appendMode = false);

    virtual ~FileOutputStream();
    
    /// Closes the file handle and set it to NULL.
    virtual int close();

    /**
     * Writes 'size' bytes of data to the stream.
     * Returns the number of bytes effectively written.
     * @param buffer    the buffer of data to be written
     * @param size      the size of data to write
     * @return          the number of bytes effectively written (= size)
     */
    virtual int write(const void* buffer, unsigned int size);
    /*
    size_t getOffset(){
        return offset;
    }
    */
    //void setOffset(size_t offset);
       
    /// Resets the output file.
    void reset();
    
    /**
    * Return the path where the stream is build on.
    */
    const StringBuffer& getFilePath() { return path; }
};

END_FUNAMBOL_NAMESPACE

/** @endcond */
#endif
