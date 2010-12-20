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

#ifndef INCL_STRING_OUTPUT_STREAM
#define INCL_STRING_OUTPUT_STREAM
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/StringBuffer.h"
#include "inputStream/OutputStream.h"


BEGIN_FUNAMBOL_NAMESPACE


/**
 * Extends the OutputStream class: writes into a StringBuffer.
 * The output string can be returned calling getString().
 */
class StringOutputStream : public OutputStream {

private:

    // The buffer
    StringBuffer buf;

public:

    /// Constructor
    StringOutputStream() : OutputStream() {}

    virtual ~StringOutputStream() {}


    /**
     * Writes 'size' bytes of data to the stream.
     * Returns the number of bytes effectively written.
     * @param buffer    the buffer of data to be written
     * @param size      the size of data to write
     * @return          the number of bytes effectively written
     *                  if this number differs from the size parameter, it
     *                  indicates an error
     */
    virtual int write(const void* buffer, unsigned int size) {
        buf.append((const char*)buffer, size);
        return size;
    }

    /// Returns the output stream buffer.
    StringBuffer& getString() {
        return buf;
    }
    
    /// Resets the output stream buffer.
    void reset() {
        buf.reset();
    }

    /// Overload OutputStream::size() to use StringBuffer lenght
    int size() {
        return buf.length();
    }

};

END_FUNAMBOL_NAMESPACE

/** @endcond */
#endif
