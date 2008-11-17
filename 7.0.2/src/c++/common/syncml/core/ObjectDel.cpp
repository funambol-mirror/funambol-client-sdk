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


#include "syncml/core/ObjectDel.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/*
* delete all the char* type in the list.
* The first parameter is the number of char* pointer array to delete
*
*/
/*
// To be developed
void deleteAll(int count, char** s, ...) {

    va_list ap;
    int i = 0;

    va_start (ap, s);

    for (i = 0; i < count; i++)
    safeDel((va_arg (ap, char**)));

    va_end (ap);

}
*/

void deleteAll(int, char** s) {
    safeDel(s);
}
void deleteAll(int, char** s, char** s1) {
    safeDel(s); safeDel(s1);
}
void deleteAll(int, char** s, char** s1, char** s2) {
    safeDel(s); safeDel(s1); safeDel(s2);
}
void deleteAll(int, char** s, char** s1, char** s2, char** s3) {
    safeDel(s); safeDel(s1); safeDel(s2); safeDel(s3);
}
void deleteAll(int, char** s, char** s1, char** s2, char** s3, char** s4) {
    safeDel(s); safeDel(s1); safeDel(s2); safeDel(s3); safeDel(s4);
}
void deleteAll(int, char** s, char** s1, char** s2, char** s3, char** s4,
                    char** s5) {
    safeDel(s); safeDel(s1); safeDel(s2); safeDel(s3); safeDel(s4); safeDel(s5);
}
void deleteAll(int, char** s, char** s1, char** s2, char** s3, char** s4,
                    char** s5, char** s6) {
    safeDel(s); safeDel(s1); safeDel(s2); safeDel(s3); safeDel(s4); safeDel(s5); safeDel(s6);
}
void deleteAll(int, char** s, char** s1, char** s2, char** s3, char** s4,
                    char** s5, char** s6, char** s7) {
    safeDel(s); safeDel(s1); safeDel(s2); safeDel(s3); safeDel(s4); safeDel(s5);
    safeDel(s6); safeDel(s7);
}
void deleteAll(int, char** s, char** s1, char** s2, char** s3, char** s4,
                    char** s5, char** s6, char** s7, char** s8) {
    safeDel(s); safeDel(s1); safeDel(s2); safeDel(s3); safeDel(s4); safeDel(s5);
    safeDel(s6); safeDel(s7); safeDel(s8);
}


void deleteStringBuffer(StringBuffer** s) {
    if (*s) {
        delete *s; *s = NULL;

    }
}

