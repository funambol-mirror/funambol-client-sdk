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


#ifndef INCL_SYNC
#define INCL_SYNC
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/AbstractCommand.h"
#include "syncml/core/Source.h"
#include "syncml/core/Target.h"


#define SYNC_COMMAND_NAME "Sync"

class Sync : public AbstractCommand {

     // ------------------------------------------------------------ Private data
    private:
        char*  COMMAND_NAME;
        Target* target;
        Source* source;
        ArrayList* commands;
        long numberOfChanges;

    public:

        Sync();
        ~Sync();
        void initialize();
        /**
         * Creates a new Sync object
         *
         * @param cmdID the command identifier - NOT NULL
         * @param noResp is <b>true</b> if no response is required
         * @param cred the authentication credential
         * @param target the target object
         * @param source the source object
         * @param meta the meta object
         * @param numberOfChanges the number of changes
         * @param commands an array of elements that must be of one of the
         *                 following types: {@link Add}, {@link Atomic},
         *                 {@link Copy}, {@link Delete}, {@link Replace},
         *                 {@link Sequence}
         *
         *
         */
        Sync(CmdID* cmdID,
                    BOOL noResp,
                    Cred* cred,
                    Target* target,
                    Source* source,
                    Meta* meta,
                    long numberOfChanges,
                    ArrayList* commands);


        /**
         * Gets the Target object property
         *
         * @return target the Target object property
         */
        Target* getTarget();

        /**
         * Sets the Target object property
         *
         * @param target the Target object property
         *
         */
        void setTarget(Target* target);

        /**
         * Gets the Source object property
         *
         * @return source the Source object property
         */
        Source* getSource();

        /**
         * Gets the Source object property
         *
         * @param source the Source object property
         */
        void setSource(Source* source);

        /**
         *
         * @return The return value is guaranteed to be non-null.
         *          The array elements are guaranteed to be non-null.
         *
         */
        ArrayList* getCommands();

        /**
         * Sets the sequenced commands. The given commands must be of the allowed
         * types.
         *
         * @param commands the commands - NOT NULL and o the allawed types
         *
         */
        void setCommands(ArrayList* commands);

        /**
         * Gets the total number of changes
         *
         * @return the total number of changes
         */
        long getNumberOfChanges();


        /**
         * Sets the numberOfChanges property
         *
         * @param numberOfChanges the total number of changes
         */
        void setNumberOfChanges(long numberOfChanges) ;

        const char* getName();

        ArrayElement* clone();


};

/** @endcond */
#endif
