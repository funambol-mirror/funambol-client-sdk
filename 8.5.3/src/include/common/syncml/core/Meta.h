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



#ifndef INCL_META
#define INCL_META
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/MetInf.h"
#include "syncml/core/Anchor.h"
#include "syncml/core/NextNonce.h"
#include "syncml/core/Mem.h"
#include "syncml/core/EMI.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class Meta {

     // ------------------------------------------------------------ Private data
    private:
       MetInf* metInf;

       void set(const char*     format,
                const char*     type  ,
                const char*     mark  ,
                long        size      ,
                Anchor*     anchor    ,
                const char*     version,
                NextNonce*  nonce     ,
                long        maxMsgSize,
                long        maxObjSize,
                ArrayList*  emi       ,
                Mem*        mem      );


    // ---------------------------------------------------------- Public data
    public:

        Meta();

        ~Meta();

        MetInf* getMetInf();

		void setMetInf(MetInf* metInf);

		 /**
		 * This get method always returns null. This is a used in the JiBX mapping
		 * in order to do not output the MetInf element.
		 *
		 * @return always null
		 */
		MetInf* getNullMetInf();

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
		const char* getMark();

		/**
		 * Sets mark
		 *
		 * @param mark the new mark value
		 */
		void setMark(const char*  mark);

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
		 * Sets emi
		 *
		 * @param emi the new emi value
		 */
		void setEMI(ArrayList* emi);

        /**
		 * Gets emi
		 *
		 * @return emi
		 */
		ArrayList* getEMI();

        /**
		 * clone meta
		 *
		 * @return meta
		 */
        Meta* clone();
};


END_NAMESPACE

/** @endcond */
#endif
