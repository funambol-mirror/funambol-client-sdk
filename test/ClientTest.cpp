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

/** @cond API */
/** @addtogroup ClientTest */
/** @{ */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef ENABLE_INTEGRATION_TESTS

#include "ClientTest.h"
#include "base/globalsdef.h"
#include "base/test.h"
#include "base/util/StringBuffer.h"

#include <memory>
#include <vector>
#include <utility>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <algorithm>

USE_NAMESPACE

/** utility function to iterate over different kinds of items in a sync source */
static std::list<std::string> listAnyItems(
    SyncSource *source,
    SyncItem * (SyncSource::*first)(),
    SyncItem * (SyncSource::*next)() )
{
    SyncItem *item = NULL;
    std::list<std::string> res;

    CPPUNIT_ASSERT(source);
    CPPUNIT_ASSERT(!source->getReport() || source->getReport()->getState() != SOURCE_ERROR);
    SOURCE_ASSERT_NO_FAILURE(source, item = (source->*first)());
    while ( item ) {
        const char *key = item->getKey();
        SOURCE_ASSERT(source, key);
        SOURCE_ASSERT(source, key[0]);
        res.push_back(key);
        delete item;
        SOURCE_ASSERT_NO_FAILURE(source, item = (source->*next)());
    }

    return res;
}

static std::list<std::string> listNewItems( SyncSource *source )
{
    std::list<std::string> res = listAnyItems(
        source,
        &SyncSource::getFirstNewItem,
        &SyncSource::getNextNewItem );
    return res;
}

static std::list<std::string> listUpdatedItems( SyncSource *source )
{
    std::list<std::string> res = listAnyItems(
        source,
        &SyncSource::getFirstUpdatedItem,
        &SyncSource::getNextUpdatedItem );
    return res;
}

static std::list<std::string> listDeletedItems( SyncSource *source )
{
    std::list<std::string> res = listAnyItems(
        source,
        &SyncSource::getFirstDeletedItem,
        &SyncSource::getNextDeletedItem );
    return res;
}

static std::list<std::string> listItems( SyncSource *source )
{
    std::list<std::string> res = listAnyItems(
        source,
        &SyncSource::getFirstItem,
        &SyncSource::getNextItem );
    return res;
}


std::list<std::string> listItemsOfType(SyncSource *source, itemType type)
{
    std::list<std::string> res;
    
    switch(type) {
     case NEW_ITEMS:
        res = listNewItems(source);
        break;
     case UPDATED_ITEMS:
        res = listUpdatedItems(source);
        break;
     case DELETED_ITEMS:
        res = listDeletedItems(source);
        break;
     case TOTAL_ITEMS:
        res = listItems(source);
        break;
     default:
        CPPUNIT_ASSERT(false);
        break;
    }
    return res;
}

static int countNewItems( SyncSource *source ) { return listNewItems(source).size(); }
static int countUpdatedItems( SyncSource *source ) { return listUpdatedItems(source).size(); }
static int countDeletedItems( SyncSource *source ) { return listDeletedItems(source).size(); }
static int countItems( SyncSource *source ) { return listItems(source).size(); }
int countItemsOfType(SyncSource *source, itemType type) { return listItemsOfType(source, type).size(); }


static void importItem(SyncSource *source, std::string &data)
{
    CPPUNIT_ASSERT(source);
    if (data.size()) {
        SyncItem item;
        item.setData( data.c_str(), (long)data.size() );
        item.setDataType( TEXT("raw") );
        int status = STC_OK;
        SOURCE_ASSERT_NO_FAILURE(source, status = source->addItem(item));
        CPPUNIT_ASSERT(status == STC_OK || status == STC_ITEM_ADDED);
        CPPUNIT_ASSERT(item.getKey() != 0);
        CPPUNIT_ASSERT(wcslen(item.getKey()) > 0);
    }
}

/** adds the supported tests to the instance itself */
void LocalTests::addTests() {
    if (config.createSourceA) {
        ADD_TEST(LocalTests, testOpen);
        ADD_TEST(LocalTests, testIterateTwice);
        if (config.insertItem) {
            ADD_TEST(LocalTests, testSimpleInsert);
            ADD_TEST(LocalTests, testLocalDeleteAll);
            ADD_TEST(LocalTests, testComplexInsert);

            if (config.updateItem) {
                ADD_TEST(LocalTests, testLocalUpdate);

                if (config.createSourceB) {
                    ADD_TEST(LocalTests, testChanges);
                }
            }

            if (config.import &&
                config.dump &&
                config.compare &&
                config.testcases) {
                ADD_TEST(LocalTests, testImport);
                ADD_TEST(LocalTests, testImportDelete);
            }

            if (config.templateItem &&
                config.uniqueProperties) {
                ADD_TEST(LocalTests, testManyChanges);
            }

            if (config.parentItem &&
                config.childItem) {
                ADD_TEST(LocalTests, testLinkedItemsParent);
                ADD_TEST(LocalTests, testLinkedItemsChild);
                ADD_TEST(LocalTests, testLinkedItemsParentChild);
                ADD_TEST(LocalTests, testLinkedItemsChildParent);
                ADD_TEST(LocalTests, testLinkedItemsChildChangesParent);
                ADD_TEST(LocalTests, testLinkedItemsRemoveParentFirst);
                ADD_TEST(LocalTests, testLinkedItemsRemoveNormal);
                ADD_TEST(LocalTests, testLinkedItemsInsertParentTwice);
                ADD_TEST(LocalTests, testLinkedItemsInsertChildTwice);
                ADD_TEST(LocalTests, testLinkedItemsParentUpdate);
                ADD_TEST(LocalTests, testLinkedItemsUpdateChild);
                ADD_TEST(LocalTests, testLinkedItemsInsertBothUpdateChild);
                ADD_TEST(LocalTests, testLinkedItemsInsertBothUpdateParent);
            }
        }
    }
}

std::string LocalTests::insert(CreateSource createSource, const char *data, bool relaxed) {
    std::string uid;

    // create source
    std::auto_ptr<SyncSource> source(createSource());
    CPPUNIT_ASSERT(source.get() != 0);

    // count number of already existing items
    SOURCE_ASSERT(source.get(), source->beginSync() == 0);
    int numItems = 0;
    CPPUNIT_ASSERT_NO_THROW(numItems = countItems(source.get()));
    SyncItem item;
    item.setData(data, (long)strlen(data));
    int status = STC_OK;
    SOURCE_ASSERT_NO_FAILURE(source.get(), status = source->addItem(item));
    CPPUNIT_ASSERT(item.getKey() != 0);
    CPPUNIT_ASSERT(wcslen(item.getKey()) > 0);
    uid = item.getKey();
    SOURCE_ASSERT(source.get(), source->endSync() == 0);

    // delete source again
    CPPUNIT_ASSERT_NO_THROW(source.reset());

    if (!relaxed) {
        // two possible results:
        // - a new item was added
        // - the item was matched against an existing one
        CPPUNIT_ASSERT_NO_THROW(source.reset(createSource()));
        CPPUNIT_ASSERT(source.get() != 0);
        SOURCE_ASSERT(source.get(), source->beginSync() == 0);
        CPPUNIT_ASSERT(status == STC_OK || status == STC_ITEM_ADDED || status == STC_CONFLICT_RESOLVED_WITH_MERGE);
        CPPUNIT_ASSERT_EQUAL(numItems + (status == STC_CONFLICT_RESOLVED_WITH_MERGE ? 0 : 1),
                             countItems(source.get()));
        CPPUNIT_ASSERT(countNewItems(source.get()) == 0);
        CPPUNIT_ASSERT(countUpdatedItems(source.get()) == 0);
        CPPUNIT_ASSERT(countDeletedItems(source.get()) == 0);
        SOURCE_ASSERT(source.get(), source->endSync() == 0 );
        CPPUNIT_ASSERT_NO_THROW(source.reset());
    }

#if 0
    /* source.createItem() is a SyncEvolution extension which cannot be used here */
    SyncItem *sameItem;
    SOURCE_ASSERT_NO_FAILURE(
        source,
        sameItem = source.createItem(item.getKey(), item.getState()));
    CPPUNIT_ASSERT(sameItem != 0);
    CPPUNIT_ASSERT(!strcmp( sameItem->getKey(), item.getKey()));
    delete sameItem;
#endif

    return uid;
}

/** deletes specific item locally via sync source */
static std::string updateItem(CreateSource createSource, const std::string &uid, const char *data) {
    std::string newuid;

    CPPUNIT_ASSERT(createSource.createSource);

    // create source
    std::auto_ptr<SyncSource> source(createSource());
    CPPUNIT_ASSERT(source.get() != 0);
    SOURCE_ASSERT(source.get(), source->beginSync() == 0);

    // insert item
    SyncItem item;
    item.setKey(uid.c_str());
    item.setData(data, (long)strlen(data) + 1);
    item.setDataType(TEXT(""));
    SOURCE_ASSERT_EQUAL(source.get(), (int)STC_OK, source->updateItem(item));
    SOURCE_ASSERT(source.get(), item.getKey());
    newuid = item.getKey();

    SOURCE_ASSERT(source.get(), source->endSync() == 0);
    CPPUNIT_ASSERT_NO_THROW(source.reset());

    return newuid;
}


/**
 * assumes that exactly one element is currently inserted and updates it with the given item
 *
 * The type of the item is cleared, as in insert() above.
 */
