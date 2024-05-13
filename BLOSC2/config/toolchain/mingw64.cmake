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
set (TOOLCHAIN_PREFIX x86_64-w64-mingw32)
set (CMAKE_SYSTEM_NAME Windows)
set (CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set (CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set (CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)
set (CMAKE_Fortran_COMPILER ${TOOLCHAIN_PREFIX}-gfortran)
set (CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set (CMAKE_CROSSCOMPILING_EMULATOR wine64)

include_directories(/usr/${TOOLCHAIN_PREFIX}/include)
set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS On CACHE BOOL "Export windows symbols")
