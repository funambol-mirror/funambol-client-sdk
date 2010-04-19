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
#include "spds/ItemReader.h"
#include "spds/SyncItem.h"
#include "spds/Chunk.h"
#include "base/util//EncodingHelper.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

#include "spds/DataTransformerFactory.h"
#include "spds/B64Decoder.h"
#include "spds/B64Encoder.h"
#include "spds/DESDecoder.h"
#include "spds/DESEncoder.h"

USE_NAMESPACE

// 265 char for the TEST_STRING string 
#define TEST_STRING  "Here an example of text to be in b64: <Folder><name>Email Home</name><created>20090428T162654Z<created><role>account</role><Ext><XNam>VisibleName</XNam> <XVal>Name Surname</XVal></Ext><Ext><XNam>EmailAddress</XNam> <XVal>Name.Surname@email.com</XVal></Ext></Folder>"
#define TEST_STRING_B64 "SGVyZSBhbiBleGFtcGxlIG9mIHRleHQgdG8gYmUgaW4gYjY0OiA8Rm9sZGVyPjxuYW1lPkVtYWlsIEhvbWU8L25hbWU+PGNyZWF0ZWQ+MjAwOTA0MjhUMTYyNjU0WjxjcmVhdGVkPjxyb2xlPmFjY291bnQ8L3JvbGU+PEV4dD48WE5hbT5WaXNpYmxlTmFtZTwvWE5hbT4gPFhWYWw+TmFtZSBTdXJuYW1lPC9YVmFsPjwvRXh0PjxFeHQ+PFhOYW0+RW1haWxBZGRyZXNzPC9YTmFtPiA8WFZhbD5OYW1lLlN1cm5hbWVAZW1haWwuY29tPC9YVmFsPjwvRXh0PjwvRm9sZGVyPg=="


class ItemReaderTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(ItemReaderTest);
        CPPUNIT_TEST(testSimpleItemReaderb64);
        CPPUNIT_TEST(testItemReaderMultiChunkb64);  
        CPPUNIT_TEST(testSimpleItemReaderBin);
        CPPUNIT_TEST(testItemReaderMultiChunkBin);   
        CPPUNIT_TEST(testSimpleItemReaderDes);            
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(){        
        syncItem = new SyncItem(TEXT("Key1"));
        syncItem->setData(TEST_STRING, (long)strlen(TEST_STRING));        
    }

    void tearDown(){
        

    }

private:
        
    SyncItem* syncItem;

    void testSimpleItemReaderb64(){      
        int maxMsgSize = 1024;
        EncodingHelper helper("b64", NULL, NULL);
        ItemReader itemReader(maxMsgSize, helper);
        itemReader.setSyncItem(syncItem);
        Chunk* c = itemReader.getNextChunk(maxMsgSize);
        CPPUNIT_ASSERT( c != NULL );        
        CPPUNIT_ASSERT(!strcmp(c->getData(), TEST_STRING_B64));
        delete c;
    }

    void testItemReaderMultiChunkb64(){      
        int maxMsgSize = 101;
        EncodingHelper helper("b64", NULL, NULL);
        ItemReader itemReader(maxMsgSize, helper);
        itemReader.setSyncItem(syncItem);
        StringBuffer result;
        Chunk* c = NULL;
        int i = 0;
        do {    
            delete c;
            c = itemReader.getNextChunk(maxMsgSize);            
            i++;
            result.append(c->getData());
            if (c && c->isLast()) {
                break;
            }
        } while(c);

        CPPUNIT_ASSERT(i == 4);        
        CPPUNIT_ASSERT(result == TEST_STRING_B64);
        delete c;
    }
    
    void testSimpleItemReaderBin(){      
        int maxMsgSize = 1024;
        EncodingHelper helper("bin", NULL, NULL);
        ItemReader itemReader(maxMsgSize, helper);
        itemReader.setSyncItem(syncItem);
        Chunk* c = itemReader.getNextChunk(maxMsgSize);
        CPPUNIT_ASSERT( c != NULL );        
        CPPUNIT_ASSERT(!strcmp(c->getData(), TEST_STRING));
        delete c;
    }
    
    void testItemReaderMultiChunkBin(){      
        int maxMsgSize = 101;
        EncodingHelper helper("bin", NULL, NULL);
        ItemReader itemReader(maxMsgSize, helper);
        itemReader.setSyncItem(syncItem);
        StringBuffer result;
        Chunk* c = NULL;
        int i = 0;
        do {    
            delete c;
            c = itemReader.getNextChunk(maxMsgSize);            
            i++;
            result.append(c->getData());
            if (c && c->isLast()) {
                break;
            }
        } while(c);

        CPPUNIT_ASSERT(i == 3);        
        CPPUNIT_ASSERT(result == TEST_STRING);
        delete c;
    }
    
    char* getEncodedWithDes(const char* password) {
        
        DataTransformer* b64e;
        DataTransformer* dese;       

        b64e = DataTransformerFactory::getEncoder("b64");
        dese = DataTransformerFactory::getEncoder("des");
        
        TransformationInfo infoe;

        infoe.size = (long)strlen(TEST_STRING)*sizeof(char);
        infoe.password = password;

        char* desText = dese->transform(TEST_STRING, infoe);
        char* b64Text = b64e->transform(desText, infoe);

        delete [] desText;  desText = NULL;
        return b64Text;
       
    }

    void testSimpleItemReaderDes(){      
        int maxMsgSize = 1024;
        char* result = getEncodedWithDes("test");
        EncodingHelper helper("b64", "des", "test");
        ItemReader itemReader(maxMsgSize, helper);
        itemReader.setSyncItem(syncItem);
        Chunk* c = itemReader.getNextChunk(maxMsgSize);
        CPPUNIT_ASSERT( c != NULL );        
        CPPUNIT_ASSERT(!strcmp(c->getData(), result));
        delete c;
        delete [] result;
    }
    
    void testItemReaderMultiChunkDes(){      
        int maxMsgSize = 96;
        char* result = getEncodedWithDes("test");
        EncodingHelper helper("b64", "des", "test");
        ItemReader itemReader(maxMsgSize, helper);
        itemReader.setSyncItem(syncItem);
        StringBuffer res;
        Chunk* c = NULL;
        int i = 0;
        do {    
            delete c;
            c = itemReader.getNextChunk(maxMsgSize);            
            i++;
            res.append(c->getData());
            if (c && c->isLast()) {
                break;
            }
        } while(c);
        
        CPPUNIT_ASSERT_EQUAL(i,5);
        CPPUNIT_ASSERT(!strcmp(result, res.c_str()));
        delete c;
        delete [] result;
    }
    

};

CPPUNIT_TEST_SUITE_REGISTRATION(ItemReaderTest);