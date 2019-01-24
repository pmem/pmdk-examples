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


#include "task_list.hpp"

task_list::task_list (pmem::obj::pool_base &pop)
{

	pmem::obj::transaction::run (pop, [&] {

		head_map = nullptr;
		head_red = nullptr;

	});
}

void
task_list::insert (pmem::obj::pool_base &pop,
                   pmem::obj::persistent_ptr<list_entry> entry)
{

	pmem::obj::persistent_ptr<list_entry> aux;

	pmem::obj::transaction::run (pop, [&] {

		if (entry->get_task_type () == TASK_TYPE_MAP) {

			if (head_map == nullptr)
				head_map = entry;
			else {
				aux = head_map;
				head_map = entry;
				entry->set_next (pop, aux);
			}
		}

		if (entry->get_task_type () == TASK_TYPE_REDUCE) {

			if (head_red == nullptr)
				head_red = entry;
			else {
				aux = head_red;
				head_red = entry;
				entry->set_next (pop, aux);
			}
		}

	});
}

void
task_list::clean_up_dirty_tasks (pmem::obj::pool_base &pop)
{

	pmem::obj::persistent_ptr<list_entry> itr = head_map;

	while (itr != nullptr) {
		if (itr->get_status () == TASK_ST_BUSY)
			itr->set_status (pop, TASK_ST_NEW);

		itr = itr->get_next ();
	}
	itr = head_red;
	while (itr != nullptr) {
		if (itr->get_status () == TASK_ST_BUSY)
			itr->set_status (pop, TASK_ST_NEW);

		itr = itr->get_next ();
	}
}

int
task_list::ret_map (pmem::obj::persistent_ptr<list_entry> &maptask)
{

	pmem::obj::persistent_ptr<list_entry> itr = head_map;

	while (itr != nullptr) {

		if (itr->get_status () == TASK_ST_NEW) {

			maptask = itr;
			return 0;
		}
		itr = itr->get_next ();
	}
	return 1;
}

int
task_list::ret_red (pmem::obj::persistent_ptr<list_entry> &redtask)
{

	pmem::obj::persistent_ptr<list_entry> itr = head_red;

	while (itr != nullptr) {

		if (itr->get_status () != TASK_ST_BUSY
		    && itr->get_status () != TASK_ST_DONE) {

			redtask = itr;
			return 0;
		}
		itr = itr->get_next ();
	}
	return 1;
}

int
task_list::ret_last (pmem::obj::persistent_ptr<list_entry> &lasttask)
{

	if (head_red == nullptr)
		return 1;

	lasttask = head_red;
	return 0;
}

void
task_list::get_stats (size_t &t_tasks, size_t &t_map_tasks, size_t &c_tasks,
                      size_t &c_map_tasks)
{


	t_tasks = 0;
	t_map_tasks = 0;
	c_tasks = 0;
	c_map_tasks = 0;

	pmem::obj::persistent_ptr<list_entry> itr = head_map;
	while (itr != nullptr) {

		bool is_done = itr->get_status () == TASK_ST_DONE ? true : false;

		t_tasks += 1;
		t_map_tasks += 1;
		if (is_done) {
			c_tasks += 1;
			c_map_tasks += 1;
		}

		itr = itr->get_next ();
	}

	itr = head_red;
	while (itr != nullptr) {

		bool is_done = itr->get_status () == TASK_ST_DONE ? true : false;

		t_tasks += 1;
		if (is_done)
			c_tasks += 1;

		itr = itr->get_next ();
	}
}

bool
task_list::all_red_done (void)
{

	pmem::obj::persistent_ptr<list_entry> itr = head_red;

	while (itr != nullptr) {

		if (itr->get_status () != TASK_ST_DONE)
			return false;

		itr = itr->get_next ();
	}

	return true;
}

bool
task_list::all_map_done (void)
{

	pmem::obj::persistent_ptr<list_entry> itr = head_map;

	while (itr != nullptr) {

		if (itr->get_status () != TASK_ST_DONE)
			return false;

		itr = itr->get_next ();
	}

	return true;
}

bool
task_list::only_one_left (void)
{

	size_t num_total = 0;
	size_t num_busy = 0;
	size_t num_done = 0;

	pmem::obj::persistent_ptr<list_entry> itr = head_map;
	while (itr != nullptr) {

		num_total++;

		if (itr->get_status () == TASK_ST_BUSY)
			num_busy++;
		else if (itr->get_status () == TASK_ST_DONE)
			num_done++;

		itr = itr->get_next ();
	}

	itr = head_red;
	while (itr != nullptr) {

		num_total++;

		if (itr->get_status () == TASK_ST_BUSY)
			num_busy++;
		else if (itr->get_status () == TASK_ST_DONE)
			num_done++;

		itr = itr->get_next ();
	}

	if (num_busy == 1 && num_busy + num_done == num_total)
		return true;
	else
		return false;
}
