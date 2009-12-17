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

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include "base/fscapi.h"
#include "base/messages.h"
#include "base/Log.h"
#include "spds/DataTransformerFactory.h"
#include "spds/B64Decoder.h"
#include "spds/B64Encoder.h"
#include "spds/DESDecoder.h"
#include "spds/DESEncoder.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

USE_NAMESPACE


class EncryptionTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(EncryptionTest);
        CPPUNIT_TEST(testEncryption);        
        
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(){

    }

    void tearDown(){
        if (b64e){
            delete b64e;
            b64e = NULL;
        }
        if (b64d){
            delete b64d;
            b64d = NULL;
        }
        if (dese){
            delete dese;
            dese = NULL;
        }
        if (desd){
            delete desd;
            desd = NULL;
        }
        if (startText){
            delete startText;
            startText = NULL;
        }
        if (finalText){
            delete finalText;
            finalText = NULL;
        }

    }

private:
    DataTransformer* b64e;
    DataTransformer* b64d;
    DataTransformer* dese;
    DataTransformer* desd;
    StringBuffer* startText;
    StringBuffer* finalText;


    void testEncryption(){
        char* clearText = "This is clear text.\nLet's see if encryption/decryption works!";
        startText = new StringBuffer(clearText);
        char* password = "dummypassword";

        b64e = DataTransformerFactory::getEncoder("b64");
        b64d = DataTransformerFactory::getDecoder("b64");
        dese = DataTransformerFactory::getEncoder("des");
        desd = DataTransformerFactory::getDecoder("des");

        TransformationInfo infoe, infod;

        infoe.size = (long)strlen(clearText)*sizeof(char);
        infoe.password = password;


        char* desText = dese->transform(clearText, infoe);
        char* b64Text = b64e->transform(desText, infoe);

        delete [] desText;  desText = NULL;

        infod.size = infoe.size;
        infod.password = infoe.password;
        desText = b64d->transform(b64Text, infod);
        clearText = desd->transform(desText, infod);

        char* clearString = new char[infod.size/sizeof(char)+1];
        strncpy(clearString, clearText, infod.size/sizeof(char));
        clearString[infod.size/sizeof(char)] = 0;
        finalText = new StringBuffer(clearString);
        if (clearString){
            delete [] clearString; clearString = NULL;
        }
        if (clearString) {
            delete [] clearText; clearText = NULL;
        }
        if (b64Text){
            delete [] b64Text;  b64Text = NULL;
        }

        CPPUNIT_ASSERT( *startText == *finalText );

    }

    void testCustomEncryption(){
        char* clearText = "12345678";
        startText = new StringBuffer(clearText);
        // char* password = "dummypassword";
        char* password = "test";

        b64e = DataTransformerFactory::getEncoder("b64");
        b64d = DataTransformerFactory::getDecoder("b64");
        dese = DataTransformerFactory::getEncoder("des");
        desd = DataTransformerFactory::getDecoder("des");

        TransformationInfo infoe, infod;

        infoe.size = (long)strlen(clearText)*sizeof(char);
        infoe.password = password;


        char* desText = dese->transform(clearText, infoe);
        char* b64Text = b64e->transform(desText, infoe);
        
        TransformationInfo info1, info2, info3, info4, info5;
        info1.size = 4; info2.size = 4; info3.size = 3; info4.size = 8; info5.size = 16;
        info1.password = password; info2.password = password; info3.password = password;
        info4.password = password; info5.password = password;
        
        char* desText1 = dese->transform("1234", info1);
        char* desText2 = dese->transform("5678", info2);
        char* desText3 = dese->transform("123", info3);
        char* desText4 = dese->transform("12345678", info4);
        char* desText5 = dese->transform("1234567812345678", info5);
        char* b64Text1 = b64e->transform(desText1, info1);
        char* b64Text2 = b64e->transform(desText2, info2);
        char* b64Text3 = b64e->transform(desText3, info3);
        char* b64Text4 = b64e->transform(desText4, info4);
        char* b64Text5 = b64e->transform(desText5, info5);

        delete [] desText;  desText = NULL;
/*
        infod.size = infoe.size;
        infod.password = infoe.password;
        desText = b64d->transform(b64Text, infod);
        clearText = desd->transform(desText, infod);

        char* clearString = new char[infod.size/sizeof(char)+1];
        strncpy(clearString, clearText, infod.size/sizeof(char));
        clearString[infod.size/sizeof(char)] = 0;
        finalText = new StringBuffer(clearString);
        if (clearString){
            delete [] clearString; clearString = NULL;
        }
        if (clearString) {
            delete [] clearText; clearText = NULL;
        }
        if (b64Text){
            delete [] b64Text;  b64Text = NULL;
        }

        CPPUNIT_ASSERT( *startText == *finalText );
*/
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( EncryptionTest );