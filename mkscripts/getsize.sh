#!/bin/bash

# If the ARCH passed in has 64 in the name, return 64 - otherwise 32
if [[ $1 == *"64"* ]]
then
    echo 64
else
    echo 32
fi