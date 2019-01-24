# Install script for directory: /home/tcs/server/man

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local/mysql")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xManPagesServerx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1" TYPE FILE FILES
    "/home/tcs/server/man/innochecksum.1"
    "/home/tcs/server/man/my_print_defaults.1"
    "/home/tcs/server/man/myisam_ftdump.1"
    "/home/tcs/server/man/myisamchk.1"
    "/home/tcs/server/man/aria_chk.1"
    "/home/tcs/server/man/aria_dump_log.1"
    "/home/tcs/server/man/aria_ftdump.1"
    "/home/tcs/server/man/aria_pack.1"
    "/home/tcs/server/man/aria_read_log.1"
    "/home/tcs/server/man/myisamlog.1"
    "/home/tcs/server/man/myisampack.1"
    "/home/tcs/server/man/mysql.server.1"
    "/home/tcs/server/man/mysql_convert_table_format.1"
    "/home/tcs/server/man/mysql_fix_extensions.1"
    "/home/tcs/server/man/mysql_install_db.1"
    "/home/tcs/server/man/mysql_secure_installation.1"
    "/home/tcs/server/man/mysql_setpermission.1"
    "/home/tcs/server/man/mysql_tzinfo_to_sql.1"
    "/home/tcs/server/man/mysql_upgrade.1"
    "/home/tcs/server/man/mysqld_multi.1"
    "/home/tcs/server/man/mysqld_safe.1"
    "/home/tcs/server/man/mysqldumpslow.1"
    "/home/tcs/server/man/mysqlhotcopy.1"
    "/home/tcs/server/man/mysqltest.1"
    "/home/tcs/server/man/perror.1"
    "/home/tcs/server/man/replace.1"
    "/home/tcs/server/man/resolve_stack_dump.1"
    "/home/tcs/server/man/resolveip.1"
    "/home/tcs/server/man/mariadb-service-convert.1"
    "/home/tcs/server/man/mysqld_safe_helper.1"
    "/home/tcs/server/man/tokuftdump.1"
    "/home/tcs/server/man/wsrep_sst_common.1"
    "/home/tcs/server/man/wsrep_sst_mysqldump.1"
    "/home/tcs/server/man/wsrep_sst_rsync.1"
    "/home/tcs/server/man/wsrep_sst_xtrabackup-v2.1"
    "/home/tcs/server/man/wsrep_sst_xtrabackup.1"
    "/home/tcs/server/man/galera_recovery.1"
    "/home/tcs/server/man/galera_new_cluster.1"
    "/home/tcs/server/man/tokuft_logdump.1"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xManPagesServerx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man8" TYPE FILE FILES "/home/tcs/server/man/mysqld.8")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xManPagesClientx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1" TYPE FILE FILES
    "/home/tcs/server/man/msql2mysql.1"
    "/home/tcs/server/man/mysql.1"
    "/home/tcs/server/man/mysql_find_rows.1"
    "/home/tcs/server/man/mysql_waitpid.1"
    "/home/tcs/server/man/mysqlaccess.1"
    "/home/tcs/server/man/mysqladmin.1"
    "/home/tcs/server/man/mysqlbinlog.1"
    "/home/tcs/server/man/mysqlcheck.1"
    "/home/tcs/server/man/mysqldump.1"
    "/home/tcs/server/man/mysqlimport.1"
    "/home/tcs/server/man/mysqlshow.1"
    "/home/tcs/server/man/mysqlslap.1"
    "/home/tcs/server/man/mysql_plugin.1"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xManPagesDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1" TYPE FILE FILES "/home/tcs/server/man/mysql_config.1")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xManPagesTestx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1" TYPE FILE FILES
    "/home/tcs/server/man/mysql-stress-test.pl.1"
    "/home/tcs/server/man/mysql-test-run.pl.1"
    "/home/tcs/server/man/mysql_client_test.1"
    "/home/tcs/server/man/mysqltest_embedded.1"
    "/home/tcs/server/man/mysql_client_test_embedded.1"
    "/home/tcs/server/man/my_safe_process.1"
    )
endif()

