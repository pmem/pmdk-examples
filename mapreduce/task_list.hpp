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


#ifndef TASK_LIST_H
#define TASK_LIST_H

#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/transaction.hpp>

#include "list_entry.hpp"

class task_list
{

	public:
	/*
	 * Constructor
	 */
	task_list (pmem::obj::pool_base &pop);

	/*
	 * Inserts new element in the list
	 */
	void
	insert (pmem::obj::pool_base &pop,
	        pmem::obj::persistent_ptr<list_entry> entry);

	/*
	 * If a program was restarted after a crash, there
	 * could be some tasks in an inconsistent state.
	 * This function makes sure the state is correct.
	 */
	void
	clean_up_dirty_tasks (pmem::obj::pool_base &pop);

	/*
	 * sets "maptask" to the first free and uncompleted Map task found in
	 * the list and returns 0. Returns /= 0 if such task is not found
	 * or any other error occur.
	 */
	int
	ret_map (pmem::obj::persistent_ptr<list_entry> &maptask);

	/*
	 * sets "redtask" to the first free and uncompleted Reduce tasks
	 * found in the list and returns 0.Returns /= 0 if it can not
	 * find any task or any other error occur.
	 */
	int
	ret_red (pmem::obj::persistent_ptr<list_entry> &redtask);

	/*
	 * sets "lastsk" to the last task in the list and returns 0.
	 * Returns 1 if the list is empty
	 */
	int
	ret_last (pmem::obj::persistent_ptr<list_entry> &lasttsk);

	/* Returns statistics about the job:
	 *   t_tasks     -> Total tasks in the list
	 *   t_map_tasks -> Total MAP tasks in the list
	 *   c_tasks     -> Total completed tasks in the list
	 *   c_map_tasks -> Total completed MAP tasks in the list
	 *
	 **/
	void
	get_stats (size_t &t_tasks, size_t &t_map_tasks, size_t &c_tasks,
	           size_t &c_map_tasks);

	/*
	 * Are all REDUCE tasks in the list completed?
	 * (true if the status of all REDUCE tasks is TASK_ST_DONE)
	 */
	bool
	all_red_done (void);

	/*
	 * Are all MAP tasks in the list completed?
	 * (true if the status of all MAP tasks is TASK_ST_DONE)
	 */
	bool
	all_map_done (void);

	/*
	 * Returns true if the status of all tasks but one is TASK_ST_DONE.
	 * The status of the remainder task should be TASK_ST_BUSY.
	 */
	bool
	only_one_left (void);

	private:
	/* persistent pointer to a list of MAP tasks */
	pmem::obj::persistent_ptr<list_entry> head_map;
	/* persistent pointer to a list of REDUCE tasks */
	pmem::obj::persistent_ptr<list_entry> head_red;
};

#endif
