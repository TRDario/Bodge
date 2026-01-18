# Bodge

[![Itch.io](https://img.shields.io/badge/itch.io-%23FF0B34.svg?logo=Itch.io&logoColor=white)](https://trdario.itch.io/bodge)
[![GitHub tag](https://img.shields.io/github/tag/TRDario/Bodge.svg)](https://github.com/TRDario/Bodge/tags/latest)

Source code repository for Bodge.

## Build Instructions

Requires a C++20-compatible compiler and standard library (though a fallback for std::format exists), as well as CMake 3.30 to build.

Tested on and guaranteed to build with Clang(-cl) on Linux and Windows. Probably works with MSVC and GCC too, but I haven't tested them.

A docker setup for a Linux build environment and instructions to run it are provided in /linux.

## Documentation

The source code provides basic documentation for all types, functions, constants and globals in the program.

Additional information about the structure of the program is provided in some of the headers (more may be added in the future).

Bodge uses [tr](https://github.com/TRDario/tr/tree/fc6aaceee275443aa1223a50563664127d90ff84) as the underlying framework.
The version of tr used for Bodge (1.1) is not as extensively documented as the current version, but should have at least rudimentary notes about almost everything.

## Licence

All assets and code are under the [![license](https://img.shields.io/github/license/TRDario/Bodge.svg?)](https://github.com/TRDario/Bodge/blob/master/LICENSE.md)
