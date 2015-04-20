#!/bin/bash

# Get the uname output to compare against
uout=`uname -a`

# ppc checks
if [[ $uout == *"ppc"* ]]
then
    if [[ $uout == *"ppc64le"* ]]
    then
        echo ppc64le
    elif [[ $uout == *"ppc64"* ]]
    then
        echo ppc64
    else
        echo ppc
    fi

# x86 checks
elif [[ $uout == *"x86"* ]]
then
    if [[ $uout == *"x86_64"* ]]
    then
        echo x86_64
    else
        echo x86
    fi

# aix checks
elif [[ $uout == *"AIX"* ]]
then
    echo aix

# ERROR
else
    echo NONE
fi
