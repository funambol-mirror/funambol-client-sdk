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

#include <eikenv.h>
#include <e32cmn.h>
#include <bautils.h>
#include "base/SymbianLog.h"
#include "base/util/symbianUtils.h"
#include "base/util/stringUtils.h"
#include "base/util/timeUtils.h"
#include "base/globalsdef.h"

USE_NAMESPACE


// Formats for date&time printed into log.
_LIT(KFormatDateAndTime, "%*D%*N%Y%1-%2-%3 %:0%J%:1%T%:2%S");
_LIT(KFormatOnlyTime,    "%:0%J%:1%T%:2%S");

// The name of the semaphore
_LIT(KLogSemaphoreName,  "FLogSemaphore");


SymbianLog::SymbianLog(bool resetLog, const char* path, const char* name) 
{
    TInt err = KErrNone;
    
    const char* p = (path)? path : SYMBIAN_LOG_PATH;
    const char* n = (name)? name : SYMBIAN_LOG_NAME;
    
    // assign all the paths and names
    StringBuffer logName(p);
    logName += n;
    iLogName.Assign(charToNewBuf(logName.c_str()));
    
    StringBuffer rollName(logName);
    rollName += ".0";
    iRollLogName.Assign(charToNewBuf(rollName.c_str()));
    
    StringBuffer pathSb(p);
    iLogPathName.Assign(charToNewBuf(pathSb.c_str()));
    
    StringBuffer nameSb(n);
    iLogFileName.Assign(charToNewBuf(nameSb.c_str()));
    
    // try open log 
    err = iSemaphore.OpenGlobal(KLogSemaphoreName);  

    if (err == KErrNotFound) {
        // Create a semaphore, to avoid accessing the FileSystem at
        // the same time by different threads.
        // The semaphore is global, so that it could be used also by
        // other processes that (in future) will use this Log.
        err = iSemaphore.CreateGlobal(KLogSemaphoreName, 1);
        if (err != KErrNone) {
            setError(ERR_SEMAPHORE_CREATION, ERR_SEMAPHORE_CREATION_MSG);
        }
    } 

    iSemaphore.Wait();

    
    // Connect to the file server session.
    fsSession.Connect();
    err = fsSession.ShareAuto();
    if (err != KErrNone) {
        setErrorF(err, "SymbianLog error: unable to share RFs session (code %d)", err);
        return;
    }
    
    // ensure path exists!
    BaflUtils::EnsurePathExistsL(fsSession, iLogPathName);
    
    if (resetLog) {
        err = file.Replace(fsSession, iLogName, EFileWrite|EFileShareAny);
        if (err != KErrNone) {
            setErrorF(err, "SymbianLog: could not open the log file '%ls'", iLogName.Ptr());
            return;
        }
        
        // Write the Header
        StringBuffer header = createHeader();
        RBuf8 data;
        data.Assign(stringBufferToNewBuf8(header));
        file.Write(data);
        data.Close();
        file.Close();
    }
    
    iSemaphore.Signal();
    return;
}

SymbianLog::~SymbianLog() {
    iLogPathName.Close();
    iLogFileName.Close();
    iLogName.Close();
    iRollLogName.Close();
    fsSession.Close();
    iSemaphore.Close();
}

void SymbianLog::setLogPath(const char* configLogPath ) 
{
    if (configLogPath == NULL)
    {
        return;
    }
    iSemaphore.Wait();
    
    iLogPathName.Close();
    iLogPathName.Assign(charToNewBuf(configLogPath));
    
    TInt size;
    size = iLogPathName.Size() + iLogFileName.Size();
    iLogName.Close();
    iLogName.CreateL(size);
    iLogName.Copy(iLogPathName);
    iLogName.Append(iLogFileName);

    iRollLogName.Close();
    iRollLogName.CreateL(iLogName.Size());
    iRollLogName.Copy(iLogName);
    iRollLogName.Append(_L(".0"));
    
    // ensure path exists!
    BaflUtils::EnsurePathExistsL(fsSession,iLogPathName);
    
    iSemaphore.Signal();
    return;
}

void SymbianLog::setLogName(const char* configLogName ) 
{
    if (configLogName == NULL)
    {
        return;
    }
    iSemaphore.Wait();
    
    iLogFileName.Close();
    iLogFileName.Assign(charToNewBuf(configLogName));
    
    TInt size;
    size = iLogPathName.Size() + iLogFileName.Size();
    iLogName.Close();
    iLogName.CreateL(size);
    iLogName.Copy(iLogPathName);
    iLogName.Append(iLogFileName);

    iRollLogName.Close();
    iRollLogName.CreateL(iLogName.Size());
    iRollLogName.Copy(iLogName);
    iRollLogName.Append(_L(".0"));
    
    iSemaphore.Signal();
    return;
}


StringBuffer SymbianLog::createHeader(const char* title) 
{
    const char *t = (title)? title : SYMBIAN_LOG_HEADER;
    
    StringBuffer header = createCurrentTime(true);      // e.g. "2008/04/02 10:58:03 GMT +2:00"
    header += " - ";
    header += t;
    header += "\n";

    return header;
}

StringBuffer SymbianLog::createCurrentTime(bool complete) 
{
    if (iFormattedBias.length() == 0) {
        // Create the string e.g. "GMT +2:00" only once (it's always the same)
        createFormattedBias();
    }
    
    TTime local;
    StringBuffer ret;
    TBuf<50> formattedTime;
    
    local.HomeTime();

    if (complete) { 
        local.FormatL(formattedTime, KFormatDateAndTime); 
        StringBuffer date = bufToStringBuffer(formattedTime);
        ret.sprintf("%s %s", date.c_str(), iFormattedBias.c_str());
    }
    else { 
        local.FormatL(formattedTime, KFormatOnlyTime);
        ret = bufToStringBuffer(formattedTime);
    }
    return ret;
}


