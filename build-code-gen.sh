#!/usr/bin/env sh

#Set this to your compiler (e.g. g++)
compiler=g++

#If any command fails, exit the script
set -e


#First, build pattern-match.o, which contains the basic functions for generating
#code for matching a language's keywords/types. It will be linked into every
#mode's code-gen program
$compiler -std=c++17 -Wall -Wextra -pedantic-errors -c pattern-match.cpp


#Next, generate ./cpp-mode, the code-gen program for C++ highlighting. This program
#will generate cpp_matcher.h and cpp_matcher.cpp which will contain a specialized
#function that matches C++ keywords on an NCurses terminal screen
$compiler -std=c++17 -I. -o cpp-mode modes/cpp-mode.cpp pattern-match.o
#Generate cpp-matcher.h and cpp-matcher.cpp
./cpp-mode
rm cpp-mode

#Next, build cpp_matcher.cpp and cpp_matcher.h into an object file, cpp-matcher.o.
#This file will be linked into the main executable, ./editorial, when ./build.sh
#is run, preventing code-gen from happening on normal builds
$compiler -std=c++17 -c cpp_matcher.cpp


#Same as above, but for MIPS assembly matching
$compiler -std=c++17 -I. -o mips-mode modes/mips-mode.cpp pattern-match.o
#Generate mips_matcher.h and mips_matcher.cpp
./mips-mode
rm mips-mode
$compiler -std=c++17 -c mips_matcher.cpp

#Cleanup after code gen
rm pattern-match.o
