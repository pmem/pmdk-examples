# Install script for directory: /home/tcs/server

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xReadmex" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES
    "/home/tcs/server/README.md"
    "/home/tcs/server/CREDITS"
    "/home/tcs/server/COPYING"
    "/home/tcs/server/COPYING.thirdparty"
    "/home/tcs/server/EXCEPTIONS-CLIENT"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xReadmex" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES
    "/home/tcs/server/Docs/INSTALL-BINARY"
    "/home/tcs/server/Docs/README-wsrep"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/tcs/server/unittest/mytap/cmake_install.cmake")
  include("/home/tcs/server/unittest/strings/cmake_install.cmake")
  include("/home/tcs/server/unittest/examples/cmake_install.cmake")
  include("/home/tcs/server/unittest/mysys/cmake_install.cmake")
  include("/home/tcs/server/unittest/my_decimal/cmake_install.cmake")
  include("/home/tcs/server/unittest/json_lib/cmake_install.cmake")
  include("/home/tcs/server/unittest/sql/cmake_install.cmake")
  include("/home/tcs/server/libmariadb/cmake_install.cmake")
  include("/home/tcs/server/storage/archive/cmake_install.cmake")
  include("/home/tcs/server/storage/blackhole/cmake_install.cmake")
  include("/home/tcs/server/storage/cassandra/cmake_install.cmake")
  include("/home/tcs/server/storage/connect/cmake_install.cmake")
  include("/home/tcs/server/storage/csv/cmake_install.cmake")
  include("/home/tcs/server/storage/federated/cmake_install.cmake")
  include("/home/tcs/server/storage/federatedx/cmake_install.cmake")
  include("/home/tcs/server/storage/heap/cmake_install.cmake")
  include("/home/tcs/server/storage/innobase/cmake_install.cmake")
  include("/home/tcs/server/storage/maria/cmake_install.cmake")
  include("/home/tcs/server/storage/mroonga/cmake_install.cmake")
  include("/home/tcs/server/storage/myisam/cmake_install.cmake")
  include("/home/tcs/server/storage/myisammrg/cmake_install.cmake")
  include("/home/tcs/server/storage/oqgraph/cmake_install.cmake")
  include("/home/tcs/server/storage/perfschema/cmake_install.cmake")
  include("/home/tcs/server/storage/pmdk/cmake_install.cmake")
  include("/home/tcs/server/storage/rocksdb/cmake_install.cmake")
  include("/home/tcs/server/storage/sequence/cmake_install.cmake")
  include("/home/tcs/server/storage/sphinx/cmake_install.cmake")
  include("/home/tcs/server/storage/spider/cmake_install.cmake")
  include("/home/tcs/server/storage/test_sql_discovery/cmake_install.cmake")
  include("/home/tcs/server/storage/tokudb/cmake_install.cmake")
  include("/home/tcs/server/plugin/audit_null/cmake_install.cmake")
  include("/home/tcs/server/plugin/auth_dialog/cmake_install.cmake")
  include("/home/tcs/server/plugin/auth_ed25519/cmake_install.cmake")
  include("/home/tcs/server/plugin/auth_examples/cmake_install.cmake")
  include("/home/tcs/server/plugin/auth_gssapi/cmake_install.cmake")
  include("/home/tcs/server/plugin/auth_pam/cmake_install.cmake")
  include("/home/tcs/server/plugin/auth_pipe/cmake_install.cmake")
  include("/home/tcs/server/plugin/auth_socket/cmake_install.cmake")
  include("/home/tcs/server/plugin/aws_key_management/cmake_install.cmake")
  include("/home/tcs/server/plugin/cracklib_password_check/cmake_install.cmake")
  include("/home/tcs/server/plugin/daemon_example/cmake_install.cmake")
  include("/home/tcs/server/plugin/debug_key_management/cmake_install.cmake")
  include("/home/tcs/server/plugin/disks/cmake_install.cmake")
  include("/home/tcs/server/plugin/example_key_management/cmake_install.cmake")
  include("/home/tcs/server/plugin/feedback/cmake_install.cmake")
  include("/home/tcs/server/plugin/file_key_management/cmake_install.cmake")
  include("/home/tcs/server/plugin/fulltext/cmake_install.cmake")
  include("/home/tcs/server/plugin/handler_socket/cmake_install.cmake")
  include("/home/tcs/server/plugin/locale_info/cmake_install.cmake")
  include("/home/tcs/server/plugin/metadata_lock_info/cmake_install.cmake")
  include("/home/tcs/server/plugin/qc_info/cmake_install.cmake")
  include("/home/tcs/server/plugin/query_response_time/cmake_install.cmake")
  include("/home/tcs/server/plugin/server_audit/cmake_install.cmake")
  include("/home/tcs/server/plugin/simple_password_check/cmake_install.cmake")
  include("/home/tcs/server/plugin/sql_errlog/cmake_install.cmake")
  include("/home/tcs/server/plugin/user_variables/cmake_install.cmake")
  include("/home/tcs/server/plugin/userstat/cmake_install.cmake")
  include("/home/tcs/server/plugin/versioning/cmake_install.cmake")
  include("/home/tcs/server/plugin/win_auth_client/cmake_install.cmake")
  include("/home/tcs/server/plugin/wsrep_info/cmake_install.cmake")
  include("/home/tcs/server/include/cmake_install.cmake")
  include("/home/tcs/server/dbug/cmake_install.cmake")
  include("/home/tcs/server/strings/cmake_install.cmake")
  include("/home/tcs/server/vio/cmake_install.cmake")
  include("/home/tcs/server/mysys/cmake_install.cmake")
  include("/home/tcs/server/mysys_ssl/cmake_install.cmake")
  include("/home/tcs/server/client/cmake_install.cmake")
  include("/home/tcs/server/extra/cmake_install.cmake")
  include("/home/tcs/server/libservices/cmake_install.cmake")
  include("/home/tcs/server/sql/share/cmake_install.cmake")
  include("/home/tcs/server/tests/cmake_install.cmake")
  include("/home/tcs/server/sql/cmake_install.cmake")
  include("/home/tcs/server/wsrep/cmake_install.cmake")
  include("/home/tcs/server/mysql-test/cmake_install.cmake")
  include("/home/tcs/server/mysql-test/lib/My/SafeProcess/cmake_install.cmake")
  include("/home/tcs/server/sql-bench/cmake_install.cmake")
  include("/home/tcs/server/man/cmake_install.cmake")
  include("/home/tcs/server/scripts/cmake_install.cmake")
  include("/home/tcs/server/support-files/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/tcs/server/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
