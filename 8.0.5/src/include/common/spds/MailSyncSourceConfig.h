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



#ifndef INCL_MAIL_SYNC_SOURCE_CONFIG
    #define INCL_MAIL_SYNC_SOURCE_CONFIG
/** @cond DEV */

    #include "base/fscapi.h"
    #include "spds/constants.h"
    #include "spds/SyncSourceConfig.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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

        StringBuffer print();
    };


END_NAMESPACE

/** @endcond */
#endif
