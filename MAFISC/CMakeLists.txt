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
project (H5MAFISC C)

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
# Basic H5MAFISC stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5MAFISC_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5MAFISC_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (${MAFISC_PACKAGE_NAME})

#-----------------------------------------------------------------------------
# Specific H5MAFISC here
#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
set (H5MAFISC_SRC_LIBLZMA_DIR           ${H5MAFISC_SOURCE_DIR}/src/liblzma)
#-----------------------------------------------------------------------------
# Set the core names of all the libraries
#-----------------------------------------------------------------------------
set (LZMA_LIB_CORENAME              "lzma")
#-----------------------------------------------------------------------------
# Set the true names of all the libraries if customized by external project
#-----------------------------------------------------------------------------
set (LZMA_LIB_NAME              "${H5MAFISC_EXTERNAL_LIB_PREFIX}${LZMA_LIB_CORENAME}")
#-----------------------------------------------------------------------------
# Set the target names of all the libraries
#-----------------------------------------------------------------------------
set (LZMA_LIB_TARGET              ${LZMA_LIB_CORENAME})
#-----------------------------------------------------------------------------

set (H5MAFISC_VERS_MAJOR 1)
set (H5MAFISC_VERS_MINOR 0)
set (H5MAFISC_VERS_RELEASE 0)

set (H5MAFISC_SOVERS_MAJOR 1)
set (H5MAFISC_SOVERS_MINOR 0)
set (H5MAFISC_SOVERS_INTERFACE 1)
set (H5MAFISC_SOVERS_RELEASE 0)
math (EXPR H5MAFISC_SOVERS_MAJOR ${H5MAFISC_SOVERS_INTERFACE}-${H5MAFISC_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5MAFISC_PACKAGE_VERSION "${H5MAFISC_VERS_MAJOR}.${H5MAFISC_VERS_MINOR}")
set (H5MAFISC_PACKAGE_VERSION_STRING "${H5MAFISC_PACKAGE_VERSION}")
set (H5MAFISC_PACKAGE_VERSION_MAJOR "${H5MAFISC_VERS_MAJOR}")
set (H5MAFISC_PACKAGE_VERSION_MINOR "${H5MAFISC_VERS_MINOR}")
set (H5MAFISC_PACKAGE_STRING "${H5MAFISC_PACKAGE_NAME} ${H5MAFISC_PACKAGE_VERSION}")
set (H5MAFISC_PACKAGE_TARNAME "${H5MAFISC_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5MAFISC_PACKAGE_URL "http://www.hdfgroup.org")
set (H5MAFISC_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5MAFISC_PACKAGE_SOVERSION "${H5MAFISC_SOVERS_MAJOR}.${H5MAFISC_SOVERS_RELEASE}.${H5MAFISC_SOVERS_MINOR}")
set (H5MAFISC_PACKAGE_SOVERSIONMAJOR "${H5MAFISC_SOVERS_MAJOR}")
message("Configuring for mafisc HDF5 Plugin version: " ${H5MAFISC_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5MAFISC_RESOURCES_DIR}/H5MAFISCMacros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5MAFISC_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# The MAFISC filter options
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
message (STATUS "H5MAFISC link libs: ${LINK_LIBS}")

#-----------------------------------------------------------------------------
# Generate the mafisc_config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5MAFISC_RESOURCES_DIR}/config.h.in ${H5MAFISC_BINARY_DIR}/mafisc_config.h @ONLY)

option (BUILD_MAFISC_LIBRARY_SOURCE "build the mafisc library within the plugin" ON)
if (NOT BUILD_MAFISC_LIBRARY_SOURCE)
  include (ExternalProject)
  set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
  set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    option (MAFISC_USE_EXTERNAL "Use External Library Building for MAFISC" 1)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (MAFISC_URL ${MAFISC_GIT_URL} CACHE STRING "Path to mafisc git repository")
      set (MAFISC_BRANCH ${MAFISC_GIT_BRANCH})
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5MAFISC_SOURCE_DIR})
      endif ()
      set (MAFISC_URL ${TGZPATH}/${MAFISC_TGZ_NAME})
    else ()
      set (MAFISC_USE_EXTERNAL 0)
    endif ()
  endif ()
  #-----------------------------------------------------------------------------
  # mafisc Library Settings
  #-----------------------------------------------------------------------------
  if (NOT MAFISC_USE_EXTERNAL)
    find_package (MAFISC NAMES ${MAFISC_PACKAGE_NAME})
    if (NOT MAFISC_FOUND)
      find_package (MAFISC) # Legacy find
      if (NOT MAFISC_FOUND)
        message (FATAL_ERROR " mafisc is Required for ${H5MAFISC_PACKAGE_NAME} library")
      endif ()
    endif ()
  endif ()
  if (MAFISC_FOUND)
    set (H5MAFISC_HAVE_MAFISC_H 1)
    set (H5MAFISC_HAVE_MAFISC 1)
    set (H5MAFISC_MAFISC_HEADER "mafisc.h")
    set (H5MAFISC_INCLUDE_DIR_GEN ${MAFISC_INCLUDE_DIR})
    set (H5MAFISC_INCLUDE_DIRS ${MAFISC_INCLUDE_DIR})
  else ()
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      EXTERNAL_MAFISC_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT} STATIC)
      set (H5MAFISC_HAVE_MAFISC_H 1)
      set (H5MAFISC_HAVE_MAFISC 1)
      message (STATUS "Filter MAFISC is built")
    else ()
      message (FATAL_ERROR " MAFISC is Required for ${H5MAFISC_PACKAGE_NAME} library")
    endif ()
  endif ()
  message (STATUS "mafisc found: INC=${MAFISC_INCLUDE_DIR} TOOLS=${MAFISC_LIBRARIES}")
  set (LINK_LIBS ${LINK_LIBS} ${MAFISC_LIBRARIES})

  if (MAFISC_FOUND)
    PACKAGE_MAFISC_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
  endif ()
endif ()

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5mafisc Unit Testing" OFF)
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
  if (MAFISC_FOUND AND MAFISC_USE_EXTERNAL)
    add_dependencies (${H5MAFISC_LIB_TARGET} MAFISC)
  endif ()
endif ()

option (BUILD_EXAMPLES "Build h5mafisc Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5MAFISC)
