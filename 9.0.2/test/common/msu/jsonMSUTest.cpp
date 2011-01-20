/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2010 Funambol, Inc.
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

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "base/util/StringBuffer.h"
#include "base/Log.h"
#include "msu/MSUDeviceInfo.h"
#include "msu/JsonMSUMessage.h"
#include "cJSON.h"

static const char* loginSapiResponseMessage =
"{\"data\":{\n\"jsessionid\":\"ED8F3F87C5B0927222C913BD7889A690\",\n\"roles\":[\n                                           \
         {\n                                                  \
            \"name\":\"standard\",\n                          \
            \"description\":\"\"\n                            \
         },\n                                                 \
         {\n                                                  \
            \"name\":\"sync_user\",\n                         \
            \"description\":\"\"\n                            \
         }\n                                                  \
         ]\n                                                  \
    }\n                                                       \
}";

static const char* loginSapiResponseMessage2 =
"{\"data\":{\n                                                \
      \"roles\":[\n                                           \
         {\n                                                  \
            \"name\":\"standard\",\n                          \
            \"description\":\"\"\n                            \
         },\n                                                 \
         {\n                                                  \
            \"name\":\"sync_user\",\n                         \
            \"description\":\"\"\n                            \
         }\n                                                  \
         ]\n                                                  \
      }\n                                                     \
    }\n                                                       \
}";


static const char* signUpSapiResponseMessage = "{\"data\":{\n \"user\":{ \"active\":\"true\"\n }\n }\n }";
 
static const char* signUpSapiResponseMessage2 =
"{\"data\":{\n                          \
      \"user\":{ \"active\":\"false\"\n \
      }\n                               \
   }\n                                  \
}";
 
static const char* signUpSapiRequestMessage = 
"{\"data\":{\n                                \
    \"user\":{\n                              \
        \"phonenumber\":\"+1234300000\",\n    \
        \"password\": \"12345\",\n            \
        \"platform\":\"iphone\",\n            \
        \"manufacturer\": \"Apple\",\n        \
        \"model\": \"3GS\",\n                 \
        \"carrier\":\"Vodafone\",\n           \
        \"countrya2\":\"IT\"\n                \
      }\n                                     \
   }\n                                        \
}";

static const char* captchaUrlSapiResponseMessage = 
"{\"data\":{\n\"captchaurl\":{\n\"portalurl\":\"http://dogfood.funambol.com:8080\",\n\"imagepath\":\"/Captcha.jpg\",\n\"active\":\"true\"\n}\n}\n}";

static const char* captchaUrlSapiResponseMessage2 = 
"{\"data\":{\n                                               \
      \"captchaurl\":{\n                                     \
            \"portalurl\":\"http://my.server.com:8080\",\n   \
            \"imagepath\":\"/Captcha.jpg\",\n                \
            \"active\":\"false\"\n                           \
      }\n                                                    \
   }\n                                                       \
}";

static const char* sapiJsonErrorMessage = 
"{\n                                                                              \
   \"error\":{\n                                                                  \
      \"code\":\"COM-1002\",\n                                                    \
      \"message\":\"Country not found\",\n                                        \
      \"parameters\":[],\n                                                        \
      \"cause\":\"com.funambol.portal.api.system.CountryNotFoundException\"\n     \
   }\n                                                                            \
}";

USE_FUNAMBOL_NAMESPACE

/**
 * This is a test class for the Mobile Sign Up Json object handling
 */
class jsonMSUTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(jsonMSUTest);
    CPPUNIT_TEST(MSUloginTest);
    CPPUNIT_TEST(MSUSignUpTest);
    CPPUNIT_TEST(MSUCaptchaUrlTest);
    CPPUNIT_TEST(MSUFormatSignUpTest);
    CPPUNIT_TEST(MSUJsonErrorTest);
    CPPUNIT_TEST_SUITE_END();

private:
    JsonMSUMessage* jsonMessage;

public:

    void setUp()    {
        jsonMessage = new JsonMSUMessage();
    }

    void tearDown() {
        delete jsonMessage;
    }

