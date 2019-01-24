AC_INIT([grep], [0.1])
AM_INIT_AUTOMAKE
AC_PROG_CXX
PKG_CHECK_MODULES([PMEM], [libpmem >= 1.5])
PKG_CHECK_MODULES([PMEMOBJ], [libpmemobj >= 1.5])
PKG_CHECK_MODULES([PMEMOBJPP], [libpmemobj++ >= 1.5])
AC_CHECK_LIB(pthread, pthread_create)
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
