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

/**
 * This file is used by source files which depend on CPPUnit and thus
 * the STL => using it here is allowed.
 */

#ifndef INCL_TEST_H
#define INCL_TEST_H
/** @cond DEV */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

// These defines are set in config.h when using the autotools
// mechanism and by the project setup on Windows. They control whether
// tests inside the libraries source code (ENABLE_UNIT_TESTS) or of
// the final library are enabled (ENABLE_INTEGRATION_TESTS).
#if defined(ENABLE_UNIT_TESTS) || defined(ENABLE_INTEGRATION_TESTS)

// make common macros like CPPUNIT_TEST_ASSERT() available */
# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>
# include <string>

// use this macros instead of the plain CPPUNIT_ one to ensure
// that the autotools Makefile and Visual Studio helper script can find
// all tests and link them into the test binary
#define FUNAMBOL_TEST_SUITE_REGISTRATION( ATestFixtureType ) \
    CPPUNIT_TEST_SUITE_REGISTRATION( ATestFixtureType ); \
    extern "C" { int funambolAutoRegisterRegistry ## ATestFixtureType = 12345; }


// until a better solution is found use the helper function from TestMain.cpp
// to get the name of the currently running test; beware, will contain colons
extern const std::string &getCurrentTest();

// removes special characters like colons and slashes
extern void simplifyFilename(std::string &filename);

#endif // ENABLE_UNIT_TESTS

/** @endcond */
#endif // INCL_TEST_H
