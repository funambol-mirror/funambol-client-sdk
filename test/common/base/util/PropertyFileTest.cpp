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

#include "base/globalsdef.h"
#include "base/util/PropertyFile.h"

USE_NAMESPACE

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class PropertyFileTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PropertyFileTest);
    CPPUNIT_TEST(testSetProperty);
    CPPUNIT_TEST(testGetProperties);
    CPPUNIT_TEST(testRemoveProperty);
    CPPUNIT_TEST(testRemoveAll);
    CPPUNIT_TEST(testSetPropertyFailsStorage);
    CPPUNIT_TEST(testGetPropertiesFromStorage); 
    CPPUNIT_TEST(testEscapeLinesFunciton); 
    
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        propFile = new PropertyFile("test.properties");
        CPPUNIT_ASSERT(propFile);
    }

    void tearDown() {
        delete propFile;
    }

protected:
    void testSetProperty() {
        
        // Write a value into the property file
        propFile->setPropertyValue("property", "value");
        propFile->setPropertyValue(" property space ", " value space ");
        propFile->close();
        // Now read back
        StringBuffer value = propFile->readPropertyValue("property");
        CPPUNIT_ASSERT(value == "value");
        StringBuffer valueSpace = propFile->readPropertyValue("property space");
        CPPUNIT_ASSERT(valueSpace == "value space");

    }

    void testGetProperties() {
        // Add three properties
        propFile->setPropertyValue("property1", "value1");
        propFile->setPropertyValue("property2", "value2");
        propFile->setPropertyValue("property3", "value3");
        propFile->setPropertyValue(" property5 ", " value5 ");
        // Now iterate on them
        Enumeration& enumeration = propFile->getProperties();
        bool prop1 = false, prop2 = false, prop3 = false, prop4 = false;
        while (enumeration.hasMoreElement()) {
            KeyValuePair* item = (KeyValuePair*)enumeration.getNextElement();
            const char* key = item->getKey();
            const char* value = item->getValue();
            CPPUNIT_ASSERT(value != NULL && strlen(value) > 0);
            if (strcmp(key, "property1") == 0) {
                prop1 = true;
            } else if (strcmp(key, "property2") == 0) {
                prop2 = true;
            } else if (strcmp(key, "property3") == 0) {
                prop3 = true;
            } else if (strcmp(key, "property5") == 0) {
                prop4 = true;
            }
        }
        propFile->close();
        CPPUNIT_ASSERT(prop1 && prop2 && prop3 && prop4);
    }

    void testRemoveProperty() {
        // Write a value into the property file
        propFile->setPropertyValue("property4", "value4");
        propFile->setPropertyValue("  space  ", "  val space ");
        propFile->close();
        // Remove it
        int success = propFile->removeProperty("property4");
        CPPUNIT_ASSERT(success == 0);
        // Now read back
        StringBuffer value = propFile->readPropertyValue("property4");
        CPPUNIT_ASSERT(value.empty());
        
        StringBuffer valueS = propFile->readPropertyValue("space");
        CPPUNIT_ASSERT(value.empty());

    }

    void testRemoveAll() {
        // Iterate on all properties
        Enumeration& enumeration = propFile->getProperties();
        while (enumeration.hasMoreElement()) {
            KeyValuePair* item = (KeyValuePair*)enumeration.getNextElement();
            const char* key = item->getKey();
            int success = propFile->removeProperty(key);
            CPPUNIT_ASSERT(success == 0);
        }
        propFile->close();
        // Now iterate again, and we should have now more elements
        enumeration = propFile->getProperties();
        CPPUNIT_ASSERT(!enumeration.hasMoreElement());
    }
    
    /**
    * it doesn't close the storage
    */
    void testSetPropertyFailsStorage() {
        
        // Write a value into the property file
        propFile->setPropertyValue("property1", "value1");
        propFile->setPropertyValue("property2", "value2");
        // overwrite the property1
        propFile->setPropertyValue("property1", "valueNew");  
        propFile->setPropertyValue("1234", "5678"); 
        propFile->setPropertyValue("123=4", "567=8");        
        propFile->setPropertyValue("1234=", "567=8");
        propFile->setPropertyValue("1=234=", "5=67=8");
        propFile->setPropertyValue(" 678= ", "  9=10=11  ");

        Enumeration& enumeration = propFile->getProperties();
        int i = 0;
        while (enumeration.hasMoreElement()) {
            i++;
            enumeration.getNextElement();
        }
        CPPUNIT_ASSERT(i == 7);
        
    }
    
    void testGetPropertiesFromStorage() {        
        // Now iterate on them
        Enumeration& enumeration = propFile->getProperties();
        bool prop1 = false, prop2 = false, prop3 = false, prop4 = false, prop5 = false, prop6 = false;
        while (enumeration.hasMoreElement()) {
            KeyValuePair* item = (KeyValuePair*)enumeration.getNextElement();
            const char* key = item->getKey();
            const char* value = item->getValue();
            CPPUNIT_ASSERT(value != NULL && strlen(value) > 0);
            if (strcmp(key, "property1") == 0 && strcmp(value, "valueNew") == 0 ) {
                prop1 = true;
            } else if (strcmp(key, "property2") == 0 && strcmp(value, "value2") == 0 ) {
                prop2 = true;
            } else if (strcmp(key, "1234") == 0 && strcmp(value, "5678") == 0 ) {
                prop3 = true;
            } else if (strcmp(key, "123=4") == 0 && strcmp(value, "567=8") == 0 ) {
                prop4 = true;
            }  else if (strcmp(key, "1234=") == 0 && strcmp(value, "567=8") == 0 ) {
                prop5 = true;
            }  else if (strcmp(key, "1=234=") == 0 && strcmp(value, "5=67=8") == 0 ) {
                prop6 = true;
            }  else if (strcmp(key, "678=") == 0 && strcmp(value, "9=10=11") == 0 ) {
                prop6 = true;
            }   

        }
        propFile->close();
        CPPUNIT_ASSERT(prop1 && prop2 && prop3 && prop4 && prop5 && prop6);        
    }

    
    void testEscapeLinesFunciton() {       
        StringBuffer a("1234=5678\n");
        StringBuffer b("123\\=4=567\\=8\n");
        StringBuffer c("1234\\==567\\=8\n");
        StringBuffer d("1234\\==  567\\=8 \n");
        StringBuffer e(" 1234 = 567\\=8 \n");
        StringBuffer f(" 12 34 = 56 7\\=8 \n");
        StringBuffer g(" 12 34 = 56 7\\=8 \\=   \n");
        
        StringBuffer key;
        StringBuffer value;

        propFile->separateKeyValue(a, key, value);
        CPPUNIT_ASSERT(key.trim() == "1234");
        CPPUNIT_ASSERT(value.trim() == "5678");        
        propFile->separateKeyValue(b, key, value);        
        CPPUNIT_ASSERT(key.trim() == "123=4");
        CPPUNIT_ASSERT(value.trim() == "567=8");        
        propFile->separateKeyValue(c, key, value);
        CPPUNIT_ASSERT(key.trim() == "1234=");
        CPPUNIT_ASSERT(value.trim() == "567=8");        
        propFile->separateKeyValue(d, key, value);
        CPPUNIT_ASSERT(key.trim() == "1234=");
        CPPUNIT_ASSERT(value.trim() == "567=8");
        propFile->separateKeyValue(e, key, value);
        CPPUNIT_ASSERT(key.trim() == "1234");
        CPPUNIT_ASSERT(value.trim() == "567=8");
        propFile->separateKeyValue(f, key, value);
        CPPUNIT_ASSERT(key.trim() == "12 34");
        CPPUNIT_ASSERT(value.trim() == "56 7=8");
        propFile->separateKeyValue(g, key, value);
        CPPUNIT_ASSERT(key.trim() == "12 34");
        CPPUNIT_ASSERT(value.trim() == "56 7=8 =");
       
        propFile->close();

    }

private:
    PropertyFile *propFile;
};

CPPUNIT_TEST_SUITE_REGISTRATION( PropertyFileTest );

