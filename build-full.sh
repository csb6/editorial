#!/usr/bin/env sh
cd termbox
./waf configure
./waf
./waf install --targets=termbox_static
cp build/src/libtermbox.a ..
echo "Cleaning up..."
rm -r build
cd ..
echo "Running build.sh..."
./build.sh
