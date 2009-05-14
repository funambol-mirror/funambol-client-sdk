/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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

#ifndef INCL_OPTION_PARSE
#define INCL_OPTION_PARSE

#include "base/fscapi.h"
#include "base/util/StringBuffer.h"
#include "base/util/ArrayList.h"
#include "base/util/StringMap.h"

BEGIN_NAMESPACE

/**
 * Generic option parser.
 */
class OptionParser {

    private:
        
        /** The program name, used in the usage page. */
        StringBuffer programName;

        /** A list of Option objects, representing the options 
         * supported by the program.
         */
        ArrayList options;

        /**
         * A map of arguments, with the name of the arg and the help message
         * This is used only for the usage page. The arguments are positional
         * and there is no syntax checking on them.
         */
        ArrayList arguments;

		/** error string */
        StringBuffer errMsg;

    public:

        /** 
         * Constructor: expects a string containing the name of the
         * calling program.
         */
        OptionParser(const char* progname); 

        /** Add a new option to the list.
         *
         * @param sn the short name of the option (one letter)
         * @paran ln the long name of the option
         * @param hm an help message (one line)
         * @hasArg true if the option requires an argument (default=false)
         *
         * @return true if the option has been added to the list,
         *         false in case of error.
         */
        bool addOption(char sn, const char *ln, const char *hm, bool hasArg=false);

        /** Remove an option from the list.
         *
         * @param ln the long name of the option to remove
         *
         * @return true if the option has been removed from the list
         *         false in case of error.
         */
        bool removeOption(const char *ln);

        /** Find the option in the list.
         *
         * @param ln the long name of the option to remove
         *
         * @return the position of the option in the list, -1 if not found.
         */
        int findOption(const char *ln, bool shortName=false) const;

        /**
         * Add a new argument to the argumet list.
         *
         * @param name the name of the argument
         * @param help an help message
         * @mandatory true if the option argument is mandatory (default=true)
         *
         * @return true if the argument has been added to the list,
         *         false in case of error.
         */
        int addArgument(const char *name, const char *help, bool mandatory=true);

        /**
         * Clear the argument list
         */
        void clearArguments();

        /** Parse the command line
         * @param argc the argc parameter, same as the one of the main() function
         * @param argv the argv parameter, same as the one of the main() function
         * @param OUT an ArrayList of KeyValuePair containing the options 
         *            found on the commandline
         * @param OUT an ArrayList of StringBuffer containing the other
         *            arguments found on the commandline
         *
         * @return true on success, false on error
         */
        bool parse(int argc, const char** argv, StringMap& opts, ArrayList& args);

        /**
         * Print the usage message.
         */
        void usage();

        /** get error message as a StringBuffer */
        const StringBuffer& getErrMsg() const { return errMsg; }

        const StringBuffer& getProgramName() const { return programName; };

};

END_NAMESPACE

#endif