void LocalTests::update(CreateSource createSource, const char *data, bool check) {
    CPPUNIT_ASSERT(createSource.createSource);
    CPPUNIT_ASSERT(data);

    // create source
    std::auto_ptr<SyncSource> source(createSource());
    CPPUNIT_ASSERT(source.get() != 0);
    SOURCE_ASSERT(source.get(), source->beginSync() == 0);

    // get existing item, then update it
    SOURCE_ASSERT(source.get(), source->beginSync() == 0 );
    std::auto_ptr<SyncItem> item;
    SOURCE_ASSERT_NO_FAILURE(source.get(), item.reset(source->getFirstItem()) );
    CPPUNIT_ASSERT(item.get());
    item->setData(data, (long)strlen(data) + 1);
    item->setDataType(TEXT(""));
    SOURCE_ASSERT_EQUAL(source.get(), (int)STC_OK, source->updateItem(*item));
    SOURCE_ASSERT(source.get(), source->endSync() == 0);
    CPPUNIT_ASSERT_NO_THROW(source.reset());

    if (!check) {
        return;
    }

    // check that the right changes are reported when reopening the source
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSource()));
    SOURCE_ASSERT(source.get(), source->beginSync() == 0 );
    CPPUNIT_ASSERT_EQUAL(1, countItems(source.get()));
    CPPUNIT_ASSERT_EQUAL(0, countNewItems(source.get()));
    CPPUNIT_ASSERT_EQUAL(0, countUpdatedItems(source.get()));
    CPPUNIT_ASSERT_EQUAL(0, countDeletedItems(source.get()));
    std::auto_ptr<SyncItem> modifiedItem;
    SOURCE_ASSERT_NO_FAILURE(source.get(), modifiedItem.reset(source->getFirstItem()) );
    CPPUNIT_ASSERT(modifiedItem.get());
    CPPUNIT_ASSERT( wcslen( item->getKey() ) );
    CPPUNIT_ASSERT( !wcscmp( item->getKey(), modifiedItem->getKey() ) );
}

/** deletes all items locally via sync source */
void LocalTests::deleteAll(CreateSource createSource) {
    CPPUNIT_ASSERT(createSource.createSource);

    // create source
    std::auto_ptr<SyncSource> source(createSource());
    CPPUNIT_ASSERT(source.get() != 0);
    SOURCE_ASSERT(source.get(), source->beginSync() == 0);

    // delete all items
    SOURCE_ASSERT_NO_FAILURE(source.get(), source->removeAllItems());
    SOURCE_ASSERT(source.get(), source->endSync() == 0);
    CPPUNIT_ASSERT_NO_THROW(source.reset());

    // check that all items are gone
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSource()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    SOURCE_ASSERT_MESSAGE(
        "should be empty now",
        source.get(),
        countItems(source.get()) == 0);
    CPPUNIT_ASSERT_EQUAL( 0, countNewItems(source.get()) );
    CPPUNIT_ASSERT_EQUAL( 0, countUpdatedItems(source.get()) );
    CPPUNIT_ASSERT_EQUAL( 0, countDeletedItems(source.get()) );
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
    CPPUNIT_ASSERT_NO_THROW(source.reset());
}

/** deletes specific item locally via sync source */
static void deleteItem(CreateSource createSource, const std::string &uid) {
    CPPUNIT_ASSERT(createSource.createSource);

    // create source
    std::auto_ptr<SyncSource> source(createSource());
    CPPUNIT_ASSERT(source.get() != 0);
    SOURCE_ASSERT(source.get(), source->beginSync() == 0);

    // delete item
    SyncItem item;
    item.setKey(uid.c_str());
    SOURCE_ASSERT_EQUAL(source.get(), (int)STC_OK, source->deleteItem(item));

    SOURCE_ASSERT(source.get(), source->endSync() == 0);
    CPPUNIT_ASSERT_NO_THROW(source.reset());
}

/**
 * takes two databases, exports them,
 * then compares them using synccompare
 *
 * @param refFile      existing file with source reference items, NULL uses a dump of sync source A instead
 * @param copy         a sync source which contains the copied items, begin/endSync will be called
 * @param raiseAssert  raise assertion if comparison yields differences (defaults to true)
 */
void LocalTests::compareDatabases(const char *refFile, SyncSource &copy, bool raiseAssert) {
    CPPUNIT_ASSERT(config.dump);

    std::string sourceFile, copyFile;

    if (refFile) {
        sourceFile = refFile;
    } else {
        sourceFile = getCurrentTest() + ".source.test.dat";
        simplifyFilename(sourceFile);
        std::auto_ptr<SyncSource> source;
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceA()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        SOURCE_ASSERT_EQUAL(source.get(), 0, config.dump(client, *source.get(), sourceFile.c_str()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
        CPPUNIT_ASSERT_NO_THROW(source.reset());
    }

    copyFile = getCurrentTest() + ".copy.test.dat";
    simplifyFilename(copyFile);
    SOURCE_ASSERT_EQUAL(&copy, 0, copy.beginSync());
    SOURCE_ASSERT_EQUAL(&copy, 0, config.dump(client, copy, copyFile.c_str()));
    SOURCE_ASSERT_EQUAL(&copy, 0, copy.endSync());

    CPPUNIT_ASSERT(config.compare(client, sourceFile.c_str(), copyFile.c_str()));
}

/**
 * insert artificial items, number of them determined by config.numItems
 * unless passed explicitly
 *
 * @param createSource    a factory for the sync source that is to be used
 * @param startIndex      IDs are generated starting with this value
 * @param numItems        number of items to be inserted if non-null, otherwise config.numItems is used
 * @param size            minimum size for new items
 * @return number of items inserted
 */
int LocalTests::insertManyItems(CreateSource createSource, int startIndex, int numItems, int size) {
    CPPUNIT_ASSERT(config.templateItem);
    CPPUNIT_ASSERT(config.uniqueProperties);

    std::auto_ptr<SyncSource> source;
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceA()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    CPPUNIT_ASSERT(startIndex > 1 || !countItems(source.get()));

    int firstIndex = startIndex;
    if (firstIndex < 0) {
        firstIndex = 1;
    }
    int lastIndex = firstIndex + (numItems >= 1 ? numItems : config.numItems) - 1;
    for (int item = firstIndex; item <= lastIndex; item++) {
        std::string data = config.templateItem;
        std::stringstream prefix;

        prefix << std::setfill('0') << std::setw(3) << item << " ";


        const char *prop = config.uniqueProperties;
        const char *nextProp;
        while (*prop) {
            std::string curProp;
            nextProp = strchr(prop, ':');
            if (!nextProp) {
                curProp = prop;
            } else {
                curProp = std::string(prop, 0, nextProp - prop);
            }

            std::string property;
            // property is expected to not start directly at the
            // beginning
            property = "\n";
            property += curProp;
            property += ":";
            size_t off = data.find(property);
            if (off != data.npos) {
                data.insert(off + property.size(), prefix.str());
            }

            if (!nextProp) {
                break;
            }
            prop = nextProp + 1;
        }
        if (size > 0 && (int)data.size() < size) {
            int additionalBytes = size - (int)data.size();
            int added = 0;
            /* vCard 2.1 and vCal 1.0 need quoted-printable line breaks */
            bool quoted = data.find("VERSION:1.0") != data.npos ||
                data.find("VERSION:2.1") != data.npos;
            size_t toreplace = 1;

            CPPUNIT_ASSERT(config.sizeProperty);

            /* stuff the item so that it reaches at least that size */
            size_t off = data.find(config.sizeProperty);
            CPPUNIT_ASSERT(off != data.npos);
            std::stringstream stuffing;
            if (quoted) {
                stuffing << ";ENCODING=QUOTED-PRINTABLE:";
            } else {
                stuffing << ":";
            }

            // insert after the first line, it often acts as the summary
            if (data.find("BEGIN:VJOURNAL") != data.npos) {
                size_t start = data.find(":", off);
                CPPUNIT_ASSERT( start != data.npos );
                size_t eol = data.find("\\n", off);
                CPPUNIT_ASSERT( eol != data.npos );
                stuffing << data.substr(start + 1, eol - start + 1);
                toreplace += eol - start + 1;
            }

            while(added < additionalBytes) {
                int linelen = 0;

                while(added + 4 < additionalBytes &&
                      linelen < 60) {
                    stuffing << 'x';
                    added++;
                    linelen++;
                }
                // insert line breaks to allow folding
                if (quoted) {
                    stuffing << "x=0D=0Ax";
                    added += 8;
                } else {
                    stuffing << "x\\nx";
                    added += 4;
                }
            }
            off = data.find(":", off);
            data.replace(off, toreplace, stuffing.str());
        }

        importItem(source.get(), data);
        data = "";
    }

    SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
    CPPUNIT_ASSERT_NO_THROW(source.reset());
    return lastIndex - firstIndex + 1;
}

// creating sync source
void LocalTests::testOpen() {
    // check requirements
    CPPUNIT_ASSERT(config.createSourceA);

    std::auto_ptr<SyncSource> source(createSourceA());
    CPPUNIT_ASSERT(source.get() != 0);
    CPPUNIT_ASSERT_NO_THROW(source.reset());
}

// restart scanning of items
void LocalTests::testIterateTwice() {
    // check requirements
    CPPUNIT_ASSERT(config.createSourceA);

    // open source
    std::auto_ptr<SyncSource> source(createSourceA());
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    SOURCE_ASSERT_MESSAGE(
        "iterating twice should produce identical results",
        source.get(),
        countItems(source.get()) == countItems(source.get()));
}

// insert one contact without clearing the source first
void LocalTests::testSimpleInsert() {
    // check requirements
    CPPUNIT_ASSERT(config.insertItem);
    CPPUNIT_ASSERT(config.createSourceA);

    insert(createSourceA, config.insertItem);
}

// delete all items
void LocalTests::testLocalDeleteAll() {
    // check requirements
    CPPUNIT_ASSERT(config.insertItem);
    CPPUNIT_ASSERT(config.createSourceA);

    // make sure there is something to delete, then delete again
    insert(createSourceA, config.insertItem);
    deleteAll(createSourceA);
}

// clean database, then insert
void LocalTests::testComplexInsert() {
    testLocalDeleteAll();
    testSimpleInsert();
    testIterateTwice();
}

// clean database, insert item, update it
void LocalTests::testLocalUpdate() {
    // check additional requirements
    CPPUNIT_ASSERT(config.updateItem);

    testLocalDeleteAll();
    testSimpleInsert();
    update(createSourceA, config.updateItem);
}

// complex sequence of changes
void LocalTests::testChanges() {
    // check additional requirements
    CPPUNIT_ASSERT(config.createSourceB);

    testLocalDeleteAll();
    testSimpleInsert();

    // clean changes in sync source B by creating and closing it
    std::auto_ptr<SyncSource> source;
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
    CPPUNIT_ASSERT_NO_THROW(source.reset());

    // no new changes now
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
    std::auto_ptr<SyncItem> item;
    SOURCE_ASSERT_NO_FAILURE(source.get(), item.reset(source->getFirstItem()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
    CPPUNIT_ASSERT_NO_THROW(source.reset());

    // delete item again via sync source A
    deleteAll(createSourceA);
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 1, countDeletedItems(source.get()));
    std::auto_ptr<SyncItem> deletedItem;
    SOURCE_ASSERT_NO_FAILURE(source.get(), deletedItem.reset(source->getFirstDeletedItem()));
    CPPUNIT_ASSERT( wcslen( item->getKey() ) );
    CPPUNIT_ASSERT( wcslen( deletedItem->getKey() ) );
    CPPUNIT_ASSERT( !wcscmp( item->getKey(), deletedItem->getKey() ) );
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
    CPPUNIT_ASSERT_NO_THROW(source.reset());

    // insert another item via sync source A
    testSimpleInsert();
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
    SOURCE_ASSERT_NO_FAILURE(source.get(), item.reset(source->getFirstItem()));
    std::auto_ptr<SyncItem> newItem;
    SOURCE_ASSERT_NO_FAILURE(source.get(), newItem.reset(source->getFirstNewItem()));
    CPPUNIT_ASSERT( wcslen( item->getKey() ) );
    CPPUNIT_ASSERT( wcslen( newItem->getKey() ) );
    CPPUNIT_ASSERT( !wcscmp( item->getKey(), newItem->getKey() ) );
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
    CPPUNIT_ASSERT_NO_THROW(source.reset());

    // update item via sync source A
    update(createSourceA, config.updateItem);
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 1, countUpdatedItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
    std::auto_ptr<SyncItem> updatedItem;
    SOURCE_ASSERT_NO_FAILURE(source.get(), updatedItem.reset(source->getFirstUpdatedItem()));
    CPPUNIT_ASSERT( wcslen( item->getKey() ) );
    CPPUNIT_ASSERT( wcslen( updatedItem->getKey() ) );
    CPPUNIT_ASSERT( !wcscmp( item->getKey(), updatedItem->getKey() ) );
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
    CPPUNIT_ASSERT_NO_THROW(source.reset());

    // start anew, then create and update an item -> should only be listed as new
    // or updated, but not both
    deleteAll(createSourceA);
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
    testSimpleInsert();
    update(createSourceA, config.updateItem);
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()) + countUpdatedItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));

    // start anew, then create, delete and recreate an item -> should only be listed as new or updated,
    // even if (as for calendar with UID) the same LUID gets reused
    deleteAll(createSourceA);
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
    testSimpleInsert();
    deleteAll(createSourceA);
    testSimpleInsert();
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
    SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()) + countUpdatedItems(source.get()));
    if (countDeletedItems(source.get()) == 1) {
        // It's not nice, but acceptable to send the LUID of a deleted item to a
        // server which has never seen that LUID. The LUID must not be the same as
        // the one we list as new or updated, though.
        SyncItem *deleted = source->getFirstDeletedItem();
        CPPUNIT_ASSERT(deleted);
        SyncItem *new_or_updated = source->getFirstNewItem();
        if (!new_or_updated) {
            new_or_updated = source->getFirstUpdatedItem();
        }
        CPPUNIT_ASSERT(new_or_updated);
        CPPUNIT_ASSERT(wcscmp(deleted->getKey(), new_or_updated->getKey()));
    } else {
        SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
    }
}

