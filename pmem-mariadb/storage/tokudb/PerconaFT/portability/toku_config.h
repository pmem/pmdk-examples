/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#ident "$Id$"
/*======
This file is part of PerconaFT.


Copyright (c) 2006, 2015, Percona and/or its affiliates. All rights reserved.

    PerconaFT is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License, version 2,
    as published by the Free Software Foundation.

    PerconaFT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PerconaFT.  If not, see <http://www.gnu.org/licenses/>.

----------------------------------------

    PerconaFT is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License, version 3,
    as published by the Free Software Foundation.

    PerconaFT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with PerconaFT.  If not, see <http://www.gnu.org/licenses/>.
======= */

#ident "Copyright (c) 2006, 2015, Percona and/or its affiliates. All rights reserved."

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define TOKUDB_REVISION 0

/* #undef TOKU_DEBUG_PARANOID */
/* #undef USE_VALGRIND */
#define HAVE_ALLOCA_H 1
#define HAVE_ARPA_INET_H 1
#define HAVE_BYTESWAP_H 1
#define HAVE_ENDIAN_H 1
#define HAVE_FCNTL_H 1
#define HAVE_INTTYPES_H 1
/* #undef HAVE_LIBKERN_OSATOMIC_H */
/* #undef HAVE_LIBKERN_OSBYTEORDER_H */
#define HAVE_LIMITS_H 1
/* #undef HAVE_MACHINE_ENDIAN_H */
#define HAVE_MALLOC_H 1
/* #undef HAVE_MALLOC_MALLOC_H */
/* #undef HAVE_MALLOC_NP_H */
#define HAVE_PTHREAD_H 1
/* #undef HAVE_PTHREAD_NP_H */
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_SYSCALL_H 1
/* #undef HAVE_SYS_ENDIAN_H */
#define HAVE_SYS_FILE_H 1
/* #undef HAVE_SYS_MALLOC_H */
#define HAVE_SYS_RESOURCE_H 1
#define HAVE_SYS_STATVFS_H 1
#define HAVE_SYS_SYSCALL_H 1
#define HAVE_SYS_SYSCTL_H 1
/* #undef HAVE_SYS_SYSLIMITS_H */
#define HAVE_SYS_TIME_H 1
#define HAVE_UNISTD_H 1

#define HAVE_M_MMAP_THRESHOLD 1
#define HAVE_CLOCK_REALTIME 1
#define HAVE_O_DIRECT 1
/* #undef HAVE_F_NOCACHE */

#define HAVE_MAP_ANONYMOUS 1
#define HAVE_MINCORE 1
#define HAVE_PR_SET_PTRACER 1
#define HAVE_PR_SET_PTRACER_ANY 1
/* #undef HAVE_MALLOC_SIZE */
#define HAVE_MALLOC_USABLE_SIZE 1
#define HAVE_MEMALIGN 1
#define HAVE_VALLOC 1
#define HAVE_NRAND48 1
#define HAVE_RANDOM_R 1

#define HAVE_PTHREAD_RWLOCKATTR_SETKIND_NP 1
#define HAVE_PTHREAD_YIELD 1
/* #undef HAVE_PTHREAD_YIELD_NP */
/* #undef HAVE_PTHREAD_THREADID_NP */
/* #undef HAVE_PTHREAD_GETTHREADID_NP */

#define PTHREAD_YIELD_RETURNS_INT 1
/* #undef PTHREAD_YIELD_RETURNS_VOID */

#define HAVE_SCHED_GETCPU 1

#define HAVE_GNU_TLS 1

#endif /* __CONFIG_H__ */
