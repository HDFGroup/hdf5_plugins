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

set (CMAKE_COMPILER_VENDOR "PGI")

set (CMAKE_C_COMPILER pgcc)
set (CMAKE_CXX_COMPILER pgc++)
set (CMAKE_Fortran_COMPILER pgf90)

# the following is used if cross-compiling
set (CMAKE_CROSSCOMPILING_EMULATOR "")
