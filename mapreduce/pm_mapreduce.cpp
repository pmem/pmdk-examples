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


#include "pm_mapreduce.hpp"

pm_mapreduce::pm_mapreduce (int argc, char *argv[])
{
	if (argc < 3 || read_params (argc, argv))
		print_help (std::string (argv[0]));
}

int
pm_mapreduce::read_params (int argc, char *argv[])
{
	params.pm_file = std::string (argv[1]);
	params.cmd = std::string (argv[2]);

	for (int i = 2; i < argc; i++) {
		if (strncmp (argv[i], "-d=", 3) == 0)
			params.dir_name = std::string (&argv[i][3]);
		if (strncmp (argv[i], "-o=", 3) == 0)
			params.output_file = std::string (&argv[i][3]);
		if (strncmp (argv[i], "-nm=", 4) == 0)
			params.num_map_workers = atoi (&argv[i][4]);
		if (strncmp (argv[i], "-nr=", 4) == 0)
			params.num_reduce_workers = atoi (&argv[i][4]);
	}
	if (params.cmd.empty ()
	    || (!params.cmd.compare ("load") && params.dir_name.empty ())
	    || (!params.cmd.compare ("write") && params.output_file.empty ()))
		return 1;

	if (params.num_map_workers <= 0)
		params.num_map_workers = PM_MR_NUM_MAP_WORKERS;
	if (params.num_reduce_workers <= 0)
		params.num_reduce_workers = PM_MR_NUM_RED_WORKERS;

	return 0;
}

void
pm_mapreduce::print_help (std::string program_name)
{
	std::cout << "USE: " << program_name << " pmem-file";
	std::cout << " <print | run | write -o=output_file | ";
	std::cout << "load -d=input_dir> [-nm=num_map_workers] ";
	std::cout << "[-nr=num_reduce_workers]" << std::endl;
	std::cout << "command help:" << std::endl;
	std::cout << "    print    ->  Prints mapreduce job progress";
	std::cout << std::endl;
	std::cout << "    run      ->  Runs mapreduce job" << std::endl;
	std::cout << "    load     ->  Loads input data for a new mapreduce";
	std::cout << " job" << std::endl;
	std::cout << "    write    ->  Write job solution to output file";
	std::cout << std::endl;
}

int
pm_mapreduce::init_pm_pool (void)
{
	if (file_exists (params.pm_file) != 0) {
		try {
			pop = pmem::obj::pool<root>::create (
			params.pm_file, "MAPREDUCE", PM_MR_POOLSIZE, S_IRWXU);
		} catch (const pmem::pool_error &e) {
			std::string msg = "failed to create pool: ";
			msg += std::string (e.what ());
			error (msg);
			return 1;
		}
	} else {
		std::string msg = "pool file already exists: " + params.pm_file;
		error (msg);
		return 1;
	}
	return 0;
}

int
pm_mapreduce::init_ex_pm_pool (void)
{
	if (file_exists (params.pm_file) != 0) {
		std::string msg = "pool file does not exists: ";
		msg += params.pm_file;
		error (msg);
		return 1;
	} else {
		try {
			pop = pmem::obj::pool<root>::open (params.pm_file,
			                                   "MAPREDUCE");
		} catch (const pmem::pool_error &e) {
			std::string msg = "failed to open pool: ";
			msg += std::string (e.what ());
			error (msg);
			return 1;
		}
	}
	return 0;
}

void
pm_mapreduce::run_job (void)
{
	switch (params.cmd[0]) {
	case 'p': { /* print */
		log ("Printing job progress");
		if (init_ex_pm_pool ())
			break;
		int map, reduce;
		get_job_progress (map, reduce);
		std::cout << map << "% map  " << reduce << "% reduce";
		std::cout << std::endl << std::flush;
	} break;

	case 'r': { /* run */
		log ("Running job");
		if (init_ex_pm_pool ())
			break;
		clean_up_dirty_tasks ();
		main_thread_loop ();
		log ("done");
	} break;

	case 'w': { /* write */
		log ("Writing results of finished job");
		if (init_ex_pm_pool ())
			break;
		int map, reduce;
		get_job_progress (map, reduce);
		if (map == 100 && reduce == 100)
			write_results ();
		else
			error ("Incomplete job");
	} break;

	case 'l': { /* load */
		log ("Loading input data");
		if (init_pm_pool ())
			break;
		if (load_input_data ())
			break;
		if (create_initial_tasks ())
			break;
	} break;

	default: { /* not found */
		std::string msg = "command " + params.cmd;
		msg += " not valid";
		error (msg);
	} break;
	}
}

