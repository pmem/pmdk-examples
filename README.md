# PROJECT NOT UNDER ACTIVE MANAGEMENT #  
This project will no longer be maintained by Intel.  
This project has been identified as having known security escapes.  
Intel has ceased development and contributions including, but not limited to, maintenance, bug fixes, new releases, or updates, to this project.  
Intel no longer accepts patches to this project.  
  
### Introduction

The goal of this repository is to present clear programming examples of how to
use the core libraries of the Persistent Memory Development Kit (PMDK) and the
C++ and Java bindings of libpmemobj (core library which is part of PMDK).

### Build Instructions

For compilation, a Makefile is provided.  To build all samples, just type `make`. 
The following are the needed dependencies:

 - C and C++ compilers. The default C compiler is `gcc`, and the default C++
   compiler used is `g++`. You can change that for individual samples by
   setting the `CC` and `CXX` variables in the sample's Makefile.
 - PMDK. Please, read the instructions in the 
   [PMDK repository](https://github.com/pmem/pmdk).
 - To build and run the samples using the Java bindings, you will need Java 8
   or above (including a Java compiler).

NOTE: pmem-mariadb will not be built automatically. Please, read instructions
on how to build in `pmem-mariadb/INSTALL-SOURCE`

Build everything doing:

	$ make

