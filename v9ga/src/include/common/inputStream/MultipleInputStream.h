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

#ifndef INCL_MULTIPLE_INPUT_STREAM
#define INCL_MULTIPLE_INPUT_STREAM
/** @cond DEV */

#include "base/fscapi.h"
#include "inputStream/InputStream.h"

BEGIN_NAMESPACE


/**
 * Extends the InputStream class, defines a useful inteface for data that can be 
 * rapresented as a sequence of different input streams. 
 * So each stream is defined as a section, and there's an array 'sections'
 * that is accessed in order. This way, the read() operation is nothing more 
 * than calling read() on each section defined, and the eof bit is set when 
 * the last section is ended.
 * 
 * Derived classes must implement setInputStreams() to define a specific array of
 * different input streams, and then eventually implement readFromStream() in case
 * of special operations on each stream. 
 * Then, calling this.read() method the data is directly read from the first dtream defined
 * until the last one.
 */
class MultipleInputStream : public InputStream {

protected:

    /**
     * This is an array of InputStream objects.
     * Each one rapresents a section of the whole data to return.
     */
    ArrayList sections;

    /// The index of the current section.
    int currentSection;

    /**
     * This flag is set by all standard input operations when the End Of File 
     * is reached in the sequence associated with the stream.
     * It will be set to 1 when the EOF is reached for the last section.
     */
    int eofbit;

    /**
     * The 'position' pointer determines the next location in the input 
     * sequence to be read by the next input operation.
     * Note: the position is the absolute position of the whole data.
     */
    unsigned int position;


    /**
     * This method must be implemented to set the input streams and add them into the
     * 'section' array of streams. Once done, the class is ready to work on the
     * input streams defined (for example the method read() will automatically
     * return data reading from the first stream until the last one).
     * NOTE: It MUST be called before using the class, best would be in the constructor.
     */
    virtual void setSections() = 0;


    /// Returns true if current section is the last one.
    bool isLastSection();

    /// Return the desired input stream (section) given its index.
    InputStream* getSection(const int index);

    /**
     * Reads stream data from current section: it just calls 'stream->read(buffer, size)'.
     * It's defined because a derived class may need some special actions around
     * the read() operation. That's why it's virtual.
     * Note: 'buffer' is expected already allocated for at least 'size' bytes.
     */
    virtual int readFromStream(InputStream* stream, void* buffer, const unsigned int size);


public:

    /// Constructor.
    MultipleInputStream();

    /// Closes all the streams.
    virtual ~MultipleInputStream();

    /**
     * Reads 'size' bytes from the input streams, one by one.
     * Will call readFromStream() for each stream defined in the array 'sections'.
     * 'buffer' is expected already allocated for at least 'size' bytes.
     * @note            the number of bytes read can be less than the size specified
     *                  even if the stream EOF is not reached. So please use eof() method
     *                  after a read(), to know if the end of stream is reached.
     * @note            'buffer' is expected already allocated for at least 'size' bytes
     * @param buffer    [IN/OUT] the buffer of data read, allocated by the caller
     * @param size      the size of the chunk to be read [in bytes]
     * @return          the number of bytes effectively read (<= size)
     */
    virtual int read(void* buffer, const unsigned int size);


    /**
     * Call this method to start again reading from the beginning of the file stream.
     * Resets the position indicator of the stream. Reset all streams.
     */
    virtual void reset();

    /**
     * Closes all the streams.
     * @return 0 if no errors
     */
    virtual int close();

    /**
     * The function returns a non-zero value if the eofbit stream's error flag 
     * has been set by a previous i/o operation.
     */
    int eof();

    /// Returns the absolute position of the 'position' pointer.
    int getPosition();

};

END_NAMESPACE

/** @endcond */
#endif
