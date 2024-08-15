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
cmake_minimum_required(VERSION 3.12)
project (H5PL_COMM C)

# FPZIP filter
#FILTER_OPTION (FPZIP)

#if (WIN32 AND MSVC_VERSION GREATER 1600)
#  # MAFISC filter
#  FILTER_OPTION (MAFISC)
#elseif (NOT WIN32)
#  FILTER_OPTION (MAFISC)
#else ()
#  set (ENABLE_MAFISC OFF CACHE BOOL "" FORCE)
#endif ()

# SZ filter
#if (WIN32 AND MSVC_VERSION GREATER_EQUAL 1930)
  #if (NOT CMAKE_C_COMPILER_ID MATCHES "[Cc]lang" AND NOT CMAKE_C_COMPILER_ID MATCHES "Intel[Ll][Ll][Vv][Mm]")
  #  FILTER_OPTION (SZ)
  #else ()
#    set (ENABLE_SZ OFF CACHE BOOL "" FORCE)
  #endif ()
#elseif (WIN32 AND MSVC_VERSION GREATER 1900)
#  if (CMAKE_C_COMPILER_ID MATCHES "[Cc]lang" AND NOT CMAKE_C_COMPILER_ID MATCHES "Intel[Ll][Ll][Vv][Mm]")
#    FILTER_OPTION (SZ)
#  else ()
#    set (ENABLE_SZ OFF CACHE BOOL "" FORCE)
#  endif ()
#elseif (NOT WIN32)
#  if(CMAKE_C_COMPILER_ID MATCHES "[Cc]lang" AND NOT CMAKE_C_COMPILER_ID MATCHES "Apple[Cc]lang")
#    FILTER_OPTION (SZ)
#  else ()
#    set (ENABLE_SZ OFF CACHE BOOL "" FORCE)
#  endif ()
#else ()
  set (ENABLE_SZ OFF CACHE BOOL "" FORCE)
#endif ()
