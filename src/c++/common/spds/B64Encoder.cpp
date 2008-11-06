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

#include "base/base64.h"
#include "spds/B64Encoder.h"

B64Encoder::B64Encoder() : DataTransformer(DT_B64) {
}

B64Encoder::~B64Encoder() {
}

char* B64Encoder::transform(char* data, TransformationInfo& info) {
    long len = info.size;
    //
    // get extra space for wm memory allocation
    //
    char* b64 = new char[((len/3+1)<<2) + 32];

    info.size = b64_encode(b64, data, len);
    b64[info.size] = 0;
    info.newReturnedData = TRUE;

    return b64;
}
