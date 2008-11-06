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



#ifndef INCL_MAIL_SYNC_SOURCE_CONFIG
    #define INCL_MAIL_SYNC_SOURCE_CONFIG
/** @cond DEV */

    #include "base/fscapi.h"
    #include "spds/constants.h"
    #include "spds/SyncSourceConfig.h"

    class MailSyncSourceConfig : public SyncSourceConfig {

    private:

        /**
         * How old (in days) we want download new emails?
         *
         * 0 : today's only
         * -1: all emails
         * x : emails earlier than x days
         */
        int downloadAge;

        /**
         * How much body shall be downloaded?
         *
         * 0 : only headers (e.g. subject)
         * -1: no limits
         * x : up to x Kb
         */
        int bodySize;

        /**
         * If there are attachments, which ones shall be downloaded?
         *
         * 0 : no attachments
         * -1: all attachments
         * x : attachments up to x kb
         */
        int attachSize;

        /*
        * represent the folder that have to be synched. 1 is the folder to sync, 0 is to avoid
        */
        int inbox;
        int outbox;
        int draft;
        int trash;
        int sent;

        /*
        * it represent the time of the schedule
        */
        int schedule;

    public:

        /*
         * Constructs a new MailSyncSourceConfig object
         */
        MailSyncSourceConfig();

        /*
         * Constructs a new SyncSourceConfig object from an other.
         */
        MailSyncSourceConfig(MailSyncSourceConfig& c);

        /*
         * Destructor
         */
        ~MailSyncSourceConfig();

        /*
         * Sets the downloadAge
         *
         * @param age the new downloadAge
         */
        void setDownloadAge(int age);

        /*
         * Returns downloadAge
         */
        int getDownloadAge() const;

        /*
         * Sets the bodySize
         *
         * @param size the new bodySize
         */
        void setBodySize(int size);

        /*
         * Returns bodySize
         */
        int getBodySize() const;

        /*
         * Sets the attachSize
         *
         * @param size the new attachSize
         */
        void setAttachSize(int size);

        /*
         * Returns attachSize
         */
        int getAttachSize() const;


        void setInbox(int v);

        int  getInbox() const;

        void setOutbox(int v);

        int  getOutbox() const;

        void setDraft(int v);

        int  getDraft() const;

        void setTrash(int v);

        int  getTrash() const;

        void setSent(int v);

        int  getSent() const;

        void setSchedule(int v);

        int  getSchedule() const;


        /**
         * Initialize this object with the given SyncSourceConfig
         *
         * @pram sc the source config object
         */
        void assign(const MailSyncSourceConfig& sc);

    };

/** @endcond */
#endif
