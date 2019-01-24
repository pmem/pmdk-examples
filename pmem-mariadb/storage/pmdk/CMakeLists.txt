# Copyright (c) 2006, 2010, Oracle and/or its affiliates. All rights reserved.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

SET(EXAMPLE_SOURCES ha_pmdk.cc)
MYSQL_ADD_PLUGIN(pmdk ${EXAMPLE_SOURCES} STORAGE_ENGINE MODULE_ONLY COMPONENT Test)
set(CMAKE_CXX_STANDARD_LIBRARIES "-lpmemobj -lpmem")
#SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive" )
