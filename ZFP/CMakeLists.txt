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
cmake_minimum_required (VERSION 3.12)
project (H5ZFP C)

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
# Basic H5ZFP stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5ZFP_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5ZFP_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (${ZFP_PACKAGE_NAME})

#-----------------------------------------------------------------------------
# Specific H5ZFP here
#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
set (H5ZFP_SRC_LIBZFP_DIR           ${H5ZFP_SOURCE_DIR}/src/libzfp)
#-----------------------------------------------------------------------------
# Set the core names of all the libraries
#-----------------------------------------------------------------------------
set (ZFP_LIB_CORENAME              "zfp")
#-----------------------------------------------------------------------------
# Set the true names of all the libraries if customized by external project
#-----------------------------------------------------------------------------
set (ZFP_LIB_NAME              "${H5ZFP_EXTERNAL_LIB_PREFIX}${ZFP_LIB_CORENAME}")
#-----------------------------------------------------------------------------
# Set the target names of all the libraries
#-----------------------------------------------------------------------------
set (ZFP_LIB_TARGET              ${ZFP_LIB_CORENAME})
#-----------------------------------------------------------------------------

set (H5ZFP_VERS_MAJOR 1)
set (H5ZFP_VERS_MINOR 0)
set (H5ZFP_VERS_RELEASE 0)

