/*
 * Copyright (C) 2003-2007 Funambol, Inc
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


#ifndef INCL_BASE_EVENT
#define INCL_BASE_EVENT
/** @cond DEV */


/*
	Abstract Base Event Class. All event subtypes need to extend this class
*/

class BaseEvent {

    // Event code
    int type;

    // Time Stamp of when the event is generated
    unsigned long date;

public:

    // Constructor
    BaseEvent(int type, unsigned long date);
    ~BaseEvent();

    // set time stamp / date
    void setDate(unsigned long date);

    // get the event code
    int getType();

    // get the timestamp of the event
    unsigned long getDate();

};

/** @endcond */
#endif

