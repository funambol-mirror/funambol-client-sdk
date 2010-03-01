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

#include "base/fscapi.h"
#include "client/OptionParser.h"

BEGIN_NAMESPACE

/*======================= Local class to store the option data. ==========================*/
class Option : public ArrayElement {

  public:
    StringBuffer shortName; // one-letter name
    StringBuffer longName;  // long name
    StringBuffer helpMsg;   // help message
    bool hasArgument;       // the option has an argument

    Option (const char *sn, const char *ln, const char *hm, bool hasArg=false) 
        : shortName(sn), longName(ln), helpMsg(hm), hasArgument(hasArg) {};

    ArrayElement* clone() {
        return (ArrayElement*) new Option(*this);
    }
};

/*======================= Local class to store the argument data. =======================*/
class Argument: public ArrayElement {

  public:
    StringBuffer name;  // name of the parameter
    StringBuffer help;  // help message
    bool mandatory;     // true if the parameter is mandatory

    Argument (const char *n, const char *h, bool m=false)
        : name(n), help(h), mandatory(m) {};

    ArrayElement* clone() {
        return (ArrayElement*) new Argument(*this);
    }
};


/*=======================================================================================*/

OptionParser::OptionParser(const char *progname): programName(progname) {
    addOption('h', "help", "Displays this message", false);
}

bool OptionParser::addOption(char sn, const char *ln, const char *hm, bool hasArg)
{
    char sname[2] = { sn, '\0' };
    
    if (findOption(ln) == -1 && findOption(sname,true) == -1) {
        Option opt(sname, ln, hm, hasArg);
        options.add(opt);
        return true;
    }
    return false;
}

bool OptionParser::removeOption(const char *ln) {
    int index;
    bool ret = false;

    if( (index=findOption(ln)) != -1 ) {
        if(options.removeElementAt(index) != -1) {
            ret=true;
        }
    }
    return ret;
}

int OptionParser::findOption(const char *name, bool shortName) const {
    int i;

    for(i=0; i<options.size(); i++) {
        StringBuffer &n = 
            (shortName) ?
                ((Option*)options[i])->shortName :
                ((Option*)options[i])->longName ;
        if( n == name ) {
            return i;
        }
    }
    return -1;
}

int OptionParser::addArgument(const char *name, const char *help, bool mandatory) {
    Argument arg(name, help, mandatory);
    return arguments.add(arg);
}

void OptionParser::clearArguments() {
    arguments.clear();
}


// Parses the command line
bool OptionParser::parse(int argc, const char** argv, StringMap &opts, ArrayList &args) {
    const char *arg = 0;
    int optind = 0;
    bool shortname = false;

    opts.clear();
    args.clear();

    for (int i=1; i<argc; i++) {
        arg = argv[i];

        if (arg[0] == '-') {
            if (arg[1] == '-') {
                arg += 2;       // long arg: skip the dashes
            }
            else {
                arg++;          // short arg: skip the dash
                shortname=true;
            }

            if ((optind=findOption(arg, shortname)) == -1){
                errMsg.sprintf("unknown option: %s", arg);

                //usage();
                return false;
            }

            Option *opt = (Option *)options[optind];

            if (opt->hasArgument) { // check if a value has been provided to option
				if ((argv[++i] == NULL) || (strlen(argv[i]) == 0)) {
					errMsg.sprintf("option '%s' requires an argument", arg);

					return false;
				}

                opts.put(opt->longName, argv[i]);
            } else {
                opts.put(opt->longName, "1");
            }

            continue;
        } else {
            StringBuffer s(argv[i]);
            args.add(s);
        }
    }

    if (opts["help"]){
        usage();
    }

    return true;
}

void OptionParser::usage() {
    StringBuffer optlist("["), arglist, opthelp, arghelp, line;
    Option *opt;
    Argument *arg;
    
    // Compose the options help string
    for( opt=(Option*)options.front(); opt; opt=(Option*)options.next() ) {
        optlist += opt->shortName;

        if (opt->hasArgument) {
            line.sprintf("\n  --%s,\t-%s <args>\t%s",
                opt->longName.c_str(), opt->shortName.c_str(), opt->helpMsg.c_str());
        } else {
	    	line.sprintf("\n  --%s,\t-%s \t\t%s",
                opt->longName.c_str(), opt->shortName.c_str(), opt->helpMsg.c_str());
        }

        opthelp += line;
    }
    // optlist can be [xxx] or empty
    if(optlist  != "["){
        optlist += "]";
    }
    else {
        optlist = "";
    }

    // Compose the arguments help string
    for( arg=(Argument*)arguments.front(); arg; arg=(Argument*)arguments.next() ) {
        arglist += (arg->mandatory)? " " : " [";
        arglist += arg->name;
        if (!arg->mandatory) arglist += "]";

        arghelp += "\n  ";
        arghelp += arg->name;
        arghelp += "\t\t";
        arghelp += arg->help;
    }
    printf("\nUsage: %s %s %s\n%s\n%s\n",
        programName.c_str(), optlist.c_str(), arglist.c_str(),
        opthelp.c_str(), arghelp.c_str());
}

END_NAMESPACE
