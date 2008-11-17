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
#include "base/util/utils.h"
#include "vocl/vCard/Contact.h"

Contact::Contact() {
    name           = NULL;
    notes          = NULL;
    businessDetail = NULL;
    personalDetail = NULL;

    setVersion(TEXT("2.1"));
    setProdID(TEXT("VCARD"));
}

Contact::~Contact() {
    if (name) {
        delete name; name = NULL;
    }
    if (notes) {
        delete notes; notes = NULL;
    }
    if (businessDetail) {
        delete businessDetail; businessDetail = NULL;
    }
    if (personalDetail) {
        delete personalDetail; personalDetail = NULL;
    }
}

/**
 * Returns the UID of this contact
 *
 * @return the uid of this contact or NULL if not specified
 */
WCHAR* Contact::getUID(WCHAR* buf, int size) {

    if(!containsProperty(TEXT("UID")))
        return NULL;

    if (buf == NULL) {
        return (getProperty(TEXT("UID"))->getValue());
    }

    if (size >= 0) {
        wcsncpy(buf, getProperty(TEXT("UID"))->getValue(), size);
    } else {
        wcscpy(buf, getProperty(TEXT("UID"))->getValue());
    }

    return buf;
}

WCHAR* Contact::getTimezone (WCHAR* buf, int size) {

    if(!containsProperty(TEXT("TZ")))
        return NULL;

    if (buf == NULL) {
        return (getProperty(TEXT("TZ"))->getValue());
    }

    if (size >= 0) {
        wcsncpy(buf, getProperty(TEXT("TZ"))->getValue(), size);
    } else {
        wcscpy(buf, getProperty(TEXT("TZ"))->getValue());
    }

    return buf;
}

ArrayList* Contact::getNotes() {
    if(!notes) {
        for(int i = 0; i<propertiesCount();i++) {
            if(!wcscmp(getProperty(i)->getName(), TEXT("NOTE")))
                if(getProperty(i)->getValue()) {
                    vCardProperty *property = getPropertyFromVProperty(getProperty(i));
                    Note* note = new Note();
                    note->setProperty(*property);
                    if(getProperty(i)->containsParameter(TEXT("TYPE")))
                        note->setType(getProperty(i)->getParameterValue(TEXT("TYPE")));
                    if(!notes)
                        notes = new ArrayList();
                    notes->add((ArrayElement&)*note);
                }
        }
    }

    return notes;
}

void Contact::setNotes(ArrayList& list) {
	int i,m;
    if (notes) {
        notes->clear();
    } else {
        notes = new ArrayList();
    }

    Note *note = NULL;
    VProperty *vp;

    for(i = 0, m = propertiesCount();i < m;i++) {
        if(!wcscmp(getProperty(i)->getName(), TEXT("NOTE"))) {
            removeProperty(i);
            --i;
            --m;
        }
    }

    int s = list.size();
    for (i=0; i<s; ++i) {
        notes->add(*list[i]);

        note = (Note*)list[i];
        if(note->getProperty()) {
            vp = getVPropertyFromProperty(TEXT("NOTE"), note->getProperty());
            if(note->getType())
                vp->addParameter(TEXT("TYPE"), note->getType());
            insertProperty(vp);
        }
    }
}

WCHAR* Contact::getRevision (WCHAR* buf, int size) {
    if(!containsProperty(TEXT("REV")))
        return NULL;

    if (buf == NULL) {
        return (getProperty(TEXT("REV"))->getValue());
    }

    if (size >= 0) {
        wcsncpy(buf, getProperty(TEXT("REV"))->getValue(), size);
    } else {
        wcscpy(buf, getProperty(TEXT("REV"))->getValue());
    }

    return buf;
}

Name* Contact::getName () {
    if (!name) {
        if(containsProperty(TEXT("N")) && getProperty(TEXT("N"))->getValue()) {
            VProperty* vp = getProperty(TEXT("N"));

            name = new Name();
            if(vp->getPropComponent(4)) {
                vCardProperty *salutation = getPropertyFromVProperty(vp);
                salutation->setValue(vp->getPropComponent(4));
                name->setSalutation(*salutation);

                delete salutation;
            }
            if(vp->getPropComponent(2)) {
                vCardProperty *firstName = getPropertyFromVProperty(vp);
                firstName->setValue(vp->getPropComponent(2));
                name->setFirstName(*firstName);

                delete firstName;
            }
            if(vp->getPropComponent(3)) {
                vCardProperty *middleName = getPropertyFromVProperty(vp);
                middleName->setValue(vp->getPropComponent(3));
                name->setMiddleName(*middleName);

                delete middleName;
            }
            if(vp->getPropComponent(1)) {
                vCardProperty *lastName = getPropertyFromVProperty(vp);
                lastName->setValue(vp->getPropComponent(1));
                name->setLastName(*lastName);

                delete lastName;
            }
            if(vp->getPropComponent(5)) {
                vCardProperty *suffix = getPropertyFromVProperty(vp);
                suffix->setValue(vp->getPropComponent(5));
                name->setSuffix(*suffix);

                delete suffix;
            }
        }

        if(containsProperty(TEXT("FN")) && getProperty(TEXT("FN"))->getValue()) {
            if (!name)
                name = new Name();

            VProperty* vp = getProperty(TEXT("FN"));
            vCardProperty *displayName = getPropertyFromVProperty(vp);
            name->setDisplayName(*displayName);

            delete displayName;
        }

        if(containsProperty(TEXT("NICKNAME")) && getProperty(TEXT("NICKNAME"))->getValue()) {
            if (!name)
                name = new Name();

            VProperty* vp = getProperty(TEXT("NICKNAME"));
            vCardProperty *nickName = getPropertyFromVProperty(vp);
            name->setNickname(*nickName);

            delete nickName;
        }
    }
    return name;
}

