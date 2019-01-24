## Introduction

This "Hello" example explores the low level pmem-resident array of blocks in
the libpmemblk library.  These blocks are updated atomically with respect to
power failure or program interruption without losing the data.   The goal is to
demonstrate how to create a "Hello Persistent Memory!!!" program using the
these functions to access the persistent memory and then reading back before
displaying it to the stdout. 

## Build Instructions

To build this sample, download/clone the pmdk-examples repository. A Makefile
is provided. 

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
