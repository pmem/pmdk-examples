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


#include "helpers.hpp"

using namespace std;


size_t
get_line (const char *input, size_t input_len, size_t offset, string &line)
{

	size_t i = offset; /* we start looking for a line at 'offset' */
	string line__;

	/* skipping double '\n' or empty lines */
	while (input[i] == '\n') i++;

	/* look for an end of line */
	for (; (i < input_len) && (input[i] != '\n'); i++)
		;

	/* we found some characters belonging to a line */
	if (i > offset) {
		char *newline = (char *)malloc (sizeof (char) * (i - offset + 1));
		memcpy (newline, &input[offset], i - offset);
		newline[i - offset] = '\0';
		string newinput (newline);

		line__.append (newinput, 0, i - offset);
		line = line__; /* to be returned */

		free (newline);
	}

	return line__.length ();
}

void
ret_lines_info (const char *input, vector<size_t> &lines_info)
{

	size_t nlines = 0;
	string line;

	size_t orgline = 0;
	size_t orgoffset = 0;
	size_t byteoffset = 0;

	size_t input_len = strlen (input);

	/* iterating over all the lines in "input" */
	while (get_line (input, input_len, orgoffset + byteoffset, line)) {
		if (byteoffset + line.length () + 1 > PM_MR_MAP_CHUNK_SIZE) {
			/* we read a complete CHUNK, so we create a new entry
			* in lines_info[] */

			/* offset where the CHUNK starts */
			lines_info.push_back (orgoffset);

			/* number of lines for this CHUNK */
			lines_info.push_back (nlines - orgline);

			/* preparing for the next chunk */
			orgline = nlines;
			orgoffset += byteoffset;
			byteoffset = 0;
		}

		byteoffset += line.length () + 1;
		nlines += 1;
	}
	if (nlines > 0) { /* inserting last line */
		lines_info.push_back (orgoffset);
		lines_info.push_back (nlines - orgline);
	}
}

/***** SORTING *****/
struct __kv_aux_t {
	std::string *key;
	size_t value;
};

/* For comparing two elements (to be used by sort_kv() */
int
compare_kv (const void *arg1, const void *arg2)
{
	struct __kv_aux_t *kv1 = (struct __kv_aux_t *)arg1;
	struct __kv_aux_t *kv2 = (struct __kv_aux_t *)arg2;
	return strcmp (kv1->key->c_str (), kv2->key->c_str ());
}

/* sorting function */
void
sort_kv (std::vector<std::string> &keys, std::vector<size_t> &values)
{
	struct __kv_aux_t *sList;
	sList
	= (struct __kv_aux_t *)malloc (sizeof (struct __kv_aux_t) * keys.size ());
	if (sList == nullptr) /* malloc failed */
		throw std::runtime_error ("problems allocating memory with "
		                          "sort_kv()");
	/* Convert input in 2 arrays to a unified array */
	for (size_t i = 0; i < keys.size (); i++) {
		sList[i].key = new std::string (keys[i].c_str ());
		sList[i].value = values[i];
	}
	/* so sorting... */
	qsort ((void *)sList, keys.size (), sizeof (struct __kv_aux_t),
	       compare_kv);
	/* convert sorted unified array into the 2 separated arrays */
	for (size_t i = 0; i < keys.size (); i++) {
		keys[i] = sList[i].key->c_str ();
		delete sList[i].key;
		values[i] = sList[i].value;
	}
	free (sList);
}