// clean database, import file, then export again and compare
void LocalTests::testImport() {
    // check additional requirements
    CPPUNIT_ASSERT(config.import);
    CPPUNIT_ASSERT(config.dump);
    CPPUNIT_ASSERT(config.compare);
    CPPUNIT_ASSERT(config.testcases);

    testLocalDeleteAll();

    // import via sync source A
    std::auto_ptr<SyncSource> source;
    SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceA()));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
    SOURCE_ASSERT_EQUAL(source.get(), 0, config.import(client, *source.get(), config.testcases));
    SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
    CPPUNIT_ASSERT_NO_THROW(source.reset());

    // export again and compare against original file
    std::auto_ptr<SyncSource> copy;
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceA()));
    compareDatabases(config.testcases, *copy.get());
    CPPUNIT_ASSERT_NO_THROW(source.reset());
}

// same as testImport() with immediate delete
void LocalTests::testImportDelete() {
    testImport();

    // delete again, because it was observed that this did not
    // work right with calendars in SyncEvolution
    testLocalDeleteAll();
}

// test change tracking with large number of items
void LocalTests::testManyChanges() {
    // check additional requirements
    CPPUNIT_ASSERT(config.templateItem);
    CPPUNIT_ASSERT(config.uniqueProperties);

    deleteAll(createSourceA);

    // check that everything is empty, also resets change counter of sync source B
    std::auto_ptr<SyncSource> copy;
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // now insert plenty of items
    int numItems = insertManyItems(createSourceA);

    // check that exactly this number of items is listed as new
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), numItems, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), numItems, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // delete all items
    deleteAll(createSourceA);

    // verify again
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), numItems, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
}

