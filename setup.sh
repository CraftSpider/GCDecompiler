#!/usr/bin/env bash

src_dir=$(pwd)

function cmake_build {
    git clone https://github.com/$1/$2 libs/$2
    cd libs/$2
    cmake .
    make
    cd src_dir
}

function copy_headers {
    rsync -avm --include='*.h' -f 'hide,! */' libs/$1 libs/$2
}

function copy_lib {
    cp $1/*.a libs/
}

mkdir libs

# Clone and build zlib
author=madler
project=zlib

cmake_build "madler" "zlib"

# Copy over .h files into libs/include and .a into libs
copy_headers "include" "zlib"
copy_lib "zlib"


cmake_build "craftspider" "alphatools"

# Copy over .h files into libs/include and .a into libs
copy_headers "libs" "alphatools"
copy_lib "alphatools"