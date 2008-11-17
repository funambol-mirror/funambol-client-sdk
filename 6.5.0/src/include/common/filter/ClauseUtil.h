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

#ifndef INCL_CLAUSECONVERTER
#define INCL_CLAUSECONVERTER
/** @cond DEV */

#include "filter/LogicalClause.h"
#include "filter/SourceFilter.h"
#include "syncml/core/Filter.h"

class ClauseUtil {

public:

    /**
     * Converts a Clause to a Filter object. The Filter is allocated
     * with the new operator and must be deleted by the caller with
     * the delete operator.
     *
     * @param clause the clause to convert
     *
     * @return the corresponding filter
     */
    static Filter* toFilter(SourceFilter& sf);

    /**
     * Converts a WhereClause to a CGI query string. The returned
     * string is allocated with the new [] operator and must be
     * deleted by the caller with the delete [] operator.
     *
     * @param clause the clause to convert
     *
     * @return the corresponding CGI query string
     */
    static const char*  toCGIQuery(Clause& clause);

    /**
     * Creates the filter clause given download age, body size and attach size
     *
     * @param since download age in UTC format or NULL if no constraint
     *        is specified
     * @param bodySize body size
     * @param attachSize attachment size
     */
    static SourceFilter* createSourceFilter(const WCHAR* since, int bodySize, int attachSize);

    /**
     * Creates the filter clause given an the luid and max download size
     *
     * @param luid the id of the mail
     * @param size the max size of the mail
     */

    static SourceFilter* createSourceFilterInclusive(const char* luid, int size);
};

/** @endcond */
#endif
