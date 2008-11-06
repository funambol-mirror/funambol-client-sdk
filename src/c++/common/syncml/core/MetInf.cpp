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


#include "syncml/core/MetInf.h"

MetInf::MetInf() {
     set(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL);
}

MetInf::MetInf(const char*    format    ,
               const char*    type      ,
               const char*    mark      ,
               long        size      ,
               Anchor*     anchor    ,
               const char*    version   ,
               NextNonce*  nonce     ,
               long        maxMsgSize,
               long        maxObjSize,
               ArrayList*  emi       ,
               Mem*        mem       ) {

    set(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL);
    set(format, type, mark, size, anchor, version, nonce, maxMsgSize,
        maxObjSize, emi, mem);

}

MetInf::~MetInf() {

	if (this->format)     { delete [] this->format;    this->format = NULL; }
    if (this->type)       { delete [] this->type;      this->type = NULL; }
    if (this->mark)       { delete [] this->mark;      this->mark = NULL; }
    if (this->anchor)     { delete    this->anchor;    this->anchor = NULL; }
    if (this->version)    { delete [] this->version;   this->version = NULL; }
    if (this->nextNonce)  { delete    this->nextNonce; this->nextNonce = NULL; }
	if (this->mem)        { delete    this->mem;       this->mem = NULL; }

    if (this->emi)        { this->emi->clear(); } //delete this->emi; this->emi = NULL;}

	this->maxMsgSize = 0;
    this->maxObjSize = 0;
	this->size       = 0;

}


void MetInf::set(  const char*  format   ,
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

    this->format     = stringdup(format);
    this->type       = stringdup(type);
    this->mark       = stringdup(mark);
    this->anchor     = anchor->clone();
    setSize(size);
    this->version    = stringdup(version);
    this->nextNonce  = nonce->clone();
    setMaxMsgSize(maxMsgSize);
    setMaxObjSize(maxObjSize);
	this->mem        = mem->clone();


    if (emi == NULL) {
        this->emi = NULL;
    } else {
        this->emi = emi->clone();
    }
}



/**
 * Returns emi
 *
 * @return emi
 */
ArrayList* MetInf::getEMI() {
    return emi;
}

/**
 * Sets emi
 *
 * @param emi the new emi value
 */
void MetInf::setEMI(ArrayList* emi) {
    if (this->emi) {
		this->emi->clear();
    }
    if (emi) {
	    this->emi = emi->clone();
    }

}

/**
 * Returns dateSize (in bytes)
 *
 * @return size
 */
long MetInf::getSize() {
    return size;
}

/**
 * Sets size
 *
 * @param size the new size value
 */
void MetInf::setSize(long size) {
	if (size)
		this->size = size;
	else
		this->size = 0;

}

/**
* Returns format
 *
 * @return format
 */
const char* MetInf::getFormat() {
    return format;
}

/**
 * Sets format
 *
 * @param format the new format value
 */
void MetInf::setFormat(const char* format) {
    if (this->format) {
        delete [] this->format; this->format = NULL;
    }
    this->format = stringdup(format);
}

/**
 * Returns type
 *
 * @return type
 */
const char* MetInf::getType() {
    return type;
}

/**
 * Sets type
 *
 * @param type the new type value
 */
void MetInf::setType(const char* type) {
     if (this->type) {
        delete [] this->type; this->type = NULL;
     }
	 this->type = stringdup(type);
}

/**
 * Returns mark
 *
 * @return mark
 */
const char* MetInf::getMark() {
    return mark;
}

/**
 * Sets mark
 *
 * @param mark the new mark value
 */
void MetInf::setMark(const char* mark){
    if (this->mark) {
        delete [] this->mark; this->mark = NULL;
    }
    this->mark = stringdup(mark);
}

/**
 * Returns anchor
 *
 * @return anchor
 */
Anchor* MetInf::getAnchor() {
    return anchor;
}

/**
 * Sets anchor
 *
 * @param anchor the new anchor value
 */
void MetInf::setAnchor(Anchor* anchor) {
	if (this->anchor) {
		delete this->anchor; this->anchor = NULL;
	}
    this->anchor = anchor->clone();
}


/**
 * Returns nextNonce
 *
 * @return nextNonce
 */
NextNonce* MetInf::getNextNonce() {
    return nextNonce;
}

/**
 * Sets nextNonce
 *
 * @param nextNonce the new nextNonce value
 */
void MetInf::setNextNonce(NextNonce* nextNonce) {
	if (this->nextNonce) {
		delete this->nextNonce; this->nextNonce = NULL;
	}
    this->nextNonce = nextNonce->clone();

}


/**
 * Returns mem
 *
 * @return mem
 */
Mem* MetInf::getMem() {
	return mem;
}

/**
 * Sets mem
 *
 * @param mem the new mem value
 */
void MetInf::setMem(Mem* mem) {
	if (this->mem) {
		delete this->mem; this->mem = NULL;
	}
    this->mem = mem->clone();

}


/**
 * Returns maxMsgSize
 *
 * @return maxMsgSize
 */
long MetInf::getMaxMsgSize() {
    return maxMsgSize;
}

/**
 * Sets maxMsgSize
 *
 * @param maxMsgSize the new maxMsgSize value
 */
 void MetInf::setMaxMsgSize(long maxMsgSize) {
	if (maxMsgSize)
		this->maxMsgSize = maxMsgSize;
	else
		this->maxMsgSize = 0;

}

/**
 * Returns maxObjSize
 *
 * @return maxObjSize
 */
long MetInf::getMaxObjSize() {
    return maxObjSize;
}

/**
 * Sets maObjSize
 *
 * @param maxObjSize the new maxObjSize value
 */
void MetInf::setMaxObjSize(long maxObjSize) {
    if (maxObjSize)
		this->maxObjSize = maxObjSize;
	else
		this->maxObjSize = 0;
}

/**
 * Returns version
 *
 * @return version
 */
const char* MetInf::getVersion() {
    return version;
}

/**
 * Sets version
 *
 * @param version the new version value
 */
void MetInf::setVersion(const char* version) {
    if (this->version) {
        delete [] this->version; this->version = NULL;
    }
    this->version = stringdup(version);
}

MetInf* MetInf::clone() {

	MetInf* ret = new MetInf();
	ret->set(format, type, mark, size, anchor, version, nextNonce, maxMsgSize,
        maxObjSize, emi, mem);
	return ret;

}


