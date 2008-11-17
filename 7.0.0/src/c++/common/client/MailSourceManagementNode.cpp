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


#include "base/fscapi.h"
#include "base/constants.h"
#include "base/util/utils.h"
#include "spdm/constants.h"
#include "client/MailSourceManagementNode.h"
#include "base/globalsdef.h"

USE_NAMESPACE

MailSourceManagementNode::MailSourceManagementNode(const char*  context,
                                                   const char*  name   )
    : DeviceManagementNode(context, name) {
}

MailSourceManagementNode::MailSourceManagementNode(const char*         context,
                                                   const char*         name   ,
                                                   MailSyncSourceConfig& c      )
    : DeviceManagementNode(context, name) {

    setMailSourceConfig(c);
}

MailSourceManagementNode::~MailSourceManagementNode() {
}

MailSyncSourceConfig& MailSourceManagementNode::getMailSourceConfig(bool refresh) {
    if (refresh) {
        char*  c = NULL;
        char* tmp;

        config.setName((tmp = readPropertyValue(PROPERTY_SOURCE_NAME)));
        safeDel(&tmp);
        config.setURI((tmp = readPropertyValue(PROPERTY_SOURCE_URI)));
        safeDel(&tmp);
        config.setSyncModes((tmp = readPropertyValue(PROPERTY_SOURCE_SYNC_MODES)));
        safeDel(&tmp);
        config.setSync((tmp = readPropertyValue(PROPERTY_SOURCE_SYNC)));
        safeDel(&tmp);
        config.setType((tmp = readPropertyValue(PROPERTY_SOURCE_TYPE)));
        safeDel(&tmp);

        config.setVersion((tmp = readPropertyValue(PROPERTY_SOURCE_VERSION)));
        safeDel(&tmp);
        config.setEncoding((tmp = readPropertyValue(PROPERTY_SOURCE_ENCODING)));
        safeDel(&tmp);
        config.setSupportedTypes((tmp = readPropertyValue(PROPERTY_SOURCE_SUPP_TYPES)));
        safeDel(&tmp);

        config.setLast(strtol((tmp = readPropertyValue(PROPERTY_SOURCE_LAST_SYNC)), &c, 10));
        safeDel(&tmp);
        config.setDownloadAge((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_DOWNLOAD_AGE)), &c, 10));
        safeDel(&tmp);
        config.setBodySize((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_BODY_SIZE)), &c, 10));
        safeDel(&tmp);
        config.setAttachSize((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_ATTACH_SIZE)), &c, 10));
        safeDel(&tmp);

        config.setInbox((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_INBOX)), &c, 10));
        safeDel(&tmp);
        config.setDraft((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_DRAFT)), &c, 10));
        safeDel(&tmp);
        config.setTrash((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_TRASH)), &c, 10));
        safeDel(&tmp);
        config.setOutbox((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_OUTBOX)), &c, 10));
        safeDel(&tmp);
        config.setSent((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_SENT)), &c, 10));
        safeDel(&tmp);
        config.setSchedule((int)strtol((tmp = readPropertyValue(PROPERTY_SOURCE_SCHEDULE)), &c, 10));
        safeDel(&tmp);
        config.setEncryption((tmp = readPropertyValue(PROPERTY_SOURCE_ENCRYPTION)));
        safeDel(&tmp);

    }

    return config;
}

void MailSourceManagementNode::setMailSourceConfig(MailSyncSourceConfig& c) {
    config.assign(c);

    char t[512];

    setPropertyValue(PROPERTY_SOURCE_NAME,       (char* )c.getName());
    setPropertyValue(PROPERTY_SOURCE_URI,        (char* )c.getURI());
    setPropertyValue(PROPERTY_SOURCE_SYNC_MODES, (char* )c.getSyncModes());
    setPropertyValue(PROPERTY_SOURCE_SYNC,       (char* )c.getSync());
    setPropertyValue(PROPERTY_SOURCE_TYPE,       (char* )c.getType());

    setPropertyValue(PROPERTY_SOURCE_VERSION,    (char* )c.getVersion());
    setPropertyValue(PROPERTY_SOURCE_ENCODING,   (char* )c.getEncoding());
    setPropertyValue(PROPERTY_SOURCE_SUPP_TYPES, (char* )c.getSupportedTypes());

    sprintf(t, "%ld", c.getLast());
    setPropertyValue(PROPERTY_SOURCE_LAST_SYNC, t);
    sprintf(t, "%d", c.getDownloadAge());
    setPropertyValue(PROPERTY_SOURCE_DOWNLOAD_AGE, t);
    sprintf(t, "%d", c.getBodySize());
    setPropertyValue(PROPERTY_SOURCE_BODY_SIZE, t);
    sprintf(t, "%d", c.getAttachSize());
    setPropertyValue(PROPERTY_SOURCE_ATTACH_SIZE, t);

    sprintf(t, "%d", c.getInbox());
    setPropertyValue(PROPERTY_SOURCE_INBOX, t);
    sprintf(t, "%d", c.getOutbox());
    setPropertyValue(PROPERTY_SOURCE_OUTBOX, t);
    sprintf(t, "%d", c.getTrash());
    setPropertyValue(PROPERTY_SOURCE_TRASH, t);
    sprintf(t, "%d", c.getSent());
    setPropertyValue(PROPERTY_SOURCE_SENT, t);
    sprintf(t, "%d", c.getDraft());
    setPropertyValue(PROPERTY_SOURCE_DRAFT, t);
    sprintf(t, "%d", c.getSchedule());
    setPropertyValue(PROPERTY_SOURCE_SCHEDULE, t);

    setPropertyValue(PROPERTY_SOURCE_ENCRYPTION,       (char* )c.getEncryption());

}


ArrayElement* MailSourceManagementNode::clone()  {
    return new MailSourceManagementNode(context, name, config);
}