template<class T, class V> int countEqual(const T &container,
                                          const V &value) {
    return count(container.begin(),
                 container.end(),
                 value);
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsParent() {
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // now insert main item
    parent = insert(createSourceA, config.parentItem);

    // check that exactly the parent is listed as new
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // delete all items
    deleteAll(createSourceA);

    // verify again
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsChild() {
#if LINKED_ITEMS_RELAXED_SEMANTIC
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // same as above for child item
    child = insert(createSourceA, config.childItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteAll(createSourceA);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
#endif
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsParentChild() {
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // insert parent first, then child
    parent = insert(createSourceA, config.parentItem);
    child = insert(createSourceA, config.childItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteAll(createSourceA);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsChildParent() {
#if LINKED_ITEMS_RELAXED_SEMANTIC
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // insert child first, then parent
    child = insert(createSourceA, config.childItem);
    parent = insert(createSourceA, config.parentItem, true);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteAll(createSourceA);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
#endif
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsChildChangesParent() {
#if LINKED_ITEMS_RELAXED_SEMANTIC
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // insert child first, check changes, then insert the parent
    child = insert(createSourceA, config.childItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    parent = insert(createSourceA, config.parentItem, true);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listNewItems(copy.get()), parent));
    // relaxed semantic: the child item might be considered updated now if
    // it had to be modified when inserting the parent
    SOURCE_ASSERT(copy.get(), 1 >= countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteAll(createSourceA);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
#endif
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsRemoveParentFirst() {
#if LINKED_ITEMS_RELAXED_SEMANTIC
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // insert both items, remove parent, then child
    parent = insert(createSourceA, config.parentItem);
    child = insert(createSourceA, config.childItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteItem(createSourceA, parent);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    // deleting the parent may or may not modify the child
    SOURCE_ASSERT(copy.get(), 1 >= countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteItem(createSourceA, child);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
#endif
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsRemoveNormal() {
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // insert both items, remove child, then parent
    parent = insert(createSourceA, config.parentItem);
    child = insert(createSourceA, config.childItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteItem(createSourceA, child);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteItem(createSourceA, parent);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsInsertParentTwice() {
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // add parent twice (should be turned into update)
    parent = insert(createSourceA, config.parentItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    parent = insert(createSourceA, config.parentItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listUpdatedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteItem(createSourceA, parent);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsInsertChildTwice() {
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

#if LINKED_ITEMS_RELAXED_SEMANTIC
    // add child twice (should be turned into update)
    child = insert(createSourceA, config.childItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    child = insert(createSourceA, config.childItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listUpdatedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteItem(createSourceA, child);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
#endif
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsParentUpdate() {
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // add parent, then update it
    parent = insert(createSourceA, config.parentItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    parent = updateItem(createSourceA, parent, config.parentItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listUpdatedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteItem(createSourceA, parent);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsUpdateChild() {
#if LINKED_ITEMS_RELAXED_SEMANTIC
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // add child, then update it
    child = insert(createSourceA, config.childItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    child = updateItem(createSourceA, child, config.childItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listUpdatedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteItem(createSourceA, child);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
#endif
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsInsertBothUpdateChild() {
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // add parent and child, then update child
    parent = insert(createSourceA, config.parentItem);
    child = insert(createSourceA, config.childItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    child = updateItem(createSourceA, child, config.childItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listUpdatedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteItem(createSourceA, parent);
    deleteItem(createSourceA, child);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
}

// test inserting, removing and updating of parent + child item in
// various order plus change tracking
void LocalTests::testLinkedItemsInsertBothUpdateParent() {
    // check additional requirements
    CPPUNIT_ASSERT(config.parentItem);
    CPPUNIT_ASSERT(config.childItem);

    deleteAll(createSourceA);
    std::string parent, child;
    std::auto_ptr<SyncSource> copy;

    // check that everything is empty, also resets change counter of sync source B
    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    // add parent and child, then update parent
    parent = insert(createSourceA, config.parentItem);
    child = insert(createSourceA, config.childItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    parent = updateItem(createSourceA, parent, config.parentItem);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listUpdatedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());

    deleteItem(createSourceA, parent);
    deleteItem(createSourceA, child);

    SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 2, countDeletedItems(copy.get()));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), parent));
    SOURCE_ASSERT_EQUAL(copy.get(), 1, countEqual(listDeletedItems(copy.get()), child));
    SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
    CPPUNIT_ASSERT_NO_THROW(copy.reset());
}


SyncTests::SyncTests(const std::string &name, ClientTest &cl, std::vector<int> sourceIndices, bool isClientA) :
    CppUnit::TestSuite(name),
    client(cl) {
    sourceArray = new int[sourceIndices.size() + 1];
    for (std::vector<int>::iterator it = sourceIndices.begin();
         it != sourceIndices.end();
         ++it) {
        ClientTest::Config config;
        client.getSourceConfig(*it, config);

        if (config.sourceName) {
            sourceArray[sources.size()] = *it;
            sources.push_back(std::pair<int,LocalTests *>(*it, cl.createLocalTests(config.sourceName, *it, config)));
        }
    }
    sourceArray[sources.size()] = -1;

    // check whether we have a second client
    ClientTest *clientB = cl.getClientB();
    if (clientB) {
        accessClientB = clientB->createSyncTests(name, sourceIndices, false);
    } else {
        accessClientB = 0;
    }
}

SyncTests::~SyncTests() {
    for (source_it it = sources.begin();
         it != sources.end();
         ++it) {
        delete it->second;
    }
    delete [] sourceArray;
    if (accessClientB) {
        delete accessClientB;
    }
}

/** adds the supported tests to the instance itself */
void SyncTests::addTests() {
    if (sources.size()) {
        const ClientTest::Config &config(sources[0].second->config);

        // run this test first, even if it is more complex:
        // if it works, all the following tests will run with
        // the server in a deterministic state
        if (config.createSourceA) {
            if (config.insertItem) {
                ADD_TEST(SyncTests, testDeleteAllRefresh);
            }
        }

        ADD_TEST(SyncTests, testTwoWaySync);
        ADD_TEST(SyncTests, testSlowSync);
        ADD_TEST(SyncTests, testRefreshFromServerSync);
        ADD_TEST(SyncTests, testRefreshFromClientSync);

        if (config.createSourceA) {
            if (config.insertItem) {
                ADD_TEST(SyncTests, testRefreshSemantic);
                ADD_TEST(SyncTests, testRefreshStatus);

                if (accessClientB &&
                    config.dump &&
                    config.compare) {
                    ADD_TEST(SyncTests, testCopy);
                    ADD_TEST(SyncTests, testDelete);
                    ADD_TEST(SyncTests, testAddUpdate);
                    ADD_TEST(SyncTests, testManyItems);

                    if (config.updateItem) {
                        ADD_TEST(SyncTests, testUpdate);
                    }
                    if (config.complexUpdateItem) {
                        ADD_TEST(SyncTests, testComplexUpdate);
                    }
                    if (config.mergeItem1 && config.mergeItem2) {
                        ADD_TEST(SyncTests, testMerge);
                    }
                    if (config.import) {
                        ADD_TEST(SyncTests, testTwinning);
                        ADD_TEST(SyncTests, testItems);
                    }
                    if (config.templateItem) {
                        ADD_TEST(SyncTests, testMaxMsg);
                        ADD_TEST(SyncTests, testLargeObject);
                        ADD_TEST(SyncTests, testLargeObjectBin);
                        if (client.isB64Enabled()) {
                            ADD_TEST(SyncTests, testLargeObjectEncoded);
                        }
                        ADD_TEST(SyncTests, testOneWayFromServer);
                        ADD_TEST(SyncTests, testOneWayFromClient);
                    }
                }
            }
        }
    }
}

/** compare databases of first and second client */
void SyncTests::compareDatabases() {
    source_it it1;
    source_it it2;

    CPPUNIT_ASSERT(accessClientB);
    for (it1 = sources.begin(), it2 = accessClientB->sources.begin();
         it1 != sources.end() && it2 != accessClientB->sources.end();
         ++it1, ++it2) {
        std::auto_ptr<SyncSource> copy;
        SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(it2->second->createSourceB()));
        it1->second->compareDatabases(NULL, *copy.get());
        CPPUNIT_ASSERT_NO_THROW(copy.reset());
    }
    CPPUNIT_ASSERT(it1 == sources.end());
    CPPUNIT_ASSERT(it2 == accessClientB->sources.end());
}

/** deletes all items locally and on server */
void SyncTests::deleteAll(DeleteAllMode mode) {
    source_it it;

    switch(mode) {
     case DELETE_ALL_SYNC:
        // a refresh from server would slightly reduce the amount of data exchanged, but not all servers support it
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->deleteAll(it->second->createSourceA);
        }
        sync(SYNC_SLOW, ".deleteall.init");
        // now that client and server are in sync, delete locally and sync again
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->deleteAll(it->second->createSourceA);
        }
        sync(SYNC_TWO_WAY, ".deleteall.twoway", CheckSyncReport(0,0,0, 0,0,-1));
        break;
     case DELETE_ALL_REFRESH:
        // delete locally and then tell the server to "copy" the empty databases
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->deleteAll(it->second->createSourceA);
        }
        sync(SYNC_REFRESH_FROM_CLIENT, ".deleteall.refreshserver", CheckSyncReport(0,0,0, 0,0,-1));
        break;
    }
}

/** get both clients in sync with empty server, then copy one item from client A to B */
void SyncTests::doCopy() {
    // check requirements
    CPPUNIT_ASSERT(accessClientB);

    deleteAll();
    accessClientB->deleteAll();

    // insert into first database, copy to server
    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->testSimpleInsert();
    }
    sync(SYNC_TWO_WAY, ".send", CheckSyncReport(0,0,0, 1,0,0));

    // copy into second database
    accessClientB->sync(SYNC_TWO_WAY, ".recv", CheckSyncReport(1,0,0, 0,0,0));

    compareDatabases();
}

/**
 * replicate server database locally: same as SYNC_REFRESH_FROM_SERVER,
 * but done with explicit local delete and then a SYNC_SLOW because some
 * servers do no support SYNC_REFRESH_FROM_SERVER
 */
void SyncTests::refreshClient() {
    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->deleteAll(it->second->createSourceA);
    }
    sync(SYNC_SLOW, ".refresh", CheckSyncReport(-1,0,0, 0,0,0));
}


// delete all items, locally and on server using refresh-from-client sync
void SyncTests::testDeleteAllRefresh() {
    source_it it;

    // copy something to server first; doesn't matter whether it has the
    // item already or not, as long as it exists there afterwards
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->testSimpleInsert();
    }
    sync(SYNC_SLOW, ".insert");

    // now ensure we can delete it
    deleteAll(DELETE_ALL_SYNC);

    // nothing stored locally?
    for (it = sources.begin(); it != sources.end(); ++it) {
        std::auto_ptr<SyncSource> source;
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceA()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
        CPPUNIT_ASSERT_NO_THROW(source.reset());
    }

    // make sure server really deleted everything
    sync(SYNC_SLOW, ".check", CheckSyncReport(0,0,0, 0,0,0));
    for (it = sources.begin(); it != sources.end(); ++it) {
        std::auto_ptr<SyncSource> source;
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceA()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
        CPPUNIT_ASSERT_NO_THROW(source.reset());
    }
}

// test that a refresh sync of an empty server leads to an empty datatbase
// and no changes are sent to server during next two-way sync
void SyncTests::testRefreshSemantic() {
    source_it it;

    // clean client and server
    deleteAll();

    // insert item, then refresh from empty server
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->testSimpleInsert();
    }
    sync(SYNC_REFRESH_FROM_SERVER, "", CheckSyncReport(0,0,-1, 0,0,0));

    // check
    for (it = sources.begin(); it != sources.end(); ++it) {
        std::auto_ptr<SyncSource> source;
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceA()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
        CPPUNIT_ASSERT_NO_THROW(source.reset());
    }
    sync(SYNC_TWO_WAY, ".two-way", CheckSyncReport(0,0,0, 0,0,0));
}

// tests the following sequence of events:
// - insert item
// - delete all items
// - insert one other item
// - refresh from client
// => no items should now be listed as new, updated or deleted for this client during another sync
void SyncTests::testRefreshStatus() {
    source_it it;

    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->testSimpleInsert();
    }
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->deleteAll(it->second->createSourceA);
    }
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->testSimpleInsert();
    }
    sync(SYNC_REFRESH_FROM_CLIENT, ".refresh-from-client", CheckSyncReport(0,0,0 /* 1,0,0 - not sure exactly what the server will be told */));
    sync(SYNC_TWO_WAY, ".two-way", CheckSyncReport(0,0,0, 0,0,0));
}

// test that a two-way sync copies updates from database to the other client,
// using simple data commonly supported by servers
void SyncTests::testUpdate() {
    CPPUNIT_ASSERT(sources.begin() != sources.end());
    CPPUNIT_ASSERT(sources.begin()->second->config.updateItem);

    // setup client A, B and server so that they all contain the same item
    doCopy();

    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->update(it->second->createSourceA, it->second->config.updateItem);
    }

    sync(SYNC_TWO_WAY, ".update", CheckSyncReport(0,0,0, 0,1,0));
    accessClientB->sync(SYNC_TWO_WAY, ".update", CheckSyncReport(0,1,0, 0,0,0));

    compareDatabases();
}

// test that a two-way sync copies updates from database to the other client,
// using data that some, but not all servers support, like adding a second
// phone number to a contact
void SyncTests::testComplexUpdate() {
    // setup client A, B and server so that they all contain the same item
    doCopy();

    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->update(it->second->createSourceA,
                           /* this test might get executed with some sources which have
                              a complex update item while others don't: use the normal update item
                              for them or even just the same item */
                           it->second->config.complexUpdateItem ? it->second->config.complexUpdateItem :
                           it->second->config.updateItem ? it->second->config.updateItem :
                           it->second->config.insertItem
                           );
    }

    sync(SYNC_TWO_WAY, ".update", CheckSyncReport(0,0,0, 0,1,0));
    accessClientB->sync(SYNC_TWO_WAY, ".update", CheckSyncReport(0,1,0, 0,0,0));

    compareDatabases();
}


// test that a two-way sync deletes the copy of an item in the other database
void SyncTests::testDelete() {
    // setup client A, B and server so that they all contain the same item
    doCopy();

    // delete it on A
    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->deleteAll(it->second->createSourceA);
    }

    // transfer change from A to server to B
    sync(SYNC_TWO_WAY, ".delete", CheckSyncReport(0,0,0, 0,0,1));
    accessClientB->sync(SYNC_TWO_WAY, ".delete", CheckSyncReport(0,0,1, 0,0,0));

    // check client B: shouldn't have any items now
    for (it = sources.begin(); it != sources.end(); ++it) {
        std::auto_ptr<SyncSource> copy;
        SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(it->second->createSourceA()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
        SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
        CPPUNIT_ASSERT_NO_THROW(copy.reset());
    }
}

// test what the server does when it finds that different
// fields of the same item have been modified
void SyncTests::testMerge() {
    // setup client A, B and server so that they all contain the same item
    doCopy();

    // update in client A
    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->update(it->second->createSourceA, it->second->config.mergeItem1);
    }

    // update in client B
    for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
        it->second->update(it->second->createSourceA, it->second->config.mergeItem2);
    }

    // send change to server from client A (no conflict), then from B (conflict)
    sync(SYNC_TWO_WAY, ".send", CheckSyncReport(0,0,0, 0,1,0));
    sync(SYNC_TWO_WAY, ".recv");

    // figure out how the conflict during ".recv" was handled
    for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
        std::auto_ptr<SyncSource> copy;
        SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(it->second->createSourceA()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
        int numItems = 0;
        SOURCE_ASSERT_NO_FAILURE(copy.get(), numItems = countItems(copy.get()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
        CPPUNIT_ASSERT(numItems >= 1);
        CPPUNIT_ASSERT(numItems <= 2);
        std::cout << " " << it->second->config.sourceName << ": " << (numItems == 1 ? "conflicting items were merged" : "both of the conflicting items were preserved") << " ";
        std::cout.flush();
        CPPUNIT_ASSERT_NO_THROW(copy.reset());
    }
}

// test what the server does when it has to execute a slow sync
// with identical data on client and server:
// expected behaviour is that nothing changes
void SyncTests::testTwinning() {
    // clean server and client A
    deleteAll();

    // import test data
    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->testImport();
    }

    // send to server
    sync(SYNC_TWO_WAY, ".send");

    // ensure that client has the same data, thus ignoring data conversion
    // issues (those are covered by testItems())
    refreshClient();

    // copy to client B to have another copy
    accessClientB->refreshClient();

    // slow sync should not change anything
    sync(SYNC_TWO_WAY, ".twinning");

    // check
    compareDatabases();
}

// tests one-way sync from server:
// - get both clients and server in sync with no items anywhere
// - add one item on first client, copy to server
// - add a different item on second client, one-way-from-server
// - two-way sync with first client
// => one item on first client, two on second
// - delete on first client, sync that to second client
//   via two-way sync + one-way-from-server
// => one item left on second client (the one inserted locally)
void SyncTests::testOneWayFromServer() {
    // no items anywhere
    deleteAll();
    accessClientB->refreshClient();

    // check that everything is empty, also resets change tracking
    // in second sources of each client
    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }
    for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }

    // add one item on first client, copy to server, and check change tracking via second source
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->insertManyItems(it->second->createSourceA, 1, 1);
    }
    sync(SYNC_TWO_WAY, ".send", CheckSyncReport(0,0,0, 1,0,0));
    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }

    // add a different item on second client, one-way-from-server
    // => one item added locally, none sent to server
    for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
        it->second->insertManyItems(it->second->createSourceA, 2, 1);

        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }
    accessClientB->sync(SYNC_ONE_WAY_FROM_SERVER, ".recv", CheckSyncReport(1,0,0, 0,0,0));
    for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 2, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }

    // two-way sync with first client for verification
    // => no changes
    sync(SYNC_TWO_WAY, ".check", CheckSyncReport(0,0,0, 0,0,0));
    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }

    // delete items on clientA, sync to server
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->deleteAll(it->second->createSourceA);

        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 1, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }
    sync(SYNC_TWO_WAY, ".delete", CheckSyncReport(0,0,0, 0,0,1));
    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }

    // sync the same change to second client
    // => one item left (the one inserted locally)
    accessClientB->sync(SYNC_ONE_WAY_FROM_SERVER, ".delete", CheckSyncReport(0,0,1, 0,0,0));
    for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 1, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }
}

