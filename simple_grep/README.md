### Introduction

This example shows how to transform a simple C++ program -- in this case a
simplified version of the famous UNIX command-line utility grep -- in order to
take advantage of persistent memory (PMEM). You can find the volatile,
unmodified version of the program under the subdirectory `grep/`. The
persistent memory version, located under `pmemgrep/`, adds a persistent cache
of search results. Caching improves grep by (1) adding fault tolerance
capabilities and (2) speeding up queries for already seen search patterns.
Finally, parallelism (at file granularity) is added using threads and PMEM
aware synchronization. This version is located under `pmemgrep_thx/`.

### Build Instructions

To build this sample, download/clone the [pmdk-examples
repository](https://github.com/pmem/pmdk-examples).  A Makefile is provided. To
compile this sample, just type `make` from the root directory of
the repository.

### How to Run

After compilation, you can run the simple volatile grep like this:

	$ cd simple_grep/grep
	$ ./grep int grep.cpp
	FILE = grep.cpp
	44: int
	54:     int ret = 0;
	77: int
	100: int
	115: int
	135: int
	136: main (int argc, char *argv[])

As you can see, grep finds 7 lines with the word "`int`" on it (lines 44, 54,
77, 100, 115, 135, and 136).

Let's run now the persistent version with with two patterns: "`int`" and
"`void`".  This assumes that a PMEM device (real or emulated using RAM) is
mounter at `/mnt/mem`:

	$ cd simple_grep/pmemgrep
	$ ./pmemgrep /mnt/mem/grep.pool int pmemgrep.cpp
	$ ./pmemgrep /mnt/mem/grep.pool void pmemgrep.cpp
	$

If we run the program without parameters, we get the cached patterns:

	$ ./pmemgrep /mnt/mem/grep.pool
	2 PATTERNS PROCESSED
	void
	int

When passing a pattern we get the actual cached results:

	$ ./pmemgrep /mnt/mem/grep.pool void
	PATTERN = void
        	1 file(s) scanned
	###############
	FILE = pmemgrep.cpp
	###############
	*** pattern present in 15 lines ***
	80:     get_name (void)
	86:     get_nlines (void)
	98:     get_next (void)
	103:    void
	110:    get_mtime (void)
	115:    void
	121:    void
	170:    void
	207:    get_next (void)
	212:    void
	219:    get_str (void)
	254:    void
	255:    print (void)
	326:    void
	327:    print_patterns (void)
	$
	$ ./pmemgrep /mnt/mem/grep.pool int
	PATTERN = int
        	1 file(s) scanned
	###############
	FILE = pmemgrep.cpp
	###############
	*** pattern present in 14 lines ***
	137:    int
	147:            int ret = 0;
	255:    print (void)
	327:    print_patterns (void)
	337: int
	356: int
	381: int
	395: int
	416: int
	417: main (int argc, char *argv[])
	436:    if (argc == 2) /* No pattern is provided. Print stored patterns and exit
	438:            proot->print_patterns ();
	444:            if (argc == 3) /* No input is provided. Print data and exit */
	445:                    p->print ();
	$

Of course, we can keep adding files to existing patterns:

	$ ./pmemgrep /mnt/mem/grep.pool void Makefile
	$ ./pmemgrep /mnt/mem/grep.pool void
	PATTERN = void
        	2 file(s) scanned
	###############
	FILE = Makefile
	###############
	*** pattern present in 0 lines ***
	###############
	FILE = pmemgrep.cpp
	###############
	*** pattern present in 15 lines ***
	80:     get_name (void)
	86:     get_nlines (void)
	98:     get_next (void)
	103:    void
	110:    get_mtime (void)
	115:    void
	121:    void
	170:    void
	207:    get_next (void)
	212:    void
	219:    get_str (void)
	254:    void
	255:    print (void)
	326:    void
	327:    print_patterns (void)

Finally, we have the threaded version. A new global variable is added to set
the number of threads in the program. This number can be set with the command
line option `-nt=number_of_threads` (passed as last argument).  If `-nt` is not
explicitly set, one thread is used as default:

	$ cd simple_grep/pmemgrep_thx
	$ ./pmemgrep /mnt/mem/grep.pool pattern dir_with_lots_of_files/ -nt=4
	...


