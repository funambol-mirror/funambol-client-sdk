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

#include "syncml/core/Mem.h"
#include "syncml/core/Meta.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

Meta::Meta() {
        this->metInf = NULL;
        set(
            NULL,
            NULL,
            NULL,
            0,
            NULL,
            NULL,
            NULL,
            0,
            0,
            NULL,
            NULL
        );
}

Meta::~Meta() {

    if (metInf) {
        delete metInf; metInf = NULL;
    }
}


void Meta::set(const char*    format ,
               const char*    type   ,
               const char*    mark   ,
               long        size      ,
               Anchor*     anchor    ,
               const char*    version,
               NextNonce*  nonce     ,
               long        maxMsgSize,
               long        maxObjSize,
               ArrayList*  emi       ,
               Mem*        mem       ) {

        getMetInf(); // if still null, a new instance will be created

        metInf->setFormat     (format    );
        metInf->setType       (type      );
        metInf->setMark       (mark      );
        metInf->setAnchor     (anchor    );
        metInf->setSize       (size      );
        metInf->setVersion    (version   );
        metInf->setNextNonce  (nonce     );
        metInf->setMaxMsgSize (maxMsgSize);
        metInf->setMaxObjSize (maxObjSize);
        metInf->setMem        (mem       );
        metInf->setEMI        (emi       );
    }


MetInf* Meta::getMetInf() {
    if (metInf == NULL) {
        return (metInf = new MetInf());
    }

    return metInf;
}


void Meta::setMetInf(MetInf* metInf) {
    if (this->metInf) {
            delete this->metInf; this->metInf = NULL;
    }
    if (metInf) {
        this->metInf = metInf->clone();
    }
}

MetInf* Meta::getNullMetInf() {
    return NULL;
}

/**
 * Returns dateSize (in bytes)
 *
 * @return size
 */
long Meta::getSize() {
    return getMetInf()->getSize();
}

/**
 * Sets size
 *
 * @param size the new size value
 */
void Meta::setSize(long size) {
    getMetInf()->setSize(size);
}

/**
 * Returns format
 *
 * @return format
 */
const char* Meta::getFormat() {
    return getMetInf()->getFormat();
}

/**
 * Sets format
 *
 * @param format the new format value
 */
void Meta::setFormat(const char* format) {
    getMetInf()->setFormat(format);
}

/**
 * Returns type
 *
 * @return type
 */
const char* Meta::getType() {
    return getMetInf()->getType();
}

/**
 * Sets type
 *
 * @param type the new type value
 */
void Meta::setType(const char* type) {
    getMetInf()->setType(type);
}

/**
 * Returns mark
 *
 * @return mark
 */
const char* Meta::getMark() {
    return getMetInf()->getMark();
}

/**
 * Sets mark
 *
 * @param mark the new mark value
 */
void Meta::setMark(const char* mark) {
    getMetInf()->setMark(mark);
}


/**
 * Returns version
 *
 * @return version
 */
const char* Meta::getVersion() {
    return getMetInf()->getVersion();
}

/**
 * Sets version
 *
 * @param version the new version value
 */
void Meta::setVersion(const char* version) {
    getMetInf()->setVersion(version);
}

/**
 * Returns anchor
 *
 * @return anchor
 */
Anchor* Meta::getAnchor() {
    return getMetInf()->getAnchor();
}

/**
 * Sets anchor
 *
 * @param anchor the new anchor value
 */
void Meta::setAnchor(Anchor* anchor) {
    getMetInf()->setAnchor(anchor);
}

 /**
 * Returns nextNonce
 *
 * @return nextNonce
 */
NextNonce* Meta::getNextNonce() {
    return getMetInf()->getNextNonce();
}

/**
 * Sets nextNonce
 *
 * @param nextNonce the new nextNonce value
 */
void Meta::setNextNonce(NextNonce* nextNonce) {
    getMetInf()->setNextNonce(nextNonce);
}

/**
 * Returns maxMsgSize
 *
 * @return maxMsgSize
 */
long Meta::getMaxMsgSize() {
    return getMetInf()->getMaxMsgSize();
}

/**
 * Sets maxMsgSize
 *
 * @param maxMsgSize the new maxMsgSize value
 */
void Meta::setMaxMsgSize(long maxMsgSize) {
    getMetInf()->setMaxMsgSize(maxMsgSize);
}

/**
 * Returns maxObjSize
 *
 * @return maxObjSize
 */
long Meta::getMaxObjSize() {
    return getMetInf()->getMaxObjSize();
}

/**
 * Sets maObjSize
 *
 * @param maxObjSize the new maxObjSize value
 */
void Meta::setMaxObjSize(long maxObjSize) {
    getMetInf()->setMaxObjSize(maxObjSize);
}

/**
 * Returns mem
 *
 * @return mem
 */
Mem* Meta::getMem() {
    return getMetInf()->getMem();
}

/**
 * Sets mem
 *
 * @param mem the new mem value
 */
void Meta::setMem(Mem* mem) {
    getMetInf()->setMem(mem);
}

/**
 * Returns emi
 *
 * @return emi
 */
ArrayList* Meta::getEMI() {
    return getMetInf()->getEMI();
}

/**
 *
 * This property is binding with set-method and there is a
 * bug into JiBx: it uses the first method with the specified
 * name without checking the parameter type.
 * This method must be written before all other setEMI() methods
 * to have a right marshalling.
 *
 * @param emi ArrayList of EMI object
 */
void Meta::setEMI(ArrayList* emi) {
    getMetInf()->setEMI(emi);
}

Meta* Meta::clone() {
    Meta* ret = new Meta();

    MetInf* retMetInf = new MetInf(getFormat(), getType(), getMark(), getSize(), getAnchor(), getVersion(),
                   getNextNonce(), getMaxMsgSize(), getMaxObjSize(), getEMI(), getMem());
    ret->setMetInf(retMetInf);

    if (retMetInf) {
        delete retMetInf; retMetInf = NULL;
    }

    return ret;

}

END_NAMESPACE
