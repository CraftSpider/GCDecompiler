#!/usr/bin/env bash

function copy_headers {
    rsync -avm --include='*.h' -f 'hide,! */' libs/$1 libs/$2
}

function copy_lib {
    cp $1/*.a libs/
}

mkdir libs

# Clone and build zlib
git clone https://github.com/madler/zlib libs/zlib
cmake libs/zlib
make -C libs/zlib zlibstatic

# Copy over .h files into libs/include and .a into libs
copy_headers "include" "zlib"
copy_lib "zlib"


git clone https://github.com/craftspider/alphatools libs/alphatools
cmake libs/alphatools
make -C libs/alphatools

# Copy over .h files into libs/include and .a into libs
copy_headers "libs" "alphatools"
copy_lib "alphatools"