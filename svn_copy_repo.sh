#!/bin/sh
#--------------------------------------------------------
# Copies between two repositories
#--------------------------------------------------------

usage()
{
    printf "Syntax: svn_copy_repo <src_url> <dest_url>\n"
}

if [ "$1" = 'test' ]; then
    shift
    testrun=1
else
    testrun=0
    yn='y'
fi

if [ "$1" = "" ]; then
    usage
    exit
fi

if [ "$2" = "" ]; then
    usage
    exit
fi

printf "Copying: $1\nto: $2\n"

if [ "$testrun" -eq 1 ]; then
    printf "Continue: "
    read yn
    echo $yn
fi

if [ "$yn" = 'y' ]; then

    rm -rf staging
    svn export $1 staging
    cd staging
    svn import -m "Import from $1" $2

fi

