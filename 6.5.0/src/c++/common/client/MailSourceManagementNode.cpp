/*
 * Copyright (C) 2003-2007 Funambol, Inc.
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


#include "base/fscapi.h"
#include "base/constants.h"
#include "base/util/utils.h"
#include "spdm/constants.h"
#include "client/MailSourceManagementNode.h"

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

MailSyncSourceConfig& MailSourceManagementNode::getMailSourceConfig(BOOL refresh) {
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
