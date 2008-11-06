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

#include <stdlib.h>

#include "base/fscapi.h"
#include "base/messages.h"
#include "base/Log.h"
#include "spds/DataTransformerFactory.h"
#include "spds/B64Decoder.h"
#include "spds/B64Encoder.h"
#include "spds/DESDecoder.h"
#include "spds/DESEncoder.h"

//
// IMPORTANT: this test case encodes/decodes a UNICODE string; make sure
// to update it to UTF8 strings when used with such content.
//
void testEncryption() {
    char* clearText = "This is clear text.\nLet's see if encryption/decryption works!";
    char* password = "dummypassword";

    DataTransformer* b64e = DataTransformerFactory::getEncoder("b64");
    DataTransformer* b64d = DataTransformerFactory::getDecoder("b64");
    DataTransformer* dese = DataTransformerFactory::getEncoder("des");
    DataTransformer* desd = DataTransformerFactory::getDecoder("des");

    TransformationInfo infoe, infod;

    infoe.size = strlen(clearText)*sizeof(char);
    infoe.password = password;

    LOG.info("Clear text");
    LOG.info(clearText);

    char* desText = dese->transform(clearText, infoe);
    char* b64Text = b64e->transform(desText, infoe);

    LOG.info("Clear text");
    LOG.info(b64Text);

    delete [] desText;

    infod.size = infoe.size;
    infod.password = infoe.password;
    desText = b64d->transform(b64Text, infod);
    clearText = desd->transform(desText, infod);

    char* clearString = new char[infod.size/sizeof(char)+1];
    strncpy(clearString, clearText, infod.size/sizeof(char));
    clearString[infod.size/sizeof(char)] = 0;

    LOG.info("Clear text");
    LOG.info(clearString);

    delete [] clearString; delete [] clearText;
    delete [] b64Text; delete [] desText;
}