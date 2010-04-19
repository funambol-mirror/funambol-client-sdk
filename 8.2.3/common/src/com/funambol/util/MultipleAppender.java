/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.funambol.util;

import com.funambol.storage.DataAccessException;
import java.util.Vector;


/**
 * an appender that contains multiple appenders
 */
public class MultipleAppender implements Appender {

    Vector appenders = new Vector();

    /**
     * ad an appender to the list of appenders
     * @param appender
     */
    public void addAppender(Appender appender) {
        appenders.addElement(appender);
    }
    
    /**
     * remove given appender if present
     * @param appender
     * @return true if appender has been found and removed
     */
    public boolean removeAppender(Appender appender) {
        return appenders.removeElement(appender);
    }

    /**
     *  remove all the appenders
     */
    public void removeAllAppenders() {
        appenders.removeAllElements();
    } 
    
    public void initLogFile() {
        for (int i = 0; i < appenders.size(); i++) {
            ((Appender) appenders.elementAt(i)).initLogFile();
        }
    }

    public void openLogFile() {
        for (int i = 0; i < appenders.size(); i++) {
            ((Appender) appenders.elementAt(i)).openLogFile();
        }
    }

    public void closeLogFile() {
        for (int i = 0; i< appenders.size(); i++) {
            ((Appender)appenders.elementAt(i)).closeLogFile();
        }
    }

    public void deleteLogFile() {
        for (int i = 0; i< appenders.size(); i++) {
            ((Appender)appenders.elementAt(i)).deleteLogFile();
        }
    }

    /**
     * Perform additional actions needed when setting a new level.
     */
    public void setLogLevel(int level) {
        for (int i = 0; i< appenders.size(); i++) {
            ((Appender)appenders.elementAt(i)).setLogLevel(level);
        }
    }

    public void writeLogMessage(String level, String msg) throws DataAccessException {
        for (int i = 0; i< appenders.size(); i++) {
            ((Appender)appenders.elementAt(i)).writeLogMessage(level, msg);
        }
    }
}
