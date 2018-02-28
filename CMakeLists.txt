cmake_minimum_required(VERSION 3.9.0)
project(GCDecompiler)

#Add uninstall target
configure_file("cmake/uninstall.cmake" "cmake/uninstall.cmake" COPYONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "cmake/uninstall.cmake")

#Use C++ 11
set(CMAKE_CXX_STANDARD 11)

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

set(SOURCE_FILES
    ./GCDecompiler/Import.cpp
    ./GCDecompiler/rel_reader.cpp
    ./GCDecompiler/Relocation.cpp
    ./GCDecompiler/Section.cpp
    ./GCDecompiler/utils.cpp
    ./GCDecompiler/ppc_reader.cpp
    )

set(HEADER_FILES
    ./GCDecompiler/Import.h
    ./GCDecompiler/rel_reader.h
    ./GCDecompiler/Relocation.h
    ./GCDecompiler/Section.h
    ./GCDecompiler/utils.h
    ./GCDecompiler/ppc_reader.h
    ./GCDecompiler/types.h
    )

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

