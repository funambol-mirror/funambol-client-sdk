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

#include "spds/DataTransformerFactory.h"
#include "spds/B64Encoder.h"
#include "spds/B64Decoder.h"
#include "spds/DESEncoder.h"
#include "spds/DESDecoder.h"

DataTransformer* DataTransformerFactory::getEncoder(const char* name) {
    DataTransformer* ret = NULL;

    if (isSupportedEncoder(name) == FALSE) {
        lastErrorCode = ERR_DT_UNKNOWN;
        sprintf (lastErrorMsg, ERRMSG_DT_UNKNOWN, name);
        goto exit;
    } else if (strcmp(name, DT_B64) == 0) {
        //
        // base 64 encoder
        //
        ret = new B64Encoder();
    } else if (strcmp(name, DT_DES) == 0) {
        //
        // DES encoder
        //
        ret = new DESEncoder();
    }

exit:

    return ret;
}

DataTransformer* DataTransformerFactory::getDecoder(const char* name) {
    DataTransformer* ret = NULL;

    if (isSupportedDecoder(name) == FALSE) {
        lastErrorCode = ERR_DT_UNKNOWN;
        sprintf (lastErrorMsg, ERRMSG_DT_UNKNOWN, name);
        goto exit;
    } else if (strcmp(name, DT_B64) == 0) {
        //
        // base 64 decoder
        //
        ret = new B64Decoder();
    } else if (strcmp(name, DT_DES) == 0) {
        //
        // DES decoder
        //
        ret = new DESDecoder();
    }

exit:

    return ret;
}

BOOL DataTransformerFactory::isSupportedEncoder(const char* name) {
    char* t = new char[strlen(name)+2];

    sprintf(t, "%s;", name);
    //strcpy(t, name); strcat(t, ";");

    BOOL ret = (strstr(DF_FORMATTERS, t) != NULL);

    delete [] t;

    return ret;

}

BOOL DataTransformerFactory::isSupportedDecoder(const char* name) {
    //
    // Currently, same encoders/decoders are supported
    //
    return isSupportedEncoder(name);

}

