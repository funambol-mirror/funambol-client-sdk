/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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
#include "client/OptionParser.h"

USE_NAMESPACE

class OptionParserTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(OptionParserTest);
    CPPUNIT_TEST(testUsage);
    CPPUNIT_TEST(testParseGood);
    CPPUNIT_TEST(testParseBad);
    CPPUNIT_TEST_SUITE_END();

public:

    OptionParserTest() : parser("testparse") {};

    void setUp() {
        parser.addOption('v', "verbose", "increase verbosity level");
        parser.addOption('q', "quiet", "decrease verbosity level", false);
        parser.addOption('c', "config", "set config file to use", true);

        parser.addArgument("infile", "input file");
        parser.addArgument("outfile", "output file", false);
    }

    void tearDown() {
        parser.clearArguments();
        parser.removeOption("verbose");
        parser.removeOption("quiet");
        parser.removeOption("config");
    }

private:

    void testUsage() {
        const char *argv[] = { "testparse", "-h", 0 };
        parser.parse(2, argv, opts, args);
    }

    void testParseGood() {
        bool ret;

        const char *argv1[] = { "testparse", "-v", "inputfile", "outputfile", 0 };
        ret = parser.parse(4, argv1, opts, args);
        CPPUNIT_ASSERT(ret);

        const char *argv2[] = { "testparse", "--config", "configfile", "inputfile", 0 };
        ret = parser.parse(4, argv2, opts, args);
        CPPUNIT_ASSERT(ret);
    }

    void testParseBad() {
        int ret;

        const char *argv1[] = { "testparse", "-x", "inputfile", "outputfile", 0 };
        ret = parser.parse(4, argv1, opts, args);
        CPPUNIT_ASSERT(ret =! 0 );

        const char *argv2[] = { "testparse", "--config", 0 };
        ret = parser.parse(2, argv2, opts, args);
        CPPUNIT_ASSERT(ret =! 0);
    }

    OptionParser parser;
    StringMap opts;
    ArrayList args;

};

CPPUNIT_TEST_SUITE_REGISTRATION( OptionParserTest );