void Contact::setName(Name& n) {
    if (name) delete name;

    name = n.clone();

    if (containsProperty(TEXT("N")))
        removeProperty(TEXT("N"));
    if (containsProperty(TEXT("FN")))
        removeProperty(TEXT("FN"));
    if (containsProperty(TEXT("NICKNAME")))
        removeProperty(TEXT("NICKNAME"));

    if(name) {
        WCHAR *nameProp = new WCHAR[MAX_VPROPERTY_VALUE + 1];
        wcscpy(nameProp,TEXT(""));
        if(name->getLastName())
            wcscat(nameProp,name->getLastName()->getValue());
        wcscat(nameProp,TEXT(";"));

        if(name->getFirstName())
            wcscat(nameProp,name->getFirstName()->getValue());
            wcscat(nameProp,TEXT(";"));

        if(name->getMiddleName())
            wcscat(nameProp,name->getMiddleName()->getValue());
            wcscat(nameProp,TEXT(";"));

        if(name->getSalutation())
            wcscat(nameProp,name->getSalutation()->getValue());
            wcscat(nameProp,TEXT(";"));

        if(name->getSuffix())
            wcscat(nameProp,name->getSuffix()->getValue());

        VProperty* vpName = new VProperty(TEXT("N"), nameProp);
        if(name->getFirstName()) {
            if(name->getFirstName()->getCharset())
                vpName->addParameter(TEXT("CHARSET"), name->getFirstName()->getCharset());
            if(name->getFirstName()->getEncoding())
                vpName->addParameter(TEXT("ENCODING"), name->getFirstName()->getEncoding());
            if(name->getFirstName()->getLanguage())
                vpName->addParameter(TEXT("LANGUAGE"), name->getFirstName()->getLanguage());
        }
        insertProperty(vpName);

        if(name->getDisplayName()) {
            VProperty* vpDisplayName = getVPropertyFromProperty(TEXT("FN"), name->getDisplayName());
            insertProperty(vpDisplayName);
        }
        if(name->getNickname()) {
            VProperty* vpNickName = getVPropertyFromProperty(TEXT("NICKNAME"), name->getNickname());
            insertProperty(vpNickName);
        }
    }
}

BusinessDetail* Contact::getBusinessDetail () {
    if(!businessDetail) {
        WCHAR* titles[MAX_TITLES];
        int titlesIndex = 0;
        ArrayList* phones = NULL;
        ArrayList* emails = NULL;
        ArrayList* webPages = NULL;
        ContactDetail* contactDetail = NULL;
        Address* adr = NULL;

        for(int i = 0; i<propertiesCount();i++) {
            if(!wcscmp(getProperty(i)->getName(), TEXT("ADR")) && getProperty(i)->isType(TEXT("WORK")))
                if(getProperty(i)->getValue()) {
                    adr = composeAddress(getProperty(i), BUSINESS);
                    if(!businessDetail)
                        businessDetail = new BusinessDetail();

                    businessDetail->setAddress(*adr);

                    delete adr;
                }
            if(!wcscmp(getProperty(i)->getName(), TEXT("LABEL")) && getProperty(i)->isType(TEXT("WORK")))
                if(getProperty(i)->getValue()) {
                    adr = addLabelAddress(getProperty(i), BUSINESS);

                    if(!businessDetail)
                        businessDetail = new BusinessDetail();

                    businessDetail->setAddress(*adr);

                    delete adr;
                }
            if(!wcscmp(getProperty(i)->getName(), TEXT("ROLE")))
                if(getProperty(i)->getValue()) {
                    vCardProperty *prop = getPropertyFromVProperty(getProperty(i));
                    if(!businessDetail)
                        businessDetail = new BusinessDetail();
                    businessDetail->setRole(*prop);

                    delete prop;
                }
            if(!wcscmp(getProperty(i)->getName(), TEXT("TITLE")))
                if(getProperty(i)->getValue()) {
                    titles[titlesIndex] = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                    wcscpy(titles[titlesIndex], getProperty(i)->getValue());
                    titlesIndex++;
                }

            if(!wcscmp(getProperty(i)->getName(), TEXT("ORG")))
                if(getProperty(i)->getValue()) {
                    vCardProperty *prop = getPropertyFromVProperty(getProperty(i));
                    if(!businessDetail)
                        businessDetail = new BusinessDetail();
                    businessDetail->setCompany(*prop);

                    delete prop;
                }
            if(!wcscmp(getProperty(i)->getName(), TEXT("LOGO")))
                if(getProperty(i)->getValue()) {
                    vCardProperty *prop = getPropertyFromVProperty(getProperty(i));
                    if(!businessDetail)
                        businessDetail = new BusinessDetail();
                    businessDetail->setLogo(*prop);

                    delete prop;
                }
            if(!wcscmp(getProperty(i)->getName(), TEXT("TEL")))
                if(getProperty(i)->isType(TEXT("WORK"))) {
                    if(getProperty(i)->isType(TEXT("CELL"))) {
                        vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                        Phone* phone = new Phone();
                        phone->setPhoneNumber(*prop);
                        phone->setType(TEXT("MobileBusinessTelephoneNumber"));
                        if(!phones)
                            phones = new ArrayList();
                        phones->add((ArrayElement&) *phone);
                        delete phone;
                    }

                    if(getProperty(i)->isType(TEXT("VOICE"))) {
                        vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                        Phone* phone = new Phone();
                        phone->setPhoneNumber(*prop);
                        phone->setType(TEXT("BusinessTelephoneNumber"));
                        if(!phones)
                            phones = new ArrayList();
                        phones->add((ArrayElement&) *phone);
                        delete phone;
                    }

                    if(getProperty(i)->isType(TEXT("FAX"))) {
                        vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                        Phone* phone = new Phone();
                        phone->setPhoneNumber(*prop);
                        phone->setType(TEXT("BusinessFaxNumber"));
                        if(!phones)
                            phones = new ArrayList();
                        phones->add((ArrayElement&) *phone);
                        delete phone;
                    }
                }
            if(!wcscmp(getProperty(i)->getName(), TEXT("EMAIL")))
                if(getProperty(i)->containsParameter(TEXT("WORK")) && getProperty(i)->isType(TEXT("INTERNET"))) {
                    vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                    Email* email = new Email();
                    email->setEmailAddress(*prop);
                    email->setType(TEXT("Email3Address"));
                    if(!emails)
                        emails = new ArrayList();
                    emails->add((ArrayElement&) *email);
                    delete email;
                }
            if(!wcscmp(getProperty(i)->getName(), TEXT("URL")))
                if(getProperty(i)->containsParameter(TEXT("WORK"))) {
                    vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                    WebPage* url = new WebPage();
                    url->setWebPage(*prop);
                    url->setType(TEXT("BusinessWebPage"));
                    if(!webPages)
                        webPages = new ArrayList();
                    webPages->add((ArrayElement&) *url);
                    delete url;
                }
        }
       if(titlesIndex>0) {
            if(!businessDetail)
                businessDetail = new BusinessDetail();
            businessDetail->setTitles(titles, titlesIndex);
            delete *titles;
        }
        if(phones) {
            if(!contactDetail)
                contactDetail = new ContactDetail();
            contactDetail->setPhones(*phones);
            delete phones;
        }
        if(emails) {
            if(!contactDetail)
                contactDetail = new ContactDetail();
            contactDetail->setEmails(*emails);
            delete emails;
        }
        if(webPages) {
            if(!contactDetail)
                contactDetail = new ContactDetail();
            contactDetail->setWebPages(*webPages);
            delete webPages;
        }
        if(contactDetail) {
            if(!businessDetail)
                businessDetail = new BusinessDetail();
            businessDetail->setContactDetail(*contactDetail);
            delete contactDetail;
        }
    }
    return businessDetail;
}