// tests one-way sync from client:
// - get both clients and server in sync with no items anywhere
// - add one item on first client, copy to server
// - add a different item on second client, one-way-from-client
// - two-way sync with first client
// => two items on first client, one on second
// - delete on second client, sync that to first client
//   via one-way-from-client, two-way
// => one item left on first client (the one inserted locally)
void SyncTests::testOneWayFromClient() {
    // no items anywhere
    deleteAll();
    accessClientB->deleteAll();

    // check that everything is empty, also resets change tracking
    // in second sources of each client
    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }
    for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }

    // add one item on first client, copy to server, and check change tracking via second source
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->insertManyItems(it->second->createSourceA, 1, 1);
    }
    sync(SYNC_TWO_WAY, ".send", CheckSyncReport(0,0,0, 1,0,0));
    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }

    // add a different item on second client, one-way-from-client
    // => no item added locally, one sent to server
    for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
        it->second->insertManyItems(it->second->createSourceA, 2, 1);

        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }
    accessClientB->sync(SYNC_ONE_WAY_FROM_CLIENT, ".send", CheckSyncReport(0,0,0, 1,0,0));
    for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }

    // two-way sync with client A for verification
    // => receive one item
    sync(SYNC_TWO_WAY, ".check", CheckSyncReport(1,0,0, 0,0,0));
    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 2, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }

    // delete items on client B, sync to server
    for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
        it->second->deleteAll(it->second->createSourceA);

        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 1, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }
    accessClientB->sync(SYNC_ONE_WAY_FROM_CLIENT, ".delete", CheckSyncReport(0,0,0, 0,0,1));
    for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }

    // sync the same change to client A
    // => one item left (the one inserted locally)
    sync(SYNC_TWO_WAY, ".delete", CheckSyncReport(0,0,1, 0,0,0));
    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it->second->config.createSourceB) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 1, countDeletedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }
}

// creates several items, transmits them back and forth and
// then compares which of them have been preserved
void SyncTests::testItems() {
    // clean server and first test database
    deleteAll();

    // import data
    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->testImport();
    }

    // transfer from client A to server to client B
    sync(SYNC_TWO_WAY, ".send");
    accessClientB->refreshClient();

    compareDatabases();
}

// tests the following sequence of events:
// - both clients in sync with server
// - client 1 adds item
// - client 1 updates the same item
// - client 2 gets item: the client should be asked to add the item
//
// However it has been observed that sometimes the item was sent as "update"
// for a non-existant local item. This is a server bug, the client does not
// have to handle that. See
// http://forge.objectweb.org/tracker/?func=detail&atid=100096&aid=305018&group_id=96
void SyncTests::testAddUpdate() {
    // clean server and both test databases
    deleteAll();
    accessClientB->refreshClient();

    // add item
    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->insert(it->second->createSourceA, it->second->config.insertItem);
    }
    sync(SYNC_TWO_WAY, ".add", CheckSyncReport(0,0,0, 1,0,0));

    // update it
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->update(it->second->createSourceB, it->second->config.updateItem);
    }
    sync(SYNC_TWO_WAY, ".update", CheckSyncReport(0,0,0, 0,1,0));

    // now download the updated item into the second client
    accessClientB->sync(SYNC_TWO_WAY, ".recv", CheckSyncReport(1,0,0, 0,0,0));

    // compare the two databases
    compareDatabases();
}

