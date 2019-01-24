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
#include <vector>

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

	/* opening pmem-file */
	if (access (argv[1], F_OK)) {
		cout << "pmem-file '" << string (argv[1]) << "' does not "
		                                             "exist.";
		cout << endl << flush;
		return 1;
	}
	pop = pool<root>::open (argv[1], "PMEMLEAK");
	auto proot = pop.root ();

	/* iterating over all objects... */
	persistent_ptr<employee> tail_ptr;
	vector<size_t> missing_ids;

	for (size_t i = 0; i < proot->num_employees; i++) {
		bool found = false;
		tail_ptr = proot->employees;
		while (tail_ptr != nullptr) {
			if (i == tail_ptr->id) {
				found = true;
				break;
			}
			tail_ptr = tail_ptr->next;
		}
		if (found == false)
			missing_ids.push_back (i);
	}
	cout << "There are " << missing_ids.size () << " missing IDs." << endl
	     << flush;

	if (missing_ids.size () > 0) {
		cout << "Do you want to fix missing objects? (Y/n) ";
		string input;
		getline (cin, input);
		if (input == "n") {
			cout << "bye bye" << endl << flush;
			return 0;
		}

		/* recovering missing IDs */
		PMEMoid raw_root = proot->employees.raw ();
		PMEMoid raw_obj;
		persistent_ptr<employee> emp;
		size_t recovered = 0;

		/* iterating over all allocated objects in the pool... */
		POBJ_FOREACH (pop.handle (), raw_obj)
		{

			/* checking if object is of type 'employee' */
			if (pmemobj_type_num (raw_obj)
			    == pmemobj_type_num (raw_root)) {

				/* transforming to persistent_ptr (C to C++) */
				emp = persistent_ptr<employee> (raw_obj);

				bool found = false;
				for (vector<size_t>::iterator it
				     = missing_ids.begin ();
				     it != missing_ids.end (); ++it) {
					if (emp->id == *it) {
						found = true;
						break;
					}
				}
				if (found == true) { /* recovering object */
					transaction::run(pop, [&] {
						emp->next = proot->employees;
						proot->employees = emp;
					});
					recovered += 1;
				}
			}
		}
		cout << recovered << " objects recovered." << endl << flush;
	}

	return 0;
}
