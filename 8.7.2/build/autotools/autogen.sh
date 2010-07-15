#!/bin/sh

set -e 

libtoolize=${LIBTOOLIZE:-libtoolize}

if [ -z $(which "$libtoolize") ]; then
	echo "libtoolize not found: maybe you need to export or add it to your path. If it is a MacOS use glibtoolize."

	exit 1
fi

$libtoolize -c -f
aclocal
autoheader
automake -a -c -f
autoconf -f
