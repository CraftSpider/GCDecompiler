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

include_directories(include)
include_directories(include/filetypes)
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
        include/gcd_main.h
        include/enums.h
        include/import.h
        include/relocation.h
        include/section.h
        include/filetypes/dol.h
        include/filetypes/lz.h
        include/filetypes/rel.h
        include/filetypes/tpl.h
        include/filetypes/png.h
        include/filetypes/image.h
        include/filetypes/stagedef.h
        include/ppc/ppc_reader.h
        include/ppc/codes.h
        include/ppc/instruction.h
        include/ppc/register.h
        include/ppc/symbol.h)

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
        test/ppc/test_instructions.h
        test/ppc/test_registers.h
        test/ppc/test_symbols.h)

set(TEST_SOURCES
        test/main.cpp
        test/ppc/test_instructions.cpp
        test/ppc/test_registers.cpp
        test/ppc/test_symbols.cpp)

set(ALL_CODE ${ZLIB} ${HEADERS} ${SOURCES})

set(ALL_TEST ${TEST_HEADERS} ${TEST_SOURCES})

add_executable(${PROJECT_NAME} ${ALL_CODE} src/gcd_main.cpp)

add_executable(test_${PROJECT_NAME} ${ALL_CODE} ${ALL_TEST})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "gcd")

# Add various target link libraries

file(GLOB files "${CMAKE_SOURCE_DIR}/libs/*.a")
set(TARGET_LINKS stdc++fs ${files})

foreach(link ${TARGET_LINKS})
    target_link_libraries(${PROJECT_NAME} ${link})
    target_link_libraries(test_${PROJECT_NAME} ${link})
endforeach()
