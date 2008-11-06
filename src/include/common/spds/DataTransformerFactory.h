/*
 * Copyright (C) 2003-2007 Funambol, Inc
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

/**
 * This class is factory for other DataTransformer istances.
 */

 #ifndef INCL_DATA_TRANSFORMER_FACTORY
    #define INCL_DATA_TRANSFORMER_FACTORY
/** @cond DEV */

    #include "spds/DataTransformer.h"

    #define DF_FORMATTERS "b64;des;"


    class DataTransformerFactory {

    public:

        static DataTransformer* getEncoder(const char*  name);
        static DataTransformer* getDecoder(const char*  name);
        static BOOL isSupportedEncoder(const char*  name);
        static BOOL isSupportedDecoder(const char*  name);

    };

/** @endcond */
#endif
