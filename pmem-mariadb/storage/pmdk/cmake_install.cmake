# Install script for directory: /home/tcs/server/storage/pmdk

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xTestx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin/ha_pmdk.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin/ha_pmdk.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin/ha_pmdk.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/plugin" TYPE MODULE FILES "/home/tcs/server/storage/pmdk/ha_pmdk.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin/ha_pmdk.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin/ha_pmdk.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/plugin/ha_pmdk.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xTestx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/mysql-test/plugin/pmdk" TYPE DIRECTORY FILES "/home/tcs/server/storage/pmdk/mysql-test/" USE_SOURCE_PERMISSIONS REGEX "/var$" EXCLUDE REGEX "/lib\\/My\\/SafeProcess$" EXCLUDE REGEX "/lib\\/t[^/]*$" EXCLUDE REGEX "/CPack$" EXCLUDE REGEX "/CMake[^/]*$" EXCLUDE REGEX "/cmake\\_install\\.cmake$" EXCLUDE REGEX "/mtr\\.out[^/]*$" EXCLUDE REGEX "/\\.cvsignore$" EXCLUDE REGEX "/[^/]*\\.am$" EXCLUDE REGEX "/[^/]*\\.in$" EXCLUDE REGEX "/Makefile$" EXCLUDE REGEX "/[^/]*\\.vcxproj$" EXCLUDE REGEX "/[^/]*\\.vcxproj\\.filters$" EXCLUDE REGEX "/[^/]*\\.vcxproj\\.user$" EXCLUDE REGEX "/CTest[^/]*$" EXCLUDE REGEX "/[^/]*\\~$" EXCLUDE)
endif()

