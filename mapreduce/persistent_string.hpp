/*
Copyright (c) 2017-2019, Intel Corporation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/*
 * This class defines a string class to be stored in PMEM
 */

/* NOTE: Since this is a persistent memory data structure,
         we need to pass the pool_base object---or persistent
         memory pool---to each method that creates or deletes
         objects. This pool object is created anew every time
         the program is run.
*/


#ifndef PERSISTENT_STRING_H
#define PERSISTENT_STRING_H

#include <string.h>
#include <string>

#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/transaction.hpp>

#define SSO_CHARS 15
#define SSO_SIZE (SSO_CHARS + 1)

class persistent_string
{
	public:
	/* constructor */
	persistent_string (pmem::obj::pool_base &pop);

	/* returns a direct pointer to the data
	*/
	char *
	data () const
	{
		return str ? str.get () : const_cast<char *> (sso);
	}

	/* destroys the string freeing the allocated bytes in
	 * PMEM
	 */
	void
	reset (pmem::obj::pool_base &pop);

	/*
	 * sets the value of the persistent string to "value". If
	 * the string has any data it is freed first.
	 */
	void
	set (pmem::obj::pool_base &pop, std::string *value);

	private:
	/* sso is used for very small strings (hence avoiding allocation
	 * overhead) */
	char sso[SSO_SIZE];

	/* persistent pointer to the string */
	pmem::obj::persistent_ptr<char[]> str;
};

#endif
