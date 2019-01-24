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


#include "list_entry.hpp"

list_entry::list_entry (pmem::obj::pool_base &pop)
{

	pmem::obj::transaction::run (pop, [&] {

		this->next = nullptr;
		this->status = TASK_ST_NEW;
		this->task_type = TASK_TYPE_NOTYPE;
		this->start_byte = 0;
		this->n_lines = 0;
		this->kv = nullptr;
		this->kv_size = 0;
		this->alloc_bytes = 0;

	});
}

void
list_entry::set_next (pmem::obj::pool_base &pop,
                      pmem::obj::persistent_ptr<list_entry> next)
{

	pmem::obj::transaction::run (pop, [&] {

		this->next = next;

	});
}

void
list_entry::set_status (pmem::obj::pool_base &pop, uint8_t status)
{

	pmem::obj::transaction::run (pop, [&] {

		this->status = status;

	});
}

void
list_entry::set_task_type (pmem::obj::pool_base &pop, uint8_t task_type)
{

	pmem::obj::transaction::run (pop, [&] {

		this->task_type = task_type;

	});
}

void
list_entry::set_start_byte (pmem::obj::pool_base &pop, size_t s_byte)
{

	pmem::obj::transaction::run (pop, [&] {

		this->start_byte = s_byte;

	});
}

void
list_entry::set_n_lines (pmem::obj::pool_base &pop, size_t n)
{

	pmem::obj::transaction::run (pop, [&] {

		this->n_lines = n;

	});
}

void
list_entry::allocate_kv (pmem::obj::pool_base &pop, size_t bytes)
{

	pmem::obj::transaction::run (pop, [&] {

		kv = pmem::obj::make_persistent<char[]> (bytes);
		alloc_bytes = bytes;

	});
}

void
list_entry::add_to_kv (pmem::obj::pool_base &pop, std::vector<std::string> &keys,
                       std::vector<size_t> &values)
{

	pmem::obj::transaction::run (pop, [&] {

		struct kv_tuple *kvt;
		size_t offset = 0;

		for (size_t i = 0; i < keys.size (); i++) {

			kvt = (struct kv_tuple *)&(kv[offset]);
			kvt->value = values[i];
			strcpy (kvt->key, keys[i].c_str ());
			offset
			+= sizeof (struct kv_tuple) + strlen (kvt->key) + 1;
		}

		kv_size = keys.size ();

	});
}

void
list_entry::delete_kv (pmem::obj::pool_base &pop)
{

	pmem::obj::transaction::run (pop, [&] {

		pmem::obj::delete_persistent<char[]> (kv, alloc_bytes);
		kv = nullptr;
		kv_size = 0;
		alloc_bytes = 0;

	});
}
