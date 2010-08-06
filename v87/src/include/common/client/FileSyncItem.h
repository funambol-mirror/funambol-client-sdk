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

 #ifndef INCL_FILE_SYNC_ITEM
#define INCL_FILE_SYNC_ITEM
/** @cond DEV */

#include "base/fscapi.h"
#include "base/constants.h"
#include "base/util/ArrayElement.h"
#include "spds/SyncItem.h"

#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * FileSyncItem rapresents a SyncItem whose data is a generic file.
 * Passing the file's path in the constructor, it will open an input stream on
 * the file's content, in order to optimize memory allocation.
 * If the boolean isFileData is set to true, the file's data read will be returned 
 * as a file data object (OMA specs), otherwise raw data is returned.
 * 
 * @note  it's NOT intended to get the data from the caller: the data can be
 *        retrieved chunk by chunk calling getInputStream()->read() method.
 */
class FileSyncItem : public SyncItem {


private:

    /// The full file name+path, set by the constructor.
    StringBuffer filePath;

    /** 
     * If true, the InputStream is a FileDataInputStream.
     * If false, the InputStream is a FileInputStream (raw file data).
     * It's set by the constructor.
     */
    bool isFileData;


public:

    /**
     * Default constructor.
     * Initializes the SyncItem and creates the right InputStream based on
     * the boolean 'isFileData'.
     * @param path        the full file name+path
     * @param key         the item's key (can't be NULL)
     * @param isFileData  if true, the InputStream is created new as a FileDataInputStream.
     */
    FileSyncItem(const StringBuffer& path, const WCHAR* key, const bool isFileData = true);

    /// Constructor. sets the file name as 'key'
    FileSyncItem(const StringBuffer& path, const bool isFileData);
    
    ~FileSyncItem();


    /**
     * DEPRECATED METHOD.
     * This method is no more used, as the file's data is directly
     * read from the InputStream, in order to avoid loading the whole file
     * in memory. 
     * Clients should just call getInputStream()->read() to read the file's data
     * from the input stream, that is initialized passing the file path in the constructor.
     *
     * Current implementation just logs a warning.
     * @see getInputStream()
     */
    void* setData(const void* data, long size);

    /**
     * DEPRECATED METHOD.
     * Please use getInputStream()->read() to read the file's data.
     * Current implementation just logs a warning.
     * @see setData()
     * @see getInputStream()
     */
    void* getData() const;

    /// Returns the InputStream's total size.
    long getDataSize() const;

     /**
      * DEPRECATED METHOD.
      * Data size is calculated by the internal InputStream.
      */
    void setDataSize(long s);
    
    /**
     * DEPRECATED METHOD.
     * The changeDataEncoding is not implemented in FileSyncItem
     */
    virtual int changeDataEncoding(const char* encoding, const char* encryption, const char* credentialInfo = NULL);


    /**
     * Creates a new instance of FileSyncItem from the content of this
     * object. The new instance is created the the C++ new operator and
     * must be removed with the C++ delete operator.
     */
    ArrayElement* clone();

};

END_NAMESPACE

/** @endcond */
#endif
