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

#ifndef INCL_CONSTANTS_WIN
#define INCL_CONSTANTS_WIN

#include "vocl/AppDefs.h"

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */


/**
 * This is defined in MS Outlook and Pocket Outlook libraries.
 * Following are the possible values for WinEvent property "Sensitivity".
 */
enum WinSensitivity 
{
    winNormal       = 0,
    winPersonal     = 1,
    winPrivate      = 2,
    winConfidential = 3
};


/**
 * This is defined in MS Outlook and Pocket Outlook libraries.
 * Following are the possible values for WinEvent/WinTask property "Importance".
 */
enum WinImportance 
{
    winImportanceLow    = 0,
    winImportanceNormal = 1,
    winImportanceHigh   = 2
};


/**
 * This is defined in MS Outlook and Pocket Outlook libraries.
 * Recurring property "DaysOfWeekMask" is one or a combination of following values.
 */
enum WinDaysOfWeek
{
    winSunday    = 1,
    winMonday    = 2,
    winTuesday   = 4,
    winWednesday = 8,
    winThursday  = 16,
    winFriday    = 32,
    winSaturday  = 64
};


/**
 * This is defined in MS Outlook and Pocket Outlook libraries.
 * Following are the possible values for WinRecurrence property "RecurrenceType".
 */
enum WinRecurrenceType
{
    winRecursDaily    = 0,
    winRecursWeekly   = 1,
    winRecursMonthly  = 2,
    winRecursMonthNth = 3,
    winRecursYearly   = 5,
    winRecursYearNth  = 6
};

enum WinMeetingResponse
{
    winMeetingTentative = 2,
    winMeetingAccepted = 3,
    winMeetingDeclined = 4
};

enum WinMeetingStatus
{
    winNonMeeting = 0,
    winMeeting = 1,
    winMeetingReceived = 3,
    winMeetingCanceled = 5
};

/**
 * This is defined in MS Outlook libraries for note property "Color".
 */
enum WinNoteColor
{
    winBlue   = 0,
    winGreen  = 1,
    winPink   = 2,
    winYellow = 3,
    winWhite  = 4,
    NUM_NOTE_COLOR
};

enum WinBusyStatus
{
    winFree = 0,
    winTentative = 1,
    winBusy = 2,
    winOutOfOffice = 3
};

/**
 * This is defined in MS Outlook and Pocket Outlook libraries.
 * Following are the possible values for WinTask property "Status".
 */
enum WinTaskStatus
{
    winTaskNotStarted = 0,
    winTaskInProgress = 1,
    winTaskComplete   = 2,
    winTaskWaiting    = 3,
    winTaskDeferred   = 4
};



/** @} */
/** @endcond */
#endif
