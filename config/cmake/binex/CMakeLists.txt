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

# Bitshuffle filter
set (BSHUF_AVAILABLE 0)
if (NOT CMAKE_C_COMPILER_ID STREQUAL "Intel")
  set (BSHUF_AVAILABLE 1)
endif ()

# BLOSC filter
if (WIN32 AND MSVC_VERSION GREATER 1600)
  set (BLOSC_AVAILABLE 1)
elseif (NOT WIN32)
  set (BLOSC_AVAILABLE 1)
endif ()

# BLOSC2 filter
if (WIN32 AND MSVC_VERSION GREATER 1600)
  set (BLOSC2_AVAILABLE 1)
elseif (NOT WIN32)
  set (BLOSC2_AVAILABLE 1)
endif ()

# BZIP2 filter
set (BZIP2_AVAILABLE 1)

# FPZIP filter
#set (FPZIP_AVAILABLE 1)

# JPEG filter
set (JPEG_AVAILABLE 1)

# LZ4 filter
set (LZ4_AVAILABLE 1)

# LZF filter
set (LZF_AVAILABLE 1)

set (MAFISC_AVAILABLE 0)
#if (WIN32 AND MSVC_VERSION GREATER 1600)
#  # MAFISC filter
#  set (MAFISC_AVAILABLE 1)
#elseif (NOT WIN32)
#  set (MAFISC_AVAILABLE 1)
#endif ()

# SZ filter
set (SZ_AVAILABLE 0)
if (WIN32 AND MSVC_VERSION GREATER 1900)
  set (SZ_AVAILABLE 1)
elseif (NOT WIN32)
  set (SZ_AVAILABLE 1)
endif ()

# ZFP filter
set (ZFP_AVAILABLE 1)

# ZSTD filter
set (ZSTD_AVAILABLE 1)

add_subdirectory (example)
