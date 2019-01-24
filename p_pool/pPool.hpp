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

#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/mutex.hpp>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <mutex>
#include <random>
#include <string.h>
#include <string>
#include <thread>

#define MIN_THREADS 1
#define MAX_THREADS 80
#define MIN_ITR 1
#define MAX_ITR 30
#define PRINT_WIDTH 20
#define DIVISOR 100000.0 // reduce the value by 100K if it gets too large
#define P_DIGIT 2        // Precision digits
#define LAYOUT1 "pool_1"
#define LAYOUT2 "pool_2"
#define POOLSIZE ((int)(1024 * 1024 * 256)) /* 256 MB */

using namespace std;
namespace pobj = pmem::obj;

/* globals */
struct root;
std::mutex print_mutex;

struct params {
	pobj::pool<root> pop;
	double height, r, base;

	params (pobj::pool<root> pop, double height, double r, double base)
	: pop (pop), height (height), r (r), base (base)
	{
	}
};

/* Cone class  */
/****************************
 * This class has all the functions to compute the volume of a Cone
 * and print out the result.
 *
 *****************************/
class Cone
{
	private:
	pobj::p<double> radius;
	pobj::p<double> height;
	pobj::mutex pmutex;

	public:
	Cone (params *pdata)
	{
		pobj::transaction::run (pdata->pop,
		                            [&] {
			                            radius = pdata->r;
			                            height = pdata->height;
		                            },
		                            pmutex);
	}

	double
	get_vol ()
	{
		return ((M_PI * pow (radius, 2.0) * height) / 3.0);
	}

	string
	name ()
	{
		return string ("Cone");
	}

	void
	print_vol ()
	{
		cout << fixed;
		cout << setw (PRINT_WIDTH - 5) << name () << setw (PRINT_WIDTH)
		     << " (volume): " << setprecision (P_DIGIT) << get_vol ()
		     << endl;
	}

	void
	set_values (params *pdata)
	{
		pobj::transaction::run (
		pdata->pop,
		[&] {
			if (radius > DIVISOR) {
				radius = radius / DIVISOR;
				height = height / DIVISOR;
				cout << endl
				     << " Cone radius+height Reduce by "
				     << DIVISOR << endl;
			} else {
				radius = radius + pdata->base;
				height = height + pdata->height;
			}
		},
		pmutex);
	}
};

/* Triangle class  */
/****************************
 * This class has all the functions to compute the area of a Triangle
 * and print out the result.
 *
 *****************************/
class Triangle
{
	private:
	pobj::p<double> base;
	pobj::p<double> height;
	pobj::mutex pmutex;

	public:
	Triangle (params *pdata)
	{
		pobj::transaction::run (pdata->pop,
		                            [&] {
			                            base = pdata->base;
			                            height = pdata->height;
		                            },
		                            pmutex);
	}
	double
	get_area ()
	{
		return ((height * base) / 2.0);
	}

	string
	name ()
	{
		return string ("Triangle");
	}

	void
	print_area ()
	{
		cout << fixed;
		cout << setw (PRINT_WIDTH - 5) << name () << setw (PRINT_WIDTH)
		     << " (area): " << setprecision (P_DIGIT) << get_area ()
		     << endl;
	}

	void
	set_values (params *pdata)
	{
		pobj::transaction::run (
		pdata->pop,
		[&] {
			if (base > DIVISOR) {
				base = base / DIVISOR;
				height = height / DIVISOR;
				cout << endl
				     << " Triangle base+height Reduce by "
				     << DIVISOR << endl;
			} else {
				base = base + pdata->base;
				height = height + pdata->height;
			}
		},
		pmutex);
	}
};

/* Sphere class  */
/****************************
 * This class has all the functions to compute the volume of a Sphere
 * and print out the result.
 *
 *****************************/
class Sphere
{
	private:
	pobj::p<double> radius;
	pobj::mutex pmutex;

	public:
	Sphere (params *pdata)
	{
		pobj::transaction::run (pdata->pop,
		                            [&] {
			                            radius = pdata->r; // 8
			                                               // bytes
			                                               // only
		                            },
		                            pmutex);
	}

	double
	get_vol ()
	{
		return ((pow (radius, 3.0) * 4.0 * M_PI) / 3.0);
	}

	string
	name ()
	{
		return string ("Sphere");
	}

	void
	print_vol ()
	{
		cout << fixed;
		cout << setw (PRINT_WIDTH - 5) << name () << setw (PRINT_WIDTH)
		     << " (volume): " << setprecision (P_DIGIT) << get_vol ()
		     << endl;
	}
	void
	set_values (params *pdata)
	{
		pmutex.lock (); // Persistent lock
		if (radius > DIVISOR) {
			radius = radius / DIVISOR; // 8 bytes only
			cout << endl
			     << " Sphere radius Reduce by " << DIVISOR << ":"
			     << radius << endl;
		} else
			radius = radius + pdata->r; // 8 bytes only
		pdata->pop.persist (radius);
		pmutex.unlock (); // Persistent unlock
	}
};

/* Circle class  */
/****************************
 * This class has all the functions to compute the area of a Circle
 * and print out the result.
 *
 *****************************/
class Circle
{
	private:
	pobj::p<double> radius;
	pobj::mutex pmutex;

	public:
	Circle (params *pdata)
	{
		pobj::transaction::run (pdata->pop,
		                            [&] {
			                            radius = pdata->r; // 8
			                                               // bytes
			                                               // only
		                            },
		                            pmutex);
	}

	string
	name ()
	{
		return string ("Circle");
	}

	double
	get_area ()
	{
		return (pow (radius, 2.0) * M_PI);
	}

	void
	print_area ()
	{
		cout << fixed;
		cout << setw (PRINT_WIDTH - 5) << string (name ())
		     << setw (PRINT_WIDTH)
		     << " (area): " << setprecision (P_DIGIT) << get_area ()
		     << endl;
	}

	void
	set_values (params *pdata)
	{
		pmutex.lock (); // Persistent lock
		if (radius > DIVISOR) {
			radius = radius / DIVISOR; // 8 bytes only
			cout << endl
			     << " Circle radius Reduce by " << DIVISOR << endl;
		} else
			radius = radius + pdata->r; // 8 bytes only
		pdata->pop.persist (radius);
		pmutex.unlock (); // Persistent unlock
	}
};

/* root structure  */
/****************************
 * This root structure contains all the connections various pools and persistent
 *pointers to the persistent objects. Using this root structure component to
 *access all main has all the functions to compute the area of a Circle and
 *print out the result.
 *
 *****************************/
struct root {
	pobj::persistent_ptr<Circle> circle;
	pobj::persistent_ptr<Sphere> sphere;
	pobj::persistent_ptr<Triangle> triangle;
	pobj::persistent_ptr<Cone> cone;
};
