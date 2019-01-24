## Introduction

This "Hello" example explores the Low Level Persistent Library (LLPL) stored in
persistent memory such using MemoryBlocks to allocate from a persistent Heap.
The Java library uses the libpmemobj library from the PMDK.  The goal is to
demonstrate how to create a "Hello Persistent Memory!!!" program using the
these functions to access the persistent memory and then reading back before
displaying it to the stdout. 

## Build Instructions

To build this sample, download/clone the pmdk-examples repository. A Makefile
is provided. To compile this sample, just type make from this location; an
Oracle Java version 8 or above needs to be properly installed in your system.
The default java compiler used is javac. Please, read the README at the root
directory of the repository for more information.

If you need to install LLPL, please refer to LLPL on GitHub on how to install
(https://github.com/pmem/llpl). 

## PREREQUISITES TO RUN

This library assumes the availability of hardware persistent memory or emulated
persistent memory. 

Set the environment variables for compilation:

	export JAVA_HOME=/opt/jdk
	export PATH=$JAVA_HOME/bin:$PATH

## How to compile

	$ javac  -cp .:/home/thai/github/llpl/target/classes  Hello\_llpl.java

2nd option on how to compile

	$ make

## How to Run

After building the binary, the code sample can be run with a statement such as
the following:

	$ java  -cp .:/home/thai/github/llpl/target/classes -Djava.library.path=/home/thai/github/llpl/target/cppbuild Hello\_llpl

2nd option on how to run:

	$ make run

Below is an example of the result output:

	Hello Persistent Memory!!!
