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


#include <string>
#include <vector>

#include "pm_mapreduce.hpp"

using namespace std;


/*
 * To use the class pm_mapreduce,
 * do inheritance and implement the virtual
 * functions map() and reduce()
 */
class pm_wordcount : public pm_mapreduce
{

	public:
	/* constructor */
	pm_wordcount (int argc, char *argv[]) : pm_mapreduce (argc, argv) {}

	/* map */
	virtual void
	map (const string line, vector<string> &keys, vector<size_t> &values)
	{

		size_t i = 0;
		while (true) {
			string buf;
			while (i < line.length ()
			       && (isalpha (line[i]) || isdigit (line[i]))) {
				buf += line[i++];
			}

			if (buf.length () > 0) {
				keys.push_back (buf);
				values.push_back (1);
			}
			if (i == line.length ())
				break;
			i++;
		}
	}

	/* reduce */
	virtual void
	reduce (const string key, const vector<size_t> &valuesin,
	        vector<size_t> &valuesout)
	{
		size_t total = 0;
		for (vector<size_t>::const_iterator it = valuesin.begin ();
		     it != valuesin.end (); ++it) {
			total += *it;
		}
		valuesout.push_back (total);
	}
};

/*** MAIN ***/
int
main (int argc, char *argv[])
{
	/* declare a new instance of the new class */
	pm_wordcount wc (argc, argv);
	/* And just call run_job() */
	wc.run_job ();
	return 0;
}
