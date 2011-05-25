/**
 * 
 */
package com.funambol.client.push;

import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;

import junit.framework.TestCase;

/**
 */
public class SANMessageParserTest extends TestCase {
    //---------- Private fields
    SANMessageParser parser;
    

    //---------- Constructor
    /** Creates a new instance of SANMessageParserTest */
    public SANMessageParserTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }

    
    //---------- Setup and TearDown
    public void setUp() {
        parser = new SANMessageParser();
    }

    
    //---------- Test cases
    public void testParseMessage() throws Exception {
        byte[] mess = new byte[] {-40, -119, 39, -97, -101, 71, -29, 104, -6, -88, -103, 47, 49,
                                  -122, -84, 61, 3, 8, 0, 0, 0, 0, 0, 16, 65, 84, 84, 32, 65, 100,
                                  100, 114, 101, 115, 115, 32, 66, 111, 111, 107, 16, 96, 0, 0, 7,
                                  4, 99, 97, 114, 100 };
        
        parser.parseMessage(mess, false);
        assertEquals("Wrong version", "1.2", parser.getVersion());
        assertTrue("Wrong syncinfo", parser.getSyncInfoArray() != null);
        assertEquals("Wrong syncinfo length", 1, parser.getSyncInfoArray().length);
        assertEquals("Wrong syncinfo server uri", "card", parser.getSyncInfoArray()[0].getServerUri());
        assertEquals("Wrong syncinfo server uri", 206, parser.getSyncInfoArray()[0].getSyncType());
        assertEquals("Wrong syncinfo server uri", "7", parser.getSyncInfoArray()[0].getContentType());
        assertEquals("Wrong serviceId", "ATT Address Book", parser.getServerId());
    }

    
    //---------- Private methods
}
