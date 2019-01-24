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


#include "persistent_string.hpp"

persistent_string::persistent_string (pmem::obj::pool_base &pop)
{
	pmem::obj::transaction::run (pop, [&] { str = nullptr; });
}

void
persistent_string::reset (pmem::obj::pool_base &pop)
{
	pmem::obj::transaction::run (pop, [&] {
		pmemobj_tx_add_range_direct (sso, 1);
		sso[0] = 0;
		if (str) {
			pmem::obj::delete_persistent<char[]> (
			str, strlen (str.get ()) + 1);
		}
	});
}

void
persistent_string::set (pmem::obj::pool_base &pop, std::string *value)
{
	pmem::obj::transaction::run (pop, [&] {
		unsigned long length = value->length ();
		if (length <= SSO_CHARS) {
			if (str) { /* reset old string */
				pmem::obj::delete_persistent<char[]> (
				str, strlen (str.get ()) + 1);
				str = nullptr;
			}
			pmemobj_tx_add_range_direct (sso, SSO_SIZE);
			strcpy (sso, value->c_str ());
		} else {
			if (str) { /* reset old string */
				pmem::obj::delete_persistent<char[]> (
				str, strlen (str.get ()) + 1);
			}
			str = pmem::obj::make_persistent<char[]> (length + 1);
			strcpy (str.get (), value->c_str ());
		}
	});
}
