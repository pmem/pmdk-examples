/*
Copyright (c) 2018-2019, Intel Corporation

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


#include "common.hpp"

using namespace std;
using namespace pmem;
using namespace pmem::obj;

/* globals */
pool<root> pop;


/* main */
int
main (int argc, char *argv[])
{
	/* reading params */
	if (argc < 2) {
		cout << "USE " << string (argv[0]) << " pmem-file ";
		cout << endl << flush;
		return 1;
	}

	/* Opening pmem-file */
	if (!access (argv[1], F_OK)) { /* file exists, deleting it... */
		cout << "pmem-file '" << string (argv[1]) << "' exists. ";
		cout << "Do you want to overwrite it? (Y/n) ";

		string input;
		getline (cin, input);
		if (input == "n") {
			cout << "bye bye" << endl << flush;
			return 0;
		}
		remove (argv[1]);
	}
	pop = pool<root>::create (argv[1], "PMEMLEAK", POOLSIZE, S_IRWXU);
	auto proot = pop.root ();

	/* initialization... */
	transaction::run (pop, [&] {
		proot->num_employees = 0;
		proot->employees = nullptr;
	});

	/* creating some objects, leaking even IDs */
	persistent_ptr<employee> new_ptr;

	for (size_t i = 64; i > 0; i--) {
		transaction::run (pop, [&] {
			new_ptr = make_persistent<employee> ();

			new_ptr->id = i - 1;
			pmemobj_tx_add_range_direct (new_ptr->name, SSIZE);
			pmemobj_tx_add_range_direct (new_ptr->department, SSIZE);
			strcpy (new_ptr->name, "Test Name");
			strcpy (new_ptr->department, "Fake Department");

			if ((i - 1) % 2 == 1
			    || (i - 1) == 0) { /* only linking odd IDs */
				new_ptr->next = proot->employees;
				proot->employees = new_ptr;
			}
			proot->num_employees = proot->num_employees + 1;
		});
	}

	return 0;
}
