cmake_minimum_required(VERSION 3.12)
project(GCDecompiler)

#Add uninstall target
configure_file("cmake/uninstall.cmake" "cmake/uninstall.cmake" COPYONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "cmake/uninstall.cmake")

#Use C++ 17
set(CMAKE_CXX_STANDARD 17)

#Export compile commands for editor autocomplete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#Be really pedantic! Also make sure we use unsigned chars
add_definitions(-Wall -Wextra -pedantic -funsigned-char)

#Show as an executable, not a shared library in file managers
if(UNIX)
    #-nopie is unused with AppleClang
    if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
    endif(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
endif(UNIX)

include_directories(GCDecompiler)
include_directories(GCDecompiler/filetypes)
include_directories(GCDecompiler/ppc)
include_directories(zlib)
include_directories(zlib/contrib/blast)
include_directories(zlib/contrib/infback9)
include_directories(zlib/contrib/iostream)
include_directories(zlib/contrib/iostream2)
include_directories(zlib/contrib/iostream3)
include_directories(zlib/contrib/minizip)
include_directories(zlib/contrib/puff)
include_directories(zlib/examples)

add_executable(GCDecompiler
        GCDecompiler/filetypes/dol.cpp
        GCDecompiler/filetypes/dol.h
        GCDecompiler/filetypes/lz.cpp
        GCDecompiler/filetypes/lz.h
        GCDecompiler/filetypes/rel.cpp
        GCDecompiler/filetypes/rel.h
        GCDecompiler/filetypes/tpl.cpp
        GCDecompiler/filetypes/tpl.h
        GCDecompiler/ppc/codes.h
        GCDecompiler/ppc/instruction.cpp
        GCDecompiler/ppc/instruction.h
        GCDecompiler/ppc/register.cpp
        GCDecompiler/ppc/register.h
        GCDecompiler/ppc/symbol.cpp
        GCDecompiler/ppc/symbol.h
        GCDecompiler/enums.h
        GCDecompiler/gcd_main.cpp
        GCDecompiler/gcd_main.h
        GCDecompiler/import.cpp
        GCDecompiler/import.h
        GCDecompiler/ppc_reader.cpp
        GCDecompiler/ppc_reader.h
        GCDecompiler/relocation.cpp
        GCDecompiler/relocation.h
        GCDecompiler/section.cpp
        GCDecompiler/section.h
        GCDecompiler/types.h
        GCDecompiler/utils.cpp
        GCDecompiler/utils.h
        zlib/zconf.h
        zlib/zutil.h
        zlib/zutil.c
        zlib/zlib.h
        zlib/crc32.h
        zlib/crc32.c
        zlib/trees.h
        zlib/trees.c
        zlib/adler32.c
        zlib/deflate.h
        zlib/deflate.c
        zlib/inflate.h
        zlib/inflate.c
        zlib/inftrees.h
        zlib/inftrees.c
        zlib/inffast.h
        zlib/inffast.c
        GCDecompiler/logging.cpp
        GCDecompiler/logging.h GCDecompiler/argparser.cpp GCDecompiler/argparser.h)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "gcd")
target_link_libraries(${PROJECT_NAME} stdc++fs)
