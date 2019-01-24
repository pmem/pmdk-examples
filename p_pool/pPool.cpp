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

#include "pPool.hpp"

/* Functions */

/* file_exists function  */
/****************************
 * This checks to see if the filename exists.  A boolean value (true or false)
 * will return to the calling function.
 *
 *****************************/
inline bool
file_exists (const std::string &name)
{
	std::ifstream f (name.c_str ());
	return f.good ();
}

/* print_header  */
/****************************
 * This function prints the header for the result screen.
 *
 *****************************/
void
print_header ()
{
	cout << endl
	     << endl
	     << "Pool ID" << setw (PRINT_WIDTH - 5) << " Thrd#:Itr#"
	     << setw (PRINT_WIDTH - 6) << " Objects " << setw (PRINT_WIDTH - 2)
	     << " Area/Volume " << setw (PRINT_WIDTH - 10) << " Result " << endl
	     << string (75, '*') << endl;
}

/* print_mesg function  */
/****************************
 * This function prints out the volume/area from various pools, threads, and
 *iterations. pool_id - is the pool number thread_is - is the current thread
 *number num_itr - is the iteration counter
 *
 *****************************/
void
print_mesg (int pool_id, int thread_id, int num_itr)
{
	cout << "Pool" << pool_id << setw (PRINT_WIDTH - 8) //<< "   Thread #:"
	                                                    //<<
	                                                    //setw(PRINT_WIDTH -
	                                                    //17)
	     << thread_id << ":" << setw (PRINT_WIDTH - 18) << num_itr
	     << setw (PRINT_WIDTH);
}

/* print_safe function  */
/****************************
 * This function prints using std::lock_guard for i/o thread synchronization
 * thread_id - is the current thread number
 * num_itr - is the iteration counter
 * pool1 and pool2 - persistent root pointer to the persistent object*
 *
 *****************************/
void
print_safe (int pool_id, int thread_id, int num_itr,
            pobj::persistent_ptr<root> pool1, pobj::persistent_ptr<root> pool2)
{
	std::lock_guard<std::mutex> lock (print_mutex); // thread safe
	if (pool_id == 1) {                             // access pool1
		print_mesg (pool_id, thread_id, num_itr);
		pool1->circle->print_area ();

		print_mesg (pool_id, thread_id, num_itr);
		pool1->sphere->print_vol ();
	} else { // access pool2
		print_mesg (pool_id, thread_id, num_itr);
		pool2->triangle->print_area ();

		print_mesg (pool_id, thread_id, num_itr);
		pool2->cone->print_vol ();
	}
}

/* access_pool function  */
/****************************
 * This function accesses various pools and calls various functions.
 * thread_is - is the current thread number
 * num_itr - is the iteration counter
 * pop1 and pop2 - has the address to the persistent pool
 * pool1 and pool2 - persistent root pointer to the persistent object
 *
 *****************************/
void
access_pool (int thread_id, int num_itr, pobj::pool<root> pop1,
             pobj::persistent_ptr<root> pool1, pobj::pool<root> pop2,
             pobj::persistent_ptr<root> pool2, double Rand_X)
{

	int remainder = thread_id % 2;

	for (int i = 0; i < num_itr; i++) { // Start For-loop

		if (remainder == 0) { // Process EVEN threads
			print_safe (1, thread_id, i, pool1, pool2);
			params vars = params (pop1, (Rand_X * 2.0) + 10.0,
			                      (Rand_X * 2.0) + 88.76,
			                      (Rand_X * 2.0) + 16.19);

			pool1->circle->set_values (&vars);
			pool1->sphere->set_values (&vars);

		} else { // Process ODD threads
			print_safe (2, thread_id, i, pool1, pool2);
			params vars = params (pop2, (Rand_X * 6.0) + 32.0,
			                      (Rand_X * 6.0) + 43.76,
			                      (Rand_X * 6.0) + 51.19);

			pool2->triangle->set_values (&vars);
			pool2->cone->set_values (&vars);
		}

	} // End For-loop
} // End access_pool function

/* show_usage function  */
/****************************
 * This function print out the correct syntax to run the program.
 */
static void
show_usage (string name)
{
	cerr << "Usage: " << name
	     << " <Pmem-Pool1-name> "
	        "<Pmem-Pool2-name> "
	        "<Number of threads "
	     << MIN_THREADS << "-" << MAX_THREADS << "> <Number of iterations "
	     << MIN_ITR << "-" << MAX_ITR << ">" << endl;
}
/* End Functions */

