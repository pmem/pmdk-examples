## Introduction

This "Hello" example explores the low level persistent memory support of the
libpmem library such support for the persistent memory instructions for
flushing changes to pmem is provided. The goal is to demonstrate how to create
a "Hello Persistent Memory!!!" program using the these low level functions to
access the persistent memory and then reading back before display it to the
stdout.  

## Build Instructions

To build this sample, download/clone the pmdk-examples repository. A Makefile
is provided. To compile this sample, just type make from this location.

## How to Run

After building the binary, the code sample can be run with a statement such as the following:

	$ ./manpage test 

Below is an example of the result output:

	$ ./hello_libpmem -w t

Write the (Hello Persistent Memory!!!) string to persistent memory.

	$ ./hello_libpmem -r t

Read the (Hello Persistent Memory!!!) string from persistent memory.

	$ ./hello_libpmem -rw t

	Usage: ./hello_libpmem <-w/-r> <filename>
	$
