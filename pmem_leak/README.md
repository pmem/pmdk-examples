### Introduction

Bugs in programs that dynamically allocate and deallocate objects have the
potential to leak memory. Persistent memory (PMEM) code is no different. While
in volatile programs memory leaking is mainly thought of as a problem related
to resource exhaustion and performance degradation, in PMEM there is the need
to also think about data corruption and loss.  This example presents a simple
C++ PMEM code with an obvious leaking bug, and a fixer program showing how you
can use the internal list of allocated objects in libpmemobj to recover leaked
objects (through the C API of the library). 


### Build Instructions

To build this sample, download/clone the [pmdk-examples
repository](https://github.com/pmem/pmdk-examples).  A Makefile is provided. To
compile this sample, just type `make pmem_leak` from the root directory of
the repository; a C++ compiler and the C++ Boost Library (http://boost.org. In
ubuntu 16.04 you can install the package `libboost-all-dev`) needs to be
properly installed in your system. The default C++ compiler used is `g++`. You
can change that by setting the `CXX` variable in `common.mk`. Alternatively,
you can build all samples by typing `make`.

The build process will compile and use the latest master branch version of PMDK
(libpmemobj is part of PMDK). If you want to use a custom installed version of
PMDK, you can change the `CXXFLAGS` and `LDFLAGS` variables in `common.mk`
appropriately.

### How to Run

There are four programs: `normal`, `leaker`, `checker`, and `fixer`. All
programs expect one parameter: the file location for the PMEM pool.


	$ ./<program> <pool_file>

This is what every program does:

`normal`: Creates a healthy data structure with 64 objects on it.

`leaker`: Creates a corrupted data structure with 33 objects on it but of
"size" 64 (31 objects are leaked).

`checker`: Checks the data structure and output the IDs of missing objects (if
any).

`fixer`: Fixes the data structure by inserting back the leaked objects.


