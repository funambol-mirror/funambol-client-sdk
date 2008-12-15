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

/** @cond API */
/** @addtogroup ClientTest */
/** @{ */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif


#include <string>
#include <stdexcept>

#include <cppunit/CompilerOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestListener.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFailure.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <stdlib.h>
#include <stdio.h>

#include "base/globalsdef.h"
#include <base/Log.h>

USE_NAMESPACE

#include "TestFileSource.h"

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif

using namespace std;

void simplifyFilename(string &filename)
{
    size_t pos = 0;
    while (true) {
        pos = filename.find(":", pos);
        if (pos == filename.npos ) {
            break;
        }
        filename.replace(pos, 1, "_");
    }
}

class ClientOutputter : public CppUnit::CompilerOutputter {
public:
    ClientOutputter(CppUnit::TestResultCollector *result, std::ostream &stream) :
        CompilerOutputter(result, stream) {}
    void write() {
        // ensure that output goes to console again
        LOG.setLogName("test.log");
        CompilerOutputter::write();
    }
};

class ClientListener : public CppUnit::TestListener {
public:
    ClientListener() :
        m_failed(false) {
#ifdef HAVE_SIGNAL_H
        // install signal handler which turns an alarm signal into a runtime exception
        // to abort tests which run too long
        const char *alarm = getenv("CLIENT_TEST_ALARM");
        m_alarmSeconds = alarm ? atoi(alarm) : -1;

        struct sigaction action;
        memset(&action, 0, sizeof(action));
        action.sa_handler = alarmTriggered;
        action.sa_flags = SA_NODEFER;
        sigaction(SIGALRM, &action, NULL);
#endif
    }

    void addAllowedFailures(string allowedFailures) {
        size_t start = 0, end;
        while ((end = allowedFailures.find(',', start)) != allowedFailures.npos) {
            size_t len = end - start;
            if (len) {
                m_allowedFailures.insert(allowedFailures.substr(start, len));
            }
            start = end + 1;
        }
        if (allowedFailures.size() > start) {
            m_allowedFailures.insert(allowedFailures.substr(start));
        }
    }

    void startTest (CppUnit::Test *test) {
        m_currentTest = test->getName();
        LOG.setLogName("test.log");
        cerr << m_currentTest;
        string logfile = m_currentTest + ".log";
        simplifyFilename(logfile);
        remove(logfile.c_str());
        LOG.setLogName(logfile.c_str());
        m_testFailed = false;

#ifdef HAVE_SIGNAL_H
        if (m_alarmSeconds > 0) {
            alarm(m_alarmSeconds);
        }
#endif
    }

    void addFailure(const CppUnit::TestFailure &failure) {
        m_testFailed = true;
    }

    void endTest (CppUnit::Test *test) {
#ifdef HAVE_SIGNAL_H
        if (m_alarmSeconds > 0) {
            alarm(0);
        }
#endif

        LOG.setLogName("test.log");
        if (m_testFailed) {
            if (m_allowedFailures.find(m_currentTest) == m_allowedFailures.end()) {
                cerr << " *** failed ***";
                m_failed = true;
            } else {
                cerr << " *** failure ignored ***";
            }
        }
        cerr << "\n";
    }

    bool hasFailed() { return m_failed; }
    const string &getCurrentTest() const { return m_currentTest; }

private:
    set<string> m_allowedFailures;
    bool m_failed, m_testFailed;
    string m_currentTest;
    int m_alarmSeconds;

    static void alarmTriggered(int signal) {
        CPPUNIT_ASSERT_MESSAGE("test timed out", false);
    }
} syncListener;

const string &getCurrentTest() {
    return syncListener.getCurrentTest();
}

static void printTests(CppUnit::Test *test, int indention)
{
    if (!test) {
        return;
    }

    std::string name = test->getName();
    printf("%*s%s\n", indention * 3, "", name.c_str());
    for (int i = 0; i < test->getChildTestCount(); i++) {
        printTests(test->getChildTestAt(i), indention+1);
    }
}


int main(int argc, char* argv[])
{
  TestFileSource testFileSource("A");
  
  testFileSource.registerTests();

  // Get the top level suite from the registry
  CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

  if (argc >= 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
      printf("usage: %s [test name]+\n\n"
             "Without arguments all available tests are run.\n"
             "Otherwise only the tests or group of tests listed are run.\n"
             "Here is the test hierarchy of this test program:\n",
             argv[0]);
      printTests(suite, 1);
      return 0;
  }

  // Adds the test to the list of test to run
  CppUnit::TextUi::TestRunner runner;
  runner.addTest( suite );

  // Change the default outputter to a compiler error format outputter
  runner.setOutputter( new ClientOutputter( &runner.result(),
                                            std::cerr ) );

  // track current test and failure state
  const char *allowedFailures = getenv("CLIENT_TEST_FAILURES");
  if (allowedFailures) {
      syncListener.addAllowedFailures(allowedFailures);
  }
  runner.eventManager().addListener(&syncListener);

  try {
      // Run the tests.
      if (argc <= 1) {
          // all tests
          runner.run("", false, true, false);
      } else {
          // run selected tests individually
          for (int test = 1; test < argc; test++) {
              runner.run(argv[test], false, true, false);
          }
      }

      // Return error code 1 if the one of test failed.
      return syncListener.hasFailed() ? 1 : 0;
  } catch (invalid_argument e) {
      // Test path not resolved
      std::cerr << std::endl
                << "ERROR: " << e.what()
                << std::endl;
      return 1;
  }
}

/** @} */
/** @endcond */
