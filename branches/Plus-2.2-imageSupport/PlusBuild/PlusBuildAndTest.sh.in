#!/bin/bash

# Hide all your directories from users

# Input parameters:
# if "$1" == ""   goto experimental
# if "$1" == "-E" goto experimental
# if "$1" == "-N" goto nightly
# if "$1" == "-C" goto continuous
# if "$1" == "-I" goto individual

# Run PlusBuild tests 
cd "/home/perklab/Devel/PlusBuild-bin"
/bin/bash BuildAndTest.sh $1

# Run PlusLib tests 
cd "/home/perklab/Devel/PlusBuild-bin/PlusLib-bin"
/bin/bash BuildAndTest.sh $1

# Run PlusApp tests 
cd "/home/perklab/Devel/PlusBuild-bin/PlusApp-bin"
/bin/bash BuildAndTest.sh $1

cd "/home/perklab/Devel/PlusBuild-bin"
