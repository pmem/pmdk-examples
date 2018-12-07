## Introduction

This "Hello" example explores the low level pmem-resident array of blocks in
the libpmemblk library.  These blocks are updated atomically with respect to
power failure or program interruption without losing the data.   The goal is to
demonstrate how to create a "Hello Persistent Memory!!!" program using the
these functions to access the persistent memory and then reading back before
displaying it to the stdout. 

## Build Instructions

To build this sample, download/clone the pmdk-examples repository. A Makefile
is provided. To compile this sample, just type make from this location; a C
compiler needs to be properly installed in your system. The default C compiler
used is gcc. You can change that by setting the CC variable in common.mk.
Alternatively, you can build all samples by typing make. Please, read the
README at the root directory of the repository for more information.

The build process will compile and use the latest master branch version of PMDK
(libpmem is part of PMDK). If you want to use a custom installed version of
PMDK, you can change the PMDK_EXAMPLES_INCLUDE and
PMDK_EXAMPLES_LIB_NONDEBUG variables in common.mk appropriately.

## How to build

	$ make

## How to Run

After building the binary, the code sample can be run with a statement such as
the following:

	$./manpage test 

Below is an example of the result output:

	$ ./hello_libpmemblk -w t

Write the (Hello Persistent Memory!!!) string to persistent memory.

	$ ./hello_libpmemblk -r t

Read the (Hello Persistent Memory!!!) string from persistent memory.

	$ 
