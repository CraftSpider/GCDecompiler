#!/usr/bin/env bash

src_dir=$(pwd)

function cmake_build {
    if [ -z "$3" ]; then
        echo "Cloning branch $3 of project $2"
        git clone --single-branch --branch $3 https://github.com/$1/$2 libs/$2;
    else
        echo "Cloning project $2"
        git clone https://github.com/$1/$2 libs/$2;
    fi

    cd libs/$2
    echo "Running CMake for $2"
    cmake .
    echo "Running Make for $2"
    make
    cd ${src_dir}
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


cmake_build "craftspider" "alphatools" "alphalib"

# Copy over .h files into libs/include and .a into libs
copy_headers "libs" "alphatools"
copy_lib "alphatools"