/* Main */
int
main (int argc, char *argv[])
{
	double Rand_X; // Random multiplier (double)

	pobj::pool<root> pop1, pop2;
	pobj::persistent_ptr<root> pool1, pool2;

	/* Reading parameters from command line */
	if (argc < 5) {
		show_usage (argv[0]);
		return 1;
	}

	string PERS_MEM_POOL1 = argv[1]; //
	string PERS_MEM_POOL2 = argv[2];
	int num_threads = atoll (argv[3]); // Number of threads
	int num_itr = atoll (argv[4]);     // Number of iterations

	if ((num_threads < MIN_THREADS) || (num_threads > MAX_THREADS)) {
		cout << endl
		     << "******** The number of threads is NOT within range "
		        "between <"
		     << MIN_THREADS << "-" << MAX_THREADS
		     << ">! Please try again. ********" << endl
		     << endl;
		show_usage (argv[0]);
		return 1;
	}
	thread threads[num_threads]; /* Define an array of threads */

	if ((num_itr < MIN_ITR) || (num_itr > MAX_ITR)) {
		cout << endl
		     << "******** The number of iterations is NOT within range "
		        "between <"
		     << MIN_ITR << "-" << MAX_ITR
		     << ">! Please try again! ********" << endl
		     << endl;
		show_usage (argv[0]);
		return 1;
	}
	/* Reading parameters from command line */


	/* Initialize the Random engine */
	random_device rd; // Used to obtain a seed for the random number engine
	mt19937 gen (rd ()); // Standard mersenne_twister_engine seeded with
	                     // rd()
	uniform_real_distribution<> dis (1.0, 100.0); // Generate a uniform of
	                                              // random double number
	                                              // between 1 to 100
	/* Initialize the Random engine */

	/* pool1  */
	cout << endl
	     << "PERS_MEM_POOL1: " << PERS_MEM_POOL1 << endl
	     << "LAYOUT: " << LAYOUT1 << endl;
	if (file_exists (PERS_MEM_POOL1) == false) /* New file */ {
		cout << endl
		     << "New File -- File create: " << PERS_MEM_POOL1 << endl;

		pop1 = pobj::pool<root>::create (PERS_MEM_POOL1, LAYOUT1,
		                                 POOLSIZE, S_IRUSR | S_IWUSR);
		pool1 = pop1.root ();
		params vars = params (pop1, 5.76, 10, 20.19);

		pobj::transaction::run (pop1, [&] {
			pool1->circle = pobj::make_persistent<Circle> (&vars);
			pool1->sphere = pobj::make_persistent<Sphere> (&vars);
		});
	} else /*file exists */ {
		cout << endl
		     << "File Exists -- Open file: " << PERS_MEM_POOL1 << endl;

		pop1 = pobj::pool<root>::open (PERS_MEM_POOL1, LAYOUT1);
		pool1 = pop1.root ();
	}
	/* pool1 End */

	/* pool2  */
	cout << endl
	     << "PERS_MEM_POOL2: " << PERS_MEM_POOL2 << endl
	     << "LAYOUT2: " << LAYOUT2 << endl;
	if (file_exists (PERS_MEM_POOL2) == false) /* New file */ {
		cout << endl
		     << "New File -- File create: " << PERS_MEM_POOL2 << endl;

		pop2 = pobj::pool<root>::create (PERS_MEM_POOL2, LAYOUT2,
		                                 POOLSIZE, S_IRUSR | S_IWUSR);
		pool2 = pop2.root ();
		params vars = params (pop2, 7.76, 11, 22.19);

		pobj::transaction::run (pop2, [&] {
			pool2->triangle = pobj::make_persistent<Triangle> (&vars);
			pool2->cone = pobj::make_persistent<Cone> (&vars);
		});
	} else /*file exists */ {
		cout << endl
		     << "File Exists -- Open file: " << PERS_MEM_POOL2 << endl;
		pop2 = pobj::pool<root>::open (PERS_MEM_POOL2, LAYOUT2);
		pool2 = pop2.root ();
	}
	/* pool2 End */

	print_header (); // Print header for the result screen

	/* Threading */
	for (int i = 0; i < num_threads; i++) {
		Rand_X = dis (gen); // Generate random numbers
		threads[i] = thread (access_pool, i, num_itr, pop1, pool1, pop2,
		                     pool2, Rand_X);
	}

	/* Join Threads */
	for (int i = 0; i < num_threads; i++) {
		threads[i].join ();
	}
	/* Threading End */

	/* Cleanup */
	/* Close persisent pools */
	pop2.close ();
	pop1.close ();

	return 0;
}
/* End Main */
