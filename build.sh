#!/usr/bin/env sh
#Set this to your compiler (e.g. g++)
compiler=clang++
#Add debug flag when using static analyzer
#When running, you can do `./build.sh [any other flags you want to pass to compiler]`
$compiler -std=c++2a -Wall -Wextra -pedantic-errors $@ -lncurses -o editorial main.cpp syntax-highlight.cpp screen.cpp cpp_matcher.o mips_matcher.o
