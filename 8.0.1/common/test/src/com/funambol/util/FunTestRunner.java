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
package com.funambol.util;

import j2meunit.midletui.TestRunner;

import j2meunit.framework.AssertionFailedError;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestFailure;
import j2meunit.framework.TestListener;
import j2meunit.framework.TestResult;
import j2meunit.framework.TestSuite;
import j2meunit.util.StringUtil;
import j2meunit.util.Version;

import java.io.PrintStream;
import java.util.Enumeration;

import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Gauge;
import javax.microedition.lcdui.List;
import javax.microedition.lcdui.StringItem;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

import java.util.Vector;

public class FunTestRunner extends TestRunner {
    public FunTestRunner() {
        super();
    }

    protected void startApp() throws MIDletStateChangeException
    {
        try
        {
            String sTestClasses = getAppProperty("J2MEUnitTestClasses");
            // Build the test list
            int comma = sTestClasses.indexOf(',');
            int beginning = 0;
            Vector tests = new Vector(10);
            while (comma != -1) {
                String test = sTestClasses.substring(beginning, comma);
                tests.addElement(test);
                beginning   = comma + 1;
                comma = sTestClasses.indexOf(',', beginning);
            }
            String test = sTestClasses.substring(beginning);
            tests.addElement(test);

            System.out.println("Number of tests: " + tests.size());

            String list[] = new String[tests.size()];
            for(int i = 0; i<tests.size();++i) {
                list[i] = (String)tests.elementAt(i);
                System.out.println("Test " + i + " is: " + list[i]);
            }
            start(list);
        } catch (Exception e) {
            System.out.println("Exception while setting up tests: " + e);
            e.printStackTrace();
        }
    }

    protected void start(String[] rTestCaseClasses)
    {
        final Test    aTestSuite = createTestSuite(rTestCaseClasses);
        final Display rDisplay = Display.getDisplay(this);
        Form  aForm    = new Form("TestRunner");

        nCount = aTestSuite.countTestSteps();

        aProgressBar = new Gauge(null, false, nCount, 0);
        aFailureInfo = new StringItem("Failures:", "0");
        aErrorInfo   = new StringItem("Errors:", "0");

        aForm.append("Testing...");
        aForm.append(aProgressBar);
        aForm.append(aFailureInfo);
        aForm.append(aErrorInfo);
        rDisplay.setCurrent(aForm);

        Thread t = new Thread()
        {
            public void run() {
                try {
                    doRun(aTestSuite);
                    showResult();
                } catch (Exception e) {
                    System.out.println("Exception while running test: " + e);
                    e.printStackTrace();
                }
            }
        };
        
        t.start();
    }

    protected void destroyApp(boolean bUnconditional)
    {
        notifyDestroyed();
    }

}

