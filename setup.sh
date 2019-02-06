#!/usr/bin/env bash

src_dir=$(pwd)

function cmake_build {
    if [ -z "$3" ]; then
        echo "Cloning project $2"
        git clone https://github.com/$1/$2 libs/$2;
    else
        echo "Cloning branch $3 of project $2"
        git clone --single-branch --branch $3 https://github.com/$1/$2 libs/$2;
    fi

    cd libs/$2
    echo "Running CMake for $2"
    cmake .
    echo "Running Make for $2"
    make
    cd ${src_dir}
}

function copy_headers {
    rsync -avm --include='*.h' --include='*.tpp' --include='at_*' -f 'hide,! */' libs/$1/ libs/$2/
}

function copy_lib {
    cp libs/$1/*.a libs/
}

function cmake_dep {
    mkdir libs/$2

    cmake_build $1 $2
    copy_headers $2 $3
    copy_lib $2

    rm -rf libs/$2
}

mkdir -p libs/include


cmake_dep "madler" "zlib" "include"
cmake_dep "craftspider" "alphatools"