void deleteAllStringBuffer(int, StringBuffer** s) {
    deleteStringBuffer(s);
}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1) {
    deleteStringBuffer(s); deleteStringBuffer(s1);
}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2) {
    deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);

}
void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3) {
    deleteStringBuffer(s); deleteStringBuffer(s1);
    deleteStringBuffer(s2); deleteStringBuffer(s3);

}
void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4) {
    deleteStringBuffer(s); deleteStringBuffer(s1);
    deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4);

}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4, StringBuffer** s5) {
    deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);
    deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);

}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4, StringBuffer** s5,
                                StringBuffer** s6) {
    deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);
    deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);
    deleteStringBuffer(s6);

}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4, StringBuffer** s5,
                                StringBuffer** s6, StringBuffer** s7) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);
     deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);
     deleteStringBuffer(s6); deleteStringBuffer(s7);
}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4, StringBuffer** s5,
                                StringBuffer** s6, StringBuffer** s7,
                                StringBuffer** s8) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);
     deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);
     deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8);
}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4, StringBuffer** s5,
                                StringBuffer** s6, StringBuffer** s7,
                                StringBuffer** s8, StringBuffer** s9) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);
     deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);
     deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8);
     deleteStringBuffer(s9);
}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4, StringBuffer** s5,
                                StringBuffer** s6, StringBuffer** s7,
                                StringBuffer** s8, StringBuffer** s9,
                                StringBuffer** s10) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);
     deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);
     deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8);
     deleteStringBuffer(s9); deleteStringBuffer(s10);
}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4, StringBuffer** s5,
                                StringBuffer** s6, StringBuffer** s7,
                                StringBuffer** s8, StringBuffer** s9,
                                StringBuffer** s10, StringBuffer** s11) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);
     deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);
     deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8);
     deleteStringBuffer(s9); deleteStringBuffer(s10); deleteStringBuffer(s11);
}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4, StringBuffer** s5,
                                StringBuffer** s6, StringBuffer** s7,
                                StringBuffer** s8, StringBuffer** s9,
                                StringBuffer** s10, StringBuffer** s11,
                                StringBuffer** s12) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);
     deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);
     deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8);
     deleteStringBuffer(s9); deleteStringBuffer(s10);
     deleteStringBuffer(s11); deleteStringBuffer(s12);
}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4, StringBuffer** s5,
                                StringBuffer** s6, StringBuffer** s7,
                                StringBuffer** s8, StringBuffer** s9,
                                StringBuffer** s10, StringBuffer** s11,
                                StringBuffer** s12, StringBuffer** s13) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);
     deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);
     deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8);
     deleteStringBuffer(s9); deleteStringBuffer(s10); deleteStringBuffer(s11);
     deleteStringBuffer(s12); deleteStringBuffer(s13);
}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4, StringBuffer** s5,
                                StringBuffer** s6, StringBuffer** s7,
                                StringBuffer** s8, StringBuffer** s9,
                                StringBuffer** s10, StringBuffer** s11,
                                StringBuffer** s12, StringBuffer** s13,
                                StringBuffer** s14) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);
     deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);
     deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8);
     deleteStringBuffer(s9); deleteStringBuffer(s10); deleteStringBuffer(s11);
     deleteStringBuffer(s12); deleteStringBuffer(s13); deleteStringBuffer(s14);
}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4, StringBuffer** s5,
                                StringBuffer** s6, StringBuffer** s7,
                                StringBuffer** s8, StringBuffer** s9,
                                StringBuffer** s10, StringBuffer** s11,
                                StringBuffer** s12, StringBuffer** s13,
                                StringBuffer** s14, StringBuffer** s15) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);
     deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);
     deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8);
     deleteStringBuffer(s9); deleteStringBuffer(s10); deleteStringBuffer(s11);
     deleteStringBuffer(s12); deleteStringBuffer(s13); deleteStringBuffer(s14);
     deleteStringBuffer(s15);
}

void deleteAllStringBuffer(int, StringBuffer** s, StringBuffer** s1,
                                StringBuffer** s2, StringBuffer** s3,
                                StringBuffer** s4, StringBuffer** s5,
                                StringBuffer** s6, StringBuffer** s7,
                                StringBuffer** s8, StringBuffer** s9,
                                StringBuffer** s10, StringBuffer** s11,
                                StringBuffer** s12, StringBuffer** s13,
                                StringBuffer** s14, StringBuffer** s15,
                                StringBuffer** s16) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);
     deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);
     deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8);
     deleteStringBuffer(s9); deleteStringBuffer(s10); deleteStringBuffer(s11);
     deleteStringBuffer(s12); deleteStringBuffer(s13); deleteStringBuffer(s14);
     deleteStringBuffer(s15); deleteStringBuffer(s16);
}



/*
//To be developed....
void deleteAllStringBuffer(int count, StringBuffer** s, ...) {

    va_list ap;
    int i = 0;

    va_start (ap, s);

    for (i = 0; i < count; i++) {
        StringBuffer** s = va_arg (ap, StringBuffer**);

        deleteStringBuffer(s);
        // deleteStringBuffer(va_arg (ap, StringBuffer**));
    }
    va_end (ap);

}
*/

bool SingleNotNullCheck(char* s) {
    return (s) ? true : false;
}

