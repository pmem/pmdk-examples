### Introduction

This sample presents an introduction to the Java API for persistent memory
programming. This API emphasizes persistent collections, since collection
classes seem to map well to the use cases observed for many persistent memory
applications. The sample is comprised of a persistent array of employees,
being an Employee a custom persistent class implemented from scratch.

### Build Instructions

To build this sample, download/clone the [pmdk-examples
repository](https://github.com/pmem/pmdk-examples).  A Makefile is provided. To
compile this sample, just type `make employees` from the root directory of the
repository; C++ and Java compilers need to be properly installed in your
system. The default C++ compiler used is `g++`. You can change that by setting
the `CXX` variable in `common.mk`. Alternatively, you can build all samples by
typing `make`. Please, read the README at the root directory of the repository
for more information.

The build process will compile and use the latest master branch version of PMDK
and PCJ. If you want to use a custom installed version of PMDK, you can change
the `PMDK_EXAMPLE_INCLUDE` and `PMKD_EXAMPLE_LIB_NONDEBUG` variables in
`common.mk` appropriately.

### How to Run

To run do:

	$ make run

