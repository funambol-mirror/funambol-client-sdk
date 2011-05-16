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
#include "base/quoted-printable.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

static int hex2int( char x )
{
    return (x >= '0' && x <= '9') ? x - '0' :
        (x >= 'A' && x <= 'F') ? x - 'A' + 10 :
        (x >= 'a' && x <= 'f') ? x - 'a' + 10 :
        0;
}

char *qp_decode(const char *qp)
{
    const char *in;
    char *ret = new char[strlen(qp)+1];
    char *out = ret;

    for (in = qp; *in; in++ ) {
        // Handle encoded chars
        if ( *in == '=' ) {
            if (in[1] && in[2] ) {
                // The sequence is complete, check it
                in++;   // skip the '='
                if (in[0] == '\r' && in[1] == '\n') {
                    // soft line break, ignore it
                    in ++;
                    continue;
                }
                else if ( isxdigit(in[0]) && isxdigit(in[1]) ) {
                    // Ok, we have two hex digit: decode them
                    *out = (hex2int(in[0]) << 4) | hex2int(in[1]);
                    out++;
                    in ++;
                    continue;
                }
            }
            // In all wrong cases leave the original bytes
            // (see RFC 2045). They can be incomplete sequence,
            // or a '=' followed by non hex digit.
        }
        // TODO:
        // RFC 2045 says to exclude control characters mistakenly
        // present (unencoded) in the encoded stream.

        // Copy other characters
        *out = *in;
        out++;
    }
    *out = 0;

    return ret;
}

bool qp_isNeed(const char *in) {
    for(int i = 0; i < int(strlen(in)); i++)
        if ( (0x21 > in[i]) || (in[i] > 0x7e) || in[i] == '=' )
            return true;

    return false;
}

/*
 * Convert a char* string into QUOTED-PRINTABLE format.
 * @param psz     : the input char* string
 * @param start   : the start point of the line
 * @return        : the char* string in QP format (new - must be freed by the caller!)
 *                  (NULL if conversion failed)
 */
char* qp_encode(const char* input, int start) {

    int   count   = start;
    int   maxLen  = 3*strlen(input);         // This is the max length for output string
    char *sAppend = NULL;
    char  szTemp[10];
    const char *p;

    // new - must be freed by the caller
    char* qpString = new char[maxLen + 1];
    strcpy(qpString, "");

    if (maxLen>0) {
        sAppend = new char[maxLen + 1];
        strncpy(sAppend, input, maxLen);
        sAppend[maxLen]=0;

        if(!sAppend)
            return NULL;

        for (p = sAppend; *p; p++) {
            //if (count > QP_MAX_LINE_LEN) {
            //    strcat(qpString, "=\r\n");
            //    count = 0;
            //}
            //else
            if (*p == '\t' || *p == ' ') {
                const char *pScan = p;
                while (*pScan && (*pScan == '\t' || *pScan == ' ')) {
                    pScan++;
                }
                if (*pScan == '\0') {
                    while (*p) {
                        unsigned char ind = *p;
                        sprintf(szTemp, "=%02X", ind);
                        strcat(qpString, szTemp);
                        count += 3;
                        p++;

                        //if (count > QP_MAX_LINE_LEN) {
                        //    strcat(qpString, "=\r\n");
                        //    count = 0;
                        //}
                    }
                    break;
                }
                else {
                    strncat(qpString, p, 1);
                    count++;
                }
                continue;
            }
            else if (('!' <= *p) && ('~' >= *p) && ('=' != *p)) {
                strncat(qpString, p, 1);
                count++;
            }
            else {
                unsigned char ind = *p;
                sprintf(szTemp, "=%02X", ind);
                strcat(qpString, szTemp);
                count += 3;
            }
        }

        delete [] sAppend;
    }
    return qpString;
}


END_NAMESPACE

