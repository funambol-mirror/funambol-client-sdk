 /*
 * Copyright (C) 2007 Funambol, Inc.
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

#ifndef INCL_CONSTANTS_WIN
#define INCL_CONSTANTS_WIN

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




/** @} */
/** @endcond */
#endif