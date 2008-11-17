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


#include "syncml/core/ObjectDel.h"



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

void deleteAll(int count, char** s) {
    safeDel(s);
}
void deleteAll(int count, char** s, char** s1) {
    safeDel(s); safeDel(s1);
}
void deleteAll(int count, char** s, char** s1, char** s2) {
    safeDel(s); safeDel(s1); safeDel(s2);
}
void deleteAll(int count, char** s, char** s1, char** s2, char** s3) {
    safeDel(s); safeDel(s1); safeDel(s2); safeDel(s3);
}
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4) {
    safeDel(s); safeDel(s1); safeDel(s2); safeDel(s3); safeDel(s4);
}
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4, char** s5) {
    safeDel(s); safeDel(s1); safeDel(s2); safeDel(s3); safeDel(s4); safeDel(s5);
}
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4, char** s5, char** s6) {
    safeDel(s); safeDel(s1); safeDel(s2); safeDel(s3); safeDel(s4); safeDel(s5); safeDel(s6);
}
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4, char** s5, char** s6, char** s7) {
    safeDel(s); safeDel(s1); safeDel(s2); safeDel(s3); safeDel(s4); safeDel(s5); safeDel(s6); safeDel(s7);
}
void deleteAll(int count, char** s, char** s1, char** s2, char** s3, char** s4, char** s5, char** s6, char** s7, char** s8) {
    safeDel(s); safeDel(s1); safeDel(s2); safeDel(s3); safeDel(s4); safeDel(s5); safeDel(s6); safeDel(s7); safeDel(s8);
}


void deleteStringBuffer(StringBuffer** s) {
    if (*s) {
        delete *s; *s = NULL;

    }
}

void deleteAllStringBuffer(int count, StringBuffer** s) {
    deleteStringBuffer(s);
}

void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1) {
    deleteStringBuffer(s); deleteStringBuffer(s1);
}

void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2) {
    deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2);

}
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3) {
    deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3);

}
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4) {
    deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4);

}

void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5) {
    deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5);

}

void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6) {
    deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5); deleteStringBuffer(s6);

}

void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5); deleteStringBuffer(s6); deleteStringBuffer(s7);


}
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5); deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8);
}
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5); deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8); deleteStringBuffer(s9);
}

void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5); deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8); deleteStringBuffer(s9);
     deleteStringBuffer(s10);
}
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10, StringBuffer** s11) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5); deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8); deleteStringBuffer(s9);
     deleteStringBuffer(s10); deleteStringBuffer(s11);
}
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10, StringBuffer** s11, StringBuffer** s12) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5); deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8); deleteStringBuffer(s9);
     deleteStringBuffer(s10); deleteStringBuffer(s11); deleteStringBuffer(s12);
}
void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10, StringBuffer** s11, StringBuffer** s12, StringBuffer** s13) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5); deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8); deleteStringBuffer(s9);
     deleteStringBuffer(s10); deleteStringBuffer(s11); deleteStringBuffer(s12); deleteStringBuffer(s13);
}

void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10, StringBuffer** s11, StringBuffer** s12, StringBuffer** s13, StringBuffer** s14) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5); deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8); deleteStringBuffer(s9);
     deleteStringBuffer(s10); deleteStringBuffer(s11); deleteStringBuffer(s12); deleteStringBuffer(s13); deleteStringBuffer(s14);
}

void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10, StringBuffer** s11, StringBuffer** s12, StringBuffer** s13, StringBuffer** s14, StringBuffer** s15) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5); deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8); deleteStringBuffer(s9);
     deleteStringBuffer(s10); deleteStringBuffer(s11); deleteStringBuffer(s12); deleteStringBuffer(s13); deleteStringBuffer(s14); deleteStringBuffer(s15);
}

