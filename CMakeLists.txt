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

include_directories(src)
include_directories(src/filetypes)
include_directories(src/ppc)
include_directories(zlib)
include_directories(libs/include)

set(ZLIB
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
        zlib/inffast.c)

set(HEADERS
        src/gcd_main.h
        src/enums.h
        src/import.h
        src/relocation.h
        src/section.h
        src/filetypes/dol.h
        src/filetypes/lz.h
        src/filetypes/rel.h
        src/filetypes/tpl.h
        src/filetypes/png.h
        src/filetypes/image.h
        src/filetypes/stagedef.h
        src/ppc/ppc_reader.h
        src/ppc/codes.h
        src/ppc/instruction.h
        src/ppc/register.h
        src/ppc/symbol.h)

set(SOURCES
        src/import.cpp
        src/relocation.cpp
        src/section.cpp
        src/filetypes/dol.cpp
        src/filetypes/lz.cpp
        src/filetypes/rel.cpp
        src/filetypes/tpl.cpp
        src/filetypes/png.cpp
        src/filetypes/image.cpp
        src/filetypes/stagedef.cpp
        src/ppc/ppc_reader.cpp
        src/ppc/instruction.cpp
        src/ppc/register.cpp
        src/ppc/symbol.cpp)

set(TEST_HEADERS
        test/test_instructions.h)

set(TEST_SOURCES
        test/main.cpp
        test/test_instructions.cpp)

set(ALL_CODE ${ZLIB} ${HEADERS} ${SOURCES})

set(ALL_TEST ${TEST_HEADERS} ${TEST_SOURCES})

add_executable(${PROJECT_NAME} ${ALL_CODE} src/gcd_main.cpp)

add_executable(test_${PROJECT_NAME} ${ALL_CODE} ${ALL_TEST})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "gcd")

target_link_libraries(${PROJECT_NAME} stdc++fs)
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/libalpha_tools.a)

target_link_libraries(test_${PROJECT_NAME} stdc++fs)
target_link_libraries(test_${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libs/libalpha_tools.a)
