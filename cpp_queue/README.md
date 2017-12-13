### Introduction

This is an implementation of a very simple queue with a singly linked list as
the backing data structure. This example contains two implementations,
a normal, volatile queue and a persistent memory resident version with an
identical API. The purpose of this example is to show what kind of effort is
needed to convert existing applications. **NOTE:** this implementation in not
optimal, it is meant to be an illustration not a robust, high performing
implementation. 

### Build Instructions

There is a Makefile provided by the sample. To compile the sample, just type
`make`; NVML (libpmemobj is part of NVML) needs to be properly installed in
your system, as well as a C++ compiler. The default C++ compiler used is `g++`.
You can change that by setting the `CXX` variable in the Makefile.

If compilation fails because pkg-config cannot find the configuration files
for NVML, you may need to set the `PKG_CONFIG_PATH` variable before doing make.
If you installed NVML with the default configuration, these files will probably
be either in `/usr/local/lib/pkgconfig` or `/usr/local/lib64/pkgconfig`.

```bash
$ export PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig:$PKG_CONFIG_PATH
```

There is also a rudimentary CMakeLists.txt file provided for convenience.

### How to Run

After compilation, you can run the program without parameters to get usage help:

```
$ ./cpp_queue
usage: ./cpp_queue [v | p] file-name [push [value]|pop|show]
```

The `[v | p]` stands for *volatile*/*persistent*. The binary contains both the
volatile and persistent implementation. The contents of the volatile version
do not however survive between application runs. It is however provided should
the user wish to modify the implementation to experiment.

To run the persistent queue and push some initial value into it execute:

```
$ ./cpp_queue p /mnt/pmem/pqueue.pool push 1
```

This will create a pqueue.pool file in the /mnt/pmem directory and push the value
1 into the queue. This can be repeated a number of times to populate the queue with
data.

To print the contents of the queue run:

```
$ ./cpp_queue p /mnt/pmem/pqueue.pool show
1
12
123
1234
12345
123456
1234567
12345678
123456789
1
```

To pop the first value from the queue:

```
$ ./cpp_queue p /mnt/pmem/pqueue.pool pop
1
```

### Final Note

The reader is encouraged to play with the implementation and check different scenarios
like inserting values in a loop, randomly crashing the application and checking whether
it is possible to print the queue.