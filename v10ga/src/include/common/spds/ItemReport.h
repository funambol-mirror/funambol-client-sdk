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

#ifndef INCL_ITEMREPORT
#define INCL_ITEMREPORT
/** @cond DEV */

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/utils.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


/*
 * ----------------- ItemReport Class ------------------------
 * temReport class rapresents the result information on a
 * single item synchronized, such as the luid of the item
 * and its status code (200/201/500...)
 */
class ItemReport : public ArrayElement {

private:

    // The status code of the operation executed.
    int  status;

    // The LUID of item.
    WCHAR* luid;

    // The message associated to the status. It can be referred to the whole sync process if the error
    // is on the sync header (for example a 506 status code in the sync header) or to a single item as a
    // 500 status code on an inserting item.
    // in the first case the last error message and code must be set. In the other cases only the status
    //
    WCHAR* statusMessage;


    /*
     * Assign this object with the given ItemReport
     * @param ir: the ItemReport object
     */
    void assign(const ItemReport& ir);


public:

    ItemReport();
    ItemReport(ItemReport& ir);
    ItemReport(const WCHAR* luid, const int statusCode, const WCHAR* statusMess);
    virtual ~ItemReport();

    const WCHAR* getId() const;
    void setId(const WCHAR* v);

    int getStatus() const;
    void setStatus(const int v);

    const WCHAR* getStatusMessage() const;
    void setStatusMessage(const WCHAR* v);

    ArrayElement* clone();

    /*
     * Assign operator
     */
    ItemReport& operator = (const ItemReport& ir) {
        assign(ir);
        return *this;
    }
};


END_NAMESPACE

/** @endcond */
#endif

