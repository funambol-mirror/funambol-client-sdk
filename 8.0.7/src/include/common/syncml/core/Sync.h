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


#ifndef INCL_SYNC
#define INCL_SYNC
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/AbstractCommand.h"
#include "syncml/core/Source.h"
#include "syncml/core/Target.h"


#define SYNC_COMMAND_NAME "Sync"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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
                    bool noResp,
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


END_NAMESPACE

/** @endcond */
#endif