//
// stress tests: execute some of the normal operations,
// but with large number of artificially generated items
//

// two-way sync with clean client/server,
// followed by slow sync and comparison
// via second client
void SyncTests::testManyItems() {
    // clean server and client A
    deleteAll();

    // import artificial data: make them large to generate some
    // real traffic and test buffer handling
    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        it->second->insertManyItems(it->second->createSourceA, 0, it->second->config.numItems, 2000);
    }

    // send data to server
    sync(SYNC_TWO_WAY, ".send", CheckSyncReport(0,0,0, -1,0,0), 64 * 1024, 64 * 1024, true);

    // ensure that client has the same data, ignoring data conversion
    // issues (those are covered by testItems())
    refreshClient();

    // also copy to second client
    accessClientB->refreshClient();

    // slow sync now should not change anything
    sync(SYNC_SLOW, ".twinning", CheckSyncReport(-1,-1,-1, -1,-1,-1), 64 * 1024, 64 * 1024, true);

    // compare
    compareDatabases();
}


/**
 * implements testMaxMsg(), testLargeObject(), testLargeObjectEncoded()
 * using a sequence of items with varying sizes
 */
void SyncTests::doVarSizes(bool withMaxMsgSize,
                           bool withLargeObject,
                           const char *encoding) {
    static const int maxMsgSize = 8 * 1024;

    // clean server and client A
    deleteAll();

    // insert items, doubling their size, then restart with small size
    source_it it;
    for (it = sources.begin(); it != sources.end(); ++it) {
        int item = 1;
        for (int i = 0; i < 2; i++ ) {
            int size = 1;
            while (size < 2 * maxMsgSize) {
                it->second->insertManyItems(it->second->createSourceA, item, 1, (int)strlen(it->second->config.templateItem) + 10 + size);
                size *= 2;
                item++;
            }
        }
    }

    // transfer to server
    sync(SYNC_TWO_WAY, ".send",
         CheckSyncReport(0,0,0, -1,0,0), // number of items sent to server depends on source
         withMaxMsgSize ? maxMsgSize : 0,
         withMaxMsgSize ? maxMsgSize * 100 : 0,
         withLargeObject,
         encoding);

    // copy to second client
    accessClientB->sync(SYNC_REFRESH_FROM_SERVER, ".recv",
                        CheckSyncReport(-1,0,-1, 0,0,0), // number of items received from server depends on source
                        withLargeObject ? maxMsgSize : withMaxMsgSize ? maxMsgSize * 100 /* large enough so that server can sent the largest item */ : 0,
                        withMaxMsgSize ? maxMsgSize * 100 : 0,
                        withLargeObject,
                        encoding);

    // compare
    compareDatabases();
}

void SyncTests::sync(SyncMode syncMode,
                     const std::string &logprefix,
                     CheckSyncReport checkReport,
                     long maxMsgSize,
                     long maxObjSize,
                     bool loSupport,
                     const char *encoding) {
    int res = 0;
    static int syncCounter = 0;
    static std::string lastTest;
    std::stringstream logstream;

    // reset counter when switching tests
    if (lastTest != getCurrentTest()) {
        syncCounter = 0;
        lastTest = getCurrentTest();
    }

    logstream << std::setw(4) << std::setfill('0') << syncCounter << "_" << getCurrentTest() << logprefix;
    std::string logname = logstream.str();
    simplifyFilename(logname);
    syncCounter++;

    std::string logfile = logname + ".client." + (accessClientB ? "A" : "B") + ".log";
    remove(logfile.c_str());
    LOG.setLogName(logfile.c_str());
    LOG.reset();

    try {
        res = client.sync(sourceArray,
                          syncMode,
                          checkReport,
                          maxMsgSize,
                          maxObjSize,
                          loSupport,
                          encoding);

        client.postSync(res, logname);
    } catch (...) {
        res = 1;
        client.postSync(res, logname);

        // this logs the original exception using CPPUnit mechanisms
        CPPUNIT_ASSERT_NO_THROW( throw );
    }
    CPPUNIT_ASSERT( !res );
}


/** generates tests on demand based on what the client supports */
class ClientTestFactory : public CppUnit::TestFactory {
public:
    ClientTestFactory(ClientTest &c) :
        client(c) {}

    virtual CppUnit::Test *makeTest() {
        int source;
        CppUnit::TestSuite *alltests = new CppUnit::TestSuite("Client");
        CppUnit::TestSuite *tests;

        // create local source tests
        tests = new CppUnit::TestSuite(alltests->getName() + "::Source");
        for (source=0; source < client.getNumSources(); source++) {
            ClientTest::Config config;
            client.getSourceConfig(source, config);
            if (config.sourceName) {
                LocalTests *sourcetests =
                    client.createLocalTests(tests->getName() + "::" + config.sourceName, source, config);
                sourcetests->addTests();
                tests->addTest(sourcetests);
            }
        }
        alltests->addTest(tests);
        tests = 0;

        // create sync tests with just one source
        tests = new CppUnit::TestSuite(alltests->getName() + "::Sync");
        for (source=0; source < client.getNumSources(); source++) {
            ClientTest::Config config;
            client.getSourceConfig(source, config);
            if (config.sourceName) {
                std::vector<int> sources;
                sources.push_back(source);
                SyncTests *synctests =
                    client.createSyncTests(tests->getName() + "::" + config.sourceName, sources);
                synctests->addTests();
                tests->addTest(synctests);
            }
        }

        // create sync tests with all sources enabled, unless we only have one:
        // that would be identical to the test above
        std::vector<int> sources;
        std::string name, name_reversed;
        for (source=0; source < client.getNumSources(); source++) {
            ClientTest::Config config;
            client.getSourceConfig(source, config);
            if (config.sourceName) {
                sources.push_back(source);
                if (name.size() > 0) {
                    name += "_";
                    name_reversed = std::string("_") + name_reversed;
                }
                name += config.sourceName;
                name_reversed = config.sourceName + name_reversed;
            }
        }
        if (sources.size() > 1) {
            SyncTests *synctests =
                client.createSyncTests(tests->getName() + "::" + name, sources);
            synctests->addTests();
            tests->addTest(synctests);
            synctests = 0;

            // now also in reversed order - who knows, it might make a difference
            std::reverse(sources.begin(), sources.end());
            synctests =
                client.createSyncTests(tests->getName() + "::" + name_reversed, sources);
            synctests->addTests();
            tests->addTest(synctests);
            synctests = 0;
        }

        alltests->addTest(tests);
        tests = 0;

        return alltests;
    }

private:
    ClientTest &client;
};

void ClientTest::registerTests()
{
    factory = (void *)new ClientTestFactory(*this);
    CppUnit::TestFactoryRegistry::getRegistry().registerFactory((CppUnit::TestFactory *)factory);
}

ClientTest::ClientTest(int serverSleepSec, const std::string &serverLog) :
    serverSleepSeconds(serverSleepSec),
    serverLogFileName(serverLog),
    factory(NULL)
{
}

ClientTest::~ClientTest()
{
    if(factory) {
        CppUnit::TestFactoryRegistry::getRegistry().unregisterFactory((CppUnit::TestFactory *)factory);
        delete (CppUnit::TestFactory *)factory;
        factory = 0;
    }
}

LocalTests *ClientTest::createLocalTests(const std::string &name, int sourceParam, ClientTest::Config &co)
{
    return new LocalTests(name, *this, sourceParam, co);
}

SyncTests *ClientTest::createSyncTests(const std::string &name, std::vector<int> sourceIndices, bool isClientA)
{
    return new SyncTests(name, *this, sourceIndices, isClientA);
}

int ClientTest::dump(ClientTest &client, SyncSource &source, const char *file)
{
    std::ofstream out(file);

    std::auto_ptr<SyncItem> item;
    SOURCE_ASSERT_NO_FAILURE(&source, item.reset(source.getFirstItem()));
    while (item.get()) {
        out << (char *)item->getData() << std::endl;
        SOURCE_ASSERT_NO_FAILURE(&source, item.reset(source.getNextItem()));
    }
    out.close();
    return out.bad();
}

int ClientTest::import(ClientTest &client, SyncSource &source, const char *file)
{
    // import the file
    std::ifstream input;
    input.open(file);
    CPPUNIT_ASSERT(!input.bad());
    CPPUNIT_ASSERT(input.is_open());
    std::string data, line;
    while (input) {
        bool wasend = false;
        do {
            getline(input, line);
            CPPUNIT_ASSERT(!input.bad());
            // empty lines directly after line which starts with END mark end of record;
            // check for END necessary becayse vCard 2.1 ENCODING=BASE64 may have empty lines in body of VCARD!
            if (line != "\r" && line.size() > 0 || !wasend) {
                data += line;
                data += "\n";
            } else {
                importItem(&source, data);
                data = "";
            }
            wasend = !line.compare(0, 4, "END:");
        } while(!input.eof());
    }
    importItem(&source, data);

    return 0;
}

bool ClientTest::compare(ClientTest &client, const char *fileA, const char *fileB)
{
    std::string cmdstr = std::string("perl synccompare.pl ") + fileA + " " + fileB;
    return system(cmdstr.c_str()) == 0;
}


#ifndef WIN32
#include <fcntl.h>
#include "base/globalsdef.h"

