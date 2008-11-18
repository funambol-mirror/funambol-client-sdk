/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.funambol.util;


/**
 * the simplest conneection handler ever. just save the config
 * 
 */
public class BasicConnectionHandler implements ConnectionHandler {

    public boolean isOkToUseConfig(String config) {
        Log.debug("[BasicConnectionHandler] saving current tcp config using bbhelper");
        return true;
    }

   

}