void Contact::setBusinessDetail(BusinessDetail& d) {
    if (businessDetail) delete businessDetail;

    businessDetail = d.clone();

    for(int i = 0, m = propertiesCount(); i < m;i++) {
        if(!wcscmp(getProperty(i)->getName(), TEXT("ADR")) && getProperty(i)->isType(TEXT("WORK"))) {
            removeProperty(i);
            --i;
            --m;
        }
        if(!wcscmp(getProperty(i)->getName(), TEXT("LABEL")) && getProperty(i)->isType(TEXT("WORK"))) {
            removeProperty(i);
            --i;
            --m;
        }
        if(!wcscmp(getProperty(i)->getName(), TEXT("ROLE"))) {
            removeProperty(i);
            --i;
            --m;
        }
        if(!wcscmp(getProperty(i)->getName(), TEXT("TITLE"))) {
            removeProperty(i);
            --i;
            --m;
        }
        if(!wcscmp(getProperty(i)->getName(), TEXT("ORG"))) {
            removeProperty(i);
            --i;
            --m;
        }
        if(!wcscmp(getProperty(i)->getName(), TEXT("LOGO"))) {
            removeProperty(i);
            --i;
            --m;
        }
        if(!wcscmp(getProperty(i)->getName(), TEXT("TEL")) && getProperty(i)->isType(TEXT("WORK"))) {
                    removeProperty(i);
                    --i;
                    --m;
                }
        if(!wcscmp(getProperty(i)->getName(), TEXT("EMAIL")) && getProperty(i)->containsParameter(TEXT("WORK"))
            && getProperty(i)->isType(TEXT("INTERNET"))) {
                removeProperty(i);
                --i;
                --m;
            }
            if(!wcscmp(getProperty(i)->getName(), TEXT("URL")) && getProperty(i)->containsParameter(TEXT("WORK"))) {
                removeProperty(i);
                --i;
                --m;
            }
    }

    if(businessDetail) {
        if(businessDetail->getAddress()) {
            VProperty* vp = composeVAddress(businessDetail->getAddress());
            vp->addParameter(TEXT("TYPE"), TEXT("WORK"));
            insertProperty(vp);

            if(businessDetail->getAddress()->getLabel()) {
                VProperty* vp = getVPropertyFromProperty(TEXT("LABEL"),businessDetail->getAddress()->getLabel());
                vp->addParameter(TEXT("TYPE"), TEXT("WORK"));
                insertProperty(vp);
            }
        }
        if(businessDetail->getContactDetail()) {
            if(businessDetail->getContactDetail()->getEmails()) {
                ArrayList* emails = businessDetail->getContactDetail()->getEmails();
                for (int i=0; i<emails->size(); ++i) {
                    if(((Email*) emails->get(i))->getProperty()) {
                        VProperty* vp = getVPropertyFromProperty(TEXT("EMAIL"), ((Email*)emails->get(i))->getProperty());
                        vp->addParameter(TEXT("WORK"), NULL);
                        vp->addParameter(TEXT("TYPE"),TEXT("INTERNET"));
                        insertProperty(vp);
                    }
                }
            }
            if(businessDetail->getContactDetail()->getWebPages()) {
                ArrayList* webs = businessDetail->getContactDetail()->getWebPages();
                for (int i=0; i<webs->size(); ++i) {
                    if(((WebPage*) webs->get(i))->getProperty()) {
                        VProperty* vp = getVPropertyFromProperty(TEXT("URL"), ((WebPage*)webs->get(i))->getProperty());
                        vp->addParameter(TEXT("WORK"), NULL);
                        insertProperty(vp);
                    }
                }
            }
            if(businessDetail->getContactDetail()->getPhones()) {
                ArrayList* phones = businessDetail->getContactDetail()->getPhones();
                for (int i=0; i<phones->size(); ++i)
                    if(((Phone*) phones->get(i))->getProperty()) {
                        VProperty* vp = getVPropertyFromProperty(TEXT("TEL"), ((Phone*)phones->get(i))->getProperty());
                        if(!wcscmp(TEXT("MobileBusinessTelephoneNumber"),((Phone*)phones->get(i))->getType()))
                            vp->addParameter(TEXT("TYPE"),TEXT("WORK,CELL"));
                        if(!wcscmp(TEXT("BusinessTelephoneNumber"),((Phone*)phones->get(i))->getType()))
                            vp->addParameter(TEXT("TYPE"),TEXT("WORK,VOICE"));
                        if(!wcscmp(TEXT("BusinessFaxNumber"),((Phone*)phones->get(i))->getType()))
                            vp->addParameter(TEXT("TYPE"),TEXT("WORK,FAX"));
                        insertProperty(vp);
                    }
            }
        }
        if(businessDetail->getRole()) {
            VProperty* vp = getVPropertyFromProperty(TEXT("ROLE"), businessDetail->getRole());
            insertProperty(vp);
        }
        if(businessDetail->getLogo()) {
            VProperty* vp = getVPropertyFromProperty(TEXT("LOGO"), businessDetail->getLogo());
            insertProperty(vp);
        }
        if(businessDetail->getCompany()) {
            VProperty* vp = getVPropertyFromProperty(TEXT("ORG"), businessDetail->getCompany());
            insertProperty(vp);
        }

        if(businessDetail->getTitles()) {
            ArrayList* titles = businessDetail->getTitles();
            for (int i=0; i<titles->size(); ++i) {
                if(((Title*)titles->get(i))->getProperty()) {
                    VProperty* vp = getVPropertyFromProperty(TEXT("TITLE"), ((Title*)titles->get(i))->getProperty());
                    insertProperty(vp);
                }
            }
        }
    }
}

