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


#include "syncml/core/SyncML.h"


SyncML::SyncML() {
    header = NULL;
    body   = NULL;
}

SyncML::~SyncML() {
    if (header) {
        delete header; header = NULL;
    }
    if (body) {
        delete body;   body   = NULL;
    }
}

/**
* Creates a new SyncML object from header and body.
*
* @param header the SyncML header - NOT NULL
* @param body the SyncML body - NOT NULL
*
*/
SyncML::SyncML(SyncHdr*  header,
               SyncBody* body) {

    this->header = NULL;
    this->body   = NULL;
    setSyncHdr(header);
    setSyncBody(body);
}

/**
* Returns the SyncML header
*
* @return the SyncML header
*
*/
SyncHdr* SyncML::getSyncHdr() {
    return header;
}

/**
* Sets the SyncML header
*
* @param header the SyncML header - NOT NULL
*
*/
void SyncML::setSyncHdr(SyncHdr* header) {
    if (header == NULL) {
        // TBD
    }
    if (this->header) {
        delete this->header; this->header = NULL;
    }
    if (header) {
        this->header = header->clone();
    }
}

/**
* Returns the SyncML body
*
* @return the SyncML body
*
*/
SyncBody* SyncML::getSyncBody() {
    return body;
}

/**
* Sets the SyncML body
*
* @param body the SyncML body - NOT NULL
*
*/
void SyncML::setSyncBody(SyncBody* body) {
    if (body == NULL) {
        // TBD
    }
    if (this->body) {
        delete this->body; this->body = NULL;
    }
    if (body) {
        this->body = body->clone();
    }
}

/**
* Is this message the last one of the package?
*
* @return lastMessage
*/
BOOL SyncML::isLastMessage() {
    return body->isFinalMsg();
}

/**
* Sets lastMessage
*
* @param lastMessage the new lastMessage value
*
*/
void SyncML::setLastMessage() {
    body->setFinalMsg(TRUE);
}
