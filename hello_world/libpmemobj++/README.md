## Introduction

This "Hello" example explores the C++ bindings for libpmemobj library such as
the transaction, persistent pointer, and pool to access persistent memory. The
goal is to demonstrate how to create a "Hello Persistent Memory!!!" program
using the these functions to access the persistent memory and then reading back
before displaying it to the stdout. 

## Build Instructions

To build this sample, download/clone the pmdk-examples repository. A Makefile
is provided. 

## How to Run

After building the binary, the code sample can be run with a statement such as the following:

	$ ./hello_libpmemobj++ -w t

Write the (Hello Persistent Memory!!!) string to persistent-memory.

	$ ./hello_libpmemobj++ -r t

Read the (Hello Persistent Memory!!!) string from persistent-memory.

	$ ./hello_libpmemobj++ -r t

Read the (Hello Persistent Memory!!!) string from persistent-memory.

	$
