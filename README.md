# Bodge

Source code repository for Bodge.

If you are looking for a download link for the game itself, it is here:
https://trdario.itch.io/bodge

## Build Instructions

Requires a C++20-compatible compiler and standard library (though a fallback for std::format exists), as well as CMake 3.30 to build.

Tested on and guaranteed to build with Clang(-cl) on Linux and Windows. Probably works with MSVC and GCC too, but I haven't tested them.

A docker setup for a Linux build environment and instructions to run it are provided in /linux.