void SymbianLog::createFormattedBias() 
{
    TTime local, utc;
    TTimeIntervalMinutes bias;
    
    local.HomeTime();
    utc.UniversalTime();
    local.MinutesFrom(utc, bias);

    TInt totalMinutes = bias.Int();
    TInt hours   = totalMinutes / 60;
    TInt minutes = totalMinutes % 60;
    
    if (totalMinutes >= 0) { iFormattedBias.sprintf("GMT +%d:%02d", hours, minutes); }
    else                   { iFormattedBias.sprintf("GMT %d:%02d",  hours, minutes); }
}



void SymbianLog::error(const char*  msg, ...) 
{
    PLATFORM_VA_LIST argList;
    PLATFORM_VA_START (argList, msg);
    printMessage(LOG_ERROR, msg, argList);
    PLATFORM_VA_END(argList);
}
void SymbianLog::info (const char*  msg, ...) 
{
    if (isLoggable(LOG_LEVEL_INFO)) {
        PLATFORM_VA_LIST argList;
        PLATFORM_VA_START (argList, msg);
        printMessage(LOG_INFO, msg, argList);
        PLATFORM_VA_END(argList);
    }
}
void SymbianLog::debug(const char*  msg, ...) 
{
    if (isLoggable(LOG_LEVEL_DEBUG)) {
        PLATFORM_VA_LIST argList;
        PLATFORM_VA_START (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);
        PLATFORM_VA_END(argList);
    }
}
void SymbianLog::developer(const char*  msg, ...) 
{
    if (isLoggable(LOG_LEVEL_INFO)) {
        PLATFORM_VA_LIST argList;
        PLATFORM_VA_START (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);
        PLATFORM_VA_END(argList);
    }
}

void SymbianLog::printMessage(const char* level, const char* msg, PLATFORM_VA_LIST argList) 
{
    iSemaphore.Wait();
    
    StringBuffer currentTime = createCurrentTime(true);
    
    TInt err = file.Open(fsSession, iLogName, EFileWrite|EFileShareAny);
    TInt pos = 0;
    
    if (err == KErrNotFound) 
    {
        // First time: file does not exist. Create it.
        err = file.Create(fsSession, iLogName, EFileWrite|EFileShareAny);
        if (err != KErrNone) {
            setErrorF(err, "SymbianLog: could not open log file (code %d)", err);
            goto finally;
        }
        StringBuffer header = createHeader();
        RBuf8 data;
        data.Assign(stringBufferToNewBuf8(header));
        file.Write(data);
        data.Close();
    }
    else 
    {
        err = file.Seek(ESeekEnd, pos);
        if (err != KErrNone) {
            setErrorF(err, "SymbianLog: seek error on log file (code %d)", err);
            goto finally;
        }
    }

    {
        // Write the data
        StringBuffer line, data;
        line.sprintf("%s -%s- %s", currentTime.c_str(), level, msg);
        data.vsprintf(line.c_str(), argList);
        data.append("\n");
        
        RBuf8 buf;
        buf.Assign(stringBufferToNewBuf8(data));
        file.Write(buf);
        buf.Close();
    }
    
finally:
    file.Close();
    // we need closed file to operate on it
    if ( LogSize() > SYMBIAN_LOG_SIZE ){
        // roll log file
        RollLogFile();
    }
    iSemaphore.Signal();
}

void SymbianLog::reset(const char* title) 
{
    iSemaphore.Wait();
    
    TInt err = file.Replace(fsSession, iLogName, EFileWrite|EFileShareAny);
    if (err != KErrNone) {
        setErrorF(err, "SymbianLog: error resetting the log file (code %d)", err);
        return;
    }
    
    // Write the Header
    StringBuffer header = createHeader(title);
    RBuf8 buf;
    buf.Assign(stringBufferToNewBuf8(header));
    file.Write(buf);
    buf.Close();
    
    file.Close();
    iSemaphore.Signal();
}


size_t SymbianLog::getLogSize() 
{
    return (size_t)LogSize();
}

TInt SymbianLog::LogSize()
{
    TEntry entry;
    fsSession.Entry(iLogName,entry);
    TInt size = entry.iSize;

    return size;
}

void SymbianLog::RollLogFile(void)
{
    CFileMan* fileMan = CFileMan::NewL(fsSession);
    CleanupStack::PushL(fileMan);
    //copy the current file into the roll file, file must be open
    TInt err = KErrNone;
    err = file.Open(fsSession, iLogName, EFileWrite|EFileShareAny);
    if (err != KErrNone) {
        setErrorF(err, "SymbianLog: could not open log file (code %d)", err);
    } else {
        err = fileMan->Copy(file,iRollLogName,CFileMan::EOverWrite);
    }
    if (err != KErrNone) {
        setErrorF(err, "SymbianLog: copy error on roll log file (code %d)", err);
    }
    file.Close();
    // reset the current file
    // we don't use reset() method because we don't want 
    // nested semaphores
    err = KErrNone;
    err = file.Replace(fsSession, iLogName, EFileWrite|EFileShareAny);
    if (err != KErrNone) {
        setErrorF(err, "SymbianLog: error resetting the log file (code %d)", err);
        return;
    }
    // Write the Header
    StringBuffer header = createHeader();
    RBuf8 buf;
    buf.Assign(stringBufferToNewBuf8(header));
    file.Write(buf);
    buf.Close();
    file.Close();
    
    CleanupStack::PopAndDestroy(fileMan);

}

Log *Log::logger;

Log &Log::instance() {
    if (!logger) {
        logger = new SymbianLog();
    }
    return *logger;
}