bool NotNullCheck(int, char* s) {
    return SingleNotNullCheck(s);
}
bool NotNullCheck(int, char* s, char* s1) {
    return (SingleNotNullCheck(s) || SingleNotNullCheck(s1));
}
bool NotNullCheck(int, char* s, char* s1, char* s2) {
    return (SingleNotNullCheck(s) || SingleNotNullCheck(s1) || SingleNotNullCheck(s2));
}
bool NotNullCheck(int, char* s, char* s1, char* s2, char* s3) {
    return (SingleNotNullCheck(s)  || SingleNotNullCheck(s1) ||
            SingleNotNullCheck(s2) || SingleNotNullCheck(s3));
}
bool NotNullCheck(int, char* s, char* s1, char* s2, char* s3, char* s4) {
    return (SingleNotNullCheck(s)  || SingleNotNullCheck(s1) ||
            SingleNotNullCheck(s2) || SingleNotNullCheck(s3) || SingleNotNullCheck(s4));
}
bool NotNullCheck(int, char* s, char* s1, char* s2, char* s3, char* s4, char* s5) {
    return (SingleNotNullCheck(s)  || SingleNotNullCheck(s1) ||
            SingleNotNullCheck(s2) || SingleNotNullCheck(s3) ||
            SingleNotNullCheck(s4) || SingleNotNullCheck(s5));
}
bool NotNullCheck(int, char* s, char* s1, char* s2, char* s3, char* s4,
                       char* s5, char* s6) {
    return (SingleNotNullCheck(s)  || SingleNotNullCheck(s1) ||
            SingleNotNullCheck(s2) || SingleNotNullCheck(s3) ||
            SingleNotNullCheck(s4) || SingleNotNullCheck(s5) ||
            SingleNotNullCheck(s6));
}
bool NotNullCheck(int, char* s, char* s1, char* s2, char* s3, char* s4, char* s5,
                       char* s6, char* s7) {
    return (SingleNotNullCheck(s)  || SingleNotNullCheck(s1) ||
            SingleNotNullCheck(s2) || SingleNotNullCheck(s3) ||
            SingleNotNullCheck(s4) || SingleNotNullCheck(s5) ||
            SingleNotNullCheck(s6) || SingleNotNullCheck(s7));
}
bool NotNullCheck(int, char* s, char* s1, char* s2, char* s3, char* s4,
                       char* s5, char* s6, char* s7, char* s8) {
    return (SingleNotNullCheck(s)  || SingleNotNullCheck(s1) ||
            SingleNotNullCheck(s2) || SingleNotNullCheck(s3) ||
            SingleNotNullCheck(s4) || SingleNotNullCheck(s5) ||
            SingleNotNullCheck(s6) || SingleNotNullCheck(s7) ||
            SingleNotNullCheck(s8));
}
bool NotNullCheck(int, char* s, char* s1, char* s2, char* s3, char* s4,
                       char* s5, char* s6, char* s7, char* s8, char* s9) {
    return (SingleNotNullCheck(s)  || SingleNotNullCheck(s1) ||
            SingleNotNullCheck(s2) || SingleNotNullCheck(s3) ||
            SingleNotNullCheck(s4) || SingleNotNullCheck(s5) ||
            SingleNotNullCheck(s6) || SingleNotNullCheck(s7) ||
            SingleNotNullCheck(s8) || SingleNotNullCheck(s9));
}
bool NotNullCheck(int, char* s, char* s1, char* s2, char* s3, char* s4,
                       char* s5, char* s6, char* s7, char* s8, char* s9,
                       char* s10) {
    return (SingleNotNullCheck(s)  || SingleNotNullCheck(s1) ||
            SingleNotNullCheck(s2) || SingleNotNullCheck(s3) ||
            SingleNotNullCheck(s4) || SingleNotNullCheck(s5) ||
            SingleNotNullCheck(s6) || SingleNotNullCheck(s7) ||
            SingleNotNullCheck(s8) || SingleNotNullCheck(s9) ||
            SingleNotNullCheck(s10));
}

