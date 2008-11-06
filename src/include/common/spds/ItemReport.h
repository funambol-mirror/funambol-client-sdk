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

#ifndef INCL_ITEMREPORT
#define INCL_ITEMREPORT
/** @cond DEV */

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/utils.h"


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
    WCHAR* id;

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

    const int getStatus() const;
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

/** @endcond */
#endif

