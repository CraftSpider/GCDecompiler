@echo off
setlocal enableextensions enabledelayedexpansion

SET VS_LOC=""
SET VS_VER=0
SET VS_YEAR=""

IF %VS_LOC%=="" (
    SET VS_LOC="%VS150COMNTOOLS%"
    SET VS_VER=15
    SET VS_YEAR=2017
)
IF %VS_LOC%=="" (
    SET VS_LOC="%VS140COMNTOOLS%"
    SET VS_VER=14
    SET VS_YEAR=2019
)

SET NUM=16
FOR %%Y in (2019 2017) DO (
    SET _DIR="C:\Program Files (x86)\Microsoft Visual Studio\%%Y\Community\Common7\Tools"
    IF exist !_DIR! (
        SET VS_LOC=!_DIR!
        SET VS_VER=!NUM!
        SET VS_YEAR=%%Y
        GOTO :loop_end
    )
    SET /a NUM -= 1
)
:loop_end

IF %VS_LOC%=="" (
ECHO Couldn't locate VS tooling
EXIT 1
)

CALL %VS_LOC%"\VsDevCmd.bat"
IF %errorlevel% neq 0 (
    ECHO Couldn't setup MSBuild
    EXIT %errorlevel%
)
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
    :: TODO: support x86 architecture?
    cmake -G"Visual Studio %VS_VER% %VS_YEAR%" -A x64 .
    msbuild "%~4.vcxproj"
    IF %errorlevel% neq 0 (
        ECHO MSbuild command failed
        EXIT %errorlevel%
    )
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
    FOR /R "libs\%~1" %%x IN (*.a *.lib *.pdb) do (copy "%%x" "libs\")
EXIT /B 0

:cmake_dep
    mkdir "libs/%~2"

    CALL :cmake_build "%~1", "%~2", "%~4", "%~5"
    CALL :copy_headers "%~2", "%~3"
    CALL :copy_lib "%~2"

    rd /S /Q "libs/%~2"
EXIT /B 0

:MAIN
mkdir "libs/include"

CALL :cmake_dep "madler", "zlib", "include", "develop", "zlibstatic"
CALL :cmake_dep "craftspider", "alphatools", "", "", "alpha_tools"
