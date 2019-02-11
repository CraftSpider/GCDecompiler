#!/usr/bin/env bash

shopt -s extglob

src_dir=$(pwd)

function cmake_build {
    # Arguments:
    #   - Author
    #   - Project Name
    #   - Branch Name (optional)
    if [ -z "$3" ]; then
        echo "Cloning project $2"
        git clone --single-branch https://github.com/$1/$2 libs/$2;
    else
        echo "Cloning branch $3 of project $2"
        git clone --single-branch --branch $3 https://github.com/$1/$2 libs/$2;
    fi

    cd libs/$2
    echo "Running CMake for $2"
    cmake -G"Unix Makefiles" .
    echo "Running Make for $2"
    make
    cd ${src_dir}
}

function copy_headers {
    # Arguments:
    #   - Source dir
    #   - Destination dir
    rsync -avm --include='*.h' --include='*.tpp' --include='at_*' -f 'hide,! */' libs/$1/ libs/$2/
}

function copy_lib {
    # Attempts to copy a library file. If this fails, then the script immediately ends with a failure
    # Arguments:
    #   - Source dir
    cp libs/$1/*.@(a|lib) libs/ || exit 1
}

function cmake_dep {
    # Arguments:
    #   - Author
    #   - Project Name
    #   - Destination for rsync
    #   - Branch
    mkdir libs/$2

    cmake_build $1 $2 $4
    copy_headers $2 $3
    copy_lib $2

    rm -rf libs/$2
}

mkdir -p libs/include

cmake_dep "madler" "zlib" "include" "develop"
cmake_dep "craftspider" "alphatools" ""

# cmake -G"Unix Makefiles" .