PersonalDetail* Contact::getPersonalDetail() {
    if(!personalDetail) {
        ArrayList* phones = new ArrayList();
        ArrayList* emails = new ArrayList();
        ArrayList* webPages = new ArrayList();
        ContactDetail* contactDetail = NULL;
        Address* adr = NULL;

        for(int i = 0; i<propertiesCount();i++) {
            if(!wcscmp(getProperty(i)->getName(), TEXT("ADR")) && getProperty(i)->isType(TEXT("HOME")))
                if(getProperty(i)->getValue()) {
                    adr = composeAddress(getProperty(i), HOME);
                    if(!personalDetail)
                        personalDetail = new PersonalDetail();
                    personalDetail->setAddress(*adr);
                    delete adr;
                }
            if(!wcscmp(getProperty(i)->getName(), TEXT("LABEL")) && getProperty(i)->isType(TEXT("HOME")))
                if(getProperty(i)->getValue()) {
                    adr = addLabelAddress(getProperty(i), HOME);
                    if(!personalDetail)
                        personalDetail = new PersonalDetail();
                    personalDetail->setAddress(*adr);
                    delete adr;
                }
            if(!wcscmp(getProperty(i)->getName(), TEXT("ADR")) && !getProperty(i)->isType(TEXT("HOME"))
                && !getProperty(i)->isType(TEXT("WORK")))
                if(getProperty(i)->getValue()) {
                    adr = composeAddress(getProperty(i), OTHER);
                    if(!personalDetail)
                        personalDetail = new PersonalDetail();
                    personalDetail->setOtherAddress(*adr);
                    delete adr;
                }
            if(!wcscmp(getProperty(i)->getName(), TEXT("LABEL")) && !getProperty(i)->isType(TEXT("HOME"))
                && !getProperty(i)->isType(TEXT("WORK")))
                if(getProperty(i)->getValue()) {
                    adr = addLabelAddress(getProperty(i), OTHER);
                    if(!personalDetail)
                        personalDetail = new PersonalDetail();
                    personalDetail->setOtherAddress(*adr);
                    delete adr;
                }
            if(!wcscmp(getProperty(i)->getName(), TEXT("PHOTO")))
                if(getProperty(i)->getValue()) {
                    vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                    if(!personalDetail)
                        personalDetail = new PersonalDetail();
                    personalDetail->setPhoto(*prop);
                }
            if(!wcscmp(getProperty(i)->getName(), TEXT("BDAY")) && getProperty(i)->getValue()) {
                if(!personalDetail)
                    personalDetail = new PersonalDetail();
                personalDetail->setBirthday(getProperty(i)->getValue());
            }

            if(!wcscmp(getProperty(i)->getName(), TEXT("TEL"))) {
                if(getProperty(i)->isType(TEXT("HOME"))) {
                    if(getProperty(i)->isType(TEXT("CELL"))) {
                        vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                        Phone* phone = new Phone();
                        phone->setPhoneNumber(*prop);
                        phone->setType(TEXT("MobileHomeTelephoneNumber"));
                        phones->add((ArrayElement&) *phone);
                        delete phone;
                    }

                    if(getProperty(i)->isType(TEXT("VOICE"))) {
                        vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                        Phone* phone = new Phone();
                        phone->setPhoneNumber(*prop);
                        phone->setType(TEXT("HomeTelephoneNumber"));
                        phones->add((ArrayElement&) *phone);
                        delete phone;
                    }

                    if(getProperty(i)->isType(TEXT("FAX"))) {
                        vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                        Phone* phone = new Phone();
                        phone->setPhoneNumber(*prop);
                        phone->setType(TEXT("HomeFaxNumber"));
                        phones->add((ArrayElement&) *phone);
                        delete phone;
                    }
                }
                if(getProperty(i)->containsParameter(TEXT("PAGER"))) {
                    vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                    Phone* phone = new Phone();
                    phone->setPhoneNumber(*prop);
                    phone->setType(TEXT("PagerNumber"));
                    phones->add((ArrayElement&) *phone);
                    delete phone;
                }
                if(getProperty(i)->isType(TEXT("CELL")) && !getProperty(i)->isType(TEXT("HOME"))
                    && !getProperty(i)->isType(TEXT("WORK"))) {
                        vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                        Phone* phone = new Phone();
                        phone->setPhoneNumber(*prop);
                        phone->setType(TEXT("MobileTelephoneNumber"));
                        phones->add((ArrayElement&) *phone);
                        delete phone;
                    }
                if(getProperty(i)->isType(TEXT("VOICE")) && !getProperty(i)->isType(TEXT("HOME"))
                    && !getProperty(i)->isType(TEXT("WORK"))) {
                        vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                        Phone* phone = new Phone();
                        phone->setPhoneNumber(*prop);
                        phone->setType(TEXT("OtherTelephoneNumber"));
                        phones->add((ArrayElement&) *phone);
                        delete phone;
                    }
                if(getProperty(i)->isType(TEXT("FAX")) && !getProperty(i)->isType(TEXT("HOME"))
                    && !getProperty(i)->isType(TEXT("WORK"))) {
                        vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                        Phone* phone = new Phone();
                        phone->setPhoneNumber(*prop);
                        phone->setType(TEXT("OtherFaxNumber"));
                        phones->add((ArrayElement&) *phone);
                        delete phone;
                    }
            }
            if(!wcscmp(getProperty(i)->getName(), TEXT("EMAIL"))) {
                if(getProperty(i)->containsParameter(TEXT("HOME")) && getProperty(i)->isType(TEXT("INTERNET"))) {
                    vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                    Email* email = new Email();
                    email->setEmailAddress(*prop);
                    email->setType(TEXT("Email2Address"));
                    emails->add((ArrayElement&) *email);
                    delete email;
                }
                if(getProperty(i)->isType(TEXT("INTERNET")) && !getProperty(i)->containsParameter(TEXT("HOME"))
                    && !getProperty(i)->containsParameter(TEXT("WORK"))) {
                    vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                    Email* email = new Email();
                    email->setEmailAddress(*prop);
                    email->setType(TEXT("Email1Address"));
                    emails->add((ArrayElement&) *email);
                    delete email;
                }
            }
            if(!wcscmp(getProperty(i)->getName(), TEXT("URL"))) {
                if(getProperty(i)->containsParameter(TEXT("HOME"))) {
                    vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                    WebPage* url = new WebPage();
                    url->setWebPage(*prop);
                    url->setType(TEXT("HomeWebPage"));
                    webPages->add((ArrayElement&) *url);
                    delete url;
                }
                if(!getProperty(i)->containsParameter(TEXT("HOME")) && !getProperty(i)->containsParameter(TEXT("WORK"))) {
                    vCardProperty* prop = getPropertyFromVProperty(getProperty(i));
                    WebPage* url = new WebPage();
                    url->setWebPage(*prop);
                    url->setType(TEXT("WebPage"));
                    webPages->add((ArrayElement&) *url);
                    delete url;
                }
            }
        } //end for VProperties
        if(phones) {
            if(!contactDetail)
                contactDetail = new ContactDetail();
            contactDetail->setPhones(*phones);
            delete phones;
        }
        if(emails) {
            if(!contactDetail)
                contactDetail = new ContactDetail();
            contactDetail->setEmails(*emails);
            delete emails;
        }
        if(webPages) {
            if(!contactDetail)
                contactDetail = new ContactDetail();
            contactDetail->setWebPages(*webPages);
            delete webPages;
        }
        if(contactDetail) {
            if(!personalDetail)
                personalDetail = new PersonalDetail();
            personalDetail->setContactDetail(*contactDetail);
            delete contactDetail;
        }
    }
    return personalDetail;
}

