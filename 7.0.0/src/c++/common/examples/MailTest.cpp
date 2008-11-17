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

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "spds/spdsutils.h"
#include <stdio.h>

#include "spds/MailMessage.h"
#include "spds/EmailData.h"
#include "base/globalsdef.h"

USE_NAMESPACE

// Read a text file and convert it from UTF-8 to WCHAR
static int readFromFile(const char* path, WCHAR **message, size_t *len)
{
    char *msg = 0;
    size_t msglen;

    // Read file
    int ret = readFile(path, &msg, &msglen);
    if(ret)
        return ret;
    msg[msglen]=0;
    // Convert content
    *message = utf82wc(msg);
    *len=wcslen(*message);
    // Free memory
    delete [] msg;

    return 0;
}

#ifdef _WIN32_WCE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
#else
int main(int argc, char** argv)
#endif
{
    size_t msglen=0;
    WCHAR *name[10], *msg=0;
    int i;
    const WCHAR *attach;

#ifdef _WIN32_WCE
    name[0] = stringdup(TEXT("message.xml"));
    name[1] = stringdup(TEXT("text.xml"));
    name[2] = NULL;
    attach = TEXT("/synclog.txt");
#else
    for(i=1; i<argc; i++)
       name[i-1] = utf82wc(argv[i]);
    name[i-1] = NULL;
    attach = TEXT("c:/windows/temp/synclog.txt");
#endif
    LOG.setLevel(LOG_LEVEL_DEBUG); // Force debug level for this test.

    // Test parse/format loop if names were given on cmdline
    for (i=0; name[i]; i++) {
        msg = loadAndConvert(name[i]);
        if( !msg ){
            WCHAR dbg[256];
            wsprintf(dbg, TEXT("Can't open file %s\n"), name[i]);
	        LOG.error(dbg);
            continue;
        }

    	EmailData em;

        if (em.parse(msg))
            fprintf(stderr, "Parse failed on: %S\n", name[i]);

        delete [] msg;

        WCHAR outname[10];
        wsprintf(outname, TEXT("msgout%d.xml"), i);
        if ( convertAndSave( outname, em.format() ) ) {
            fprintf(stderr, "Error in convertAndSave(em)\n");
        }
    }

    // Try to send a new mail with attachment
    EmailData newmail;
    MailMessage n;
    BodyPart body;

    body.setContent(TEXT("Ma che bella la città!"));

    n.setFrom(TEXT("gazza@funambol.com"));
    n.setTo(TEXT("magi@funambol.com"));
    n.setSubject(TEXT("Test"));
    n.setBody(body);

	BodyPart a;
	a.setFilename( TEXT("pippo.txt") );
	a.setContent( attach );
	a.setEncoding( TEXT("base64") );
    n.addAttachment(a);

    newmail.setRead(true);
    newmail.setEmailItem(n);

    if ( convertAndSave( L"attachment.xml", newmail.format() ) ) {
        fprintf(stderr, "Error in convertAndSave(newmail)\n");
    }
    //extern size_t StringBuffer_memcount;
    //fprintf(stderr, "Memcount: %ld\n", StringBuffer_memcount);
    //getchar();
	return 0;
}
