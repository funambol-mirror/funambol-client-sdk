/* -*-C-*- */
/** @cond API */

/**
 * @defgroup ClientTest Client Testing Framework
 *
 * The testing framework of the C++ client library is based on CPPUnit.
 * It fulfills several different goals:
 * - testing of the library itself
 *   - unit testing of individual classes
 *   - full end-to-end tests of synchronization in different configurations
 * - regular testing of a SyncML server
 * - testing of SyncML clients built on top of the library
 *
 * Some of the design goals were:
 * - a simple, unified test runner on all platforms
 * - it should be possible to add tests either directly in
 *   the .cpp which implements a class or in a separate file;
 *   if done in the class .cpp file then the extra code
 *   should be protected by an ifdef so that it is possible
 *   to choose whether it is in the released library or not
 * - adding tests should not require changes in a central file
 *
 * The test runner "client-test" is implemented in
 * client-test-main.cpp. It automatically assembles all CPPUnit tests
 * it was linked against and runs them all if started without
 * parameters. Output is ASCII. The return code indicates success or
 * failure of any test. "-h|--help" print some usage
 * information and a full list of all available tests.  Tests or test
 * groups as printed in that list can be given as command line
 * parameters to run just these tests. Because tests do not
 * communicate directly with the command line front-end, they must
 * be passed parameters via environment variables. See the
 * description of TestFileSource for further information about running
 * those end-to-end synchronization tests.
 *
 * The test runner itself also understands some environment variables:
 * CLIENT_TEST_FAILURES can be set to a comma separated list of tests
 * which are allowed to fail without affecting the return code of the
 * test runner.
 *
 * On Linux the test runner supports setting a timeout which is triggered
 * if a test runs for more than the number of seconds set in the
 * environment variable CLIENT_TEST_ALARM. This relies on signals and
 * therefore is disabled in compilations on Windows with an ifdef
 * HAVE_SIGNALS_H.
 *
 * Because of the dependency on CPPUnit the testing needs to be
 * enabled explicitly. This can be done separately for tests embedded
 * in the object files of the library (unit tests) and the end-to-end
 * synchronization (integration tests) which can be used with release
 * versions of the library.
 *
 * On POSIX systems the configure --enable-integration-test and
 * --enable-unit-tests switches enable the testing. "make check"
 * automatically executes the unit tests.
 *
 * On Windows the Visual Studio project file build/win32/cppunit.sln
 * was prepared to build the "client-test" executable. Because it uses
 * the standard win32.vcproj, some changes are necessary to enable
 * unit tests:
 * - replace DISABLE_UNIT_TESTS with ENABLE_UNIT_TESTS in the preprocessor settings
 * - put cppunit-1.12.0 into the same directory as the 3x directory that contains
 *   the client-api/native directory (other layouts are of course possible,
 *   just change the include and library search paths)
 *
 * Unit tests are only found if they are pulled into the
 * executable. On Linux this is done by searching for variables with a
 * special name, created by the FUNAMBOL_TEST_SUITE_REGISTRATION()
 * macro in test.h, and setting link flags on the fly. On Windows an
 * additional C file with references to these variables is used
 * instead.  This is done by a Python script, thus python.exe must
 * be installed and found in the search path of Visual Studio. Any
 * suggestion how to avoid the dependency on external scripting tools
 * without having to manually register tests in a central file is
 * welcome...
 *
 * The integration tests depend on auxiliary files
 * (test/synccompare.pl and test/testcases) which must be found in the
 * directory where the test is run. On Linux the makefile rules
 * automatically copy these, but the Visual Studio project file does
 * not. Note that test/synccompare.pl is written in Perl (and thus
 * creates another external dependency) because a) C/C++ do not have a
 * standard library to implement the complex regular pattern
 * matching/replace done in that script and b) the script is also
 * meant to be shipped with clients (SyncEvolution does that) and thus
 * needs to run on as many systems as possible. Perl is still the most
 * universally available scripting language for that purpose.
 *
 * Adding new unit tests is best done by following the example in
 * src/c++/common/base/util/BasicTime.cpp.
 *
 * Regular testing of a server can be done by using the reference data
 * provided together with ClientTest and synchronizing the server
 * against simple files on the client side via a TestFileSource. This
 * is what enabling the integration tests adds to the "client-test"
 * binary.  By keeping the client side fixed regressions in the server
 * can be detected.  Regular testing of the client library can be done
 * by testing the current client library source against stable
 * servers.
 *
 * For testing a client based on the C++ library the client developer
 * needs to implement the ClientTest interface. There are some additional
 * requirements for the client's SyncSource implementations:
 * - they must support more than one local database so that the same
 *   client host running the "client-test" binary can act as two different
 *   clients
 * - they must support change tracking for more than just the server
 *   and accessing the databases via SyncSource instances so that the
 *   testing frame work can synchronize, then add/remove/update items
 *   and/or list changes without affecting the state of the client with
 *   respect to its server
 *
 * The TestFileSource is an example how the ClientTest interface could
 * be implemented.
 */

/** @endcond */
