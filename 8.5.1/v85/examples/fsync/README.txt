Funambol File Sync example
__________________________

Overview
========
This example shows how to build a simple but working client using the Funabol
SDK. 
This is a simple commandline client to demonstrate the usage of the Funambol C++ SDK.
The client is able to synchronize files between a local folders and a SyncML
server, with any kind of content type.

It can be configured using the command line options or by editing the config
files (see section Invoking).


Downloading
===========

The source code of the examples are bundled with the Funambol C++ SDK code.
You can obtain them both from a released package from the Funambol website, or
checking out from the Subversion repository.

Please refer to the Funambol C++ SDK for details on how to obtain and compile
it.


Building
========

Build environments are available for the supported platforms. To build
for a certain platform, go to the subdirectory in "build" which matches
your platform and follow the instructions there.

The fsync example currently works on Windows and Unix (including Linux and
MacOS).

For Windows, you'll need VisualStudio 2005. Success have been reported also
using Visual C++ 2005 Express Edition.

For Unix/Linux, under "build/posix" you need autotools. First, you need to
build and install Funambol library (see sdk readme for details), then:

$ cd build/posix
$ ./autogen.sh
$ ./configure --prefix [same_prefix_used_for_sdk]
$ make install

Please report any issue you may have using this on other Unix variants.

Invoking
=========

You can set most important parameters using command line, both with short
notation and long:

  --help,	-h 		Displays this message
  --server,	-s <args>	set server url
  --dir,	-d <args>	set the local folder to sync
  --loglevel,	-l <args>	set log level [error, info, debug]
  --user,	-u <args>	set the user name
  --password,	-p <args>	set the user password
  --verbose,	-v 		increase verbosity
  --quiet,	-q 		decrease verbosity

Without parameters, fsync tries to sync using the configuration set in
$HOME/.config/Funambol/fsync. You can tweak the config for additional
parameters not available on the command line.


Resources
=========

If you encounter any issue building or using this example, please read first the
discussions archives or the Wiki page.

Project home:
https://client-sdk.forge.funambol.org/

WIKI:
https://core.forge.funambol.org/wiki/

---------------
Copyright (c) 2008-2010 Funambol, Inc. All rights reserved. 

