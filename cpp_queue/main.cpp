/*
 * Copyright 2017-2019, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "libpmemobj++/make_persistent_atomic.hpp"
#include "persistent_queue.hpp"
#include "volatile_queue.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <functional>

#ifndef _WIN32

#include <unistd.h>
#define CREATE_MODE_RW (S_IWUSR | S_IRUSR)

#else

#include <io.h>
#define CREATE_MODE_RW (S_IWRITE | S_IREAD)

#endif

namespace
{

/* The layout string */
const char *LAYOUT = "queue";

/* Available queue operations */
enum queue_op {
	UNKNOWN_QUEUE_OP,
	QUEUE_PUSH,
	QUEUE_POP,
	QUEUE_SHOW,

	MAX_QUEUE_OP
};

/* Queue operations strings */
std::string ops_str[queue_op::MAX_QUEUE_OP] = {"", "push", "pop", "show"};

/*
 * parse_queue_op -- parses the operation string and returns matching queue_op
 */
queue_op
parse_queue_op(const char *str)
{

	for (int i = 0; i < queue_op::MAX_QUEUE_OP; ++i)
		if (ops_str[i] == str)
			return static_cast<queue_op>(i);

	return queue_op::UNKNOWN_QUEUE_OP;
}

/*
 * file_exists -- checks whether the given file exists
 */
bool
file_exists(const std::string &path)
{
	std::ifstream f(path.c_str());
	return f.good();
}

/*
 * handle_op -- handles the requested queue operation
 */
template <typename T>
void
handle_op(T &queue, queue_op op, std::function<long long int()> val)
{
	switch (op) {
		case QUEUE_PUSH:
			queue->push(val());
			break;
		case QUEUE_POP:
			std::cout << queue->pop() << std::endl;
			break;
		case QUEUE_SHOW:
			std::cout << queue->show();
			break;
		default:
			throw std::invalid_argument("invalid queue operation");
			break;
	}
}

/* The root object definition */
struct root {
	pmem::obj::persistent_ptr<pmem::examples::PersistentQueue> pqueue;
	pmem::examples::VolatileQueue *vqueue;
};
}

int
main(int argc, char *argv[])
{
	if (argc < 4) {
		std::cerr << "usage: " << argv[0]
			  << " [v | p] file-name [push [value]|pop|show]"
			  << std::endl;
		return 1;
	}

	std::string path{argv[2]};
	auto op = parse_queue_op(argv[3]);

	/* handle file creation/open */
	pmem::obj::pool<root> pop;
	if (file_exists(path)) {
		pop = pmem::obj::pool<root>::open(path, LAYOUT);
	} else {
		pop = pmem::obj::pool<root>::create(
			path, LAYOUT, PMEMOBJ_MIN_POOL * 20, CREATE_MODE_RW);

		/* allocate the persistent queue only when the pool is new */
		pmem::obj::make_persistent_atomic<
			pmem::examples::PersistentQueue>(
			pop, pop.root()->pqueue);
	}

	/* get the root object pointer */
	auto proot = pop.root();

	/* always allocate the volatile queue */
	proot->vqueue = new pmem::examples::VolatileQueue();

	/* lambda get value for push operation */
	auto value = [&argv]() -> long long int { return atoll(argv[4]); };

	std::string type{argv[1]};
	if (type == "v")
		handle_op(proot->vqueue, op, value);
	else if (type == "p")
		handle_op(proot->pqueue, op, value);
	else
		throw std::runtime_error(
			std::string{"Unknown queue type selected"} += type);

	pop.close();

	return 0;
}
