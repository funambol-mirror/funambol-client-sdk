/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */


#include "syncml/core/Meta.h"

Meta::Meta() {
        this->metInf = NULL;
		set(
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
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
