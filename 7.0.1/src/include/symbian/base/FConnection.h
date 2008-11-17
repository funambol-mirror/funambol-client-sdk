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

#ifndef INCL_FCONNECTION
#define INCL_FCONNECTION

#include <es_sock.h>            // RConnection
#include "base/fscapi.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


/// Max number of times we retry the connection, in case of errors.
#define         MAX_RETRY_CONNECTION    3




/**
 * This class is a singleton, used to manage the network connection.
 * Get the only instance of FConnection calling FConnection::getInstance().
 * Owns the RSocketServ and RConnection of the current connection,
 * that can be used exterally calling getConnection() and getSession().
 */
class FConnection 
{

private:
    
    /// pointer to FConnection sole instance
    static FConnection*     iInstance;
    
    /// The Socket Server necessary to estabilish internet connection
    RSocketServ             iSession;
    
    /// The Network connection interface
    RConnection             iConnection;
    
    /// Current local IP adress of the device
    StringBuffer            iLocalIpAddress;
    
    
    /// Current IAP ID of the active connection
    /// It's set once the connection is up and running
    TUint32                 iIAP;
    
    /// Current IAP name of the active connection.
    /// It can be set calling setIAPName() method.
    /// It's set also when the connection is up and running.
    StringBuffer            iIAPName;
    
    
    /// The last error code
    TInt                    iLastError;
    
    /// Counter to know how many times we retry to connect to network.
    TInt                    iRetryConnection;
    
    /// We don't want to open a RConnection that is already opened, 
    /// otherwise there can be errors using the connection.
    /// This bool is used to know if we called iConnection.Open().
    bool                    isConnectionOpened;

    
    // 1st and 2nd phase constructors
    static FConnection* NewL();
    static FConnection* NewLC();
    
    /**
     * Initialize the RConnection object connecting to network
     * with default preferences and without prompting the user.
     */
    void ConstructL();
    
    /**
     * Opens the connection.
     * It's used internally because we MUST have the connection opened
     * before calling iConnection.Start() or iConnection.EnumerateConnections().
     * @return  0 if connection opened without errors
     *          1 if connection already opened (so nothing done)
     *          a system error code <0 in case of errors
     */
    const int openConnection();
    
    
    //--------- utils ------------
    /**
     * Returns the IAP ID from its name.
     * Scans all the user defined accespoints inside the CommDb database.
     * @param aIAPName  the IAP name to seach
     * @return          the IAP ID (value >= 0), -1 if not found
     */
    TInt GetIAPIDFromName(const StringBuffer& aIAPName);
    
    /**
     * Returns the IAP name from its ID.
     * Scans all the user defined accespoints inside the CommDb database.
     * @param aIAPID  the IAP ID to seach
     * @return        the IAP name, empty string if not found
     */
    StringBuffer GetIAPNameFromID(const TUint aIAPID);

    
protected:
    
    /// Default constructor.
    FConnection();
    
public:
    
    /// Method to get the sole instance of FConnection
    static FConnection* getInstance();
    
    /// Method to destroy the instance of FConnection
    static void dispose();

    
    /// Destructor. Closes the connection and the session.
    virtual ~FConnection();
    
    
    /**
     * Starts a new GPRS connection using the default IAP, which name
     * is stored in the member 'iIAPDefaultName'.
     * The method setIAPDefaultName() should be called before
     * this method, to set the IAP to be used - otherwise, the user
     * will be prompted to select the IAP.
     * @return  0 if no error
     */
    const int startConnection();
    
    /**
     * Starts a new GPRS connection, given the IAP name.
     * @param aIAPName  "Ask" or "" : if we want to prompt the user to select the IAP
     *                  "Default"   : will use the default IAP of the system, no prompts
     *                  "<name>"    : will use the <name> IAP, no prompts. 
     *                                If not found, will prompt the user to select the IAP
     * @return  0 if no error
     */
    const int startConnection(const StringBuffer& aIAPName);
    
    /**
     * Closes the active connection.
     * The connection will not be dropped immediately: it will be dropped 
     * when there is no more data traffic on the connection.
     * 
     * @note    If a client needs to shutdown the connection in a hard way, then use
     *          stopConnection() instead of this method.
     */
    void closeConnection();
    
    /**
     * Stops the entire connection by disconnecting the underlying network 
     * interface immediately, regardless of whether other clients are using it or not.
     * Applications using the connection will be sent the socket error code KErrCancel.
     * 
     * @note    If a client needs to graciously shutdown the connection, closeConnection() 
     *          should be used instead of this method.
     * @return  0 if no error
     */
    const int stopConnection();
    
    /**
     * Checks if the connection is active or down.
     * Between active connection, we search for the one started with startConnection().
     * @return  true if connection is active
     */
    const bool isConnected();
    
    /**
     * Returns the local IP address of the device.
     * TODO
     */
    const StringBuffer& getLocalIpAddress();
    
    /**
     * Sets the IAP name, that will be used in startConnection().
     * This method should be called at the beginning, so that
     * subsequent calls to startConnection() will use this IAP.
     * @param  aIAPName  the name of the IAP
     */
    void setIAPName(const StringBuffer& aIAPName) { iIAPName = aIAPName; }
    
    /// Returns the current IAP name.
    StringBuffer& getIAPName() { return iIAPName; }
    
    /**
     * Returns an array of (new allocated) descriptors, with all 
     * the current IAP names.
     * The caller has the ownership of the array, so he should take
     * care of deleting it (calling Close()).
     */
    RArray<HBufC*> GetAllIAPNames() ;
    

    
    /// Returns a pointer to the (internally owned) RConnection.
    RConnection* getConnection() { return &iConnection; }

    /// Returns a pointer to the (internally owned) SocketServ.
    RSocketServ* getSession()    { return &iSession;    }

    /// Returns the last error code.
    TInt         getLastError()  { return iLastError;   }
};

END_NAMESPACE

#endif
