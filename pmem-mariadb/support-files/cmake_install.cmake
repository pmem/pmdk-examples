# Install script for directory: /home/tcs/server/support-files

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xIniFilesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/support-files" TYPE FILE FILES "/home/tcs/server/support-files/wsrep.cnf")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xServer_Scriptsx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/support-files" TYPE PROGRAM FILES "/home/tcs/server/support-files/mysqld_multi.server")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xServer_Scriptsx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/support-files" TYPE PROGRAM FILES "/home/tcs/server/support-files/mysql-log-rotate")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xServer_Scriptsx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/support-files" TYPE PROGRAM FILES "/home/tcs/server/support-files/binary-configure")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xServer_Scriptsx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/support-files" TYPE PROGRAM FILES "/home/tcs/server/support-files/wsrep_notify")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSupportFilesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/support-files" TYPE FILE FILES "/home/tcs/server/support-files/magic")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSupportFilesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/support-files" TYPE DIRECTORY FILES "/home/tcs/server/support-files/policy")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" TYPE FILE FILES "/home/tcs/server/support-files/mariadb.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelopmentx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/aclocal" TYPE FILE FILES "/home/tcs/server/support-files/mysql.m4")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSupportFilesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/support-files" TYPE PROGRAM FILES "/home/tcs/server/support-files/mysql.server")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSupportFilesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/support-files/systemd" TYPE FILE FILES
    "/home/tcs/server/support-files/use_galera_new_cluster.conf"
    "/home/tcs/server/support-files/mariadb.service"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xSupportFilesx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/support-files/systemd" TYPE FILE FILES "/home/tcs/server/support-files/mariadb@.service")
endif()

