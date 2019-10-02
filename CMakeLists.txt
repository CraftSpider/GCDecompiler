cmake_minimum_required(VERSION 3.10)
project(GCDecompiler)

#Add uninstall target
configure_file("cmake/uninstall.cmake" "cmake/uninstall.cmake" COPYONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "cmake/uninstall.cmake")

#Use C++ 17
set(CMAKE_CXX_STANDARD 17)

#Export compile commands for editor autocomplete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#Be really pedantic! Also make sure we use unsigned chars
if(MSVC)
    add_definitions(-W4 -J)
else()
    add_definitions(-Wall -Wextra -pedantic -funsigned-char)
endif()

#Show as an executable, not a shared library in file managers
if(UNIX)
    #-nopie is unused with AppleClang
    if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
    endif(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
endif(UNIX)

include_directories(include)
include_directories(libs/include)

set(HEADERS
        include/gcd_main.h
        include/enums.h
        include/import.h
        include/relocation.h
        include/section.h
        include/datatypes/color.h
        include/datatypes/image.h
        include/filetypes/dol.h
        include/filetypes/lz.h
        include/filetypes/rel.h
        include/filetypes/tpl.h
        include/filetypes/png.h
        include/filetypes/imagetype.h
        include/filetypes/stagedef.h
        include/ppc/ppc_reader.h
        include/ppc/codes.h
        include/ppc/instruction.h
        include/ppc/register.h
        include/ppc/symbol.h
        include/ppc/disassembler.h include/ppc/decompiler.h include/datatypes/stageobjects.h)

set(TEMPLATES
        include/filetypes/imagetype.tpp)

set(SOURCES
        src/import.cpp
        src/relocation.cpp
        src/section.cpp
        src/datatypes/color.cpp
        src/datatypes/image.cpp
        src/filetypes/dol.cpp
        src/filetypes/lz.cpp
        src/filetypes/rel.cpp
        src/filetypes/tpl.cpp
        src/filetypes/png.cpp
        src/filetypes/imagetype.cpp
        src/filetypes/stagedef.cpp
        src/ppc/ppc_reader.cpp
        src/ppc/instruction.cpp
        src/ppc/register.cpp
        src/ppc/symbol.cpp
        src/ppc/disassembler.cpp src/ppc/decompiler.cpp src/datatypes/stageobjects.cpp)

set(TEST_HEADERS
        test/datatypes/test_color.h
        test/ppc/test_instructions.h
        test/ppc/test_registers.h
        test/ppc/test_symbols.h test/filetypes/test_tpl.h test/filetypes/test_png.h)

set(TEST_SOURCES
        test/main.cpp
        test/datatypes/test_color.cpp
        test/ppc/test_instructions.cpp
        test/ppc/test_registers.cpp
        test/ppc/test_symbols.cpp test/filetypes/test_tpl.cpp test/filetypes/test_png.cpp)

set(ALL_CODE ${HEADERS} ${TEMPLATES} ${SOURCES})

set(ALL_TEST ${TEST_HEADERS} ${TEST_SOURCES})

add_executable(${PROJECT_NAME} ${ALL_CODE} src/gcd_main.cpp)
add_executable(test_${PROJECT_NAME} ${ALL_CODE} ${ALL_TEST})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "gcd")
set_target_properties(test_${PROJECT_NAME} PROPERTIES OUTPUT_NAME "test_gcd")

# Add various target link libraries

if(WIN32 OR CYGWIN)
    file(GLOB files "${CMAKE_SOURCE_DIR}/libs/*.lib")
else()
    file(GLOB files "${CMAKE_SOURCE_DIR}/libs/*.a")
endif()

set(TARGET_LINKS ${files})
if(NOT MSVC)
    list(APPEND TARGET_LINKS stdc++fs)
endif()

foreach(link ${TARGET_LINKS})
    message(STATUS "Linking to library " ${link})
    target_link_libraries(${PROJECT_NAME} ${link})
    target_link_libraries(test_${PROJECT_NAME} ${link})
endforeach()
