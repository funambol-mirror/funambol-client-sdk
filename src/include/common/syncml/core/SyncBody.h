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


#ifndef INCL_SYNC_BODY
#define INCL_SYNC_BODY
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/AbstractCommand.h"


class SyncBody {

     // ------------------------------------------------------------ Private data
    private:
        ArrayList* commands;
        BOOL finalMsg;
        void initialize();
    public:

        SyncBody();
        ~SyncBody();

        /**
         * Create a new SyncBody object. The commands in <i>commands</i>
         * must be of the allowed types.
         *
         * @param commands The array elements must be an instance of one of these
         *                 classes: {@link Alert},  {@link Atomic}, {@link Copy},
         *                 {@link Exec}, {@link Get}, {@link Map}, {@link Put},
         *                 {@link Results}, {@link Search}, {@link Sequence},
         *                 {@link Status}, {@link Sync}
         * @param finalMsg is true if this is the final message that is being sent
         *
         */
        SyncBody(ArrayList* commands   , // AbstractCommand[]
                 BOOL       finalMsg);


        /**
         *
         *  @return the return value is guaranteed to be non-null. Also,
         *          the elements of the array are guaranteed to be non-null.
         *
         */
        ArrayList* getCommands();

        /**
         * Sets the sequenced commands. The given commands must be of the allowed
         * types.
         *
         * @param commands the commands - NOT NULL and o the allowed types
         *
         */
        void setCommands(ArrayList* commands);

        /**
         * Sets the message as final
         *
         * @param finalMsg the Boolean value of finalMsg property
         */
        void setFinalMsg(BOOL finalMsg);

        /**
         * Gets the value of finalMsg property
         *
         * @return true if this is the final message being sent, otherwise false
         *
         */
        BOOL isFinalMsg();

        /**
         * Gets the value of finalMsg property
         *
         * @return true if this is the final message being sent, otherwise null
         *
         */
        BOOL getFinalMsg();

        SyncBody* clone();
};

/** @endcond */
#endif