USE_NAMESPACE
#endif

void ClientTest::postSync(int res, const std::string &logname)
{
#ifdef WIN32
    Sleep(serverSleepSeconds * 1000);
#else
    sleep(serverSleepSeconds);

    // make a copy of the server's log (if found), then truncate it
    if (serverLogFileName.size()) {
        int fd = open(serverLogFileName.c_str(), O_RDWR);

        if (fd >= 0) {
            std::string cmd = std::string("cp ") + serverLogFileName + " " + logname + ".server.log";
            system(cmd.c_str());
            ftruncate(fd, 0);
        } else {
            perror(serverLogFileName.c_str());
        }
    }
#endif
}

void ClientTest::getTestData(const char *type, Config &config)
{
    memset(&config, 0, sizeof(config));
    char *numitems = getenv("CLIENT_TEST_NUM_ITEMS");
    config.numItems = numitems ? atoi(numitems) : 100;

    if (!strcmp(type, "vcard30")) {
        config.sourceName = "vcard30";
        config.uri = "card3"; // ScheduleWorld
        config.type = "text/vcard";
        config.insertItem =
            "BEGIN:VCARD\n"
            "VERSION:3.0\n"
            "TITLE:tester\n"
            "FN:John Doe\n"
            "N:Doe;John;;;\n"
            "TEL;TYPE=WORK;TYPE=VOICE:business 1\n"
            "X-EVOLUTION-FILE-AS:Doe\\, John\n"
            "X-MOZILLA-HTML:FALSE\n"
            "NOTE:\n"
            "END:VCARD\n";
        config.updateItem =
            "BEGIN:VCARD\n"
            "VERSION:3.0\n"
            "TITLE:tester\n"
            "FN:Joan Doe\n"
            "N:Doe;Joan;;;\n"
            "X-EVOLUTION-FILE-AS:Doe\\, Joan\n"
            "TEL;TYPE=WORK;TYPE=VOICE:business 2\n"
            "BDAY:2006-01-08\n"
            "X-MOZILLA-HTML:TRUE\n"
            "END:VCARD\n";
        /* adds a second phone number: */
        config.complexUpdateItem =
            "BEGIN:VCARD\n"
            "VERSION:3.0\n"
            "TITLE:tester\n"
            "FN:Joan Doe\n"
            "N:Doe;Joan;;;\n"
            "X-EVOLUTION-FILE-AS:Doe\\, Joan\n"
            "TEL;TYPE=WORK;TYPE=VOICE:business 1\n"
            "TEL;TYPE=HOME;TYPE=VOICE:home 2\n"
            "BDAY:2006-01-08\n"
            "X-MOZILLA-HTML:TRUE\n"
            "END:VCARD\n";
        /* add a telephone number, email and X-AIM to initial item */
        config.mergeItem1 =
            "BEGIN:VCARD\n"
            "VERSION:3.0\n"
            "TITLE:tester\n"
            "FN:John Doe\n"
            "N:Doe;John;;;\n"
            "X-EVOLUTION-FILE-AS:Doe\\, John\n"
            "X-MOZILLA-HTML:FALSE\n"
            "TEL;TYPE=WORK;TYPE=VOICE:business 1\n"
            "EMAIL:john.doe@work.com\n"
            "X-AIM:AIM JOHN\n"
            "END:VCARD\n";
        config.mergeItem2 =
            "BEGIN:VCARD\n"
            "VERSION:3.0\n"
            "TITLE:developer\n"
            "FN:John Doe\n"
            "N:Doe;John;;;\n"
            "X-EVOLUTION-FILE-AS:Doe\\, John\n"
            "X-MOZILLA-HTML:TRUE\n"
            "BDAY:2006-01-08\n"
            "END:VCARD\n";
        config.templateItem = config.insertItem;
        config.uniqueProperties = "FN:N:X-EVOLUTION-FILE-AS";
        config.sizeProperty = "NOTE";
        config.import = import;
        config.dump = dump;
        config.compare = compare;
        config.testcases = "testcases/vcard30.vcf";
    } else if (!strcmp(type, "vcard21")) {
        config.sourceName = "vcard21";
        config.uri = "card"; // Funambol
        config.type = "text/x-vcard";
        config.insertItem =
            "BEGIN:VCARD\n"
            "VERSION:2.1\n"
            "TITLE:tester\n"
            "FN:John Doe\n"
            "N:Doe;John;;;\n"
            "TEL;TYPE=WORK;TYPE=VOICE:business 1\n"
            "X-MOZILLA-HTML:FALSE\n"
            "NOTE:\n"
            "END:VCARD\n";
        config.updateItem =
            "BEGIN:VCARD\n"
            "VERSION:2.1\n"
            "TITLE:tester\n"
            "FN:Joan Doe\n"
            "N:Doe;Joan;;;\n"
            "TEL;TYPE=WORK;TYPE=VOICE:business 2\n"
            "BDAY:2006-01-08\n"
            "X-MOZILLA-HTML:TRUE\n"
            "END:VCARD\n";
        /* adds a second phone number: */
        config.complexUpdateItem =
            "BEGIN:VCARD\n"
            "VERSION:2.1\n"
            "TITLE:tester\n"
            "FN:Joan Doe\n"
            "N:Doe;Joan;;;\n"
            "TEL;TYPE=WORK;TYPE=VOICE:business 1\n"
            "TEL;TYPE=HOME;TYPE=VOICE:home 2\n"
            "BDAY:2006-01-08\n"
            "X-MOZILLA-HTML:TRUE\n"
            "END:VCARD\n";
        /* add a telephone number, email and X-AIM to initial item */
        config.mergeItem1 =
            "BEGIN:VCARD\n"
            "VERSION:2.1\n"
            "TITLE:tester\n"
            "FN:John Doe\n"
            "N:Doe;John;;;\n"
            "X-MOZILLA-HTML:FALSE\n"
            "TEL;TYPE=WORK;TYPE=VOICE:business 1\n"
            "EMAIL:john.doe@work.com\n"
            "X-AIM:AIM JOHN\n"
            "END:VCARD\n";
        config.mergeItem2 =
            "BEGIN:VCARD\n"
            "VERSION:2.1\n"
            "TITLE:developer\n"
            "FN:John Doe\n"
            "N:Doe;John;;;\n"
            "X-MOZILLA-HTML:TRUE\n"
            "BDAY:2006-01-08\n"
            "END:VCARD\n";
        config.templateItem = config.insertItem;
        config.uniqueProperties = "FN:N";
        config.sizeProperty = "NOTE";
        config.import = import;
        config.dump = dump;
        config.compare = compare;
        config.testcases = "testcases/vcard21.vcf";
    } else if(!strcmp(type, "ical20")) {
        config.sourceName = "ical20";
        config.uri = "cal2"; // ScheduleWorld
        config.type = "text/x-vcalendar";
        config.insertItem =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VEVENT\n"
            "SUMMARY:phone meeting\n"
            "DTEND:20060406T163000Z\n"
            "DTSTART:20060406T160000Z\n"
            "UID:1234567890!@#$%^&*()<>@dummy\n"
            "DTSTAMP:20060406T211449Z\n"
            "LAST-MODIFIED:20060409T213201\n"
            "CREATED:20060409T213201\n"
            "LOCATION:my office\n"
            "DESCRIPTION:let's talk\n"
            "CLASS:PUBLIC\n"
            "TRANSP:OPAQUE\n"
            "SEQUENCE:1\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        config.updateItem =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VEVENT\n"
            "SUMMARY:meeting on site\n"
            "DTEND:20060406T163000Z\n"
            "DTSTART:20060406T160000Z\n"
            "UID:1234567890!@#$%^&*()<>@dummy\n"
            "DTSTAMP:20060406T211449Z\n"
            "LAST-MODIFIED:20060409T213201\n"
            "CREATED:20060409T213201\n"
            "LOCATION:big meeting room\n"
            "DESCRIPTION:nice to see you\n"
            "CLASS:PUBLIC\n"
            "TRANSP:OPAQUE\n"
            "SEQUENCE:1\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        /* change location in insertItem in testMerge() */
        config.mergeItem1 =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VEVENT\n"
            "SUMMARY:phone meeting\n"
            "DTEND:20060406T163000Z\n"
            "DTSTART:20060406T160000Z\n"
            "UID:1234567890!@#$%^&*()<>@dummy\n"
            "DTSTAMP:20060406T211449Z\n"
            "LAST-MODIFIED:20060409T213201\n"
            "CREATED:20060409T213201\n"
            "LOCATION:calling from home\n"
            "DESCRIPTION:let's talk\n"
            "CLASS:PUBLIC\n"
            "TRANSP:OPAQUE\n"
            "SEQUENCE:1\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        config.mergeItem2 =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VEVENT\n"
            "SUMMARY:phone meeting\n"
            "DTEND:20060406T163000Z\n"
            "DTSTART:20060406T160000Z\n"
            "UID:1234567890!@#$%^&*()<>@dummy\n"
            "DTSTAMP:20060406T211449Z\n"
            "LAST-MODIFIED:20060409T213201\n"
            "CREATED:20060409T213201\n"
            "LOCATION:my office\n"
            "DESCRIPTION:what the heck\\, let's even shout a bit\n"
            "CLASS:PUBLIC\n"
            "TRANSP:OPAQUE\n"
            "SEQUENCE:1\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        config.parentItem =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VEVENT\n"
            "UID:20080407T193125Z-19554-727-1-50@gollum\n"
            "DTSTAMP:20080407T193125Z\n"
            "DTSTART:20080406T090000Z\n"
            "DTEND:20080406T093000Z\n"
            "TRANSP:OPAQUE\n"
            "SEQUENCE:2\n"
            "SUMMARY:Recurring\n"
            "DESCRIPTION:recurs each Monday\\, 10 times\n"
            "CLASS:PUBLIC\n"
            "RRULE:FREQ=WEEKLY;COUNT=10;INTERVAL=1;BYDAY=SU\n"
            "CREATED:20080407T193241\n"
            "LAST-MODIFIED:20080407T193241\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        config.childItem =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VEVENT\n"
            "UID:20080407T193125Z-19554-727-1-50@gollum\n"
            "DTSTAMP:20080407T193125Z\n"
            "DTSTART:20080413T090000Z\n"
            "DTEND:20080413T093000Z\n"
            "TRANSP:OPAQUE\n"
            "SEQUENCE:7\n"
            "SUMMARY:Recurring: Modified\n"
            "CLASS:PUBLIC\n"
            "CREATED:20080407T193241\n"
            "LAST-MODIFIED:20080407T193647\n"
            "RECURRENCE-ID:20080413T090000Z\n"
            "DESCRIPTION:second instance modified\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        config.templateItem = config.insertItem;
        config.uniqueProperties = "SUMMARY:UID:LOCATION";
        config.sizeProperty = "DESCRIPTION";
        config.import = import;
        config.dump = dump;
        config.compare = compare;
        config.testcases = "testcases/ical20.ics";
    } if(!strcmp(type, "vcal10")) {
        config.sourceName = "vcal10";
        config.uri = "cal"; // Funambol 3.0
        config.type = "text/x-vcalendar";
        config.insertItem =
            "BEGIN:VCALENDAR\n"
            "VERSION:1.0\n"
            "BEGIN:VEVENT\n"
            "SUMMARY:phone meeting\n"
            "DTEND:20060406T163000Z\n"
            "DTSTART:20060406T160000Z\n"
            "DTSTAMP:20060406T211449Z\n"
            "LOCATION:my office\n"
            "DESCRIPTION:let's talk\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        config.updateItem =
            "BEGIN:VCALENDAR\n"
            "VERSION:1.0\n"
            "BEGIN:VEVENT\n"
            "SUMMARY:meeting on site\n"
            "DTEND:20060406T163000Z\n"
            "DTSTART:20060406T160000Z\n"
            "DTSTAMP:20060406T211449Z\n"
            "LOCATION:big meeting room\n"
            "DESCRIPTION:nice to see you\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        /* change location in insertItem in testMerge() */
        config.mergeItem1 =
            "BEGIN:VCALENDAR\n"
            "VERSION:1.0\n"
            "BEGIN:VEVENT\n"
            "SUMMARY:phone meeting\n"
            "DTEND:20060406T163000Z\n"
            "DTSTART:20060406T160000Z\n"
            "DTSTAMP:20060406T211449Z\n"
            "LOCATION:calling from home\n"
            "DESCRIPTION:let's talk\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        config.mergeItem2 =
            "BEGIN:VCALENDAR\n"
            "VERSION:1.0\n"
            "BEGIN:VEVENT\n"
            "SUMMARY:phone meeting\n"
            "DTEND:20060406T163000Z\n"
            "DTSTART:20060406T160000Z\n"
            "DTSTAMP:20060406T211449Z\n"
            "LOCATION:my office\n"
            "DESCRIPTION:what the heck, let's even shout a bit\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        config.templateItem = config.insertItem;
        config.uniqueProperties = "SUMMARY:UID:LOCATION";
        config.sizeProperty = "DESCRIPTION";
        config.import = import;
        config.dump = dump;
        config.compare = compare;
        config.testcases = "testcases/vcal10.ics";
    } else if(!strcmp(type, "itodo20")) {
        config.sourceName = "itodo20";
        config.uri = "task2"; // ScheduleWorld
        config.type = "text/x-vcalendar";
        config.insertItem =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VTODO\n"
            "UID:20060417T173712Z-4360-727-1-2730@gollum\n"
            "DTSTAMP:20060417T173712Z\n"
            "SUMMARY:do me\n"
            "DESCRIPTION:to be done\n"
            "PRIORITY:0\n"
            "STATUS:IN-PROCESS\n"
            "CREATED:20060417T173712\n"
            "LAST-MODIFIED:20060417T173712\n"
            "END:VTODO\n"
            "END:VCALENDAR\n";
        config.updateItem =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VTODO\n"
            "UID:20060417T173712Z-4360-727-1-2730@gollum\n"
            "DTSTAMP:20060417T173712Z\n"
            "SUMMARY:do me ASAP\n"
            "DESCRIPTION:to be done\n"
            "PRIORITY:1\n"
            "STATUS:IN-PROCESS\n"
            "CREATED:20060417T173712\n"
            "LAST-MODIFIED:20060417T173712\n"
            "END:VTODO\n"
            "END:VCALENDAR\n";
        /* change summary in insertItem in testMerge() */
        config.mergeItem1 =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VTODO\n"
            "UID:20060417T173712Z-4360-727-1-2730@gollum\n"
            "DTSTAMP:20060417T173712Z\n"
            "SUMMARY:do me please\\, please\n"
            "DESCRIPTION:to be done\n"
            "PRIORITY:0\n"
            "STATUS:IN-PROCESS\n"
            "CREATED:20060417T173712\n"
            "LAST-MODIFIED:20060417T173712\n"
            "END:VTODO\n"
            "END:VCALENDAR\n";
        config.mergeItem2 =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VTODO\n"
            "UID:20060417T173712Z-4360-727-1-2730@gollum\n"
            "DTSTAMP:20060417T173712Z\n"
            "SUMMARY:do me\n"
            "DESCRIPTION:to be done\n"
            "PRIORITY:7\n"
            "STATUS:IN-PROCESS\n"
            "CREATED:20060417T173712\n"
            "LAST-MODIFIED:20060417T173712\n"
            "END:VTODO\n"
            "END:VCALENDAR\n";
        config.templateItem = config.insertItem;
        config.uniqueProperties = "SUMMARY:UID";
        config.sizeProperty = "DESCRIPTION";
        config.import = import;
        config.dump = dump;
        config.compare = compare;
        config.testcases = "testcases/itodo20.ics";
    }
}

