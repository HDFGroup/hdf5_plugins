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
cmake_minimum_required(VERSION 3.18)
project (H5PL C)

if (POLICY CMP0074)
  # find_package() uses <PackageName>_ROOT variables.
  cmake_policy (SET CMP0074 NEW)
endif ()

if (POLICY CMP0083)
  # To control generation of Position Independent Executable (PIE) or not,
  # some flags are required at link time.
  cmake_policy (SET CMP0083 NEW)
endif ()

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

#-----------------------------------------------------------------------------
# Basic H5PL stuff here
#-----------------------------------------------------------------------------
include (${H5PL_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
include (${H5PL_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)
include (${H5PL_SOURCE_DIR}/config/CacheURLs.cmake)

SET_HDF_BUILD_TYPE()

set (H5PL_EXPORTED_TARGETS "")

BASIC_SETTINGS (${PL_PACKAGE_NAME})

if (NOT H5PL_VERS_MAJOR)
  set (H5PL_VERS_MAJOR "2")
endif ()
if (NOT H5PL_VERS_MINOR)
  set (H5PL_VERS_MINOR "0")
endif ()
if (NOT H5PL_VERS_RELEASE)
  set (H5PL_VERS_RELEASE "0")
endif ()
set (H5PL_OVERRIDE_VERSION "${H5PL_VERS_MAJOR}.${H5PL_VERS_MINOR}.${H5PL_VERS_RELEASE}")

set (H5PL_SOVERS_MAJOR 1)
set (H5PL_SOVERS_MINOR 0)
set (H5PL_SOVERS_INTERFACE 4)
set (H5PL_SOVERS_RELEASE 0)
math (EXPR H5PL_SOVERS_MAJOR ${H5PL_SOVERS_INTERFACE}-${H5PL_SOVERS_RELEASE})

set (H5PL_PACKAGE_VERSION "${H5PL_VERS_MAJOR}.${H5PL_VERS_MINOR}.${H5PL_VERS_RELEASE}")
set (H5PL_PACKAGE_VERSION_STRING "${H5PL_PACKAGE_VERSION}")
set (H5PL_PACKAGE_VERSION_MAJOR "${H5PL_VERS_MAJOR}")
set (H5PL_PACKAGE_VERSION_MINOR "${H5PL_VERS_MINOR}")
set (H5PL_PACKAGE_STRING "${H5PL_PACKAGE_NAME}")
set (H5PL_PACKAGE_TARNAME "${H5PL_PACKAGE_NAME}${HDF_PACKAGE_EXT}")
set (H5PL_PACKAGE_URL "http://www.hdfgroup.org")
set (H5PL_PACKAGE_BUGREPORT "help@hdfgroup.org")

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5PL_RESOURCES_DIR}/H5PLMacros.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "HDF5 link libs: ${H5PL_LINK_LIBS}")

#-----------------------------------------------------------------------------
# External plugin targets for the H5PL super project.
#-----------------------------------------------------------------------------
include (ExternalProject)
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)

# Bitgroom filter
FILTER_OPTION (BITGROOM)

# Bitround filter
FILTER_OPTION (BITROUND)

# Bitshuffle filter
FILTER_OPTION (BSHUF)

# BLOSC filter
if (WIN32)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "[Cc]lang" AND MSVC_VERSION GREATER 1600)
    FILTER_OPTION (BLOSC)
  else ()
    set (ENABLE_BLOSC OFF CACHE BOOL "" FORCE)
  endif ()
elseif (APPLE)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "Intel")
    FILTER_OPTION (BLOSC)
  elseif (CMAKE_C_COMPILER_ID MATCHES "Intel[Ll][Ll][Vv][Mm]")
    FILTER_OPTION (BLOSC)
  else ()
    set (ENABLE_BLOSC OFF CACHE BOOL "" FORCE)
  endif ()
else ()
  FILTER_OPTION (BLOSC)
endif ()

# BLOSC2 filter
if (WIN32)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "Intel[Ll][Ll][Vv][Mm]")
    if (NOT CMAKE_C_COMPILER_ID MATCHES "[Cc]lang" AND MSVC_VERSION GREATER 1600)
      FILTER_OPTION (BLOSC2)
    else ()
      set (ENABLE_BLOSC2 OFF CACHE BOOL "" FORCE)
    endif ()
  else ()
    set (ENABLE_BLOSC2 OFF CACHE BOOL "" FORCE)
  endif ()
elseif (APPLE)
  #if (CMAKE_C_COMPILER_ID MATCHES "Intel[Ll][Ll][Vv][Mm]")
  #  FILTER_OPTION (BLOSC2)
  #else
  if (NOT CMAKE_C_COMPILER_ID MATCHES "Intel" AND NOT CMAKE_C_COMPILER_ID MATCHES "Apple[Cc]lang")
      FILTER_OPTION (BLOSC2)
  else ()
    set (ENABLE_BLOSC2 OFF CACHE BOOL "" FORCE)
  endif ()
else ()
  FILTER_OPTION (BLOSC2)
endif ()

# BZIP2 filter
if (NOT MINGW)
  FILTER_OPTION (BZIP2)
else ()
  set (ENABLE_BZIP2 OFF CACHE BOOL "" FORCE)
endif ()

# JPEG filter
if (NOT WIN32)
  FILTER_OPTION (JPEG)
else ()
  set (ENABLE_JPEG OFF CACHE BOOL "" FORCE)
endif ()

# LZ4 filter
if (NOT MINGW)
  if (WIN32)
    if (NOT CMAKE_C_COMPILER_ID MATCHES "Intel[Ll][Ll][Vv][Mm]")
      if (MSVC_VERSION GREATER_EQUAL 1930 AND NOT CMAKE_C_COMPILER_ID MATCHES "[Cc]lang")
        FILTER_OPTION (LZ4)
      else ()
        set (ENABLE_LZ4 OFF CACHE BOOL "" FORCE)
      endif ()
    else ()
      set (ENABLE_LZ4 OFF CACHE BOOL "" FORCE)
    endif ()
  else ()
    FILTER_OPTION (LZ4)
  endif ()
else ()
  set (ENABLE_LZ4 OFF CACHE BOOL "" FORCE)
endif ()

# LZF filter
if (NOT MINGW AND NOT H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
  FILTER_OPTION (LZF)
else ()
  set (ENABLE_LZF OFF CACHE BOOL "" FORCE)
endif ()

# ZFP filter
if (WIN32)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "Intel")
    FILTER_OPTION (ZFP)
  else ()
    set (ENABLE_ZFP OFF CACHE BOOL "" FORCE)
  endif ()
else ()
  FILTER_OPTION (ZFP)
endif ()

# ZSTD filter
if (WIN32)
    FILTER_OPTION (ZSTD)
elseif (APPLE)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "Intel")
    FILTER_OPTION (ZSTD)
  else ()
    set (ENABLE_ZSTD OFF CACHE BOOL "" FORCE)
  endif ()
else ()
  FILTER_OPTION (ZSTD)
endif ()

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (H5PL_BUILD_TESTING "Build h5pl Unit Testing" OFF)
if (H5PL_BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE STRING
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing ()
  include (CTest)
  include (${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file (${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

#-----------------------------------------------------------------------------
# Community Filters
#-----------------------------------------------------------------------------
option (H5PL_COMMUNITY "Enable building community filters" OFF)
add_subdirectory (community)

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
option (H5PL_CPACK_ENABLE "Enable the CPACK include and components" ON)
INSTALL_SUPPORT (H5PL)

