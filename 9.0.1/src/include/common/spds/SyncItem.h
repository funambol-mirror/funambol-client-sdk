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

 #ifndef INCL_SYNC_ITEM
    #define INCL_SYNC_ITEM
/** @cond DEV */

    #include "base/fscapi.h"
    #include "base/constants.h"
    #include "base/util/ArrayElement.h"
    #include "spds/constants.h"
    #include "spds/SyncStatus.h"
    #include "inputStream/InputStream.h"

    #include <string.h>
#include "base/globalsdef.h"

BEGIN_NAMESPACE

    typedef enum {
        SYNC_STATE_NEW     = 'N',
        SYNC_STATE_UPDATED = 'U',
        SYNC_STATE_DELETED = 'D',
        SYNC_STATE_NONE    = ' '
    } SyncState;

    class SyncItem : public ArrayElement {

    protected:

        char* data;
        char* encoding;
        long size;

        WCHAR key[DIM_KEY];
        WCHAR type[DIM_MIME_TYPE];

        long lastModificationTime;
        SyncState state;

        WCHAR* targetParent;
        WCHAR* sourceParent;

    private:
        /**
         * Initializes private members
         */
        void initialize();


    protected:

        /**
         * The SyncItem's input stream. Created new in the constructor, based on the
         * syncItem type (default is BufferInputStream)
         */
        InputStream* inputStream;


    public:
        /*
         * Default constructor
         */
        SyncItem();

        virtual ~SyncItem();

        /*
         * Constructs a new SyncItem identified by the given key. The key must
         * not be longer than DIM_KEY (see SPDS Constants).
         *
         * @param key - the key
         */
        SyncItem(const WCHAR* key);

        /*
         * Returns the SyncItem's key. If key is NULL, the internal buffer is
         * returned; if key is not NULL, the value is copied in the caller
         * allocated buffer and the given buffer pointer is returned.
         *
         * @param key - buffer where the key will be stored
         */
        const WCHAR* getKey() const;

        /*
         * Changes the SyncItem key. The key must not be longer than DIM_KEY
         * (see SPDS Constants).
         *
         * @param key - the key
         */
        void setKey(const WCHAR* key);

        /*
         * Sets the SyncItem modification timestamp. timestamp is a milliseconds
         * timestamp since a reference time (which is platform specific).
         *
         * @param timestamp - last modification timestamp
         */
        void setModificationTime(long timestamp);

        /*
         * Returns the SyncItem modeification timestamp. The returned value
         * is a milliseconds timestamp since a reference time (which is
         * platform specific).
         */
        long getModificationTime() const;

        /*
         * Sets the SyncItem content data. The passed data is copied into an
         * internal buffer so that the caller can release the buffer after
         * calling setData().
         * The SyncItem's inputStream is created NEW everytime setData() is called,
         * and it's linked to the 'data' buffer (it's a BufferInputStream).
         * To read chunked data, just call getInputStream()->read(buffer, size).
         *
         * Data which is to be sent as it is currently cannot contain nul-bytes
         * because it is treated like a C-style string. The size parameter should
         * not include the nul-byte which terminates C strings, so
         * pass size==0 for an empty string. A nul-byte is always
         * appended at the end of the data automatically.
         *
         * Binary data can be sent if it is encoded during transmission. The client
         * can decide which encoding to use like this:
         * - setData() with binary data
         * - changeEncoding(SyncItem::encodings::...)
         * or
         * - setData() with data that is already encoded in some way
         * - setDataEncoding(<the encoding name>)
         *
         * If the client neither changes nor sets the encoding, then
         * the default encoding specified in the SyncSource's
         * configuration is automatically applied by the client
         * library.
         *
         * @param data        memory to be copied, may be NULL; in that case an empty buffer is allocated
         * @param size        length of the given data or, if data is NULL, the desired buffer size
         */
        virtual void* setData(const void* data, long size);

        /*
         * Returns the SyncItem data buffer, in read-write mode.
         *
         * There is guaranteed to be a nul-byte after the data which
         * is not included in the data size.
         */
        virtual void* getData() const;

        /**
         * Returns a pointer to the SyncItem's inputStream.
         * Depending on SyncItem's implementation, it can be a specialized
         * InputStream (default is BufferInputStream).
         * Can be NULL if the InputStream has not been set yet (in the
         * default implementation, calling setData())
         */
        virtual InputStream* getInputStream();

        /*
         * Returns the amount of bytes stored in the item,
         * excluding the implicit nul-byte after the real data.
         */
        virtual long getDataSize() const;

         /*
         * Sets the SyncItem data size without changing the data buffer.
         *
         * @param s the new size
         */
        virtual void setDataSize(long s);

        /**
         * Sets the encoding of the data _without_ changing the data itself.
         * The only use of calling this directly in a SyncML client is to
         * send data in an encoding that the library itself does not support.
         * Otherwise changeDataEncoding() should be used if the default behavior
         * is not sufficient.
         *
         * @param encoding      this string is sent to the server and must be
         *                      supported by it, see the constants in SyncItem::encoding
         *                      for supported values; NULL means plain data
         */
        void setDataEncoding(const char* encoding);

        /**
         * Gets the encoding of the data. May return NULL if none has been set.
         *
         * Note that for incoming items the client library automatically converts
         * to plain encoding unless the server uses an unknown encoding.
         */
        const char* getDataEncoding() const;

        /**
         * Changes the encoding and the data currently stored in the item.
         * Transformation to and from the encodings listed in SyncItem::encoding
         * are supported. The method is virtual for possible customizations
         *
         * Some encodings may require additional information (TBD).
         *
         * @param encoding         see SyncItem::encoding for valid encodings
         * @param encryption       if not NULL, this is the encryption to be used. Encrypted data
         *                         is then encoded in base64 to be sent in syncML messages, so if
         *                         this value is not NULL the 'encoding' parameter is ignored.
         *                         Actually the only valid value is "des".
         * @param credentialInfo   optional additional information: password for DES encryption
         * @return error code, usually caused by unsupported encoding either of
         *         the current data or the requested new encoding
         */
        virtual int changeDataEncoding(const char* encoding, const char* encryption, const char* credentialInfo = NULL);

        /*
         * Sets the SyncItem data mime type
         *
         * @param - type the content mimetype
         */
        void setDataType(const WCHAR* type);

        /*
         * Returns the SyncItem data mime type.
         */
        const WCHAR* getDataType() const;

        /*
         * Sets the SyncItem state
         *
         * @param state the new SyncItem state
         */
        void setState(SyncState newState);

        /*
         * Gets the SyncItem state
         */
        SyncState getState() const;

       /**
         * Returns the SyncItem targetParent
         *
         * @return the SyncItem target parent
         */
        const WCHAR* getTargetParent() const;

        /**
         * Sets the SyncItem targetParent
         *
         * @param parent the target parent
         *
         */
        void setTargetParent(const WCHAR* parent);

        /**
         * Returns the SyncItem sourceParent
         *
         * @return the SyncItem source parent
         */
        const WCHAR* getSourceParent() const;

        /**
         * Sets the SyncItem sourceParent
         *
         * @param parent the source parent
         *
         */
        void setSourceParent(const WCHAR* parent);

        /**
         * Creates a new instance of SyncItem from the content of this
         * object. The new instance is created the the C++ new operator and
         * must be removed with the C++ delete operator.
         */
        virtual ArrayElement* clone();

        /**
         * valid encodings for changeDataEncoding() and some helper functions
         */
        struct encodings {
            static const char* const plain;    /**< data is transferred as it is */
            static const char* const escaped;  /**< base64 encoded during transfer */
            static const char* const des;      /**< encrypted with DES and then base64 encoded */
            /** helper function which turns NULL into plain */
            static const char* encodingString(const char* encoding) {
                return encoding ? encoding : plain;
            }

            /** returns true if and only if the encoding is one of the supported ones */
            static bool isSupported(const char* encoding) {
                const char* enc = encodingString(encoding);
                return !strcmp(enc, plain) ||
                    !strcmp(enc, escaped) ||
                    !strcmp(enc, des);
            }
        };

      private:
        /** encode or decode the item's data with the named transformation */
        int transformData(const char* name, bool encode, const char* password);
    };


END_NAMESPACE

/** @endcond */
#endif