void Contact::setPersonalDetail(PersonalDetail& d) {
    if (personalDetail) delete personalDetail;

    personalDetail = d.clone();

    for(int i = 0, m = propertiesCount(); i < m ; i++) {
        if(!wcscmp(getProperty(i)->getName(), TEXT("ADR")) && getProperty(i)->isType(TEXT("HOME"))) {
            removeProperty(i);
            --i;
            --m;
        }
        if(!wcscmp(getProperty(i)->getName(), TEXT("ADR")) && !getProperty(i)->isType(TEXT("HOME"))
            && !getProperty(i)->isType(TEXT("WORK"))) {
            removeProperty(i);
            --i;
            --m;
        }
        if(!wcscmp(getProperty(i)->getName(), TEXT("LABEL")) && getProperty(i)->isType(TEXT("HOME"))) {
            removeProperty(i);
            --i;
            --m;
        }
        if(!wcscmp(getProperty(i)->getName(), TEXT("LABEL")) && !getProperty(i)->isType(TEXT("HOME"))
            && !getProperty(i)->isType(TEXT("WORK"))) {
                removeProperty(i);
                --i;
                --m;
            }
        if(!wcscmp(getProperty(i)->getName(), TEXT("PHOTO"))) {
            removeProperty(i);
            --i;
            --m;
        }
        if(!wcscmp(getProperty(i)->getName(), TEXT("BDAY"))) {
            removeProperty(i);
            --i;
            --m;
        }
        if(!wcscmp(getProperty(i)->getName(), TEXT("TEL")) && !getProperty(i)->isType(TEXT("WORK")))
            if(getProperty(i)->isType(TEXT("CELL")) || getProperty(i)->isType(TEXT("VOICE"))
                || getProperty(i)->isType(TEXT("FAX"))) {
                    removeProperty(i);
                    --i;
                    --m;
                }
        if(!wcscmp(getProperty(i)->getName(), TEXT("EMAIL")) && !getProperty(i)->containsParameter(TEXT("WORK"))
            && getProperty(i)->isType(TEXT("INTERNET"))) {
                removeProperty(i);
                --i;
                --m;
            }
        if(!wcscmp(getProperty(i)->getName(), TEXT("URL")) && !getProperty(i)->containsParameter(TEXT("WORK"))) {
            removeProperty(i);
            --i;
            --m;
        }
    }

    if(personalDetail) {
        if(personalDetail->getAddress()) {
            VProperty* vp = composeVAddress(personalDetail->getAddress());
            vp->addParameter(TEXT("TYPE"),TEXT("HOME"));
            insertProperty(vp);

            if(personalDetail->getAddress()->getLabel()) {
                VProperty* vp = getVPropertyFromProperty(TEXT("LABEL"),personalDetail->getAddress()->getLabel());
                vp->addParameter(TEXT("TYPE"),TEXT("HOME"));
                insertProperty(vp);
            }
        }
        if(personalDetail->getOtherAddress()) {
            VProperty* vp = composeVAddress(personalDetail->getOtherAddress());
            insertProperty(vp);

            if(personalDetail->getOtherAddress()->getLabel()) {
                VProperty* vp = getVPropertyFromProperty(TEXT("LABEL"),personalDetail->getOtherAddress()->getLabel());
                insertProperty(vp);
            }
        }
        if(personalDetail->getContactDetail()) {
            if(personalDetail->getContactDetail()->getEmails()) {
                ArrayList* emails = personalDetail->getContactDetail()->getEmails();
                for (int i=0; i<emails->size(); ++i) {
                    if(((Email*) emails->get(i))->getProperty()) {
                        VProperty* vp = getVPropertyFromProperty(TEXT("EMAIL"), ((Email*)emails->get(i))->getProperty());
                        if(!wcscmp(TEXT("Email2Address") ,((Email*)emails->get(i))->getType()))
                            vp->addParameter(TEXT("HOME"), NULL);
                        vp->addParameter(TEXT("TYPE"),TEXT("INTERNET"));
                        insertProperty(vp);
                    }
                }
            }
            if(personalDetail->getContactDetail()->getWebPages()) {
                ArrayList* webs = personalDetail->getContactDetail()->getWebPages();
                for (int i=0; i<webs->size(); ++i) {
                    if(((WebPage*) webs->get(i))->getProperty()) {
                        VProperty* vp = getVPropertyFromProperty(TEXT("URL"), ((WebPage*)webs->get(i))->getProperty());
                        if(!wcscmp(TEXT("HomeWebPage") ,((WebPage*)webs->get(i))->getType()))
                            vp->addParameter(TEXT("HOME"), NULL);
                        insertProperty(vp);
                    }
                }
            }
            if(personalDetail->getContactDetail()->getPhones()) {
                ArrayList* phones = personalDetail->getContactDetail()->getPhones();
                for (int i=0; i<phones->size(); ++i)
                    if(((Phone*) phones->get(i))->getProperty()) {
                        VProperty* vp = getVPropertyFromProperty(TEXT("TEL"), ((Phone*)phones->get(i))->getProperty());
                        if(!wcscmp(TEXT("MobileTelephoneNumber"),((Phone*)phones->get(i))->getType()))
                            vp->addParameter(TEXT("TYPE"), TEXT("CELL"));
                        if(!wcscmp(TEXT("MobileHomeTelephoneNumber"),((Phone*)phones->get(i))->getType()))
                            vp->addParameter(TEXT("TYPE"),TEXT("HOME,CELL"));
                        if(!wcscmp(TEXT("OtherTelephoneNumber"),((Phone*)phones->get(i))->getType()))
                            vp->addParameter(TEXT("TYPE"),TEXT("VOICE"));
                        if(!wcscmp(TEXT("HomeTelephoneNumber"),((Phone*)phones->get(i))->getType()))
                            vp->addParameter(TEXT("TYPE"),TEXT("HOME,VOICE"));
                        if(!wcscmp(TEXT("OtherFaxNumber"),((Phone*)phones->get(i))->getType()))
                            vp->addParameter(TEXT("TYPE"), TEXT("FAX"));
                        if(!wcscmp(TEXT("HomeFaxNumber"),((Phone*)phones->get(i))->getType()))
                             vp->addParameter(TEXT("TYPE"), TEXT("HOME,FAX"));
                        insertProperty(vp);
                    }
            }
        } //end contactDetail
        if(personalDetail->getPhoto()) {
            VProperty* vp = getVPropertyFromProperty(TEXT("PHOTO"), personalDetail->getPhoto());
            insertProperty(vp);
        }
        if(personalDetail->getBirthday()) {
            VProperty* vp = new VProperty(TEXT("BDAY"), personalDetail->getBirthday());
            insertProperty(vp);
        }
    }
}

