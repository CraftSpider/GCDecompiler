@echo off
setlocal enableextensions enabledelayedexpansion

SET SRC_DIR=%cd%

:: Because batch is a stupid language
GOTO MAIN

:cmake_build
    IF "%~3"=="" (
        ECHO Cloning project %~2
        git clone --single-branch --depth 1 https://github.com/%~1/%~2 "libs/%~2"
    ) ELSE (
        ECHO Cloning branch %~3 of project %~2
        git clone --single-branch --depth 1 --branch "%~3" https://github.com/%~1/%~2 "libs/%~2"
    )

    cd "libs/%~2"
    ECHO Running CMake for %~2
    cmake -G"Visual Studio 15 2017" .
    msbuild "%~4.vcxproj"
    cd %SRC_DIR%
EXIT /B 0

:copy_headers
    ECHO Copying '%~1' to '%~2'
    FOR %%x IN (.h .tpp) DO (
        xcopy "libs\%~1\*%%x" "libs\%~2" /SY
    )
    FOR %%x IN (at_) DO (
        xcopy "libs\%~1\%%x*" "libs\%~2" /SY
    )
EXIT /B 0

:copy_lib
    ECHO Copying library %~1
    FOR /R "libs\%~1" %%x IN (*.a *.lib) do (copy "%%x" "libs\")
EXIT /B 0

:cmake_dep
    mkdir "libs/%~2"

    CALL :cmake_build "%~1", "%~2", "%~4", "%~5"
    CALL :copy_headers "%~2", "%~3"
    CALL :copy_lib "%~2"

    :: rd /S /Q "libs/%~2"
EXIT /B 0

:MAIN
mkdir "libs/include"

CALL :cmake_dep "madler", "zlib", "include", "develop", "zlibstatic"
CALL :cmake_dep "craftspider", "alphatools", "", "", "alpha_tools"
