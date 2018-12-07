/*
Copyright (c) 2018 Intel Corporation

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
 * hello.hpp -- an example for libpmemobj++ library
 */

#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_atomic.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>

#include <fstream>
#include <iostream>
#include <string.h>

#define max_msg_size 30        // Size msg char array
#define LAYOUT "pool"


using namespace std;
namespace pobj = pmem::obj;

/* globals */

/* Hello class  */
/****************************
 * This class stores the "Hello, Persistent Memory!!!" 
 * string into the persistent memory and reading it back
 * before display the message to the screen.
 *****************************/
class Hello
{
	private:
 	char msg[max_msg_size] = {0};

	public:
	Hello (char* input)
	{ 
			strcpy(msg,input);
	}

	char* get_hello_msg ()
	{
		return msg;
	}
};

/* root structure  */
/****************************
 *This root structure contains all the connections the pool and persistent
 *pointer to the persistent objects. Using this root structure component to
 *access the pool and print out the message. 
 ******************************/
struct root {
	pobj::persistent_ptr<Hello> hello;
};
