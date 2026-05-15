# Building
## Linux (incl. Windows Subsystem for Linux) & MacOS - Makefile
### Requirements
* `make`
* Terminal access
* Typical GNU compatible development tools (e.g. `clang`, `g++`, `c++`, `ar` etc) with __C++11__ support

### Using Makefile
* `make` (default) - Compile library
* `make static_lib` - Compile library
* `make clean` - Remove all object files

## Native Win32 - Visual Studio
### Requirements
* [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) 2015 or 2017

### Compiling Library
* Open `build/visualstudio/libmbedtls.sln` in Visual Studio
* Select Target (e.g `Debug`|`Release` & `x86`|`x64`)
* Navigate to `Build`->`Build Solution`

### Including libmbedtls in another VS Solution for static linking
* Clone `libmbedtls` as a submodule into your project
* Navigate to the `Solution Explorer` window
* Right-click on the Solution Item and select `Add`->`Existing Project...`
* In the filesystem popup window open `<libmbedtls location>\build\visualstudio\libmbedtls\libmbedtls.vcxproj`
* Update each dependant project's `References` to include libmbedtls
* Update each dependant project's `Property Pages` so that for `All Configurations` and `All Platforms` the `Addition Include Directories` has the relative path to `<libmbedtls location>\include`
	* If `libmbedtls` is being included as a dependency in a similarly structured project the relative path is `$(SolutionDir)..\..\deps\libmbedtls\include`
* Update the `Project Build Order` so libmbedtls is built before any of its dependants
* Update the `Project Dependencies` so that each dependant has the box checked for libmbedtls
