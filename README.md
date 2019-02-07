# GCDecompiler

[![Build status](https://ci.appveyor.com/api/projects/status/doibev44ije1i8l9?svg=true)](https://ci.appveyor.com/project/CraftSpider/gcdecompiler)

The GameCube Decompiler (GCD) is a project to create software capable of taking in GameCube games,
and spitting out human readable assembly and data about them.

## Usage

### Requirements

- CMake 3.10 or later
- Make
- Unix:
  - GCC or similar compiler
- Windows:
  - Cygwin or WSL
  - GCC or similar compiler

### Install/Build

GCD is designed to build and import its own dependencies, simply run the `setup.sh` file in the root directory and the project will set itself up. Note that only Native Unix, Cygwin, and WSL have been formally tested. If there are problems on any other system, please report it in issues.

Once the project has been set up, simply run `make` to compile the program. By default this will compile both the gcd and test_gcd executables. gcd is the command line tool, test_gcd runs the built-in tests.

### Functionality

Though decompilation is still in an alpha state, disassembly of `.rel` and `.dol` files should work reliably. Also,
the GCD can handle TPL image files, unpacking and repacking them in a folder as PNGs. Run `gcd --help` for a
comprehensive list of available commands.
