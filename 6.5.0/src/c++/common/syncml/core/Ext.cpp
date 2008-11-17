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

#include "base/util/utils.h"
#include "syncml/core/Ext.h"


Ext::Ext(){
    initialize();
}
Ext::~Ext() {
    if (xNam) {
        delete [] xNam; xNam = NULL;
    }

    if (xVal) {
        xVal->clear(); //delete xVal;        xVal = NULL;
    }
}

/**
* Creates a new Ext object with the given name and value
*
*  @param xNam corresponds to the &lt;XNam&gt; element - NOT NULL
*  @param xVal an array of parameters that corresponds to the &ltXVal&gt
*                  element
*
*/
Ext::Ext(char* xNam, ArrayList* xVal) {
    initialize();
    setXNam(xNam);
    setXVal(xVal);
}

void Ext::initialize() {
    xNam = NULL;
    xVal = new ArrayList();   // String[]
}

/**
* Gets the name of the extension
*
* @return the name of the extension
*/
const char* Ext::getXNam() {
    return xNam;
}

/**
* Sets the name of extension
*
* @param xNam the name of extension
*
*/
void Ext::setXNam(const char*xNam) {
    if (this->xNam) {
        delete [] this->xNam; this->xNam = NULL;
    }
    this->xNam = stringdup(xNam);

}

/**
* Gets an array of extension values, if exist
*
* @return an array of extension values, if exist
*/
ArrayList* Ext::getXVal() {
    return xVal;
}

/**
* Sets the array of extension value
*
* @param xVal the array of extension value
*
*/
void Ext::setXVal(ArrayList* xVal) {
    if (this->xVal) {
        this->xVal->clear();
    }
    this->xVal = xVal->clone();
}

ArrayElement* Ext::clone() {
    Ext* ret = new Ext(xNam, xVal);
    return ret;
}