void
pm_mapreduce::get_job_progress (int &map, int &reduce)
{
	auto proot = pop.root ();
	auto task_list = &(proot->tlist);

	size_t t_tasks, t_map_tasks, c_tasks, c_map_tasks;
	task_list->get_stats (t_tasks, t_map_tasks, c_tasks, c_map_tasks);

	double map_, reduce_;
	if (t_map_tasks == 0)
		map_ = 0.0;
	else
		map_ = 100.0 * (((double)c_map_tasks) / ((double)t_map_tasks));

	if (t_tasks == t_map_tasks)
		reduce_ = 0.0;
	else {
		reduce_ = ((double)(c_tasks - c_map_tasks));
		reduce_ /= ((double)(t_tasks - t_map_tasks));
		reduce_ = reduce_ * map_;
	}

	map = (int)map_;
	reduce = (int)reduce_;
}

void
pm_mapreduce::print_job_progress (void)
{
	int map, reduce;
	static char msg[] = "0%% map  0%% reduce              ";
	get_job_progress (map, reduce);
	sprintf (msg, "%d%% map  %d%% reduce", map, reduce);
	fprintf (stdout, "%s\r", msg);
	fflush (stdout);
}

void
pm_mapreduce::write_results (void)
{
	auto proot = pop.root ();
	auto task_list = &(proot->tlist);

	pmem::obj::persistent_ptr<list_entry> tsk;
	if (task_list->ret_last (tsk)) {
		error ("there is not any tasks in the pool");
		return;
	}

	char *kv = (char *)&(tsk->get_kv_array ()[0]);
	struct kv_tuple *kvt;

	std::ofstream fd (params.output_file);
	if (fd.is_open ()) {
		size_t offset = 0;
		size_t alloc_bytes = tsk->get_alloc_bytes ();
		while (offset < alloc_bytes) {
			kvt = (struct kv_tuple *)&(kv[offset]);
			fd << std::string (kvt->key) << "\t";
			fd << kvt->value << "\n";
			offset += sizeof (struct kv_tuple) + strlen (kvt->key);
			offset += 1;
		}
		fd.flush ();
		fd.close ();
	} else {
		std::string msg = "unable to open file " + params.output_file;
		error (msg);
	}
}

int
pm_mapreduce::load_input_data (void)
{
	if (params.dir_name.substr (params.dir_name.length () - 1,
	                            params.dir_name.length ())
	    != "/")
		params.dir_name += "/";

	/* get all the input files stored in the input
	   directory */
	std::vector<std::string> input_files;
	struct dirent *entry;
	DIR *dir = opendir (params.dir_name.c_str ());
	if (dir == NULL) {
		std::string msg = "unable to open dir " + params.dir_name;
		error (msg);
		return 1;
	}

	while ((entry = readdir (dir)) != NULL) {
		struct stat path_stat;
		std::string file_name = params.dir_name;
		file_name += std::string (entry->d_name);

		if (stat (file_name.c_str (), &path_stat) >= 0) {

			if (S_ISREG (path_stat.st_mode)
			    && strcmp (entry->d_name, ".") != 0
			    && strcmp (entry->d_name, "..") != 0) {
				input_files.push_back (file_name);
			}
		}
	}
	closedir (dir);

	auto proot = pop.root ();

	/* now we go over all files loading the input data */
	std::string lines;
	for (std::vector<std::string>::iterator it = input_files.begin ();
	     it != input_files.end (); ++it) {
		std::string file_name = *it;

		std::ifstream fd (file_name);
		std::string line;
		if (fd.is_open ()) {
			while (std::getline (fd, line)) {
				lines.append (line);
				lines.append ("\n");
			}
			fd.close ();
		} else {
			std::string msg = "unable to open file " + file_name;
			error (msg);
			return 1;
		}
	}

	/* persisting... */
	pmem::obj::transaction::run (pop, [&] {
		proot->input
		= pmem::obj::make_persistent<persistent_string> (pop);
		proot->input->set (pop, &lines);
	}); /* end transaction */

	return 0;
}

