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
 * hello_lpmemblk.c -- simple example for the libpmemblk library
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <libpmemblk.h>


/* size of each element in the pmem pool */
#define ELEMENT_SIZE 1024


/****************************
 * This function writes the "Hello..." string to persistent-memory.
 *****************************/
void write_hello_string (char *input, char *path)
{
	PMEMblkpool *pbp;
	
	/* create the pmemblk pool or open it if it already exists */
	pbp = pmemblk_create(path, ELEMENT_SIZE, PMEMBLK_MIN_POOL, 0666);

	/* Check if create failed */
	if (pbp == NULL) {
		perror(path);
		exit(1); 
	}
	/* store a block at index 0 */
	if (pmemblk_write(pbp, input, 0) < 0) {
		perror("pmemblk_write");
		exit(1);
	}
	/* output a string from the persistent memory to console */
	printf("\nWrite the (%s) string to persistent memory.\n",input);
	
	/* Cleanup */
	/* Close to the pmemblock */
	pmemblk_close(pbp);

	return;
}



/****************************
 * This function reads the "Hello..." string from persistent-memory.
 *****************************/
void read_hello_string(char *path)
{
	PMEMblkpool *pbp;
	/* Define input and output buffers */
	char output[ELEMENT_SIZE];
	
	// Open the pool
	pbp = pmemblk_open(path, ELEMENT_SIZE);
	
	/* Check if open failed */
	if (pbp == NULL) {
		perror(path);
		exit(1);
	}
	
	/* read the block at index 0 */
	if (pmemblk_read(pbp, output, 0) < 0) {
		perror("pmemblk_read");
		exit(1);
	}
	
	/* Reading the string from persistent-memory and write to console */
	printf("\nRead the (%s) string from persistent memory.\n", output);	
	
	/* Cleanup */
	/* Close to the pmemblock */
	pmemblk_close(pbp);	
	
	return;
}


/****************************
 * This main function gather from the command line and call the appropriate
 * functions to perform read and write persistently to memory.
 *****************************/
int main(int argc, char *argv[])
{
	char *path = argv[2];
	
	/* Define input and output buffers */
	char input[ELEMENT_SIZE];
	//char output[ELEMENT_SIZE];

	/* store a block at index 0 */
	strcpy(input, "Hello Persistent Memory!!!");	
	
	if (strcmp (argv[1], "-w") == 0) {
	
		write_hello_string(input, path);
	} 	
	else if (strcmp (argv[1], "-r") == 0) {		
		
		read_hello_string(path);
	}	
	else { 
		fprintf(stderr, "Usage: %s <-w/-r> <filename>\n", argv[0]);
		exit(1);
	}	
	

}
