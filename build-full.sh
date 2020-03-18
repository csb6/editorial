#!/usr/bin/env sh

#If any command fails, exit the script
set -e

#Generates the code used to match C++ keywords
./build-code-gen.sh
#Build main program code, linking object file created during code-gen phase
./build.sh
