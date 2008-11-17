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


#include "syncml/core/CTCap.h"


CTCap::CTCap() {
    ctTypeSupported = NULL;
}
CTCap::~CTCap() {
    if (ctTypeSupported) {
        ctTypeSupported->clear(); ctTypeSupported = NULL;
    }
}

/**
 * Creates a new CTCap object with the given array of information
 *
 * @param ctTypeSupported the array of information on content type
 *                        capabilities - NOT NULL
 *
 */
CTCap::CTCap(ArrayList* ctTypeSupported) {
    this->ctTypeSupported = ctTypeSupported->clone();

}


/**
 * Get an array of content type information objects
 *
 * @return an array of content type information objects
 */
ArrayList* CTCap::getCTTypeSupported() {
    return ctTypeSupported;
}

/**
 * Sets an array of content type information objects
 *
 * @param ctTypeSupported an array of content type information objects
 */
void CTCap::setCTTypeSupported(ArrayList* ctTypeSupported) {
    if (this->ctTypeSupported) {
        this->ctTypeSupported->clear();
    }
    if (ctTypeSupported) {
        this->ctTypeSupported = ctTypeSupported->clone();
    }
    else {
        this->ctTypeSupported = NULL;
    }
}

ArrayElement* CTCap::clone() {
    CTCap* ret = new CTCap(ctTypeSupported);
    return ret;
}