void
pm_mapreduce::clean_up_dirty_tasks (void)
{
	auto proot = pop.root ();
	auto task_list = &(proot->tlist);
	task_list->clean_up_dirty_tasks (pop);
}

int
pm_mapreduce::create_initial_tasks (void)
{
	auto proot = pop.root ();
	auto task_list = &(proot->tlist);

	std::vector<size_t> lines_info;
	size_t n_chunks;

	ret_lines_info (proot->input->data (), lines_info);
	n_chunks = lines_info.size () / 2;
	if (n_chunks == 0) /* empty input */
		return 1;

	pmem::obj::transaction::run (pop, [&] {
		/* Adding new MAP tasks to list */
		for (size_t i = 0; i < n_chunks; i++) {
			auto tsk = pmem::obj::make_persistent<list_entry> (pop);
			tsk->set_task_type (pop, TASK_TYPE_MAP);
			size_t sb = lines_info[i * 2];
			tsk->set_start_byte (pop, sb);

			size_t nlines = lines_info[(i * 2) + 1];
			tsk->set_n_lines (pop, nlines);
			task_list->insert (pop, tsk);
		}
	});
	return 0;
}

void
pm_mapreduce::main_thread_loop (void)
{
	/* start worker threads */
	int tt = params.num_map_workers + params.num_reduce_workers;
	std::thread threads[tt];
	start_threads (threads);

	int o_map = 0;
	int map = 0;
	int o_reduce = 0;
	int reduce = 0;

	auto proot = pop.root ();

	print_job_progress ();
	/* iterate and print progress until job is completed */
	do {
		std::this_thread::sleep_for (std::chrono::seconds (1));
		get_job_progress (map, reduce);
		if (map != o_map || reduce != o_reduce) {
			print_job_progress ();
			o_map = map;
			o_reduce = reduce;
		}
	} while (map < 100 || reduce < 100);
	std::cout << std::endl << std::flush;

	/* notify all threads to stop waiting for more work and finish */
	std::this_thread::sleep_for (std::chrono::seconds (1));
	proot->cond.notify_all ();
	/* join threads */
	join_threads (threads);
}

void
pm_mapreduce::start_threads (std::thread t[])
{
	int tt = params.num_map_workers + params.num_reduce_workers;
	int i = 0;

	/* MAP workers */
	for (; i < params.num_map_workers; i++) {
		t[i] = std::thread ([&] { map_thread (); });
	}
	/* REDUCE workers */
	for (; i < tt; i++) {
		t[i] = std::thread ([&] { reduce_thread (); });
	}
}

void
pm_mapreduce::join_threads (std::thread t[])
{
	int tt = params.num_map_workers + params.num_reduce_workers;
	for (int i = 0; i < tt; i++) {
		t[i].join ();
	}
}

void
pm_mapreduce::ret_available_map_task (pmem::obj::persistent_ptr<list_entry> &tsk,
                                      bool &all_done)
{
	auto proot = pop.root ();
	auto task_list = &(proot->tlist);

	/* LOCKED TRANSACTION */
	pmem::obj::transaction::run (
	pop,
	[&] {
		all_done = false;
		if ((task_list->ret_map (tsk)) != 0) {
			tsk = nullptr;
			all_done = task_list->all_map_done ();
		} else
			tsk->set_status (pop, TASK_ST_BUSY);
	},
	proot->pmutex);
}

