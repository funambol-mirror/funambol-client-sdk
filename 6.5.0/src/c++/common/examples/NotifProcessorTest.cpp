/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */

//#include <sys/types.h>
//#include <sys/stat.h>

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/utils.h"
//#include "base/util/UTF8StringBuffer.h"
#include "syncml/core/SyncNotification.h"

/////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32_WCE
void Print(TCHAR *pFormat, ...) {
    va_list ArgList;
    TCHAR   Buffer[256];

    va_start (ArgList, pFormat);

    (void)wvsprintf (Buffer, pFormat, ArgList);
    MessageBox(NULL, Buffer, TEXT(""), MB_OK);

    va_end(ArgList);
}
#else
#define Print wprintf
#endif

/////////////////////////////////////////////////////////////////////////////////
class SyncNotificationTest {
    public:
        SyncNotificationTest();
        SyncNotificationTest(const char *filename);
        ~SyncNotificationTest();

        size_t getMsgLen() { return msglen; };

        bool test(int bytenum);
    private:
        SyncNotification sn;
        SN_Errors errcode;
        char *msg;
        size_t msglen;

        void readFromFile(const char* path);
};

///////////////////////////////////////////////////////////////////////////////
static const unsigned char message1_bin[] = {
  0x6d, 0x0d, 0x2e, 0x6f, 0xa3, 0x1a, 0x94, 0xbc, 0xc8, 0xbd, 0x7c, 0xa2,
  0x46, 0xd8, 0xc6, 0x5f, 0x03, 0x18, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0f,
  0x73, 0x79, 0x6e, 0x63, 0x2e, 0x73, 0x65, 0x72, 0x76, 0x65, 0x72, 0x2e,
  0x63, 0x6f, 0x6d, 0x20, 0x80, 0x00, 0x00, 0x00, 0x05, 0x6e, 0x6f, 0x74,
  0x65, 0x73, 0x80, 0x00, 0x00, 0x00, 0x08, 0x63, 0x61, 0x6c, 0x65, 0x6e,
  0x64, 0x61, 0x72
};
static const unsigned int message1_bin_len = 63;

SyncNotificationTest::SyncNotificationTest() {
    errcode=SNErr_Undefined;
    msglen=message1_bin_len;
    msg=new char[message1_bin_len+1];
    memcpy(msg, message1_bin, message1_bin_len);
}
SyncNotificationTest::SyncNotificationTest(const char *filename) {
    errcode=SNErr_Undefined;
    readFile(filename, &msg, &msglen, true);
}
SyncNotificationTest::~SyncNotificationTest() {
    if (msg)
        delete [] msg;
}

bool SyncNotificationTest::test(int bytenum)
{
    for(size_t i=bytenum; ; i+=bytenum){
        if(i>msglen) i=msglen;
        fprintf(stderr, "Try with len=%d\n", i);

        fprintf(stderr, "Start parse (%d)\t", i);
        errcode=sn.parse(msg, i);
        fprintf(stderr, "parse finished with code: %d\n", errcode);

        if(errcode != SNErr_Incomplete) break;
#if 0
        Print(TEXT("Message incomplete. Press enter\n"));
#ifndef _WIN32_WCE
        getchar();
#endif
#endif
    }
    if(errcode){
        fprintf(stderr,"Error number: %d\n", errcode);
        return false;
    }
    else{
        fprintf(stderr,"Version:\t%d\n", sn.getVersion());
        fprintf(stderr,"UIMode:\t%d\n", sn.getUIMode());
        fprintf(stderr,"Initiator:\t%s\n",
                ( sn.getInitiator() == UserInitiated )
                    ? "UserInitiated" : "ServerInitiated" );
        fprintf(stderr,"Session ID:\t%d (%x)\n", sn.getSessionId(), sn.getSessionId() );
        fprintf(stderr,"Server ID:\t%S\n", sn.getServerId() );
        fprintf(stderr,"NumSyncs:\t%d\n", sn.getNumSyncs() );

        for (int i=0; i<sn.getNumSyncs(); i++){
            SyncAlert *sync;

            sync=sn.getSyncAlert(i);

            if( ! sync ){
                fprintf(stderr, "Error on sync #%d\n", i);
                return false;
            }
            fprintf(stderr,"Sync type:\t%d\n", sync->getSyncType() );
            fprintf(stderr,"Content type:\t%x\n", sync->getContentType() );
            fprintf(stderr,"Server URI:\t%S\n", sync->getServerURI() );
        }

        return true;
    }
}

/////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32_WCE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
    //argc=0;
    FILE *stream = _wfreopen( L"testlog.txt", L"w", stderr );

    if( stream == NULL ) {
        fprintf(stderr,"error on freopen\n" );
        exit(1);
    }
    int argc = 1;
    char *argv[] = { "test", NULL };
#else
int main(int argc, char** argv)
{
#endif

    fprintf(stderr, "Notification processor debug\n\n");

    SyncNotificationTest *t;
    int len;

//    if (argc>1) {
//        fprintf(stderr, "Message: %s\n", argv[1]);
//        t = new SyncNotificationTest(argv[1]);
//    }
//    else {
        fprintf(stderr, "Default message.\n");
        t = new SyncNotificationTest;
//    }

//    len  = (argc>2) ? atoi(argv[2]) : t->getMsgLen();
    len = t->getMsgLen();

    fprintf(stderr, "Start test\n");
    if ( t->test(len) ){
        fprintf(stderr, "Success\n");
        return 0;
    }
    else{
        fprintf(stderr, "Failure\n");
        return 1;
    }
}

