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
cmake_minimum_required (VERSION 3.18)
project (H5PLExamples C)

#-----------------------------------------------------------------------------
# For standard build of HDF5  plugin examples.
#-----------------------------------------------------------------------------
include (${PROJECT_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)
BASIC_SETTINGS (H5PLExamples)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "HDF5 link libs: ${H5PL_LINK_LIBS}")

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (H5PL_BUILD_TESTING "Build H5PL Example Testing" OFF)
if (H5PL_BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200
      CACHE STRING
      "Timeout in seconds for each test (default 1200=20minutes)"
  )
  enable_testing ()
  include (CTest)
  include (${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file (${H5PL_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

#-----------------------------------------------------------------------------
# Build examples
#-----------------------------------------------------------------------------
# Bitgroom filter
set (BITGROOM_AVAILABLE 1)

# Bitround filter
set (BITROUND_AVAILABLE 1)

# Bitshuffle filter
set (BSHUF_AVAILABLE 1)

# BLOSC filter
if (WIN32)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "[Cc]lang" AND MSVC_VERSION GREATER 1600)
    set (BLOSC_AVAILABLE 1)
  endif ()
elseif (APPLE)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "Intel")
    set (BLOSC_AVAILABLE 1)
  elseif (CMAKE_C_COMPILER_ID MATCHES "Intel[Ll][Ll][Vv][Mm]")
    set (BLOSC_AVAILABLE 1)
  endif ()
else ()
  set (BLOSC_AVAILABLE 1)
endif ()

# BLOSC2 filter
if (WIN32)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "Intel[Ll][Ll][Vv][Mm]")
    if (NOT CMAKE_C_COMPILER_ID MATCHES "[Cc]lang" AND MSVC_VERSION GREATER 1600)
      set (BLOSC2_AVAILABLE 1)
    endif ()
  endif ()
elseif (APPLE)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "Intel" AND NOT CMAKE_C_COMPILER_ID MATCHES "Apple[Cc]lang")
    set (BLOSC2_AVAILABLE 1)
  endif ()
else ()
  set (BLOSC2_AVAILABLE 1)
endif ()

# BZIP2 filter
if (NOT MINGW)
  set (BZIP2_AVAILABLE 1)
endif ()

# JPEG filter
if (NOT WIN32)
  set (JPEG_AVAILABLE 1)
endif ()

# LZ4 filter
if (NOT MINGW)
  if (WIN32)
    if (NOT CMAKE_C_COMPILER_ID MATCHES "Intel[Ll][Ll][Vv][Mm]")
      if (MSVC_VERSION GREATER_EQUAL 1930 AND NOT CMAKE_C_COMPILER_ID MATCHES "[Cc]lang")
        set (LZ4_AVAILABLE 1)
      endif ()
    endif ()
  else ()
    set (LZ4_AVAILABLE 1)
  endif ()
endif ()

# LZF filter
if (NOT MINGW AND NOT H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
  set (LZF_AVAILABLE 1)
endif ()

# ZFP filter
if (WIN32)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "Intel")
    set (ZFP_AVAILABLE 1)
  endif ()
else ()
  set (ZFP_AVAILABLE 1)
endif ()

# ZSTD filter
if (WIN32)
  set (ZSTD_AVAILABLE 1)
elseif (APPLE)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "Intel")
    set (ZSTD_AVAILABLE 1)
  endif ()
else ()
  set (ZSTD_AVAILABLE 1)
endif ()

add_subdirectory (example)
