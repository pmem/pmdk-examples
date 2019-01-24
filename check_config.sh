#!/bin/bash

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

pkg-config --exists libpmem
if (( $? > 0 )); then
	echo libpmem not found
	exit 1
fi
pkg-config --exists libpmemblk
if (( $? > 0 )); then
	echo libpmemblk not found
	exit 1
fi
pkg-config --exists libpmemobj
if (( $? > 0 )); then
	echo libpmemobj not found
	exit 1
fi
pkg-config --exists libpmemobj++
if (( $? > 0 )); then
	echo libpmemobj++ not found
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
