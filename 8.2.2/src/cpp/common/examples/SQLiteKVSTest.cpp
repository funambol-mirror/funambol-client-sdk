
#include <iostream>

#include "base/util/Enumeration.h"
#include "base/util/KeyValuePair.h"
#include "client/SQLiteKeyValueStore.h"
#include "base/globalsdef.h"


#include <sqlite3.h>

USE_NAMESPACE

int main()
{
    sqlite3 * db;
    sqlite3_stmt * stmt;
    int ret;
    
    // Forcibly remove database to start a clean test
    remove("test.db");
    
    char * create = "CREATE TABLE test (key varchar(64) PRIMARY KEY, value varchar(64));";
    char * drop   = "DROP TABLE test;";
    
    ret = sqlite3_open    ("test.db", &db);
    if (ret != SQLITE_OK)   {  std::cout << "ERROR: Could not open connection" << std::endl; exit(0); }
    ret = sqlite3_prepare (db, create, strlen(create), &stmt, NULL);
    if (ret != SQLITE_OK)   {  std::cout << "ERROR: Could prepare create"      << std::endl; exit(0); }
    ret = sqlite3_step    (stmt);
    if (ret != SQLITE_DONE) {  std::cout << "ERROR: Could not exec create"     << std::endl; exit(0); }
 
    std::cout << "Constructing SQLiteKeyValueStore" << std::endl;
    SQLiteKeyValueStore kvs("test", "key", "value", "test.db");
    std::cout << "Connecting SQLiteKeyValueStore" << std::endl;
    ret = kvs.connect();
    
    std::cout << "Adding values" << std::endl;
    ret = kvs.setPropertyValue("1", "2");
    if (ret) { std::cout << "ERROR: Could not set element 1" << std::endl; exit(0); }
    ret = kvs.setPropertyValue("2", "3");
    if (ret) { std::cout << "ERROR: Could not set element 2" << std::endl; exit(0); }
    ret = kvs.setPropertyValue("3", "4");
    if (ret) { std::cout << "ERROR: Could not set element 3" << std::endl; exit(0); }
    ret = kvs.setPropertyValue("4", "5");
    if (ret) { std::cout << "ERROR: Could not set element 4" << std::endl; exit(0); }
    
    std::cout << "Saving" << std::endl;
    kvs.save();
    
    std::cout << "Checking for data" << std::endl;
    
    {
        Enumeration& en = kvs.getProperties();
        int count = 0;
        while (en.hasMoreElement())
        {
            KeyValuePair * kvp = (KeyValuePair*)en.getNextElement();
            std::cout << "Element " << count << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
            count++;
        }
        if (count != 4) {  std::cout << "ERROR: Not all values present"     << std::endl; exit(0); }
    }
    
    std::cout << "Removing keys 2 and 3" << std::endl;
    
    kvs.removeProperty("2");
    kvs.removeProperty("3");
    
    StringBuffer sb;
    sb = kvs.readPropertyValue("2");
    if (!sb.null()) { std::cout << "ERROR: Able to read deleted value 2" << std::endl; exit(0);}
    sb = kvs.readPropertyValue("3");
    if (!sb.null()) { std::cout << "ERROR: Able to read deleted value 3" << std::endl; exit(0);}
    sb = kvs.readPropertyValue("1");
    if (sb.null())  { std::cout << "ERROR: Unable to read value 1" << std::endl; exit(0);}
    sb = kvs.readPropertyValue("4");
    if (sb.null())  { std::cout << "ERROR: Unable to read value 4" << std::endl; exit(0);}
    
    std::cout << "Saving" << std::endl;
    ret = kvs.save();
    if (ret) { std::cout << "ERROR: Save failed" << std::endl; exit(0);}
    
    kvs.setPropertyValue("2", "100");
    kvs.setPropertyValue("3", "100");
    
    sb = kvs.readPropertyValue("1");
    if (sb.null())  { std::cout << "ERROR: Unable to read value 1" << std::endl; exit(0);}
    sb = kvs.readPropertyValue("2");
    if (sb.null())  { std::cout << "ERROR: Unable to read value 2" << std::endl; exit(0);}
    sb = kvs.readPropertyValue("3");
    if (sb.null())  { std::cout << "ERROR: Unable to read value 3" << std::endl; exit(0);}
    sb = kvs.readPropertyValue("4");
    if (sb.null())  { std::cout << "ERROR: Unable to read value 4" << std::endl; exit(0);}
    
    std::cout << "Saving" << std::endl;
    ret = kvs.save();
    if (ret) { std::cout << "ERROR: Save failed" << std::endl; exit(0);}
    
    std::cout << "Attempting modifications to CURRENT data while iterating..." << std::endl;

    {
        Enumeration& en = kvs.getProperties();
        int count = 0;
        while (en.hasMoreElement())
        {
            KeyValuePair * kvp = (KeyValuePair*)en.getNextElement();
            std::cout << "Element " << count << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
            kvs.setPropertyValue(kvp->getKey(), "500");
            count++;
        }
        if (count != 4) {  std::cout << "ERROR: Not all values present"     << std::endl; exit(0); }
    }
    
    {
        Enumeration& en = kvs.getProperties();
        int count = 0;
        while (en.hasMoreElement())
        {
            KeyValuePair * kvp = (KeyValuePair*)en.getNextElement();
            std::cout << "Element " << count << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
            if (strcmp(kvp->getValue(), "500") == 0)
                count++;
        }
        if (count != 4) {  std::cout << "ERROR: Not all values correct"     << std::endl; exit(0); }
    }
    
    std::cout << "Saving" << std::endl;
    ret = kvs.save();
    if (ret) { std::cout << "ERROR: Save failed" << std::endl; exit(0);}
    
    {
        Enumeration& en = kvs.getProperties();
        int count = 0;
        while (en.hasMoreElement())
        {
            KeyValuePair * kvp = (KeyValuePair*)en.getNextElement();
            std::cout << "Element " << count << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
            if (strcmp(kvp->getValue(), "500") == 0)
                count++;
        }
        if (count != 4) {  std::cout << "ERROR: Not all values correct"     << std::endl; exit(0); }
    }
        
    std::cout << "Attempting modifications to PAST data while iterating..." << std::endl;

    {
        Enumeration& en = kvs.getProperties();
        int count = 0;
        StringBuffer lastKey(NULL);
        while (en.hasMoreElement())
        {
            KeyValuePair * kvp = (KeyValuePair*)en.getNextElement();
            std::cout << "Element " << count << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
            if (!lastKey.null())
            {
                kvs.setPropertyValue(lastKey, "1000");
            }
            lastKey = kvp->getKey();
        }
    }
    
    std::cout << "Saving" << std::endl;
    ret = kvs.save();
    if (ret) { std::cout << "ERROR: Save failed" << std::endl; exit(0);}
    
    {
        Enumeration& en = kvs.getProperties();
        int count = 0;
        while (en.hasMoreElement())
        {
            KeyValuePair * kvp = (KeyValuePair*)en.getNextElement();
            std::cout << "Element " << count << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
            if (strcmp(kvp->getValue(), "1000") == 0)
                count++;
        }
        if (count != 3) {  std::cout << "ERROR: Not all values correct"     << std::endl; exit(0); }
    }


    std::cout << "Attempting modifications to FUTURE data while iterating..." << std::endl;

    {
        Enumeration& en = kvs.getProperties();
        int count = 0;
        StringBuffer lastKey(NULL);
        while (en.hasMoreElement())
        {
            kvs.setPropertyValue("2", "5000");
            KeyValuePair * kvp = (KeyValuePair*)en.getNextElement();
            std::cout << "Element " << count << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
            if (strcmp(kvp->getValue(), "5000") == 0)
                count++;
        }

        if (count != 1) {  std::cout << "WARNING: Future value not returned correctly"     << std::endl; exit(0); }
    }
    kvs.setPropertyValue("2", "1000");
    
    std::cout << "Saving" << std::endl;
    ret = kvs.save();
    if (ret) { std::cout << "ERROR: Save failed" << std::endl; exit(0);}
    
    {
        kvs.setPropertyValue("2", "5000");
        Enumeration& en = kvs.getProperties();
        int count = 0;
        StringBuffer lastKey(NULL);
        while (en.hasMoreElement())
        {
            KeyValuePair * kvp = (KeyValuePair*)en.getNextElement();
            std::cout << "Element " << count << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
            if (strcmp(kvp->getValue(), "5000") == 0)
                count++;
        }

        if (count != 1) {  std::cout << "WARNING: Future value not returned correctly"     << std::endl; exit(0); }
    }
    std::cout << "Saving" << std::endl;
    ret = kvs.save();
    if (ret) { std::cout << "ERROR: Save failed" << std::endl; exit(0);}

    /////////////

    std::cout << "Attempting removal to PAST data while iterating..." << std::endl;

    {
        Enumeration& en = kvs.getProperties();
        int count = 0;
        StringBuffer lastKey(NULL);
        while (en.hasMoreElement())
        {
            KeyValuePair * kvp = (KeyValuePair*)en.getNextElement();
            std::cout << "Element " << count << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
            if (!lastKey.null())
            {
                kvs.removeProperty(lastKey);
            }
            lastKey = kvp->getKey();
        }
    }
    
    std::cout << "Saving" << std::endl;
    ret = kvs.save();
    if (ret) { std::cout << "ERROR: Save failed" << std::endl; exit(0);}

    
    {
        Enumeration& en = kvs.getProperties();
        int count = 0;
        while (en.hasMoreElement())
        {
            KeyValuePair * kvp = (KeyValuePair*)en.getNextElement();
            std::cout << "Element " << count << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
            count++;
        }
        if (count != 1) {  std::cout << "ERROR: Not all values removed"     << std::endl; exit(0); }
    }
    
    kvs.setPropertyValue("1", "1");
    kvs.setPropertyValue("2", "2");
    kvs.setPropertyValue("3", "3");
    kvs.setPropertyValue("4", "4");            
    
    std::cout << "Saving" << std::endl;
    ret = kvs.save();
    if (ret) { std::cout << "ERROR: Save failed" << std::endl; exit(0);}

    std::cout << "Attempting removal to FUTURE data while iterating..." << std::endl;

    {
        Enumeration& en = kvs.getProperties();
        int count = 0;
        StringBuffer lastKey(NULL);
        kvs.removeProperty("2");
        while (en.hasMoreElement())
        {
            KeyValuePair * kvp = (KeyValuePair*)en.getNextElement();
            std::cout << "Element " << count << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
            if (strcmp(kvp->getKey(), "2") == 0)
                count++;
        }

        if (count != 0) {  std::cout << "WARNING: Future value 2 not removed correctly"     << std::endl; exit(0); }
    }
    kvs.setPropertyValue("2", "2");
    
    std::cout << "Saving" << std::endl;
    ret = kvs.save();
    if (ret) { std::cout << "ERROR: Save failed" << std::endl; exit(0);}
        
    ret = kvs.disconnect();
    
    ret = sqlite3_prepare (db, drop, strlen(drop), &stmt, NULL);
    if (ret != SQLITE_OK)   {  std::cout << "ERROR: Could prepare drop"      << std::endl; exit(0); }
    ret = sqlite3_step    (stmt);
    if (ret != SQLITE_DONE) {  std::cout << "ERROR: Could not exec drop"     << std::endl; exit(0); }
    
    std::cout << "ALL TESTS PASSED" << std::endl;
    /*
    test = kvs.readPropertyValue("2");
    if (test.null())
    {
        std::cout << "Correct!" << std::endl;
    }
    
    Enumeration& en2 = kvs.getProperties();

    int x = 0;
    while (en2.hasMoreElement())
    {
        KeyValuePair * kvp = (KeyValuePair*) (en2.getNextElement());
        if (strcmp(kvp->getKey(), "2") == 0)
        {
            int ret = kvs.removeProperty(kvp->getKey());
            std::cout << "Attempt to remove " << kvp->getKey() << ": " << ret << std::endl;
        }
        std::cout << "Element " << x << ": ( " << kvp->getKey() << " , " << kvp->getValue() << " )" << std::endl;
    }
    */
    
    kvs.disconnect();
    
    return 0;
}   