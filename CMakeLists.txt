cmake_minimum_required(VERSION 3.10)
project(GCDecompiler)

include(AlphaTest.cmake)

#Add uninstall target
configure_file("cmake/uninstall.cmake" "cmake/uninstall.cmake" COPYONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "cmake/uninstall.cmake")

#Use C++ 17
set(CMAKE_CXX_STANDARD 17)

#Export compile commands for editor autocomplete
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#Be really pedantic!
if(MSVC)
    add_definitions(-W4)
else()
    add_definitions(-Wall -Wextra -pedantic)
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

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.h)
file(GLOB_RECURSE TEMPLATES CONFIGURE_DEPENDS include/*.tpp)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

file(GLOB_RECURSE TEST_HEADERS CONFIGURE_DEPENDS tests/*.h)
file(GLOB_RECURSE TEST_TEMPLATES CONFIGURE_DEPENDS tests/*.tpp)
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)

set(ALL_CODE ${HEADERS} ${TEMPLATES} ${SOURCES})
set(ALL_TEST ${TEST_HEADERS} ${TEST_TEMPLATES} ${TEST_SOURCES})

list(REMOVE_ITEM ALL_CODE ${CMAKE_SOURCE_DIR}/src/gcd_main.cpp)

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

copy_test_resources(test_${PROJECT_NAME})
