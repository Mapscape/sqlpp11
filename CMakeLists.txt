# Copyright (c) 2013-2015, Roland Bock
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
#   Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
#   Redistributions in binary form must reproduce the above copyright notice, this
#   list of conditions and the following disclaimer in the documentation and/or
#   other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 2.6)
project (sqlpp11)

# Cygwin does not define WIN32 and warn if not use with this flag
set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required

enable_testing()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Wall ${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "-std=c++11 -Wall ${CMAKE_CXX_FLAGS}")
endif ()
set(CMAKE_CXX_FLAGS "-Wconversion -Wpedantic -Wshadow ${CMAKE_CXX_FLAGS}")

set(include_dir "${PROJECT_SOURCE_DIR}/include")
file(GLOB_RECURSE sqlpp_headers "${include_dir}/*.h")
include_directories("${include_dir}")

SET( BOOST_RELEASE_DIR ${BOOST_RELEASE_ROOT}/${CV_HOSTTYPE})

INCLUDE_DIRECTORIES( "${BOOST_RELEASE_DIR}/include")
LINK_DIRECTORIES(    "${BOOST_RELEASE_DIR}/lib")


add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
add_subdirectory(${PROJECT_SOURCE_DIR}/test_constraints)
# Mapscape specific alterations
IF (0)
add_subdirectory(examples)

ENDIF(0)

#end Mapscape specific alterations

install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/sqlpp11" DESTINATION include)

