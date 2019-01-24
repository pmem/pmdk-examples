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


#ifndef PM_MR_H
#define PM_MR_H

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "helpers.hpp"
#include "list_entry.hpp"
#include "persistent_string.hpp"
#include "task_list.hpp"

#include <libpmemobj++/condition_variable.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/mutex.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>


/* it is allocated when the pool is first created */
#define PM_MR_POOLSIZE \
	(((size_t) (1024 * 1024)) * ((size_t) (1024 * 8))) /* 8 GB */

/* 1 thread of each by default */
#define PM_MR_NUM_MAP_WORKERS 1
#define PM_MR_NUM_RED_WORKERS 1

/* One MAP task processes at most X MB of raw input data at once.
   This is used to define the size of the input chunks */
#define PM_MR_MAP_CHUNK_SIZE ((size_t) (1024 * 1024 * 8)) /* 8 MB */

/* root data structure for the PM pool file */
struct root {
	pmem::obj::mutex pmutex;
	pmem::obj::condition_variable cond;
	pmem::obj::persistent_ptr<persistent_string> input;
	task_list tlist;
};

class pm_mapreduce
{
	private:
	/* PROGRAM PARAMS */
	/* this structure is not persistent. It is
	   created anew with each program invocation */
	struct program_params {
		std::string pm_file;
		std::string cmd;
		std::string dir_name;
		std::string output_file;
		int num_map_workers = 0;
		int num_reduce_workers = 0;
	};

	/* RUNTIME VARIABLES */
	struct program_params params;
	pmem::obj::pool<root> pop;

	/* METHODS */
	/*
	 * Read the program parameters from cmd line
	 */
	int
	read_params (int argc, char *argv[]);

	/*
	 * Prints help usage message
	 */
	void
	print_help (std::string program_name);

	/*
	 * Inits a pool assuming the pool does not exists
	 * (it prints an error msg if the pool file already
	 * exists and exits)
	 */
	int
	init_pm_pool (void);

	/*
	 * Inits a pool for an already existing pool object
	 * (it prints an error msg if the pool file does
	 * not exists and exits)
	 */
	int
	init_ex_pm_pool (void);

	/*
	 * Gets job progress as a percentage of tasks
	 * completed. For example, if 50% of map tasks
	 * and 20% of reduce tasks has been completed, it
	 * returns map=50 and reduce=20
	 */
	void
	get_job_progress (int &map, int &reduce);

	/*
	 * Prints job progress to standard output
	 */
	void
	print_job_progress (void);

	/*
	 * Writes the contents of the last reduce tasks
	 * to a file. The format
	 * in the file is the following:
	 *
	 * key[0] '\t' value[0] '\n'
	 * key[1] '\t' value[1] '\n'
	 * ...
	 * key[n] '\t' value[n] '\n'
	 */
	void
	write_results (void);

	/*
	 * Load data from input files to pmem
	 *
	 */
	int
	load_input_data (void);

	/*
	 * Makes sure the state of all tasks is correct
	 * before running a job (the state of some tasks
	 * may be inconsistent after a crash)
	 */
	void
	clean_up_dirty_tasks (void);

	/*
	 * Create initial MAP tasks in the pool.
	 * This method is called before the first run
	 *
	 */
	int
	create_initial_tasks (void);

	/*
	 * This method starts all threads and prints
	 * job progress until completion (at that point
	 * joins all the threads).
	 */
	void
	main_thread_loop (void);

	/*
	 * stats all the working threads
	 */
	void
	start_threads (std::thread t[]);

	/*
	 * joins all the working threads
	 */
	void
	join_threads (std::thread t[]);

	/*
	 * This function is used by the map thread to fetch a task
	 * from the task list. If no new tasks are found, the function
	 * checks whether all map tasks are done (returning all_done=true,
	 * otherwise returns all_done=false).
	 */
	void
	ret_available_map_task (pmem::obj::persistent_ptr<list_entry> &tsk,
	                        bool &all_done);

	/*
	 * This function is used by the reduce thread to fetch either one task
	 * or two from the task list. If no new tasks are found, the function
	 * checks whether all tasks are done (returning all_done=true,
	 * otherwise returns all_done=false). If only one task is returned,
	 * the function checks whether this task is the last one
	 * (only_one_left=true) or not (only_one_left=false).
	 */
	void
	ret_available_red_task (pmem::obj::persistent_ptr<list_entry> (&tsk)[2],
	                        bool &only_one_left, bool &all_done);

	/*
	 * Process a single map task. From this method, the user-defined
	 * virtual method "map" is called.
	 */
	void
	process_map_task (pmem::obj::persistent_ptr<list_entry> tsk);

	/*
	 * Process at least a reduce task. From this method, the user-defined
	 * virtual method "reduce" is called.
	 */
	void
	process_red_tasks (pmem::obj::persistent_ptr<list_entry> tsk[2],
	                   bool only_one_left);

	/*
	 * method to run the logic of a MAP thread.
	 */
	void
	map_thread (void);

	/*
	 * method to run the logic of a REDUCE thread.
	 */
	void
	reduce_thread (void);

	public:
	/* constructor */
	pm_mapreduce (int argc, char *argv[]);

	/* Starts the mapreduce job */
	void
	run_job (void);

	/*** virtual methods that the user should implement ***/
	/*
	 * MAP: As input, the map method receives a single line
	 *      of text. The map function should return a key/value
	 *      list in the output vectors "keys" and "values".
	 *      A single kay/value pair should be stored in the same
	 *      position in both vectors.
	 */
	virtual void
	map (const std::string line, std::vector<std::string> &keys,
	     std::vector<size_t> &values)
	= 0;

	/*
	 * REDUCE: As input, the reduce method receives a vector
	 *         of input values "valuesin" for the key "key"
	 *         The reduce function should return a vector
	 *         "valuesout" with the result of the reduction.
	 *
	 */
	virtual void
	reduce (const std::string key, const std::vector<size_t> &valuesin,
	        std::vector<size_t> &valuesout)
	= 0;
};

#endif
