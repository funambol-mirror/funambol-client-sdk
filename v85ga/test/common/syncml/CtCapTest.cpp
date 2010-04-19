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

#include "syncml/formatter/Formatter.h"
#include "syncml/parser/Parser.h"
#include "base/util/utils.h"
#include "syncml/core/CTCap.h"
#include "syncml/core/Property.h"
#include "syncml/core/PropParam.h"
#include "base/globalsdef.h"

USE_NAMESPACE

class CtCapTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(CtCapTest);
    CPPUNIT_TEST(basicTest);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp(){

        StringBuffer ctType("text/x-vcard");
        StringBuffer verCt("2.1");
        
        ArrayList* props = new ArrayList();
        ArrayList* valEnum = new ArrayList();

        StringBuffer t ("VCARD");
        valEnum->add(t);
        Property p1("BEGIN",NULL,NULL,NULL, false, valEnum, NULL, NULL);
        props->add(p1);

        Property p2("END",NULL,NULL,NULL, false, valEnum, NULL, NULL);
        props->add(p2);

        valEnum->clear();
        t = "2.1";
        valEnum->add(t);
        Property p3("VERSION",NULL,NULL,NULL, false, valEnum, NULL, NULL);
        props->add(p3);

        Property p4("N",NULL,NULL,NULL, false, NULL, NULL, NULL);
        props->add(p4);

        ArrayList* pp = new ArrayList();
        valEnum->clear();
        t = "VOICE,HOME";
        valEnum->add(t);
        t = "FAX,HOME";
        valEnum->add(t);
        t = "VOICE,CELL";
        valEnum->add(t);
        PropParam* pn = new PropParam("TYPE",NULL, valEnum, NULL);

        pp->add(*pn);

        Property p5("TEL",NULL,NULL,NULL, false, NULL, NULL, pp);
        props->add(p5);

        ctcap = new CTCap(ctType.c_str(), verCt.c_str(), false, *props);

        compareTo = "<CTCap><CTType>text/x-vcard</CTType><VerCT>2.1</VerCT><Property><PropName>BEGIN</PropName><ValEnum>VCARD</ValEnum></Property><Property><PropName>END</PropName><ValEnum>VCARD</ValEnum></Property><Property><PropName>VERSION</PropName><ValEnum>2.1</ValEnum></Property><Property><PropName>N</PropName></Property><Property><PropName>TEL</PropName><PropParam><ParamName>TYPE</ParamName><ValEnum>VOICE,HOME</ValEnum><ValEnum>FAX,HOME</ValEnum><ValEnum>VOICE,CELL</ValEnum></PropParam></Property></CTCap>";
    }

    void tearDown(){
        delete ctcap;
    }

private:

    CTCap* ctcap;
    StringBuffer compareTo;

    /**
     * Perform a basic test of the CtCap formatter, comparing the
     * generated string with an expected value.
     */
    void basicTest(){
        StringBuffer* res = Formatter::getCTCap(ctcap);
        res->replaceAll("\n","");
        CPPUNIT_ASSERT( compareTo == *res );

        delete res;
    }
};



CPPUNIT_TEST_SUITE_REGISTRATION( CtCapTest );