private:

    void MSUloginTest() {
        bool ret = false;
       
        ret = jsonMessage->parseLogin(loginSapiResponseMessage);
        CPPUNIT_ASSERT(ret);

        LOG.info("parsing invalid JSON message without session ID...");
        ret = jsonMessage->parseLogin(loginSapiResponseMessage2);
        CPPUNIT_ASSERT(ret == false);
     }

    
    void MSUSignUpTest() {
        bool ret = false;

        ret = jsonMessage->parseSignUp(signUpSapiResponseMessage);
        CPPUNIT_ASSERT(ret);
       
        ret = jsonMessage->parseSignUp(signUpSapiResponseMessage2);
        CPPUNIT_ASSERT(ret == false);
    }

    void MSUCaptchaUrlTest() {
        bool ret = false;
        char* captchaUrl = NULL;

        ret = jsonMessage->parseCaptchaUrl(captchaUrlSapiResponseMessage, &captchaUrl);
        CPPUNIT_ASSERT(ret);
        CPPUNIT_ASSERT(captchaUrl); 
        LOG.info("%s - captcha URL: %s", __FUNCTION__, captchaUrl);
        CPPUNIT_ASSERT(strcmp(captchaUrl, "http://dogfood.funambol.com:8080/Captcha.jpg") == 0);
        
        free(captchaUrl);

        ret = jsonMessage->parseCaptchaUrl(captchaUrlSapiResponseMessage2, &captchaUrl);
        CPPUNIT_ASSERT(ret == false);
    }

    void MSUFormatSignUpTest() {
        bool ret = false;
        const char* signUpMessage = NULL;
        const char* formattedMessage = NULL;
        cJSON* jsonObj = NULL;
        MSUDeviceInfo* devInfo = new MSUDeviceInfo("+1234300000", "12345", "iphone");

        devInfo->setManufacturer("Apple");
        devInfo->setModel("3GS");
        devInfo->setCarrier("Vodafone");
        devInfo->setCountryCodeA2("IT");
       
        // check JSON object formatting 
        signUpMessage = jsonMessage->formatSignUp(devInfo);
        CPPUNIT_ASSERT(signUpMessage);
        
        LOG.info("sign up request message: %s", signUpMessage);
        jsonObj = cJSON_Parse(signUpSapiRequestMessage);
        CPPUNIT_ASSERT(jsonObj);
        formattedMessage = cJSON_PrintUnformatted(jsonObj);
        CPPUNIT_ASSERT(formattedMessage);
        LOG.info("formatted object: %s", formattedMessage);
      
        CPPUNIT_ASSERT(strcmp(signUpMessage, formattedMessage) == 0); 
        
        free((char *)signUpMessage);
        free((char *)formattedMessage);

        signUpMessage    = NULL;
        formattedMessage = NULL; 
        // check JSON object formatting with pretty print
        signUpMessage = jsonMessage->formatSignUp(devInfo, true);
        CPPUNIT_ASSERT(signUpMessage);
        LOG.info("sign up request message (with pretty print): %s", signUpMessage);

        formattedMessage = cJSON_Print(jsonObj);
        CPPUNIT_ASSERT(formattedMessage);
        LOG.info("formatted object (with pretty print): %s", formattedMessage);
      
        CPPUNIT_ASSERT(strcmp(signUpMessage, formattedMessage) == 0); 

        free((char *)signUpMessage);
        free((char *)formattedMessage);
        
        delete devInfo;
        cJSON_Delete(jsonObj);
    }

    void MSUJsonErrorTest() {
        bool ret = false;
        char* captchaUrl = NULL;
        StringBuffer errorCode;
        StringBuffer errorMessage;

        ret = jsonMessage->parseLogin(sapiJsonErrorMessage);
        CPPUNIT_ASSERT(ret == false);
        
        errorCode = jsonMessage->getErrorCode();
        CPPUNIT_ASSERT(errorCode);
        
        errorMessage = jsonMessage->getErrorMessage();
        CPPUNIT_ASSERT(errorMessage);

        CPPUNIT_ASSERT(strcmp(errorCode.c_str(), "COM-1002") == 0);
        CPPUNIT_ASSERT(strcmp(errorMessage.c_str(), "Country not found") == 0);
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( jsonMSUTest );