set (H5ZFP_SOVERS_MAJOR 1)
set (H5ZFP_SOVERS_MINOR 0)
set (H5ZFP_SOVERS_INTERFACE 1)
set (H5ZFP_SOVERS_RELEASE 0)
math (EXPR H5ZFP_SOVERS_MAJOR ${H5ZFP_SOVERS_INTERFACE}-${H5ZFP_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5ZFP_PACKAGE_VERSION "${H5ZFP_VERS_MAJOR}.${H5ZFP_VERS_MINOR}")
set (H5ZFP_PACKAGE_VERSION_STRING "${H5ZFP_PACKAGE_VERSION}")
set (H5ZFP_PACKAGE_VERSION_MAJOR "${H5ZFP_VERS_MAJOR}")
set (H5ZFP_PACKAGE_VERSION_MINOR "${H5ZFP_VERS_MINOR}")
set (H5ZFP_PACKAGE_STRING "${H5ZFP_PACKAGE_NAME} ${H5ZFP_PACKAGE_VERSION}")
set (H5ZFP_PACKAGE_TARNAME "${H5ZFP_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5ZFP_PACKAGE_URL "http://www.hdfgroup.org")
set (H5ZFP_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5ZFP_PACKAGE_SOVERSION "${H5ZFP_SOVERS_MAJOR}.${H5ZFP_SOVERS_RELEASE}.${H5ZFP_SOVERS_MINOR}")
set (H5ZFP_PACKAGE_SOVERSION_MAJOR "${H5ZFP_SOVERS_MAJOR}")
message("Configuring for zfp HDF5 Plugin version: " ${H5ZFP_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5ZFP_RESOURCES_DIR}/H5ZFPMacros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5ZFP_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# The ZFP filter options
#-----------------------------------------------------------------------------
option (COND_MAIN_ENCODER "Enable encoding" ON)
if (COND_MAIN_ENCODER)
  set (HAVE_MAIN_ENCODER 1)
endif ()
option (COND_MAIN_DECODER "Enable decoding" ON)
if (COND_MAIN_DECODER)
  set (HAVE_MAIN_DECODER 1)
endif ()
option (COND_CHECK_CRC32 "Enable check crc32" ON)
if (COND_CHECK_CRC32)
  set (HAVE_CHECK_CRC32 1)
endif ()
option (COND_CHECK_CRC64 "Enable check crc64" ON)
if (COND_CHECK_CRC64)
  set (HAVE_CHECK_CRC64 1)
endif ()
option (COND_CHECK_SHA256 "Enable check sha256" ON)
if (COND_CHECK_SHA256)
  set (HAVE_CHECK_SHA256 1)
endif ()
option (COND_FILTER_LZ "Enable lz filter" ON)
if (COND_FILTER_LZ)
  set (HAVE_FILTER_LZ 1)
endif ()
option (COND_ENCODER_LZ "Enable lz encoder" ON)
if (COND_ENCODER_LZ)
  set (HAVE_ENCODER_LZ 1)
endif ()
option (COND_DECODER_LZ "Enable lz decoder" ON)
if (COND_DECODER_LZ)
  set (HAVE_DECODER_LZ 1)
endif ()
option (COND_FILTER_LZMA1 "Enable lzma1 filter" ON)
if (COND_FILTER_LZMA1)
  set (HAVE_FILTER_LZMA1)
endif ()
option (COND_ENCODER_LZMA1 "Enable lzma1 encoder" ON)
if (COND_ENCODER_LZMA1)
  set (HAVE_ENCODER_LZMA1 1)
endif ()
option (COND_DECODER_LZMA1 "Enable lzma1 decoder" ON)
if (COND_DECODER_LZMA1)
  set (HAVE_DECODER_LZMA1 1)
endif ()
option (COND_ENCODER_LZMA2 "Enable lzma2 encoder" ON)
if (COND_ENCODER_LZMA2)
  set (HAVE_ENCODER_LZMA2 1)
endif ()
option (COND_DECODER_LZMA2 "Enable lzma2 decoder" ON)
if (COND_DECODER_LZMA2)
  set (HAVE_DECODER_LZMA2 1)
endif ()
option (COND_FILTER_DELTA "Enable delta filter" ON)
if (COND_FILTER_DELTA)
  set (HAVE_FILTER_DELTA 1)
endif ()
option (COND_ENCODER_SIMPLE "Enable delta encoder" ON)
if (COND_MAIN_ENCODER)
  set (HAVE_MAIN_ENCODER 1)
endif ()
option (COND_DECODER_DELTA "Enable delta decoder" ON)
if (COND_MAIN_ENCODER)
  set (HAVE_MAIN_ENCODER 1)
endif ()
option (COND_FILTER_SIMPLE "Enable simple filter" ON)
if (COND_FILTER_SIMPLE)
  set (HAVE_FILTER_SIMPLE 1)
endif ()
option (COND_ENCODER_SIMPLE "Enable simple encoder" ON)
if (COND_ENCODER_SIMPLE)
  set (HAVE_ENCODER_SIMPLE 1)
endif ()
option (COND_DECODER_SIMPLE "Enable simple decoder" ON)
if (COND_DECODER_SIMPLE)
  set (HAVE_DECODER_SIMPLE 1)
endif ()

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5ZFP link libs: ${LINK_LIBS}")

include (ExternalProject)
option (H5PL_ALLOW_EXTERNAL_SUPPORT "Allow External Library Building (NO GIT TGZ)" "NO")
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  option (ZFP_USE_EXTERNAL "Use External Library Building for ZFP" 1)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (ZFP_URL ${ZFP_GIT_URL} CACHE STRING "Path to zfp git repository")
    set (ZFP_BRANCH ${ZFP_GIT_BRANCH})
  elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${H5ZFP_SOURCE_DIR})
    endif ()
    set (ZFP_URL ${TGZPATH}/${ZFP_TGZ_NAME})
  else ()
    set (ZFP_USE_EXTERNAL 0)
  endif ()
endif ()
#-----------------------------------------------------------------------------
# zfp Library Settings
#-----------------------------------------------------------------------------
if (NOT ZFP_USE_EXTERNAL)
  find_package (ZFP NAMES ${ZFP_PACKAGE_NAME})
  if (NOT ZFP_FOUND)
    find_package (ZFP) # Legacy find
    if (NOT ZFP_FOUND)
      message (FATAL_ERROR " zfp is Required for ${H5ZFP_PACKAGE_NAME} library")
    endif ()
  endif ()
endif ()
if (ZFP_FOUND)
  set (H5_HAVE_ZFP_H 1)
  set (H5_HAVE_ZFP 1)
  set (H5_ZFP_HEADER "zfp.h")
  set (ZFP_INCLUDE_DIR_GEN ${ZFP_INCLUDE_DIR})
  set (ZFP_INCLUDE_DIRS ${ZFP_INCLUDE_DIR})
else ()
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    EXTERNAL_ZFP_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT} STATIC)
    set (H5_HAVE_ZFP_H 1)
    set (H5_HAVE_ZFP 1)
    message (STATUS "Filter ZFP is built")
  else ()
    message (FATAL_ERROR " ZFP is Required for ${H5ZFP_PACKAGE_NAME} library")
  endif ()
endif ()
message (STATUS "zfp found: INC=${ZFP_INCLUDE_DIR} TOOLS=${ZFP_LIBRARIES}")
set (LINK_LIBS ${LINK_LIBS} ${ZFP_LIBRARIES})

if (ZFP_FOUND)
  PACKAGE_ZFP_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
endif ()

#-----------------------------------------------------------------------------
# Generate the zfp_config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5ZFP_RESOURCES_DIR}/config.h.in ${H5ZFP_BINARY_DIR}/zfp_config.h @ONLY)

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5zfp Unit Testing" OFF)
if (BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE STRING
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing ()
  include (CTest)
  include (${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file (${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

add_subdirectory (src)

if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  if (ZFP_FOUND AND ZFP_USE_EXTERNAL)
    add_dependencies (${H5ZFP_LIB_TARGET} ZFP)
  endif ()
endif ()

option (BUILD_EXAMPLES "Build h5zfp Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5ZFP)
