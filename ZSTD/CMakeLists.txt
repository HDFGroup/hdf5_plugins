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
project (H5ZSTD C)

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
# Basic H5ZSTD stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5ZSTD_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5ZSTD_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (${ZSTD_PACKAGE_NAME})

set (H5ZSTD_VERS_MAJOR 1)
set (H5ZSTD_VERS_MINOR 0)
set (H5ZSTD_VERS_RELEASE 0)

set (H5ZSTD_SOVERS_MAJOR 1)
set (H5ZSTD_SOVERS_MINOR 0)
set (H5ZSTD_SOVERS_INTERFACE 1)
set (H5ZSTD_SOVERS_RELEASE 0)
math (EXPR H5ZSTD_SOVERS_MAJOR ${H5ZSTD_SOVERS_INTERFACE}-${H5ZSTD_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5ZSTD_PACKAGE_VERSION "${H5ZSTD_VERS_MAJOR}.${H5ZSTD_VERS_MINOR}.${H5ZSTD_VERS_RELEASE}")
set (H5ZSTD_PACKAGE_VERSION_STRING "${H5ZSTD_PACKAGE_VERSION}")
set (H5ZSTD_PACKAGE_VERSION_MAJOR "${H5ZSTD_VERS_MAJOR}")
set (H5ZSTD_PACKAGE_VERSION_MINOR "${H5ZSTD_VERS_MINOR}")
set (H5ZSTD_PACKAGE_STRING "${H5ZSTD_PACKAGE_NAME} ${H5ZSTD_PACKAGE_VERSION}")
set (H5ZSTD_PACKAGE_TARNAME "${H5ZSTD_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5ZSTD_PACKAGE_URL "http://www.hdfgroup.org")
set (H5ZSTD_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5ZSTD_PACKAGE_SOVERSION "${H5ZSTD_SOVERS_MAJOR}.${H5ZSTD_SOVERS_RELEASE}.${H5ZSTD_SOVERS_MINOR}")
set (H5ZSTD_PACKAGE_SOVERSION_MAJOR "${H5ZSTD_SOVERS_MAJOR}")
message("Configuring for zstd HDF5 Plugin version: " ${H5ZSTD_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5ZSTD_RESOURCES_DIR}/H5ZSTDMacros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5ZSTD_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5ZSTD link libs: ${LINK_LIBS}")

include (ExternalProject)
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  option (ZSTD_USE_EXTERNAL "Use External Library Building for ZSTD" 1)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (ZSTD_URL ${ZSTD_GIT_URL} CACHE STRING "Path to zstd git repository")
    set (ZSTD_BRANCH ${ZSTD_GIT_BRANCH})
  elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${H5ZSTD_SOURCE_DIR})
    endif ()
    set (ZSTD_URL ${TGZPATH}/${ZSTD_TGZ_NAME})
  else ()
    set (ZSTD_USE_EXTERNAL 0)
  endif ()
endif ()
#-----------------------------------------------------------------------------
# zstd Library Settings
#-----------------------------------------------------------------------------
if (NOT ZSTD_USE_EXTERNAL)
  set (FIND_ZSTD_COMPONENTS static)
  find_package (ZSTD NAMES ${ZSTD_PACKAGE_NAME} COMPONENTS ${FIND_ZSTD_COMPONENTS})
  message (STATUS "ZSTD C libs: static:${ZSTD_static_FOUND}")
  if (NOT ZSTD_FOUND)
    find_package (ZSTD) # Legacy find
  endif ()
endif ()
if (ZSTD_FOUND)
  set (H5ZSTD_HAVE_ZSTD 1)
  set (H5ZSTD_ZSTD_HEADER "zstd.h")
  set (ZSTD_INCLUDE_DIR_GEN ${ZSTD_INCLUDE_DIR})
  set (ZSTD_INCLUDE_DIRS ${ZSTD_INCLUDE_DIR})
else ()
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    EXTERNAL_ZSTD_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT} STATIC)
    set (H5ZSTD_HAVE_ZSTD 1)
    message (STATUS "Filter ZSTD is built")
  else ()
    message (FATAL_ERROR " zstd is Required for ${H5ZSTD_PACKAGE_NAME} library")
  endif ()
endif ()
message (STATUS "zstd found: INC=${ZSTD_INCLUDE_DIR} LIBS=${ZSTD_LIBRARIES}")
set (LINK_LIBS ${LINK_LIBS} ${ZSTD_LIBRARIES})

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5zstd Unit Testing" OFF)
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
  if (ZSTD_FOUND AND ZSTD_USE_EXTERNAL)
    add_dependencies (${H5ZSTD_LIB_TARGET} ZSTD)
  endif ()
endif ()

option (BUILD_EXAMPLES "Build h5zstd Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5ZSTD)
