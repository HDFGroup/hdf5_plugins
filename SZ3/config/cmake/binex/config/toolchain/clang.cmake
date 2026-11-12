#
# Copyright by The HDF Group.
# All rights reserved.
#
# This file is part of HDF5.  The full HDF5 copyright notice, including
# terms governing use, modification, and redistribution, is contained in
# the COPYING file, which can be found at the root of the source code
# distribution tree, or in https://www.hdfgroup.org/licenses.
# If you do not have access to either file, you may request a copy from
# help@hdfgroup.org.
#
# Uncomment the following to use cross-compiling
#set (CMAKE_SYSTEM_NAME Linux)

set (CMAKE_COMPILER_VENDOR "clang")

if(WIN32)
  set (CMAKE_C_COMPILER clang-cl)
  set (CMAKE_CXX_COMPILER clang-cl)
else()
  set (CMAKE_C_COMPILER clang)
  set (CMAKE_CXX_COMPILER clang++)
endif()
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# the following is used if cross-compiling
set (CMAKE_CROSSCOMPILING_EMULATOR "")
