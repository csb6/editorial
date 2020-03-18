#!/usr/bin/env sh

#First, build ./pattern-match, the code-gen program. This program will generate
#cpp-matcher.h and cpp-matcher.cpp which will contain a specialized function that
#matches C++ keywords on an NCurses terminal screen

#Next, build cpp-matcher.cpp and cpp-matcher.h into an object file, cpp-matcher.o.
#This file will be linked into the main executable, ./editorial, when ./build.sh
#is run, preventing code-gen from happening on normal builds

clang++ -std=c++17 -Wall -Wextra -pedantic-errors -o pattern-match pattern-match.cpp && ./pattern-match && rm pattern-match && clang++ -std=c++17 -c cpp-matcher.cpp