void
pm_mapreduce::process_map_task (pmem::obj::persistent_ptr<list_entry> tsk)
{
	auto proot = pop.root ();
	auto task_list = &(proot->tlist);

	/*** MAIN TRANSACTION ***/
	pmem::obj::transaction::run (pop, [&] {
		/* This is the reduce task that will
		   be outputed */
		pmem::obj::persistent_ptr<list_entry> new_red_tsk;
		new_red_tsk = pmem::obj::make_persistent<list_entry> (pop);
		new_red_tsk->set_task_type (pop, TASK_TYPE_REDUCE);

		/* going over all lines and calling the
		   user-defined
		   map() function for each one. */
		std::vector<std::string> filekeys;
		std::vector<size_t> filevalues;

		size_t offset = tsk->get_start_byte ();
		size_t input_len = strlen (proot->input->data ());

		size_t nlines = tsk->get_n_lines ();
		for (size_t l = 0; l < nlines; l++) {
			std::string line;
			get_line (proot->input->data (), input_len, offset, line);
			offset += line.length () + 1;
			std::vector<std::string> keys;
			std::vector<size_t> values;
			/************************/
			map (line, keys, values);
			/************************/
			filekeys.insert (filekeys.end (), keys.begin (),
			                 keys.end ());
			filevalues.insert (filevalues.end (), values.begin (),
			                   values.end ());
		}
		/* sorting step */
		sort_kv (filekeys, filevalues);

		/* pmem allocating */
		size_t bytes = 0;
		for (std::vector<std::string>::iterator it = filekeys.begin ();
		     it != filekeys.end (); ++it) {
			bytes += sizeof (struct kv_tuple);
			bytes += strlen (it->c_str ());
			bytes += 1;
		}
		new_red_tsk->allocate_kv (pop, bytes);

		/* adding data to the allocated array */
		new_red_tsk->add_to_kv (pop, filekeys, filevalues);

		/* This locked transaction adds the lock to
		   the main transaction from this point
		   until the end */
		pmem::obj::transaction::run (
		pop,
		[&] {
			task_list->insert (pop, new_red_tsk);
			/* notify a reduce thread worker that
			   there is
			   new work waiting to be done */
			proot->cond.notify_one ();
			/* After the new reduce task has been
			   inserted in the list
			   we can safely switch this task's
			   status to done... */
			tsk->set_status (pop, TASK_ST_DONE);
		},
		proot->pmutex);
	}); /* end main transaction */
}

void
pm_mapreduce::map_thread (void)
{

	while (true) /* Active thread loop */
	{
		bool all_map_done = false;
		pmem::obj::persistent_ptr<list_entry> tsk;

		ret_available_map_task (tsk, all_map_done);

		if (tsk != nullptr)
			process_map_task (tsk);

		if (all_map_done == true)
			break;
	}
}

void pm_mapreduce::ret_available_red_task (
pmem::obj::persistent_ptr<list_entry> (&tsk)[2], bool &only_one_left,
bool &all_done)
{
	auto proot = pop.root ();
	auto task_list = &(proot->tlist);

	/* locked region */
	std::unique_lock<pmem::obj::mutex> guard (proot->pmutex);

	proot->cond.wait (
	proot->pmutex,
	[&] { /* conditional wait */
	      tsk[0] = nullptr;
	      tsk[1] = nullptr;
	      if ((task_list->ret_red (tsk[0])) != 0) {
		      if ((all_done = task_list->all_map_done ()) == true)
			      all_done = task_list->all_red_done ();
	      } else {
		      auto old_status = tsk[0]->get_status ();
		      tsk[0]->set_status (pop, TASK_ST_BUSY);
		      if ((task_list->ret_red (tsk[1])) != 0)
			      only_one_left = task_list->only_one_left ();
		      tsk[0]->set_status (pop, old_status);
	      }

	      return (tsk[0] != nullptr
		      && (tsk[1] != nullptr
		          || (tsk[1] == nullptr
		              && tsk[0]->get_status () != TASK_ST_REDUCED)
		          || (tsk[1] == nullptr
		              && tsk[0]->get_status () == TASK_ST_REDUCED
		              && only_one_left)))
		     || (tsk[0] == nullptr && all_done);
	});

	pmem::obj::transaction::run (pop, [&] {
		if (tsk[0] != nullptr)
			tsk[0]->set_status (pop, TASK_ST_BUSY);
		if (tsk[1] != nullptr)
			tsk[1]->set_status (pop, TASK_ST_BUSY);
	});

	guard.unlock ();
	/* end locked region */
}

