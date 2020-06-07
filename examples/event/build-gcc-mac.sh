#!/bin/sh
## NOTE: latest gcc requires latest XCode command line tools. If you observe an issue with _stdio.h :
## /usr/local/Cellar/gcc/9.3.0_1/lib/gcc/9/gcc/x86_64-apple-darwin18/9.3.0/include-fixed/stdio.h:78:10: fatal error: _stdio.h: No such file or directory
## Then perform reinstallation of XCode Command Line Tools as follows:
# sudo rm -rf /Library/Developer/CommandLineTools
# xcode-select --install
## (Proceed with Accepting the license pop-up)
# cd  /Library/Developer/CommandLineTools/Packages/
# open macOS_SDK_headers_for_macOS_10.14.pkg
## (Proceed with Installation)
export PATH=/usr/local/bin:$PATH
export CC=gcc-9
#/usr/local/Cellar/gcc/9.3.0_1/bin/gcc-9
export CXX=g++-9
#/usr/local/Cellar/gcc/9.3.0_1/bin/g++-9
mkdir -p build
cd build
cmake -DCMAKE_OSX_SYSROOT=/Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk ..
make
