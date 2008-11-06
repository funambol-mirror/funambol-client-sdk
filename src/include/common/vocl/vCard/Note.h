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


//
// @author Stefano Fornari @ Funambol
// @version $Id: Note.h,v 1.4 2007-06-06 08:36:11 mvitolo Exp $
//


#ifndef INCL_PIM_NOTE
#define INCL_PIM_NOTE
/** @cond DEV */

#include "TypedProperty.h"

/**
 * An object representing contact note.
 *
 */

class Note : public TypedProperty {
    // ------------------------------------------------------------ Private data

    // -------------------------------------------- Constructors and Destructors
    public:

        /**
         * Creates an empty email
         */
        Note();
        ~Note();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the note
         *
         * @return this note content
         */
        vCardProperty* getNote();

        /**
         * Sets the note. The given Property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setNote(vCardProperty& p);

        /**
         * Creates a new instance of TypedProperty from the content of this
         * object. The new instance is created the the C++ new operator and
         * must be removed with the C++ delete operator.
         */
        ArrayElement* clone(); // FIXME
};

/** @endcond */
#endif
