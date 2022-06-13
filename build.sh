#!/usr/bin/env sh
#Set this to your compiler (e.g. g++)
compiler=g++
#Add debug flag when using static analyzer
#When running, you can do `./build.sh [any other flags you want to pass to compiler]`
$compiler -std=c++17 -Wall -Wextra -pedantic-errors $@ -o editorial *.cpp -lncurses
