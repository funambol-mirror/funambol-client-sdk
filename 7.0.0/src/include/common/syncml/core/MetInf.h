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



#ifndef INCL_METINF
#define INCL_METINF
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/Anchor.h"
#include "syncml/core/NextNonce.h"
#include "syncml/core/Mem.h"
#include "syncml/core/EMI.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


class MetInf {

     // ------------------------------------------------------------ Private data
    private:
        char*      format    ;
        char*      type      ;
        char*      mark      ;
        Anchor*      anchor    ;
        char*      version   ;
        NextNonce*   nextNonce ;
        long         maxMsgSize;
        long         maxObjSize;
        long         size      ;
        ArrayList*   emi       ;
        Mem*         mem       ;

        /**
         * Sets all properties in once.
         *
         * @param format the encoding format
         * @param type usually a MIME type
         * @param mark the mark element
         * @param sizeInBytes the data size in bytes
         * @param anchor the Anchor
         * @param version the data version
         * @param nonce the next nonce value
         * @param maxMsgSize the maximum message size in bytes
         * @param emi experimental meta info
         * @param memoryInfo memory information
         *
         */
        void set(const char*     format,
                 const char*     type  ,
                 const char*     mark  ,
                 long        size      ,
                 Anchor*     anchor    ,
                 const char*    version,
                 NextNonce*  nonce     ,
                 long        maxMsgSize,
                 long        maxObjSize,
                 ArrayList*  emi       ,
                 Mem*        mem       );

    // ---------------------------------------------------------- Protected data
    public:

        MetInf();

        ~MetInf();

     /**
     * Creates a new MetInf object with the given info.
     *
     * @param format the encoding format
     * @param type usually a MIME type
     * @param mark the mark element
     * @param size the data size in bytes
     * @param anchor the Anchor
     * @param version the data version
     * @param nonce the next nonce value
     * @param maxMsgSize the maximum message size in bytes
     * @param maxObjSize the maximum object size in bytes
     * @param emi experimental meta info
     * @param mem the memory information
     *
     */
    MetInf(const char*     format    ,
           const char*     type      ,
           const char*     mark      ,
           long        size      ,
           Anchor*     anchor    ,
           const char*     version   ,
           NextNonce*  nonce     ,
           long        maxMsgSize,
           long        maxObjSize,
           ArrayList*  emi       ,
           Mem*        mem       );


     /**
     * Returns dateSize (in bytes)
     *
     * @return size
     */
    long getSize();

    /**
     * Sets size
     *
     * @param size the new size value
     */
    void setSize(long size);

    /**
     * Returns format
     *
     * @return format
     */
    const char* getFormat();

    /**
     * Sets format
     *
     * @param format the new format value
     */
    void setFormat(const char*  format);

    /**
     * Returns type
     *
     * @return type
     */
    const char* getType();

    /**
     * Sets type
     *
     * @param type the new type value
     */
    void setType(const char*  type);

    /**
     * Returns mark
     *
     * @return mark
     */
    const char* getMark() ;

    /**
     * Sets mark
     *
     * @param mark the new mark value
     */
    void setMark(const char*  mark);

     /**
     * Sets emi
     *
     * @param emi the new emi value
     */
    void setEmi(ArrayList* emi);

    /**
     * Returns anchor
     *
     * @return anchor
     */
    Anchor* getAnchor();
    /**
     * Sets anchor
     *
     * @param anchor the new anchor value
     */
    void setAnchor(Anchor* anchor);

    /**
     * Returns nextNonce
     *
     * @return nextNonce
     */
    NextNonce* getNextNonce();

    /**
     * Sets nextNonce
     *
     * @param nextNonce the new nextNonce value
     */
    void setNextNonce(NextNonce* nextNonce);

    /**
     * Returns mem
     *
     * @return mem
     */
    Mem* getMem();

    /**
     * Sets mem
     *
     * @param mem the new mem value
     */
    void setMem(Mem* mem);


    /**
     * Returns maxMsgSize
     *
     * @return maxMsgSize
     */
    long getMaxMsgSize();

    /**
     * Sets maxMsgSize
     *
     * @param maxMsgSize the new maxMsgSize value
     */
     void setMaxMsgSize(long maxMsgSize);

    /**
     * Returns maxObjSize
     *
     * @return maxObjSize
     */
    long getMaxObjSize();

    /**
     * Sets maObjSize
     *
     * @param maxObjSize the new maxObjSize value
     */
    void setMaxObjSize(long maxObjSize);

    /**
     * Returns emi
     *
     * @return emi
     */
    ArrayList* getEMI();

    /**
     * Sets emi
     *
     * @param emi the new emi value
     */
    void setEMI(ArrayList* emi);

    /**
     * Returns version
     *
     * @return version
     */
    const char* getVersion();

    /**
     * Sets version
     *
     * @param version the new version value
     */
    void setVersion(const char*  version);

    MetInf* clone();

};


END_NAMESPACE

/** @endcond */
#endif
