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

#ifndef INCL_AUTOTOOLS_LOG
# define INCL_AUTOTOOLS_LOG
/** @cond DEV */

#include <base/fscapi.h>

/*
 * Opens the specified file for logging of messages.
 *
 * By default the LOG instance of the Log class will
 * create the file specified via its set methods
 * as soon as the first message needs to be printed or
 * when explicitly asking for a reset.
 *
 * By calling this function instead one gets more detailed
 * control over logging and avoids the (currently) insecurely
 * implemented handling of file name strings in the Log class.
 *
 * @param path            directory where file is to be created, can be NULL
 * @param name            file name relative to path or "-" when asking for
 *                        logging to stdout
 * @param redirectStderr  if TRUE, then file descriptor 2 (stderr)
 *                        will also be redirected into the log file;
 *                        the original stderr is preserved and will be
 *                        restored when turning this redirection off
 */
void setLogFile(const char *path, const char* name, BOOL redirectStderr = FALSE);

/** traditional version of setLogFile() which writes in the current directory */
void setLogFile(const char* name, BOOL redirectStderr = FALSE);

/** @endcond */
#endif
