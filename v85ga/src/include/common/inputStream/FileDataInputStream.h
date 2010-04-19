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

#ifndef INCL_FILE_OBJECT_INPUT_STREAM
#define INCL_FILE_OBJECT_INPUT_STREAM
/** @cond DEV */

#include "base/fscapi.h"
#include "inputStream/MultipleInputStream.h"
#include "base/util/EncodingHelper.h"
#include "spds/FileData.h"

BEGIN_NAMESPACE


/**
 * Extends the MultipleInputStream class, implements an input stream to read from a 
 * generic file, given its full path.
 * The file's content read is returned as a File Object Data (XML with
 * tags about the file size, name and so on).
 */
class FileDataInputStream : public MultipleInputStream {

private:

    /// The file location (path & file name)
    StringBuffer path;

    /// The formatted file data object, created by the constructor and then used
    /// to set the sections.
    StringBuffer formattedFileData;

    /// The XML prologue (before the file body). It's created in the constructor.
    StringBuffer prologue;

    /// The XML epilogue (after the file body). It's created in the constructor.
    StringBuffer epilogue;

    /// Used for utils method to encode/decode the file body in base64.
    EncodingHelper encodingHelper;


    /**
     * Creates the 3 input streams and adds them into the array of sections:
     *   1st section = BufferInputStream (the prologue xml)
     *   2nd section = FileInputStream   (the real file body)
     *   3rd section = BufferInputStream (the epilogue xml)
     */
    void setSections();

    /**
     * Reads stream data from current section.
     * Overrided because some section may need special actions:
     * Data from section 2 (file content) needs to be converted
     * into base64 encoding.
     * Note: 'buffer' is expected already allocated for at least 'size' bytes.
     */
    int readFromStream(InputStream* stream, void* buffer, const unsigned int size);


public:

    /**
     * Constructor. 
     * Opens the file named by the path 'filePath' in the file system. The FILE object
     * remains opened for reading until the close() method is called, or this object is destroyed.
     * @note  the file body is actually ALWAYS encoded in Base64 (see encodingHelper construction).
     * @param filePath  the file location (path & file name) to read from.
     */
    FileDataInputStream(const StringBuffer& filePath);


    /// From ArrayElement
    ArrayElement* clone();

};

END_NAMESPACE

/** @endcond */
#endif
