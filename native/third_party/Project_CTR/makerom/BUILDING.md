# Building
## Linux (incl. Windows Subsystem for Linux) & MacOS & Windows (MinGW)- Makefile
### Requirements
* `make`
* Terminal access
* Typical GNU compatible development tools (e.g. `clang`, `gcc`, `ar` etc) with __C11__ support

### Using Makefile
* `make` (default) - Compile program
	* Compiling the program requires local dependencies to be compiled via `make deps` beforehand
* `make clean` - Remove executable and object files
* `make deps` - Compile locally included dependency libraries
* `make clean_deps` - Remove compiled library binaries and object files