/*
* return true if an element of the char* list is not NULL
*/
/*
// To be developed
bool NotNullCheck(int count, char* s, ...) {

    va_list ap;
    int i = 0;
    bool ret = false;

    va_start (ap, s);
    char* t = NULL;

    for(i = 0; i < count; i++) {
        t = NULL;
        t = va_arg (ap, char*);
        if (t != NULL) {
            ret = true;
        }
    }
    va_end (ap);
    return ret;
}
*/
bool NotZeroCheck(int count, int s, ...) {

    va_list ap;
    int i = 0;
    bool ret = false;

    va_start (ap, s);

    for(i = 0; i < count; i++) {
        if (va_arg (ap, int) != 0) {
            ret = true;
        }
    }
    va_end (ap);
    return ret;
}

/*
* return true if at least an arrayList as lenght > 0
* To be developed
*/
/*
bool NotZeroArrayLenght(int count, ArrayList* s, ...) {

    va_list ap;
    int i    = 0;
    bool ret = false;

    va_start (ap, s);

    for(i = 0; i < count; i++) {
        ArrayList* p = va_arg (ap, ArrayList*);

        if (p->size() > 0) {
            ret = true;
        }

    }
    va_end (ap);
    return ret;
}
*/
bool NotZeroSingleArrayLenght(ArrayList* s) {
    bool ret = false;
    if (s) {
        if (s->size() > 0)
            ret = true;
    }
    return ret;
}

bool NotZeroArrayLenght(int, ArrayList* s) {
    return NotZeroSingleArrayLenght(s);
}
bool NotZeroArrayLenght(int, ArrayList* s, ArrayList* s1) {
    return (NotZeroSingleArrayLenght(s) || NotZeroSingleArrayLenght(s1));
}
bool NotZeroArrayLenght(int, ArrayList* s, ArrayList* s1, ArrayList* s2) {
    return (NotZeroSingleArrayLenght(s) || NotZeroSingleArrayLenght(s1) || NotZeroSingleArrayLenght(s2));
}




bool NotZeroSingleStringBufferLenght(StringBuffer* s) {
    bool ret = false;
    if (s) {
        if (s->length() > 0)
            ret = true;
    }
    return ret;
}

bool NotZeroStringBufferLenght(int, StringBuffer* s) {
    return NotZeroSingleStringBufferLenght(s);
}

bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1) {
    return (NotZeroSingleStringBufferLenght(s) ||
            NotZeroSingleStringBufferLenght(s1)
            );
}
bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1, StringBuffer* s2) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2)
            );
}
bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3)
            );
}
bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4)
            );
}

bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4, StringBuffer* s5) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5)
            );
}
bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4, StringBuffer* s5,
                                    StringBuffer* s6) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5) ||
            NotZeroSingleStringBufferLenght(s6)
            );
}

bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4, StringBuffer* s5,
                                    StringBuffer* s6, StringBuffer* s7) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5) ||
            NotZeroSingleStringBufferLenght(s6) ||
            NotZeroSingleStringBufferLenght(s7)
            );
}

bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4, StringBuffer* s5,
                                    StringBuffer* s6, StringBuffer* s7,
                                    StringBuffer* s8) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5) ||
            NotZeroSingleStringBufferLenght(s6) ||
            NotZeroSingleStringBufferLenght(s7) ||
            NotZeroSingleStringBufferLenght(s8)
            );
}
bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4, StringBuffer* s5,
                                    StringBuffer* s6, StringBuffer* s7,
                                    StringBuffer* s8, StringBuffer* s9) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5) ||
            NotZeroSingleStringBufferLenght(s6) ||
            NotZeroSingleStringBufferLenght(s7) ||
            NotZeroSingleStringBufferLenght(s8) ||
            NotZeroSingleStringBufferLenght(s9)
            );
}

bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4, StringBuffer* s5,
                                    StringBuffer* s6, StringBuffer* s7,
                                    StringBuffer* s8, StringBuffer* s9,
                                    StringBuffer* s10) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5) ||
            NotZeroSingleStringBufferLenght(s6) ||
            NotZeroSingleStringBufferLenght(s7) ||
            NotZeroSingleStringBufferLenght(s8) ||
            NotZeroSingleStringBufferLenght(s9) ||
            NotZeroSingleStringBufferLenght(s10)
            );
}

bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4, StringBuffer* s5,
                                    StringBuffer* s6, StringBuffer* s7,
                                    StringBuffer* s8, StringBuffer* s9,
                                    StringBuffer* s10, StringBuffer* s11) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5) ||
            NotZeroSingleStringBufferLenght(s6) ||
            NotZeroSingleStringBufferLenght(s7) ||
            NotZeroSingleStringBufferLenght(s8) ||
            NotZeroSingleStringBufferLenght(s9) ||
            NotZeroSingleStringBufferLenght(s10) ||
            NotZeroSingleStringBufferLenght(s11)
            );
}

bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4, StringBuffer* s5,
                                    StringBuffer* s6, StringBuffer* s7,
                                    StringBuffer* s8, StringBuffer* s9,
                                    StringBuffer* s10, StringBuffer* s11,
                                    StringBuffer* s12) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5) ||
            NotZeroSingleStringBufferLenght(s6) ||
            NotZeroSingleStringBufferLenght(s7) ||
            NotZeroSingleStringBufferLenght(s8) ||
            NotZeroSingleStringBufferLenght(s9) ||
            NotZeroSingleStringBufferLenght(s10) ||
            NotZeroSingleStringBufferLenght(s11) ||
            NotZeroSingleStringBufferLenght(s12)
            );
}

bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4, StringBuffer* s5,
                                    StringBuffer* s6, StringBuffer* s7,
                                    StringBuffer* s8, StringBuffer* s9,
                                    StringBuffer* s10, StringBuffer* s11,
                                    StringBuffer* s12, StringBuffer* s13) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5) ||
            NotZeroSingleStringBufferLenght(s6) ||
            NotZeroSingleStringBufferLenght(s7) ||
            NotZeroSingleStringBufferLenght(s8) ||
            NotZeroSingleStringBufferLenght(s9) ||
            NotZeroSingleStringBufferLenght(s10) ||
            NotZeroSingleStringBufferLenght(s11) ||
            NotZeroSingleStringBufferLenght(s12) ||
            NotZeroSingleStringBufferLenght(s13)
            );
}

bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4, StringBuffer* s5,
                                    StringBuffer* s6, StringBuffer* s7,
                                    StringBuffer* s8, StringBuffer* s9,
                                    StringBuffer* s10, StringBuffer* s11,
                                    StringBuffer* s12, StringBuffer* s13,
                                    StringBuffer* s14) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5) ||
            NotZeroSingleStringBufferLenght(s6) ||
            NotZeroSingleStringBufferLenght(s7) ||
            NotZeroSingleStringBufferLenght(s8) ||
            NotZeroSingleStringBufferLenght(s9) ||
            NotZeroSingleStringBufferLenght(s10) ||
            NotZeroSingleStringBufferLenght(s11) ||
            NotZeroSingleStringBufferLenght(s12) ||
            NotZeroSingleStringBufferLenght(s13) ||
            NotZeroSingleStringBufferLenght(s14)
            );
}

bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4, StringBuffer* s5,
                                    StringBuffer* s6, StringBuffer* s7,
                                    StringBuffer* s8, StringBuffer* s9,
                                    StringBuffer* s10, StringBuffer* s11,
                                    StringBuffer* s12, StringBuffer* s13,
                                    StringBuffer* s14, StringBuffer* s15) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5) ||
            NotZeroSingleStringBufferLenght(s6) ||
            NotZeroSingleStringBufferLenght(s7) ||
            NotZeroSingleStringBufferLenght(s8) ||
            NotZeroSingleStringBufferLenght(s9) ||
            NotZeroSingleStringBufferLenght(s10) ||
            NotZeroSingleStringBufferLenght(s11) ||
            NotZeroSingleStringBufferLenght(s12) ||
            NotZeroSingleStringBufferLenght(s13) ||
            NotZeroSingleStringBufferLenght(s14) ||
            NotZeroSingleStringBufferLenght(s15)
            );
}

bool NotZeroStringBufferLenght(int, StringBuffer* s, StringBuffer* s1,
                                    StringBuffer* s2, StringBuffer* s3,
                                    StringBuffer* s4, StringBuffer* s5,
                                    StringBuffer* s6, StringBuffer* s7,
                                    StringBuffer* s8, StringBuffer* s9,
                                    StringBuffer* s10, StringBuffer* s11,
                                    StringBuffer* s12, StringBuffer* s13,
                                    StringBuffer* s14, StringBuffer* s15,
                                    StringBuffer* s16) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5) ||
            NotZeroSingleStringBufferLenght(s6) ||
            NotZeroSingleStringBufferLenght(s7) ||
            NotZeroSingleStringBufferLenght(s8) ||
            NotZeroSingleStringBufferLenght(s9) ||
            NotZeroSingleStringBufferLenght(s10) ||
            NotZeroSingleStringBufferLenght(s11) ||
            NotZeroSingleStringBufferLenght(s12) ||
            NotZeroSingleStringBufferLenght(s13) ||
            NotZeroSingleStringBufferLenght(s14) ||
            NotZeroSingleStringBufferLenght(s15) ||
            NotZeroSingleStringBufferLenght(s16)
            );
}

/*
* return true if at least an StringBuffer as lenght > 0
*/
/*
bool NotZeroStringBufferLenght(int count, StringBuffer* s, ...) {

    va_list ap;
    int i    = 0;
    bool ret = false;

    va_start (ap, s);

    for(i = 0; i < count; i++) {
        StringBuffer* p = va_arg (ap, StringBuffer*);

        if (p != NULL && p->length() > 0) {
            ret = true;
        }

    }
    va_end (ap);
    return ret;
}
*/


void deleteTarget(Target ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteSource(Source ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteSourceArray(SourceArray ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteCred(Cred ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteMeta(Meta ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteMetInf(MetInf ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteNextNonce(NextNonce ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteAlert(Alert ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteItem(Item ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteCmdID(CmdID ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteAuthentication(Authentication ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteAnchor(Anchor ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteMem(Mem ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteSyncHdr(SyncHdr ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteSyncBody(SyncBody ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteSessionID(SessionID ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteVerDTD(VerDTD ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteVerProto(VerProto ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteTargetRef(TargetRef ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteSourceRef(SourceRef ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteStatus(Status ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteChal(Chal ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteData(Data ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteMap(Map ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteMapItem(MapItem ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteComplexData(ComplexData ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteAdd(Add ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteReplace(Replace ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteDelete(Delete ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteCopy(Copy ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteSync(Sync ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteSequence(Sequence ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteAtomic(Atomic ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteGet(Get ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deletePut(Put ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteDataStore(DataStore ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteSyncType(SyncType ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteContentTypeInfo(ContentTypeInfo ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteSyncCap(SyncCap ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteDSMem(DSMem ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteCTCap(CTCap ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteExt(Ext ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteStringElement(StringElement ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteResults(Results ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteExec(Exec ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteSearch(Search ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteSyncML(SyncML ** s) {
    if (*s) {
        delete *s; *s = NULL;
    }
}

void deleteArrayList(ArrayList ** s) {
    if (*s) {
        (*s)->clear();
    }
}

END_NAMESPACE

