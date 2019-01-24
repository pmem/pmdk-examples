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


#ifndef HELPERS_H
#define HELPERS_H

#include <fstream>
#include <iostream>
#include "pm_mapreduce.hpp"
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <vector>

/* -- inline functions */

/*
 * prints log messages to std out
 */
static inline void
log (std::string msg)
{
	std::cout << msg << std::endl;
}
/*
 * prints error messages to std err
 */
static inline void
error (std::string msg)
{
	std::cerr << msg << std::endl;
}
/*
 * check if file "file" exists
 */
static inline int
file_exists (std::string file)
{
	return access (file.c_str (), F_OK);
}

/* -- functions prototypes */

/*
 * stores one line (sequence of chars ending in '\n' or '\0') from char array
 * "input" into string "line"
 *
 *    input     : pointer to an array of char
 *    input_len : the length of the array input
 *    offset    : offset from where to start reading a line
 *    line      : output string containing the line
 *
 *  RETURNS the length of the line (0 if no line can be found).
 */
size_t
get_line (const char *input, size_t input_len, size_t offset, std::string &line);

/*
 * This function scans the array "input" to check how many lines should
 * be included for each chunk so as to never go above the size limit per chunk
 * (a chunk is a piece of the input data that forms a single MAP task).
 * The size limit for chunks is set by the macro PM_MR_MAP_CHUNK_SIZE
 * (defined in pm_mapreduce.hpp). Typical sizes are 8-64 MBs.
 *
 * The following info is returned in the vector "lines_info":
 *    -> Index 2*i     : The line offset of chunk i
 *    -> Index (2*i)+1 : The size in lines of chunk i
 */
void
ret_lines_info (const char *input, std::vector<size_t> &lines_info);

/*
 * Sort both arrays "keys" and "values" by the key. It is assumed that
 * a pair key/value is stored in the same position in both arrays. For
 * example, the first key/value pair is stored in keys[0] and values[0],
 * the second in keys[1] and values[1], and so forth...
 *
 */
void
sort_kv (std::vector<std::string> &keys, std::vector<size_t> &values);

#endif