void deleteAllStringBuffer(int count, StringBuffer** s, StringBuffer** s1, StringBuffer** s2, StringBuffer** s3, StringBuffer** s4, StringBuffer** s5, StringBuffer** s6, StringBuffer** s7, StringBuffer** s8, StringBuffer** s9,
                                      StringBuffer** s10, StringBuffer** s11, StringBuffer** s12, StringBuffer** s13, StringBuffer** s14, StringBuffer** s15, StringBuffer** s16) {
     deleteStringBuffer(s); deleteStringBuffer(s1); deleteStringBuffer(s2); deleteStringBuffer(s3); deleteStringBuffer(s4); deleteStringBuffer(s5); deleteStringBuffer(s6); deleteStringBuffer(s7); deleteStringBuffer(s8); deleteStringBuffer(s9);
     deleteStringBuffer(s10); deleteStringBuffer(s11); deleteStringBuffer(s12); deleteStringBuffer(s13); deleteStringBuffer(s14); deleteStringBuffer(s15); deleteStringBuffer(s16);
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

BOOL SingleNotNullCheck(char* s) {
    return (s) ? TRUE : FALSE;
}

BOOL NotNullCheck(int count, char* s) {
    return SingleNotNullCheck(s);
}
BOOL NotNullCheck(int count, char* s, char* s1) {
    return (SingleNotNullCheck(s) || SingleNotNullCheck(s1));
}
BOOL NotNullCheck(int count, char* s, char* s1, char* s2) {
    return (SingleNotNullCheck(s) || SingleNotNullCheck(s1) || SingleNotNullCheck(s2));
}
BOOL NotNullCheck(int count, char* s, char* s1, char* s2, char* s3) {
    return (SingleNotNullCheck(s) || SingleNotNullCheck(s1) || SingleNotNullCheck(s2) || SingleNotNullCheck(s3));
}
BOOL NotNullCheck(int count, char* s, char* s1, char* s2, char* s3, char* s4) {
    return (SingleNotNullCheck(s) || SingleNotNullCheck(s1) || SingleNotNullCheck(s2) || SingleNotNullCheck(s3) || SingleNotNullCheck(s4));
}
BOOL NotNullCheck(int count, char* s, char* s1, char* s2, char* s3, char* s4, char* s5) {
    return (SingleNotNullCheck(s) || SingleNotNullCheck(s1) || SingleNotNullCheck(s2) || SingleNotNullCheck(s3) || SingleNotNullCheck(s4)
           || SingleNotNullCheck(s5));
}
BOOL NotNullCheck(int count, char* s, char* s1, char* s2, char* s3, char* s4, char* s5, char* s6) {
    return (SingleNotNullCheck(s) || SingleNotNullCheck(s1) || SingleNotNullCheck(s2) || SingleNotNullCheck(s3) || SingleNotNullCheck(s4)
            || SingleNotNullCheck(s5)
            || SingleNotNullCheck(s6));
}
BOOL NotNullCheck(int count, char* s, char* s1, char* s2, char* s3, char* s4, char* s5, char* s6, char* s7) {
    return (SingleNotNullCheck(s) || SingleNotNullCheck(s1) || SingleNotNullCheck(s2) || SingleNotNullCheck(s3) || SingleNotNullCheck(s4)
            || SingleNotNullCheck(s5)
            || SingleNotNullCheck(s6)
            || SingleNotNullCheck(s7));
}
BOOL NotNullCheck(int count, char* s, char* s1, char* s2, char* s3, char* s4, char* s5, char* s6, char* s7, char* s8) {
    return (SingleNotNullCheck(s) || SingleNotNullCheck(s1) || SingleNotNullCheck(s2) || SingleNotNullCheck(s3) || SingleNotNullCheck(s4)
            || SingleNotNullCheck(s5)
            || SingleNotNullCheck(s6)
            || SingleNotNullCheck(s7)
            || SingleNotNullCheck(s8));
}
BOOL NotNullCheck(int count, char* s, char* s1, char* s2, char* s3, char* s4, char* s5, char* s6, char* s7, char* s8, char* s9) {
    return (SingleNotNullCheck(s) || SingleNotNullCheck(s1) || SingleNotNullCheck(s2) || SingleNotNullCheck(s3) || SingleNotNullCheck(s4)
            || SingleNotNullCheck(s5)
            || SingleNotNullCheck(s6)
            || SingleNotNullCheck(s7)
            || SingleNotNullCheck(s8)
            || SingleNotNullCheck(s9));
}
BOOL NotNullCheck(int count, char* s, char* s1, char* s2, char* s3, char* s4, char* s5, char* s6, char* s7, char* s8, char* s9, char* s10) {
    return (SingleNotNullCheck(s) || SingleNotNullCheck(s1) || SingleNotNullCheck(s2) || SingleNotNullCheck(s3) || SingleNotNullCheck(s4)
            || SingleNotNullCheck(s5)
            || SingleNotNullCheck(s6)
            || SingleNotNullCheck(s7)
            || SingleNotNullCheck(s8)
            || SingleNotNullCheck(s9)
            || SingleNotNullCheck(s10));
}

/*
* return TRUE if an element of the char* list is not NULL
*/
/*
// To be developed
BOOL NotNullCheck(int count, char* s, ...) {

    va_list ap;
    int i = 0;
    BOOL ret = FALSE;

    va_start (ap, s);
    char* t = NULL;

    for(i = 0; i < count; i++) {
        t = NULL;
        t = va_arg (ap, char*);
        if (t != NULL) {
            ret = TRUE;
        }
    }
    va_end (ap);
    return ret;
}
*/
BOOL NotZeroCheck(int count, int s, ...) {

    va_list ap;
    int i = 0;
    BOOL ret = FALSE;

    va_start (ap, s);

    for(i = 0; i < count; i++) {
        if (va_arg (ap, int) != 0) {
            ret = TRUE;
        }
    }
    va_end (ap);
    return ret;
}

/*
* return TRUE if at least an arrayList as lenght > 0
* To be developed
*/
/*
BOOL NotZeroArrayLenght(int count, ArrayList* s, ...) {

    va_list ap;
    int i    = 0;
    BOOL ret = FALSE;

    va_start (ap, s);

    for(i = 0; i < count; i++) {
        ArrayList* p = va_arg (ap, ArrayList*);

        if (p->size() > 0) {
            ret = TRUE;
        }

    }
    va_end (ap);
    return ret;
}
*/
BOOL NotZeroSingleArrayLenght(ArrayList* s) {
    BOOL ret = FALSE;
    if (s) {
        if (s->size() > 0)
            ret = TRUE;
    }
    return ret;
}

BOOL NotZeroArrayLenght(int count, ArrayList* s) {
    return NotZeroSingleArrayLenght(s);
}
BOOL NotZeroArrayLenght(int count, ArrayList* s, ArrayList* s1) {
    return (NotZeroSingleArrayLenght(s) || NotZeroSingleArrayLenght(s1));
}
BOOL NotZeroArrayLenght(int count, ArrayList* s, ArrayList* s1, ArrayList* s2) {
    return (NotZeroSingleArrayLenght(s) || NotZeroSingleArrayLenght(s1) || NotZeroSingleArrayLenght(s2));
}




BOOL NotZeroSingleStringBufferLenght(StringBuffer* s) {
    BOOL ret = FALSE;
    if (s) {
        if (s->length() > 0)
            ret = TRUE;
    }
    return ret;
}

BOOL NotZeroStringBufferLenght(int count, StringBuffer* s) {
    return NotZeroSingleStringBufferLenght(s);
}

BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1) {
    return (NotZeroSingleStringBufferLenght(s) ||
            NotZeroSingleStringBufferLenght(s1)
            );
}
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2)
            );
}
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3)
            );
}
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3, StringBuffer* s4) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4)
            );
}

BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5)
            );
}
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6) {
    return (NotZeroSingleStringBufferLenght(s)  ||
            NotZeroSingleStringBufferLenght(s1) ||
            NotZeroSingleStringBufferLenght(s2) ||
            NotZeroSingleStringBufferLenght(s3) ||
            NotZeroSingleStringBufferLenght(s4) ||
            NotZeroSingleStringBufferLenght(s5) ||
            NotZeroSingleStringBufferLenght(s6)
            );
}

BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7) {
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

BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
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
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
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

BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10) {
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

BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11) {
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

BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
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

BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
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

BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12, StringBuffer* s13, StringBuffer* s14) {
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

BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12, StringBuffer* s13, StringBuffer* s14, StringBuffer* s15) {
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
            NotZeroSingleStringBufferLenght(s15)
            );
}

BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, StringBuffer* s1, StringBuffer* s2, StringBuffer* s3,
                                         StringBuffer* s4, StringBuffer* s5, StringBuffer* s6, StringBuffer* s7,
                                         StringBuffer* s8, StringBuffer* s9, StringBuffer* s10, StringBuffer* s11,
                                         StringBuffer* s12, StringBuffer* s13, StringBuffer* s14, StringBuffer* s15,
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
            NotZeroSingleStringBufferLenght(s15) ||
            NotZeroSingleStringBufferLenght(s16)
            );
}
/*
* return TRUE if at least an StringBuffer as lenght > 0
*/
/*
BOOL NotZeroStringBufferLenght(int count, StringBuffer* s, ...) {

    va_list ap;
    int i    = 0;
    BOOL ret = FALSE;

    va_start (ap, s);

    for(i = 0; i < count; i++) {
        StringBuffer* p = va_arg (ap, StringBuffer*);

        if (p != NULL && p->length() > 0) {
            ret = TRUE;
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
