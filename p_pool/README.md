### Introduction

This example explores the basic PMDK building blocks such as persistent-memory
pools, persistent pointers, transactions, and persistent mutexes.  The example
code calculates the area of a triangle and circle, along with the volume of a
cone and a sphere.  Each thread performs the calculations on each shape for a
number of iterations.  The goal is to demonstrate how a multithreaded C++
application can read and update data in the persistent-memory pools.

### Build Instructions

To build this sample, download/clone the [pmdk-examples
repository](https://github.com/pmem/pmdk-examples).  A Makefile is provided. To
compile this sample, just type `make` from the root directory of the
repository.

### How to Run

After building the binary, the code sample can be run with a statement such as
the following: 

	$./pPool pool1 pool2 80 30 

Below is an example of the result output:

	Pool ID     Thrd#:Itr#      Objects       Area/Volume    Result
	***************************************************************************
	Pool1           0: 0         Circle            (area): 26075860741.19
	Pool1           0: 0         Sphere          (volume): 3167537606838138.00
	Pool2           1: 0       Triangle            (area): 341392088.13
	Pool2           1: 0           Cone          (volume): 19204950889128.68
	Pool2           7: 0       Triangle            (area): 341392088.13
	Pool2           7: 0           Cone          (volume): 19204950889128.68
	Pool2           3: 0       Triangle            (area): 341392088.13
	Pool2           3: 0           Cone          (volume): 19204950889128.68
	Pool1           4: 0         Circle            (area): 26173733753.67
	Pool1           4: 0         Sphere          (volume): 3185387861483898.50
	Pool2           5: 0       Triangle            (area): 341392088.13
	Pool2           5: 0           Cone          (volume): 19204950889128.68
	Pool1           6: 0         Circle            (area): 26237164992.44
	Pool1           6: 0         Sphere          (volume): 3196974408586274.50
	Pool1           2: 0         Circle            (area): 26370259961.46
	Pool1           2: 0         Sphere          (volume): 3196974408586274.50
	Pool1           4: 1         Circle            (area): 26370259961.46
	Pool1           4: 1         Sphere          (volume): 3196974408586274.50
	Pool2          25: 0       Triangle            (area): 341392088.13
	Pool2          25: 0           Cone          (volume): 19204950889128.68
	Pool1          10: 0         Circle            (area): 26370259961.46
	Pool1          10: 0         Sphere          (volume): 3221331482123361.50
	Pool2          31: 0       Triangle            (area): 341392088.13
	Pool2          31: 0           Cone          (volume): 19204950889128.68
	Pool1          12: 0         Circle            (area): 26494615032.38
	Pool1          12: 0         Sphere          (volume): 3244144727181956.00
	...
	Pool1          72:28         Circle            (area): 2684251154.12
	Pool1          72:28         Sphere          (volume): 97584116375516.19
	Pool1          30:27         Circle            (area): 2716396254.51
	Pool1          30:27         Sphere          (volume): 99922181516284.06
	Pool1          76:29         Circle            (area): 2758654229.57
	Pool1          76:29         Sphere          (volume): 101740392765644.80
	Pool2          75: 8       Triangle            (area): 706811244.59
	Pool2          75: 8           Cone          (volume): 51076427129836.18
	Pool1          74:28         Circle            (area): 2800924267.17
	Pool1          74:28         Sphere          (volume): 106500950728544.52
	Pool2          65: 9       Triangle            (area): 717918227.21
	Pool2          65: 9           Cone          (volume): 52405223699349.74
	Pool1          44:27         Circle            (area): 2914514878.30
	Pool1          44:27         Sphere          (volume): 113018599841733.33
	Pool1          46:29         Circle            (area): 2957958135.85
	Pool1          46:29         Sphere          (volume): 115378221801551.67
	Pool1          40:29         Circle            (area): 3004841888.49
	Pool1          40:29         Sphere          (volume): 118362250827024.00
	Pool2          15: 7       Triangle            (area): 733557291.02
	Pool2          15: 7           Cone          (volume): 54917229140807.54
	Pool1          44:28         Circle            (area): 3076605675.80
	Pool1          44:28         Sphere          (volume): 123907112816521.69
	Pool1          48:29         Circle            (area): 3076605675.80
	Pool1          48:29         Sphere          (volume): 126508792651402.20
	Pool1          42:28         Circle            (area): 3100025635.88
	Pool1          42:28         Sphere          (volume): 128372368945409.25
	Pool2          57: 6       Triangle            (area): 749589796.78
	Pool2          57: 6           Cone          (volume): 55826346226225.69
	...
 
 
