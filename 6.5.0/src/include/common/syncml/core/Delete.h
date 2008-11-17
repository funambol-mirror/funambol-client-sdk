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


#ifndef INCL_DELETE
#define INCL_DELETE
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/ModificationCommand.h"

#define DELETE_COMMAND_NAME "Delete"

class Delete : public ModificationCommand {

     // ------------------------------------------------------------ Private data
    private:
        char*  COMMAND_NAME;
        BOOL archive;
        BOOL sftDel;

    // ---------------------------------------------------------- Public data
    public:

        Delete();
        ~Delete();

        /**
         * Creates a new Delete object with the given command identifier,
         * noResponse, archiveData, softDelete, credential, meta and array of item
         *
         * @param cmdID the command identifier - NOT NULL
         * @param noResp true if no response is required
         * @param archive true if the deleted data should be archived
         * @param sftDel true if this is a "soft delete". If set to false, then
         *                   this delete command is a "hard delete"
         * @param cred the authentication credential
         * @param meta the meta data
         * @param items the array of item - NOT NULL
         *
         */
        Delete(CmdID* cmdID,
               BOOL noResp,
               BOOL archive,
               BOOL sftDel,
               Cred* cred,
               Meta* meta,
               ArrayList* items);

        /**
         * Gets the command name property
         *
         * @return the command name property
         */
        const char* getName();

        /**
         * Gets the Archive property
         *
         * @return true if the deleted data should be archived
         */
        BOOL isArchive();

        /**
         * Gets the Boolean archive property
         *
         * @return archive the Boolean archive property
         */
        BOOL getArchive();

        /**
         * Sets the archive property
         *
         * @param archive the Boolean archive object
         */
        void setArchive(BOOL archive);

        /**
         * Gets the SftDel property
         *
         * @return <b>true</b>  if this is a "Soft delete"
         *         <b>false</b> if this is a "hard delete"
         */
        BOOL isSftDel();

        BOOL getSftDel();


        void setSftDel(BOOL sftDel);

        ArrayElement* clone();

};

/** @endcond */
#endif
