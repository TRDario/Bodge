# Bodge

[![Itch.io](https://img.shields.io/badge/itch.io-%23FF0B34.svg?logo=Itch.io&logoColor=white)](https://trdario.itch.io/bodge)
[![GitHub tag](https://img.shields.io/github/tag/TRDario/Bodge.svg?color=red)](https://github.com/TRDario/Bodge/tags)

Source code repository for Bodge.

## Build Instructions

Requires a C++20-compatible compiler and standard library (though a fallback for std::format exists), as well as CMake 3.30 to build.

Tested on and guaranteed to build with Clang(-cl) on Linux and Windows. Probably works with MSVC and GCC too, but I haven't tested them.

A docker setup for a Linux build environment and instructions to run it are provided in /linux.

## Documentation

The source code provides basic documentation for all types, functions, constants and globals in the program.

Additional information about the structure of the program is provided in some of the headers (more may be added in the future).

Bodge uses [tr](https://github.com/TRDario/tr/tree/bodge) as the underlying framework.
The version of tr used for Bodge (1.1) is not as extensively documented as the current version,
but should have at least rudimentary notes about almost everything API-wise.

## Licence

[Charge Vector](https://github.com/TRDario/Bodge/blob/main/data/fonts/charge_vector_b.otf)
is licensed under a non-commercial license.

[Linux Biolinum](https://github.com/TRDario/Bodge/blob/main/data/fonts/linux_biolinum_rb.ttf)
is licensed under the SIL Open Font License (OFL).

All other assets and code are licensed under the [Apache License 2.0](https://github.com/TRDario/Bodge/blob/main/LICENSE.md).

See [copyright.txt](https://github.com/TRDario/Bodge/blob/main/metadata/copyright.txt) for more details, as well as copies of above licenses.