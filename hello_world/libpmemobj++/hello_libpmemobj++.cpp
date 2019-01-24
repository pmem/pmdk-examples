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
 * hello.cpp -- an example for libpmemobj++ library
 */

#include "hello_libpmemobj++.hpp"
#include <errno.h>

/* Functions */

/* file_exists function  */
/****************************
 * This checks to see if the filename exists.  A boolean value (true or false)
 * will return to the calling function.
 *
 *****************************/
inline bool
file_exists (const std::string &name)
{
	std::ifstream f (name.c_str ());
	return f.good ();
}

/* show_usage function  */
/****************************
 * This function print out the correct syntax to run the program.
 *****************************/
static void
show_usage (string name)
{
	cerr << "Usage: " << name
	     << " <-w/-r> <Pmem-Pool-name> "
		 << endl;
}
/* End Functions */

/****************************
 * This function writes the "Hello..." string to persistent-memory.
 *****************************/
void write_hello_string (char *input, char *path)
{
	pobj::pool<root> pop;
	pobj::persistent_ptr<root> pool;
	
	/* Create pool in persistent memory */
	// Get the root object
	pop = pobj::pool<root>::create (path, LAYOUT,
									 PMEMOBJ_MIN_POOL, S_IRUSR | S_IWUSR);
	// Get pool object
	pool = pop.root ();
	
	// Store the input into persistent memory
	pobj::make_persistent_atomic<Hello> (pop, pool->hello, input);
	
	// Write to the console
	cout << endl << "\nWrite the (" << pool->hello->get_hello_msg()
	 << ") string to persistent-memory." << endl;	
			
	/* Cleanup */
	/* Close persistent pool */
	pop.close ();	
	return;
}

/****************************
 * This function reads the "Hello..." string from persistent-memory.
 *****************************/
void read_hello_string(char *path)
{
	pobj::pool<root> pop;
	pobj::persistent_ptr<root> pool;

	/* Open the pool in persistent memory */
	pop = pobj::pool<root>::open (path, LAYOUT);
	pool = pop.root ();

	// Write to the console
	cout << endl    << "\nRead the ("	<< pool->hello->get_hello_msg()
		<< ") string from persistent-memory." << endl;		
	
	/* Cleanup */
	/* Close persistent pool */
	pop.close ();	

	return;
}


/* Main */
int main (int argc, char *argv[])
{
	pobj::pool<root> pop;
	pobj::persistent_ptr<root> pool;
	
	/* Reading parameters from command line */
	if (argc < 3) {
		show_usage (argv[0]);
		return 1;
	}
	
	char *path = argv[2]; //
	// Prepare the input to store into persistent memory
	char input[max_msg_size] = "Hello Persistent Memory!!!";
	
	if (strcmp (argv[1], "-w") == 0) {	
		 
		write_hello_string (input, path);
		
	} else if (strcmp (argv[1], "-r") == 0) {

		read_hello_string (path);
		
	} else { 
		show_usage (argv[0]);
		exit(1);
	}

	return 0;
}
/* End Main */
