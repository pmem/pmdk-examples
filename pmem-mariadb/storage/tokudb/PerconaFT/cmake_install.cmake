# Install script for directory: /home/tcs/server/storage/tokudb/PerconaFT

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xServerx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/docs" TYPE FILE FILES
    "/home/tcs/server/storage/tokudb/PerconaFT/README.md"
    "/home/tcs/server/storage/tokudb/PerconaFT/COPYING.AGPLv3"
    "/home/tcs/server/storage/tokudb/PerconaFT/COPYING.GPLv2"
    "/home/tcs/server/storage/tokudb/PerconaFT/PATENTS"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/tcs/server/storage/tokudb/PerconaFT/buildheader/cmake_install.cmake")
  include("/home/tcs/server/storage/tokudb/PerconaFT/util/cmake_install.cmake")
  include("/home/tcs/server/storage/tokudb/PerconaFT/portability/cmake_install.cmake")
  include("/home/tcs/server/storage/tokudb/PerconaFT/ft/cmake_install.cmake")
  include("/home/tcs/server/storage/tokudb/PerconaFT/locktree/cmake_install.cmake")
  include("/home/tcs/server/storage/tokudb/PerconaFT/src/cmake_install.cmake")
  include("/home/tcs/server/storage/tokudb/PerconaFT/ftcxx/cmake_install.cmake")
  include("/home/tcs/server/storage/tokudb/PerconaFT/tools/cmake_install.cmake")

endif()

