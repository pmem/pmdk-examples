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


#ifndef LIST_ENTRY_H
#define LIST_ENTRY_H

#include <errno.h>
#include <vector>

#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/transaction.hpp>

/*
 * Macros to define the types of tasks
 * in the system (NOTYPE, MAP and REDUCE)
 */
#define TASK_TYPE_NOTYPE 0
#define TASK_TYPE_MAP 1
#define TASK_TYPE_REDUCE 2

/*
 * Macros to define the status of a task.
 *   NEW:     The task is new and needs to be executed
 *   BUSY:    A worker thread is processing this task
 *   REDUCED: This task has been reduced but not combined
 *            with other task. Hence, it is not "done" yet.
 *   DONE:    This task is done.
 */
#define TASK_ST_NEW 10
#define TASK_ST_BUSY 20
#define TASK_ST_REDUCED 30
#define TASK_ST_DONE 40


/*
 * Node for the KV list
 */
struct kv_tuple {
	size_t value;
	char key[];
};

class list_entry
{

	/* entry in the list */

	/* pointer to next entry */
	pmem::obj::persistent_ptr<list_entry> next;

	/* status of the task */
	pmem::obj::p<uint8_t> status;

	/* type of the task */
	pmem::obj::p<uint8_t> task_type;

	/* Start byte in the input data (only for MAP tasks) */
	pmem::obj::p<size_t> start_byte;

	/* Number of file lines to process (only for MAP tasks) */
	pmem::obj::p<size_t> n_lines;

	/* Pointer to Key-Value list (only for REDUCE tasks) */
	pmem::obj::persistent_ptr<char[]> kv;

	/* size of the kv list */
	pmem::obj::p<size_t> kv_size;

	/* total allocated bytes in PMEM for the kv list */
	pmem::obj::p<size_t> alloc_bytes;

	public:
	/* NOTE: Since this is a persistent memory data structure,
	         we need to pass the pool_base object---or persistent
	         memory pool---to each "set_XXX" method so other objects
	         created/deleted by this object can be presisted/removed.
	         This pool object is created anew every time the program is run.
	*/

	/* Constructor */
	list_entry (pmem::obj::pool_base &pop);

	/* setters */

	/*
	* Persistent pointer to the next entry in the list of tasks
	*/
	void
	set_next (pmem::obj::pool_base &pop,
	          pmem::obj::persistent_ptr<list_entry> next);

	/*
	* Set the status for this task
	*/
	void
	set_status (pmem::obj::pool_base &pop, uint8_t status);

	/*
	* Set the type for this task
	*/
	void
	set_task_type (pmem::obj::pool_base &pop, uint8_t task_type);

	/*
	* Set the start byte (offset) for the input data to define
	* the CHUNK of data processed by a particular MAP task
	*/
	void
	set_start_byte (pmem::obj::pool_base &pop, size_t s_byte);

	/*
	* Set the number of lines from the input data to be processed by a MAP
	* task
	*/
	void
	set_n_lines (pmem::obj::pool_base &pop, size_t n);

	/*
	* Prepares an allocation of persistent memory of size "bytes"
	* so a kv list can be stored. This function is needed to reduce
	* the number of allocations done through the libpmem library. For
	* example, for a list composed of tens of thounsands of elements,
	* we would only need one allocation as opposed to tens of thousands
	* of allocations.
	*/
	void
	allocate_kv (pmem::obj::pool_base &pop, size_t bytes);

	/*
	* Persists a key/value list stored as inputs "keys" and "values"
	* in a particular task. It is assumed that a pair key/value is
	* stored in the same position in both arrays. For examlpe, the
	* first key/value pair is stored in keys[0] and values[0], the
	* second in keys[1] and values[1], and so forth...
	*
	*/
	void
	add_to_kv (pmem::obj::pool_base &pop, std::vector<std::string> &keys,
	           std::vector<size_t> &values);


	/* getters */
	pmem::obj::persistent_ptr<list_entry>
	get_next ()
	{
		return next;
	}

	uint8_t
	get_status ()
	{
		return status;
	}

	uint8_t
	get_task_type ()
	{
		return task_type;
	}

	size_t
	get_start_byte ()
	{
		return start_byte;
	}

	size_t
	get_n_lines ()
	{
		return n_lines;
	}

	pmem::obj::persistent_ptr<char[]>
	get_kv_array ()
	{
		return kv;
	}

	size_t
	get_kv_size ()
	{
		return kv_size;
	}

	size_t
	get_alloc_bytes ()
	{
		return alloc_bytes;
	}

	/* frees the memory for the key/value list. This is useful
	* when a task is done and its data is no longer needed but we want to
	* keep the task as "done" for metadata stats and bookeeping. */
	void
	delete_kv (pmem::obj::pool_base &pop);
};

#endif
