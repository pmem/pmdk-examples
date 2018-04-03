/*
Copyright (c) 2017, Intel Corporation

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


#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <iostream>
#include <regex>
#include <string.h>
#include <string>
#include <vector>

using namespace std;
using namespace boost::filesystem;


/* auxiliary functions */

int
process_reg_file (const char *pattern, const char *filename)
{
	std::ifstream fd (filename);
	string line;

	string patternstr ("(.*)(");
	patternstr += string (pattern) + string (")(.*)");
	regex exp (patternstr);

	int ret = 0;
	if (fd.is_open ()) {
		size_t linenum = 0;
		bool first_line = true;
		while (getline (fd, line)) {
			++linenum;
			if (regex_match (line, exp)) {
				if (first_line) {
					cout << "FILE = " << string (filename);
					cout << endl << flush;
					first_line = false;
				}
				cout << linenum << ": " << line << endl;
				cout << flush;
			}
		}
	} else {
		cout << "unable to open file " + string (filename) << endl;
		ret = -1;
	}
	return ret;
}

int
process_directory_recursive (const char *dirname, vector<string> &files)
{
	path dir_path (dirname);
	directory_iterator it (dir_path), eod;

	BOOST_FOREACH (path const &pa, make_pair (it, eod)) {

		/* full path name */
		string fpname = pa.string ();

		if (is_regular_file (pa)) {
			files.push_back (fpname);
		} else if (is_directory (pa) && pa.filename () != "."
		           && pa.filename () != "..") {
			if (process_directory_recursive (fpname.c_str (), files)
			    < 0)
				return -1;
		}
	}
	return 0;
}

int
process_directory (const char *pattern, const char *dirname)
{
	vector<string> files;
	if (process_directory_recursive (dirname, files) < 0)
		return -1;

	for (vector<string>::iterator it = files.begin (); it != files.end ();
	     ++it) {
		if (process_reg_file (pattern, it->c_str ()) < 0)
			cout << "problems processing file " << *it << endl;
	}
	return 0;
}

int
process_input (const char *pattern, const char *input)
{
	/* check input type */
	path pa (input);

	if (is_regular_file (pa))
		return process_reg_file (pattern, input);
	else if (is_directory (pa))
		return process_directory (pattern, input);
	else {
		cout << string (input);
		cout << " is not a valid input" << endl;
	}
	return -1;
}

/*
 * MAIN
 */
int
main (int argc, char *argv[])
{
	/* reading params */
	if (argc < 3) {
		cout << "USE " << string (argv[0]) << " pattern input ";
		cout << endl << flush;
		return 1;
	}
	return process_input (argv[1], argv[2]);
}
