#!/bin/bash

#
# Copyright (c) 2017-2019, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Intel Corporation nor the names of its contributors
#       may be used to endorse or promote products derived from this software
#       without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

if ! command -v pkg-config &> /dev/null ; then
	echo pkg-config not found
	exit 1
fi

if ! command -v gcc &> /dev/null ; then
	echo gcc not found
	exit 1
fi

if ! command -v g++ &> /dev/null ; then
	echo g++ not found
	exit 1
fi

ret=$(pkg-config --modversion libpmem)
if (( $? > 0 )); then
	echo libpmem not found
	exit 1
fi
ret1=$(echo "$ret < 1.5" | bc -l)
if (( $ret1 > 0 )); then
	echo need libpmem v1.5 or higher
	exit 1
fi
ret=$(pkg-config --modversion libpmemblk)
if (( $? > 0 )); then
	echo libpmemblk not found
	exit 1
fi
ret1=$(echo "$ret < 1.5" | bc -l)
if (( $ret1 > 0 )); then
	echo need libpmemblk v1.5 or higher
	exit 1
fi
ret=$(pkg-config --modversion libpmemobj)
if (( $? > 0 )); then
	echo libpmemobj not found
	exit 1
fi
ret1=$(echo "$ret < 1.5" | bc -l)
if (( $ret1 > 0 )); then
	echo need libpmemobj v1.5 or higher
	exit 1
fi
ret=$(pkg-config --modversion libpmemobj++)
if (( $? > 0 )); then
	echo libpmemobj++ not found
	exit 1
fi
ret1=$(echo "$ret < 1.5" | bc -l)
if (( $ret1 > 0 )); then
	echo need libpmemobj++ v1.5 or higher
	exit 1
fi

if ! command -v javac &> /dev/null ; then
	echo WARNING !! javac not found, PCJ and LLPL samples will not build
fi

if [ -z "$PCJ_HOME" ]; then
	echo WARNING !! PCJ_HOME is not defined, PCJ samples will not build
fi

if [ -z "$LLPL_HOME" ]; then
	echo WARNING !! LLPL_HOME is not defined, LLPL samples will not build
fi

touch .configok
