# libtoolchain - Toolchain Development Library
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)
![Language](https://img.shields.io/badge/langauge-c++11-blue.svg)
![Platform](https://img.shields.io/badge/platform-linux:%20x86__64,%20i386%20%7C%20win:%20x86__64,%20i386%20%7C%20macos:%20x86__64,%20arm64-lightgrey.svg)

![Version](https://img.shields.io/badge/version-0.5.0%20%7C%20prerelease-green.svg)

Library to ease the development of toolchain applications.

This library helps with the busy work that is common to many toolchain development projects including:
* Cross platform support (macOS/Windows/GNU)
* Unicode entry-point (`umain()`)
* Command-line option processing
* Formating binary data for command-line output
* Cross platform FileSystem IO with large file support for both 32bit & 64bit targets and unicode path support
* String transcoding (UTF-8/UTF-16/UTF-32)
* Extensible abstractions for generating and processing binary data
* Properly integrated wrappers for Cryptographic Algorithms (AES, RSA, SHA, HMAC, PBKDF1/PBKDF2, PRBG)

Planned features:
* Serialisation of human readable formats (XML, JSON, YAML, INI, CSV, etc)
* Properly integrated wrappers for Cryptographic Algorithms (Eliptic Curve, etc)
* Properly integrated wrappers for Compression Algorithms (LZ4, etc)


# File tree
* `bin/` - Compiled binaries
* `build/visualstudio/` - Visual Studio Project files
* `docs/` - Doxygen Generated API Documentation
* `include/` - Public headers
* `src/` - Library source code & private headers
* `test/` - Test program source code
* `Makefile` - Root makefile (GNU/Unix build system file)
* `LICENCE` - Distribution License 
* `Doxyfile` -  Doxygen config

# Building
For GNU/unix systems `make` can be used. For native Windows, Visual Studio project files are provided.

See more [here](./BUILDING.md).

# Documentation
The documentation is available at docs/index.html. Or alternatively at https://jakcron.github.io/libtoolchain.

# License 
This source code is made available under the [MIT license](./LICENSE).