void Contact::setUID (WCHAR* u) {
    if (containsProperty(TEXT("UID")))
        getProperty(TEXT("UID"))->setValue(u);
    else
        insertProperty(new VProperty(TEXT("UID"),u));
}

void Contact::setTimezone (WCHAR* tz) {
    if (containsProperty(TEXT("TZ")))
        getProperty(TEXT("TZ"))->setValue(tz);
    else
        insertProperty(new VProperty(TEXT("TZ"),tz));
}

void Contact::setRevision (WCHAR* r) {
    if (containsProperty(TEXT("REV")))
        getProperty(TEXT("REV"))->setValue(r);
    else
        insertProperty(new VProperty(TEXT("REV"),r));
}

void Contact::set(WCHAR** p, WCHAR* v) {
    if(*p)
        delete *p;
    *p = wstrdup(v);
}

vCardProperty* Contact::getPropertyFromVProperty(VProperty* vp) {

    if(vp) {
        vCardProperty *prop = new vCardProperty(vp->getValue());
        if(vp->getParameterValue(TEXT("ENCODING")))
            prop->setEncoding(vp->getParameterValue(TEXT("ENCODING")));
        if(vp->containsParameter(TEXT("BASE64")))
            prop->setEncoding(TEXT("BASE64"));
        if(vp->containsParameter(TEXT("QUOTED-PRINTABLE")))
            prop->setEncoding(TEXT("QUOTED-PRINTABLE"));
        if(vp->getParameterValue(TEXT("LANGUAGE")))
            prop->setLanguage(vp->getParameterValue(TEXT("LANGUAGE")));
        if(vp->getParameterValue(TEXT("CHARSET")))
            prop->setCharset(vp->getParameterValue(TEXT("CHARSET")));
        return prop;
    }

    return NULL;
}

