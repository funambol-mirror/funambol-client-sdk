#!/bin/sh

if [ "$1" = 'test' ]; then
    shift
    testrun=1
else
    testrun=0
    yn='y'
fi

list=`svn list $1`

if [ "$list" = "" ]; then
    echo "Nothing to do"
#    exit
fi

printf "Moving:\n$list\n\nfrom: $1\nto: $2\nwith message: $3\n"

if [ "$testrun" -eq 1 ]; then
    printf "Continue: "
    read yn
    echo $yn
fi

if [ "$yn" = 'y' ]; then

    for file in $list; do
        echo svn mv -m "$3" $1/$file $2
        [ $testrun -eq 0 ] && svn mv -m "$3" $1/$file $2
    done

fi

