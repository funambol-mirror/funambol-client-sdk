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

#ifndef INCL_CLAUSECONVERTER
#define INCL_CLAUSECONVERTER
/** @cond DEV */

#include "filter/LogicalClause.h"
#include "filter/SourceFilter.h"
#include "syncml/core/Filter.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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
    static char*  toCGIQuery(Clause& clause);

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
     * @param alsoAttachment if we want consider the attachment too. True bu default
     */

    static SourceFilter* createSourceFilterInclusive(const char* luid, int size, bool alsoAttachment = true);
};


END_NAMESPACE

/** @endcond */
#endif
