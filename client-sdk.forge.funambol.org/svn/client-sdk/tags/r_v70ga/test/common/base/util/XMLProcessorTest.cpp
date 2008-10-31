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
#include "base/util/XMLProcessor.h"
#include "base/globalsdef.h"

USE_NAMESPACE


class XMLProcessorTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(XMLProcessorTest);

    CPPUNIT_TEST(testXMLProcessor);
    CPPUNIT_TEST(testXMLProcessorAmp);
    CPPUNIT_TEST_SUITE_END();


private:
    
    void testXMLProcessorAmp(){
        //
        // the final string must be
        // hello < & > &lt; &gt; &amp;
        //
        const char target[] = "hello < & > &lt; &gt; &amp;";
        const char xml[] =
        "<document>\n\
        <LocURI>./devinf11</LocURI>\n\
        <Data>hello &lt; &amp; &gt; &amp;lt; &amp;gt; &amp;amp;</Data>\n\
        </document>" ;
        
        unsigned int pos = 0;        

        // Get 'document' tag
        char *doc = XMLProcessor::copyElementContent(xml, "Data", &pos);
        if (strcmp(target, doc) != 0) {
            CPPUNIT_ASSERT_MESSAGE("TEST FAILED.", false);            
        }        
        
        delete [] doc;
        pos = 0;

        const char target1[] = "hello &lt; &amp; &gt; &amp;lt; &amp;gt; &amp;amp;";
        const char xml1[] =
        "<document>\n\
        <LocURI>./devinf11</LocURI>\n\
        <Data><![CDATA[hello &lt; &amp; &gt; &amp;lt; &amp;gt; &amp;amp;]]></Data>\n\
        </document>" ;
        
        doc = XMLProcessor::copyElementContent(xml1, "Data", &pos);
        if (strcmp(target1, doc) != 0) {
            CPPUNIT_ASSERT_MESSAGE("TEST FAILED.", false);            
        }  

    }

    void testXMLProcessor(){
        const char xml1[] =
            "<document>\n\
            <LocURI>./devinf11</LocURI>\n\
            <plaintag>\n\
            <attrtag attr=\"val\">content</attrtag>\n\
            </plaintag>\n\
            <emptytag/>\n\
            </document>" ;

        unsigned int pos = 0, start = 0, end = 0;
        const char *p = 0;

        // Get 'document' tag
        char *doc = XMLProcessor::copyElementContent(xml1, "document", &pos);
        //LOG.debug("Document: '%s'", doc);
        //LOG.debug("xml[pos]= '%s'", xml1 + pos);

        char buf[256];

        // Get 'plaintag' content, using start/end pos
        if(!XMLProcessor::getElementContent(doc, "plaintag", &pos, &start, &end)){
            CPPUNIT_ASSERT_MESSAGE("TEST FAILED.", false);
            return;
        }
        memset(buf, 0, 255);
        memcpy(buf, doc+start, end-start);
        //LOG.debug("Plaintag: '%s'", buf);

        // Get 'LocURI' content, using start/end pos
        if(!XMLProcessor::getElementContent(doc, "LocURI", &pos, &start, &end)){
            CPPUNIT_ASSERT_MESSAGE("TEST FAILED.", false);
            return;
        }
        memset(buf, 0, 255);
        memcpy(buf, doc+start, end-start);
        //LOG.debug("LocURI: '%s'", buf);

        // Get 'attrtag' content, using start/end pos
        if(!XMLProcessor::getElementContent(doc, "attrtag", &pos, &start, &end)){
            CPPUNIT_ASSERT_MESSAGE("TEST FAILED.",false);
            return;
        }
        memset(buf, 0, 255);
        memcpy(buf, doc+start, end-start);
        //LOG.debug("Attrtag: '%s'", buf);

        // Get 'attrtag' attr list, using start/end pos
        if(!XMLProcessor::getElementAttributes(doc, "attrtag", &start, &end)){
            CPPUNIT_ASSERT_MESSAGE("TEST FAILED.", false);
            return;
        }
        memset(buf, 0, 255);
        memcpy(buf, doc+start, end-start);
        //LOG.debug("Attrlist: '%s'", buf);

        // Get 'emptytag' content, that should be empty
        const char*empty = XMLProcessor::copyElementContent(doc, "emptytag");
        if(!empty){
            CPPUNIT_ASSERT_MESSAGE("TEST FAILED.", false);
            return;
        }
        //LOG.debug("Emptytag: '%s'", empty);

        if(doc)
            delete [] doc;
        if (empty)
            delete [] empty;
    }



};

CPPUNIT_TEST_SUITE_REGISTRATION( XMLProcessorTest );