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

#include <stdlib.h>

#include "base/fscapi.h"
#include "base/messages.h"
#include "base/Log.h"
#include "spds/DataTransformerFactory.h"
#include "spds/B64Decoder.h"
#include "spds/B64Encoder.h"
#include "spds/DESDecoder.h"
#include "spds/DESEncoder.h"
#include "base/globalsdef.h"

USE_NAMESPACE

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