VProperty* Contact::getVPropertyFromProperty(WCHAR*name, vCardProperty* prop) {
    if(prop && name) {
        VProperty *vprop = new VProperty(name, prop->getValue());
        if(prop->getCharset())
            vprop->addParameter(TEXT("CHARSET"), prop->getCharset());
        if(prop->getLanguage())
            vprop->addParameter(TEXT("LANGUAGE"), prop->getLanguage());
        if(prop->getEncoding())
            vprop->addParameter(TEXT("ENCODING"), prop->getEncoding());
        return vprop;
    }

    return NULL;
}

VProperty* Contact::composeVAddress(Address* adr) {
    WCHAR *addressValue = new WCHAR[MAX_VPROPERTY_VALUE + 1];
    WCHAR *encoding = NULL;
    WCHAR *language = NULL;
    WCHAR *charset = NULL;

    wcscpy(addressValue,TEXT(""));
    if(adr->getPostOfficeAddress()) {
        wcscat(addressValue, adr->getPostOfficeAddress()->getValue());
        if(adr->getPostOfficeAddress()->getCharset()) {
            charset = new WCHAR[MAX_VPROPERTY_VALUE +1 ];
            wcscpy(charset, adr->getPostOfficeAddress()->getCharset());
        }
        if(adr->getPostOfficeAddress()->getLanguage()) {
            language = new WCHAR[MAX_VPROPERTY_VALUE + 1];
            wcscpy(language, adr->getPostOfficeAddress()->getLanguage());
        }
        if(adr->getPostOfficeAddress()->getEncoding()) {
            encoding = new WCHAR[MAX_VPROPERTY_VALUE +1 ];
            wcscpy(encoding, adr->getPostOfficeAddress()->getEncoding());
        }
    }
    wcscat(addressValue, TEXT(";"));
    if(adr->getRoomNumber()) {
        wcscat(addressValue, adr->getRoomNumber()->getValue());
        if(!charset)
            if(adr->getRoomNumber()->getCharset()) {
                charset = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(charset, adr->getRoomNumber()->getCharset());
            }
        if(!language)
            if(adr->getRoomNumber()->getLanguage()) {
                language = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(language, adr->getRoomNumber()->getLanguage());
            }
        if(!encoding)
            if(adr->getRoomNumber()->getEncoding()) {
                encoding = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(encoding, adr->getRoomNumber()->getEncoding());
            }
    }
    wcscat(addressValue, TEXT(";"));
    if(adr->getStreet()) {
        wcscat(addressValue, adr->getStreet()->getValue());
        if(!charset)
            if(adr->getStreet()->getCharset()) {
                charset = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(charset, adr->getStreet()->getCharset());
            }
        if(!language)
            if(adr->getStreet()->getLanguage()) {
                language = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(language, adr->getStreet()->getLanguage());
            }
        if(!encoding)
            if(adr->getStreet()->getEncoding()) {
                encoding = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(encoding, adr->getStreet()->getEncoding());
            }
    }
    wcscat(addressValue, TEXT(";"));
    if(adr->getCity()) {
        wcscat(addressValue, adr->getCity()->getValue());
        if(!charset)
            if(adr->getCity()->getCharset()) {
                charset = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(charset, adr->getCity()->getCharset());
            }
        if(!language)
            if(adr->getCity()->getLanguage()) {
                language = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(language, adr->getCity()->getLanguage());
            }
        if(!encoding)
            if(adr->getCity()->getEncoding()) {
                encoding = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(encoding, adr->getCity()->getEncoding());
            }
    }
    wcscat(addressValue, TEXT(";"));
    if(adr->getState()) {
        wcscat(addressValue, adr->getState()->getValue());
        if(!charset)
            if(adr->getState()->getCharset()) {
                charset = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(charset, adr->getState()->getCharset());
            }
        if(!language)
            if(adr->getState()->getLanguage()) {
                language = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(language, adr->getState()->getLanguage());
            }
        if(!encoding)
            if(adr->getState()->getEncoding()) {
                encoding = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(encoding, adr->getState()->getEncoding());
            }
    }
    wcscat(addressValue, TEXT(";"));
    if(adr->getPostalCode()) {
        wcscat(addressValue, adr->getPostalCode()->getValue());
        if(!charset)
            if(adr->getPostalCode()->getCharset()) {
                charset = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(charset, adr->getPostalCode()->getCharset());
            }
        if(!language)
            if(adr->getPostalCode()->getLanguage()) {
                language = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(language, adr->getPostalCode()->getLanguage());
            }
        if(!encoding)
            if(adr->getPostalCode()->getEncoding()) {
                encoding = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(encoding, adr->getPostalCode()->getEncoding());
            }
    }
    wcscat(addressValue, TEXT(";"));
    if(adr->getCountry()) {
        wcscat(addressValue, adr->getCountry()->getValue());
        if(!charset)
            if(adr->getCountry()->getCharset()) {
                charset = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(charset, adr->getCountry()->getCharset());
            }
        if(!language)
            if(adr->getCountry()->getLanguage()) {
                language = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(language, adr->getCountry()->getLanguage());
            }
        if(!encoding)
            if(adr->getCountry()->getEncoding()) {
                encoding = new WCHAR[MAX_VPROPERTY_VALUE + 1];
                wcscpy(encoding, adr->getCountry()->getEncoding());
            }
    }

    VProperty* vp = new VProperty(TEXT("ADR"), addressValue);
    if(charset)
        vp->addParameter(TEXT("CHARSET"), charset);
    if(language)
        vp->addParameter(TEXT("LANGUAGE"), language);
    if(encoding)
        vp->addParameter(TEXT("ENCODING"), encoding);

    return vp;
}

