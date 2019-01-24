### Introduction

This sample presents an introduction to the Java API for persistent memory
programming. This API emphasizes persistent collections, since collection
classes seem to map well to the use cases observed for many persistent memory
applications. The sample is comprised of a persistent array of employees,
being an Employee a custom persistent class implemented from scratch.

### Build Instructions

To build this sample, download/clone the [pmdk-examples
repository](https://github.com/pmem/pmdk-examples).  A Makefile is provided. 

### How to Run

In order to pass to the library the location of the pool file and its size, you
need to modify the configuration file `config.properties` (which needs to
reside on the current working directory). A sample is provided, which creates
the pool on the path `/mnt/mem/persistent_heap` with size 2GiB. Please, edit
the file to adapt it to your system:

	$ cat config.properties
	path=/mnt/mem/persistent_heap
	size=2147483648
	$

To run do:

	$ make run

