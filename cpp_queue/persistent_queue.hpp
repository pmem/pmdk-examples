/*
 * Copyright 2017, Intel Corporation
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

#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>

#include <utility>

namespace pmem
{

namespace examples
{

/*!
 * \brief Persistent memory list-based queue.
 *
 * A simple, not template based, implementation of a queue using
 * libpmemobj C++ API. It demonstrates the basic features of persistent_ptr<>
 * and p<> classes.
 */
class PersistentQueue {
public:
	/*!
	 * \brief Inserts a new element at the end of the queue.
	 *
	 * \param value the value to be pushed to the end of the queue.
	 */
	void push(long long int value);

	/*!
	 * \brief Removes the first element in the queue.
	 *
	 * \return the first element in the queue.
	 */
	long long int pop();

	/*!
	 * \brief Prints the entire contents of the queue.
	 *
	 * \return a string representation of the queue.
	 */
	std::string show(void) const;

private:
	/*!
	 * \brief Internal node definition.
	 */
	struct Node {

		/*!
		 * \brief Constructor.
		 *
		 * \param val the value held by this node.
		 * \param n the next node in the queue.
		 */
		Node(long long int val, obj::persistent_ptr<Node> n)
		    : next(std::move(n)), value(std::move(val))
		{
		}

		/* Pointer to the next node */
		obj::persistent_ptr<Node> next;
		/* Value held by this node */
		obj::p<long long int> value;
	};

	/* The head of the queue */
	obj::persistent_ptr<Node> head;
	/* The tail of the queue */
	obj::persistent_ptr<Node> tail;
};

} // namespace examples
} // namespace pmem
