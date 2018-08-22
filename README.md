# GCDecompiler

The GameCube Decompiler (GCD) is a project to create software capable of taking in GameCube games,
and spitting out human readable assembly and data about them.

## Usage

### Install/Build

GCD relies on the zlib project for some of its functionality. zlib is already configured as a submodule in git, so
after cloning the project just run `git submodule update --init` to clone it into the right path.

The GCD project uses CMake to handle its builds. Once the project has been set up, run `cmake .` to create build
files. By default, on Windows this will take the form of Visual Studio project files, and on Unix this will be a
Makefile.

### Functionality

Though decompilation is still in an alpha state, disassembly of `.rel` and `.dol` files should work reliably. Also,
the GCD can handle TPL image files, unpacking and repacking them in a folder as PNGs. Run `gcd --help` for a
comprehensive list of available commands.
