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

package com.funambol.platform;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;

import com.funambol.util.StringUtil;
import com.funambol.util.Log;

public class TestableHttpConnectionAdapter extends HttpConnectionAdapter {

    private static final String TAG_LOG = "TestableHttpConnectionAdapter";

    private String breakOnPhase;
    private int breakOnPos;

    public TestableHttpConnectionAdapter() {
    }

    public InputStream openInputStream() throws IOException {
        InputStream is = super.openInputStream();
        if (StringUtil.equalsIgnoreCase("receiving", breakOnPhase)) {
            TestableInputStream tis = new TestableInputStream(is);
            tis.breakOnByte(breakOnPos);
            is = tis;
        }
        return is;
    }

    public void execute(InputStream is) throws IOException {
        if (StringUtil.equalsIgnoreCase("sending", breakOnPhase)) {
            TestableInputStream tis = new TestableInputStream(is);
            tis.breakOnByte(breakOnPos);
            is = tis;
        }
        super.execute(is);
    }

    public void setBreakInfo(String phase, int breakOnPos) {
        this.breakOnPhase = phase;
        this.breakOnPos   = breakOnPos;
    }

    private class TestableInputStream extends InputStream {

        private InputStream is;
        private int pos = 0;
        private int breakPos = -1;

        public TestableInputStream(InputStream is) {
            this.is = is;
        }

        public void breakOnByte(int breakPos) {
            this.breakPos = breakPos;
        }

        public int available() throws IOException {
            return is.available();
        }

        public void close() throws IOException {
            is.close();
        }
        public void mark(int readlimit) {
            is.mark(readlimit);
        }

        public boolean markSupported() {
            return is.markSupported();
        }
        public int read() throws IOException {
            if (breakPos >= 0 && pos >= breakPos) {
                throw new IOException("Simulated IO Exception");
            }
            pos++;
            return is.read();
        }

        public void reset() throws IOException {
            pos = 0;
            is.reset();
        }

        public long skip(long n) throws IOException {
            pos += n;
            return is.skip(n);
        }
    }
}