Address* Contact::composeAddress(VProperty* vp, int type) {
    if(vp) {
        Address* adr;
        switch (type) {
            case BUSINESS:
                if(!businessDetail || !businessDetail->getAddress())
                    adr = new Address();
                else
                    adr = businessDetail->getAddress()->clone();
                break;
            case HOME:
                if(!personalDetail || !personalDetail->getAddress())
                    adr = new Address();
                else
                    adr = personalDetail->getAddress()->clone();
                break;
            case OTHER:
                if(!personalDetail || !personalDetail->getOtherAddress())
                    adr = new Address();
                else
                    adr = personalDetail->getOtherAddress()->clone();
                break;
            default:
                return NULL;
        }

        if(vp->getPropComponent(1)){
            vCardProperty *prop = getPropertyFromVProperty(vp);
            prop->setValue(vp->getPropComponent(1));
            adr->setPostOfficeAddress(*prop);

            delete prop;
        }
        if(vp->getPropComponent(2)){
            vCardProperty *prop = getPropertyFromVProperty(vp);
            prop->setValue(vp->getPropComponent(2));
            adr->setRoomNumber(*prop);

            delete prop;
        }
        if(vp->getPropComponent(3)){
            vCardProperty *prop = getPropertyFromVProperty(vp);
            prop->setValue(vp->getPropComponent(3));
            adr->setStreet(*prop);

            delete prop;
        }
        if(vp->getPropComponent(4)){
            vCardProperty *prop = getPropertyFromVProperty(vp);
            prop->setValue(vp->getPropComponent(4));
            adr->setCity(*prop);

            delete prop;
        }
        if(vp->getPropComponent(5)){
            vCardProperty *prop = getPropertyFromVProperty(vp);
            prop->setValue(vp->getPropComponent(5));
            adr->setState(*prop);

            delete prop;
        }
        if(vp->getPropComponent(6)){
            vCardProperty *prop = getPropertyFromVProperty(vp);
            prop->setValue(vp->getPropComponent(6));
            adr->setPostalCode(*prop);

            delete prop;
        }
        if(vp->getPropComponent(7)){
            vCardProperty *prop = getPropertyFromVProperty(vp);
            prop->setValue(vp->getPropComponent(7));
            adr->setCountry(*prop);

            delete prop;
        }
        return adr;
    }
    return NULL;
}

Address* Contact::addLabelAddress(VProperty* vp, int type) {
    if(vp) {
        Address* adr;

        switch (type) {
            case BUSINESS:
                if(!businessDetail || !businessDetail->getAddress())
                    adr = new Address();
                else
                    adr = businessDetail->getAddress()->clone();
                break;
            case HOME:
                if(!personalDetail || !personalDetail->getAddress())
                    adr = new Address();
                else
                    adr = personalDetail->getAddress()->clone();
                break;
            case OTHER:
                if(!personalDetail || !personalDetail->getOtherAddress())
                    adr = new Address();
                else
                    adr = personalDetail->getOtherAddress()->clone();
                break;
            default:
                return NULL;
        }

        if(vp->getValue()) {
            vCardProperty *prop = getPropertyFromVProperty(vp);
            adr->setLabel(*prop);

            delete prop;
        }
        return adr;
    }
    return NULL;
}

Contact* Contact::clone() {

    Contact* ret = new Contact();
    VProperty* property;

    for(int i = 0; i < this->propertiesCount(); i++) {
        property = (VProperty*)this->getProperty(i)->clone();
        ret->addProperty(property);
    }
    return ret;
}

WCHAR* Contact::toString() {
    if(!containsProperty(TEXT("BEGIN")) && !containsProperty(TEXT("VERSION")) && !containsProperty(TEXT("END"))) {
        //this means the Contact was created with new, not using vConverter::parse
        //we need to add BEGIN, VERSION, END to VObject properties
        addFirstProperty(new VProperty(TEXT("VERSION"), getVersion()));
        addFirstProperty(new VProperty(TEXT("BEGIN"), TEXT("VCARD")));
        addProperty(new VProperty(TEXT("END"), TEXT("VCARD")));
    }

    return ((VObject*)this)->toString();
}