void
pm_mapreduce::process_red_tasks (pmem::obj::persistent_ptr<list_entry> tsk[2],
                                 bool only_one_left)
{
	auto proot = pop.root ();
	auto task_list = &(proot->tlist);

	/*** MAIN TRANSACTION ***/
	pmem::obj::transaction::run (pop, [&] {

		/* this is the reduce task that will be outputed */
		pmem::obj::persistent_ptr<list_entry> new_red_tsk;
		new_red_tsk = pmem::obj::make_persistent<list_entry> (pop);
		new_red_tsk->set_task_type (pop, TASK_TYPE_REDUCE);
		/* data for the new reduce task */
		std::vector<std::string> allkeys;
		std::vector<size_t> allvalues;

		/* iterating over input tasks */
		for (uint8_t i = 0; i < 2; i++) {
			if (tsk[i] == nullptr)
				break;

			/* going over all keys and calling the
			   user-defined
			   reduce method for each one */
			char *kv = (char *)&(tsk[i]->get_kv_array ()[0]);

			struct kv_tuple *first;
			struct kv_tuple *current;

			first = (struct kv_tuple *)kv;
			current = (struct kv_tuple *)kv;

			size_t alloc_bytes = tsk[i]->get_alloc_bytes ();
			size_t offset = 0;

			while (offset < alloc_bytes) {

				/* grouping the values for the same key together */
				std::vector<size_t> valuesin;
				std::string firstkey = std::string (first->key);
				while (offset < alloc_bytes
				       && strcmp (first->key, current->key) == 0) {
					valuesin.push_back (current->value);
					offset += sizeof (struct kv_tuple)
					          + strlen (current->key) + 1;
					if (offset < alloc_bytes)
						current = (struct kv_tuple *)&(
						kv[offset]);
				}
				first = current;
				std::vector<size_t> valuesout;

				//*************************************/
				reduce (firstkey, valuesin, valuesout);
				//*************************************/

				for (std::vector<size_t>::iterator it
				     = valuesout.begin ();
				     it != valuesout.end (); ++it) {
					allkeys.push_back (firstkey);
					allvalues.push_back (*it);
				}
			}
			/* removing all key-value data to free
			  space */
			tsk[i]->delete_kv (pop);
		}
		/* sorting step */
		sort_kv (allkeys, allvalues);

		/* pmem allocating */
		size_t bytes = 0;
		for (std::vector<std::string>::iterator it = allkeys.begin ();
		     it != allkeys.end (); ++it)
			bytes
			+= sizeof (struct kv_tuple) + strlen (it->c_str ()) + 1;
		new_red_tsk->allocate_kv (pop, bytes);

		/* adding data to the allocated array */
		new_red_tsk->add_to_kv (pop, allkeys, allvalues);

		/* If only worked on one task, there are two
		   options for the new task status */
		if (tsk[1] == nullptr && only_one_left) /* completed last task
		                                           */
			new_red_tsk->set_status (pop, TASK_ST_DONE);
		else if (tsk[1] == nullptr) /* to be combined
		                               with other tasks
		                               */
			new_red_tsk->set_status (pop, TASK_ST_REDUCED);

		/* this locked transaction adds the lock to
		   the main transaction from this point
		   until the end */
		pmem::obj::transaction::run (
		pop,
		[&] {
			task_list->insert (pop, new_red_tsk);
			/* notify a reduce thread worker that
			   there is
			   new work waiting to be done */
			proot->cond.notify_one ();
			/* setting tasks to done */
			if (tsk[0])
				tsk[0]->set_status (pop, TASK_ST_DONE);
			if (tsk[1])
				tsk[1]->set_status (pop, TASK_ST_DONE);
		},
		proot->pmutex);
	}); /* end main transaction */
}

void
pm_mapreduce::reduce_thread (void)
{
	while (true) /* Active thread loop */
	{
		bool only_one_left = false;
		bool all_done = false;
		pmem::obj::persistent_ptr<list_entry> tsk[2];

		ret_available_red_task (tsk, only_one_left, all_done);

		if (tsk[0] != nullptr)
			process_red_tasks (tsk, only_one_left);

		if (all_done == true)
			break;
	}
}
