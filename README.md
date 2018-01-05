### Introduction

The goal of this repository is to present clear programming examples of
how to use the C++ bindings of libpmemobj, which is a core library of the
Persistent Memory Development Kit (PMDK).

### Build Instructions

For compilation, a Makefile is provided.  To build all samples just type
`make`. The following are the needed dependencies:

 - A C++ compiler. The default C++ compiler used is `g++`. You can change that
   by setting the `CXX` variable in `common.mk`.
 - The Boost C++ Library Collection (http://boost.org).

Alternatively, you can build single samples by typing `make <sample_name>`. For
example, `make mapreduce`.

The build process will compile and use the latest master branch version of
PMDK. If you want to use a custom installed version of PMDK, you can change the
`CXXFLAGS` and `LDFLAGS` variables in `common.mk` appropriately.

For more information, please read the READMEs for each sample.
