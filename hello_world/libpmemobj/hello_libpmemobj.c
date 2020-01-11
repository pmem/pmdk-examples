/*
Copyright (c) 2018-2019 Intel Corporation

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
 
1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 
SPDX-License-Identifier: BSD-3-Clause
*/

/*
 * hello_lpmemobj.c -- an example for libpmemobj library
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <libpmemobj.h>

// Name of our layout in the pool
#define LAYOUT "hello_layout"

// Maximum length of our buffer
#define MAX_BUF_LEN 30


// Root structure
struct my_root {
	size_t len;
	char buf[MAX_BUF_LEN];
};

/****************************
 * This function writes the "Hello..." string to persistent-memory.
 *****************************/
void write_hello_string (char *buf, char *path)
{
	PMEMobjpool *pop;
	
	// Create the pmemobj pool or open it if it already exists
	pop = pmemobj_create(path, LAYOUT, PMEMOBJ_MIN_POOL, 0666);

	// Check if create failed		
	if (pop == NULL) 
	{
		perror(path);
		exit(1);
	}
					
	// Get the PMEMObj root
	PMEMoid root = pmemobj_root(pop, sizeof (struct my_root));

	// Pointer for structure at the root
	struct my_root *rootp = pmemobj_direct(root);

	// Write the string to persistent memory

	// Copy string and persist it
	pmemobj_memcpy_persist(pop, rootp->buf, buf, strlen(buf));

	// Assign the string length and persist it
	rootp->len = strlen(buf);
	pmemobj_persist(pop, &rootp->len, sizeof (rootp->len));	

	// Write the string from persistent memory 	to console
	printf("\nWrite the (%s) string to persistent-memory.\n", rootp->buf);
	
	// Close PMEM object pool
	pmemobj_close(pop);	
		
	return;
}

/****************************
 * This function reads the "Hello..." string from persistent-memory.
 *****************************/
void read_hello_string(char *path)
{
	PMEMobjpool *pop;
	
	//Attempt open instead
	pop = pmemobj_open(path, LAYOUT);
	
	// Check if open failed
	if (pop == NULL) {
		perror(path);
		exit(1);
	} 
	
	// Get the PMEMObj root
	PMEMoid root = pmemobj_root(pop, sizeof (struct my_root));
	
	// Pointer for structure at the root
	struct my_root *rootp = pmemobj_direct(root);
	
	// Read the string from persistent memory and write to the console
	printf("\nRead the (%s) string from persistent-memory.\n", rootp->buf);
	
	// Close PMEM object pool
	pmemobj_close(pop);

	return;
}

/****************************
 * This main function gather from the command line and call the appropriate
 * functions to perform read and write persistently to memory.
 *****************************/
int main(int argc, char *argv[])
{
	char *path = argv[2];
	
	// Create the string to save to persistent memory
	char buf[MAX_BUF_LEN] = "Hello Persistent Memory!!!";
	
	if (strcmp (argv[1], "-w") == 0) {

		write_hello_string(buf, path);
		
	} else if (strcmp (argv[1], "-r") == 0) {

		read_hello_string(path);
	} else { 
		fprintf(stderr, "Usage: %s <-w/-r> <filename>\n", argv[0]);
		exit(1);
	}

}
