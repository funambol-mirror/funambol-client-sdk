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



#include "base/util/utils.h"
#include "spds/MailSyncSourceConfig.h"


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
    setCtCap         (sc.getCtCap         ());
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
