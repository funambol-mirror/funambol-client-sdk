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

#ifndef INCL_ANCHOR
#define INCL_ANCHOR
/** @cond DEV */

#include "base/fscapi.h"

class Anchor {

    // ------------------------------------------------------------ Private data
    private:
        char*  last;
        char*  next;
    // ---------------------------------------------------------- Protected data
    public:

        Anchor(const char*  last, const char*  next);
        ~Anchor();

        /**
     * Gets the last property
     *
     * @return the last property
     */
     const char*  getLast();

    /**
     * Sets the last property
     *
     * @param last the last property
     *
     */
    void setLast(const char*  last);

    /**
     * Gets the next property
     *
     * @return the next property
     */
    const char*  getNext();

    /**
     * Sets the next property
     *
     * @param next the next property
     *
     */
    void setNext(const char*  next);


    Anchor* clone();

};

/** @endcond */
#endif
