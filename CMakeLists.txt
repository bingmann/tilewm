################################################################################
# CMakeLists.txt
#
# CMake file for tilewm
#
################################################################################
# Copyright (C) 2014 Timo Bingmann <tb@panthema.net>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

project(tilewm)

cmake_minimum_required(VERSION 2.8)

# disallow in-source builds

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
   message(SEND_ERROR "In-source builds are not allowed.")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")

# enable C++11 and more compiler features

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++11 CXX_HAS_STD_CXX11)
if(CXX_HAS_STD_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  check_cxx_compiler_flag(-std=c++0x CXX_HAS_STD_CXX0X)
  if(CXX_HAS_STD_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  else()
    message(SEND_ERROR "Could not detect C++11 or C++0x features.")
  endif()
endif()

# enable compiler warnings

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -pedantic -Wno-long-long")

# detect -Wextra
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wextra CXX_HAS_FLAGS_WEXTRA)
if(CXX_HAS_FLAGS_WEXTRA)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
endif()

# check whether perl is available for auto generating code

find_package(Perl)

# use pkg-config to find xcb and fellows

include(FindPkgConfig)
pkg_check_modules(XCB REQUIRED
  x11-xcb
  xcb)

# descend into source

add_subdirectory(src)
