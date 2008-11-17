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



#include "base/util/utils.h"
#include "spds/MailSyncSourceConfig.h"
#include "base/globalsdef.h"

USE_NAMESPACE


MailSyncSourceConfig::MailSyncSourceConfig() {
    name      = NULL;
    uri       = NULL;
    syncModes = NULL;
    type      = NULL;
    sync      = NULL;

    downloadAge = 0;
    bodySize    = 0;
    attachSize  = 0;

    schedule = 0;
}

MailSyncSourceConfig::~MailSyncSourceConfig() {
}

MailSyncSourceConfig::MailSyncSourceConfig(MailSyncSourceConfig& c) {
    assign(c);
}

void MailSyncSourceConfig::setDownloadAge(int age) {
    downloadAge = age;
}

int MailSyncSourceConfig::getDownloadAge() const {
    return downloadAge;
}

void MailSyncSourceConfig::setBodySize(int size) {
    bodySize = size;
}

int MailSyncSourceConfig::getBodySize() const {
    return bodySize;
}

void MailSyncSourceConfig::setAttachSize(int size) {
    attachSize = size;
}

int MailSyncSourceConfig::getAttachSize() const {
    return attachSize;
}

void MailSyncSourceConfig::setInbox(int v) {
    inbox = v;
}

int MailSyncSourceConfig::getInbox() const {
    return inbox;
}

void MailSyncSourceConfig::setOutbox(int v) {
    outbox = v;
}

int MailSyncSourceConfig::getOutbox() const {
    return outbox;
}

void MailSyncSourceConfig::setDraft(int v) {
    draft = v;
}

int MailSyncSourceConfig::getDraft() const {
    return draft;
}

void MailSyncSourceConfig::setTrash(int v) {
    trash = v;
}

int MailSyncSourceConfig::getTrash() const {
    return trash;
}

void MailSyncSourceConfig::setSent(int v) {
    sent = v;
}

int MailSyncSourceConfig::getSent() const {
    return sent;
}

void MailSyncSourceConfig::setSchedule(int v) {
    schedule = v;
}

int MailSyncSourceConfig::getSchedule() const {
    return schedule;
}

// ------------------------------------------------------------- Private methods

void MailSyncSourceConfig::assign(const MailSyncSourceConfig& sc) {
    setName     (sc.getName     ());
    setURI      (sc.getURI      ());
    setSyncModes(sc.getSyncModes());
    setType     (sc.getType     ());
    setSync     (sc.getSync     ());
    setLast     (sc.getLast     ());

    setEncoding      (sc.getEncoding      ());
    setVersion       (sc.getVersion       ());
    setSupportedTypes(sc.getSupportedTypes());
    //setCtCap         (sc.getCtCap         ());
    setEncryption    (sc.getEncryption    ());

    setDownloadAge(sc.getDownloadAge());
    setBodySize(sc.getBodySize());
    setAttachSize(sc.getAttachSize());

    setInbox(sc.getInbox());
    setOutbox(sc.getOutbox());
    setSent(sc.getSent());
    setTrash(sc.getTrash());
    setDraft(sc.getDraft());
    setSchedule(sc.getSchedule());

}