void CheckSyncReport::check(int res, SyncReport &report) const
{
    // first dump the report
    StringBuffer str, tmp;

    report.toString(str, 1);
    str += "----------|--------CLIENT---------|--------SERVER---------|\n";
    str += "          |  NEW  |  MOD  |  DEL  |  NEW  |  MOD  |  DEL  |\n";
    str += "----------|-----------------------------------------------|\n";
    str += tmp.sprintf("Expected  |  %3d  |  %3d  |  %3d  |  %3d  |  %3d  |  %3d  |\n",
                       clientAdded, clientUpdated, clientDeleted,
                       serverAdded, serverUpdated, serverDeleted);
    LOG.info("%s", str.c_str());

    CPPUNIT_ASSERT_MESSAGE("synchronization failed", !res);

    // this code is intentionally duplicated to produce nicer CPPUNIT asserts
    for (unsigned int i=0; report.getSyncSourceReport(i); i++) {
        SyncSourceReport* ssr = report.getSyncSourceReport(i);
        if (ssr->getState() == SOURCE_INACTIVE) {
            continue;
        }

        const char *name = ssr->getSourceName();
        LOG.debug("Checking sync source %s...", name);
        CLIENT_TEST_EQUAL(name, 0, ssr->getItemReportFailedCount(CLIENT, COMMAND_ADD));
        if (clientAdded != -1) {
            CLIENT_TEST_EQUAL(name, clientAdded, ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_ADD));
        }
        CLIENT_TEST_EQUAL(name, 0, ssr->getItemReportFailedCount(CLIENT, COMMAND_REPLACE));
        if (clientUpdated != -1) {
            CLIENT_TEST_EQUAL(name, clientUpdated, ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_REPLACE));
        }
        CLIENT_TEST_EQUAL(name, 0, ssr->getItemReportFailedCount(CLIENT, COMMAND_DELETE));
        if (clientDeleted != -1) {
            CLIENT_TEST_EQUAL(name, clientDeleted, ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_DELETE));
        }

        CLIENT_TEST_EQUAL(name, 0, ssr->getItemReportFailedCount(SERVER, COMMAND_ADD));
        if (serverAdded != -1) {
            CLIENT_TEST_EQUAL(name, serverAdded, ssr->getItemReportSuccessfulCount(SERVER, COMMAND_ADD));
        }
        CLIENT_TEST_EQUAL(name, 0, ssr->getItemReportFailedCount(SERVER, COMMAND_REPLACE));
        if (serverUpdated != -1) {
            CLIENT_TEST_EQUAL(name, serverUpdated, ssr->getItemReportSuccessfulCount(SERVER, COMMAND_REPLACE));
        }
        CLIENT_TEST_EQUAL(name, 0, ssr->getItemReportFailedCount(SERVER, COMMAND_DELETE));
        if (serverDeleted != -1) {
            CLIENT_TEST_EQUAL(name, serverDeleted, ssr->getItemReportSuccessfulCount(SERVER, COMMAND_DELETE));
        }
    }
    LOG.debug("Done with checking sync report.");
}

/** @} */
/** @endcond */
#endif // ENABLE_INTEGRATION_TESTS
