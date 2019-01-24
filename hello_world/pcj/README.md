## Introduction

This "Hello" example explores the persistent collections for Java objects (PCJ)
stored in persistent memory such as the ObjectDirectory, String, and
ByteBuffer. The Java library uses the libpmemobj library from the PMDK.  The
goal is to demonstrate how to create a "Hello Persistent Memory!!!" program
using the these functions to access the persistent memory and then reading back
before displaying it to the c. 

## Build Instructions

To build this sample, download/clone the pmdk-examples repository. A Makefile
is provided. To compile this sample, just type make from this location; an
Oracle Java version 8 or above needs to be properly installed in your system.
The default java compiler used is javac. Please, read the README at the root
directory of the repository for more information.

If you need to install PCJ, please refer to PCJ on GitHub on how to install PCJ
(https://github.com/pmem/pcj). 

## PREREQUISITES TO RUN

This library assumes the availability of hardware persistent memory or emulated
persistent memory. Properties of this memory such as path and size can be
specified in the "config.properties" file at the top level directory. Further
information can be found inside the file.

Set the environment variables for compilation:

	export LD_LIBRARY_PATH=$(echo $HOME)/github/pmdk-examples/lib/pcj/src/main/java:$LD_LIBRARY_PATH

	export JAVA_HOME=/opt/jdk
	export PATH=$JAVA_HOME/bin:$PATH

## How to compile

	$ make

Notes: manually compile code --

	$ javac  -cp .:/home/thai/github/pmdk-examples/lib/pcj/target/classes  HelloW.java

## How to Run

After building the binary, the code sample can be run with a statement such as the following:

	$ make run
	java  -cp .:/home/thai/github/pmdk-examples/lib/pcj/target/classes -Djava.library.path=/home/thai/github/pmdk-examples/lib/pcj/target/cppbuild Hello_pcj
	Opening heap... Cleaning up heap... Heap opened.

	Enter <W> for Write to persistent-memory, <R> for Read from persistent-memory or <Q> to Quit: W

	Write the (Hello Persistent Memory!!!) string to persistent-memory.

	$ make run
	java  -cp .:/home/thai/github/pmdk-examples/lib/pcj/target/classes -Djava.library.path=/home/thai/github/pmdk-examples/lib/pcj/target/cppbuild Hello_pcj
	Opening heap... Cleaning up heap... Heap opened.

	Enter <W> for Write to persistent-memory, <R> for Read from persistent-memory or <Q> to Quit: R

	Read the (Hello Persistent Memory!!!) string from persistent-memory.

	$ make run
	java  -cp .:/home/thai/github/pmdk-examples/lib/pcj/target/classes -Djava.library.path=/home/thai/github/pmdk-examples/lib/pcj/target/cppbuild Hello_pcj
	Opening heap... Cleaning up heap... Heap opened.

	Enter <W> for Write to persistent-memory, <R> for Read from persistent-memory or <Q> to Quit: Q
	$ 
