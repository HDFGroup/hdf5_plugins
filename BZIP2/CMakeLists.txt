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
project (H5BZ2 C)

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
# Basic H5BZIP stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5BZ2_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5BZ2_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (${BZ2_PACKAGE_NAME})

set (H5BZ2_VERS_MAJOR 1)
set (H5BZ2_VERS_MINOR 0)
set (H5BZ2_VERS_RELEASE 0)

set (H5BZ2_SOVERS_MAJOR 1)
set (H5BZ2_SOVERS_MINOR 0)
set (H5BZ2_SOVERS_INTERFACE 1)
set (H5BZ2_SOVERS_RELEASE 0)
math (EXPR H5BZ2_SOVERS_MAJOR ${H5BZ2_SOVERS_INTERFACE}-${H5BZ2_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5BZ2_PACKAGE_VERSION "${H5BZ2_VERS_MAJOR}.${H5BZ2_VERS_MINOR}.${H5BZ2_VERS_RELEASE}")
set (H5BZ2_PACKAGE_VERSION_STRING "${H5BZ2_PACKAGE_VERSION}")
set (H5BZ2_PACKAGE_VERSION_MAJOR "${H5BZ2_VERS_MAJOR}")
set (H5BZ2_PACKAGE_VERSION_MINOR "${H5BZ2_VERS_MINOR}")
set (H5BZ2_PACKAGE_STRING "${H5BZ2_PACKAGE_NAME} ${H5BZ2_PACKAGE_VERSION}")
set (H5BZ2_PACKAGE_TARNAME "${H5BZ2_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5BZ2_PACKAGE_URL "http://www.hdfgroup.org")
set (H5BZ2_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5BZ2_PACKAGE_SOVERSION "${H5BZ2_SOVERS_MAJOR}.${H5BZ2_SOVERS_RELEASE}.${H5BZ2_SOVERS_MINOR}")
set (H5BZ2_PACKAGE_SOVERSION_MAJOR "${H5BZ2_SOVERS_MAJOR}")
message(STATUS "Configuring for bz2 HDF5 Plugin version: " ${H5BZ2_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5BZ2_RESOURCES_DIR}/H5BZ2Macros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5BZ2_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5BZ2 link libs: ${H5PL_LINK_LIBS}")

#-----------------------------------------------------------------------------
# Generate the bzip_config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5BZ2_RESOURCES_DIR}/config.h.in ${H5BZ2_BINARY_DIR}/bzip_config.h @ONLY)

include (ExternalProject)
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)

if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  option (BZ2_USE_EXTERNAL "Use External Library Building for BZ2" 1)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (BZ2_URL ${BZ2_GIT_URL} CACHE STRING "Path to bz2 git repository")
    set (BZ2_BRANCH ${BZ2_GIT_BRANCH})
  elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT H5PL_COMP_TGZPATH)
      set (H5PL_COMP_TGZPATH ${H5BZ2_SOURCE_DIR})
    endif ()
    set (BZ2_URL ${H5PL_COMP_TGZPATH}/${BZ2_TGZ_NAME})
  else ()
    set (BZ2_USE_EXTERNAL 0)
  endif ()
endif ()
#-----------------------------------------------------------------------------
# bzip2 Library Settings
#-----------------------------------------------------------------------------
if (NOT BZ2_USE_EXTERNAL)
  set (FIND_BZ2_COMPONENTS static)
  find_package (BZ2 NAMES ${BZ2_PACKAGE_NAME} COMPONENTS ${FIND_BZ2_COMPONENTS})
  message (STATUS "BZ2 C libs: static:${BZ2_static_FOUND}")
  if (NOT BZ2_FOUND)
    find_package (BZ2) # Legacy find
  endif ()
endif ()
if (BZ2_FOUND)
  set (H5BZ2_HAVE_BZLIB_H 1)
  set (H5BZ2_HAVE_BZ2 1)
  set (H5BZ2_BZ2_HEADER "bzlib.h")
  set (BZ2_INCLUDE_DIR_GEN ${BZ2_INCLUDE_DIR})
  set (BZ2_INCLUDE_DIRS ${BZ2_INCLUDE_DIR})
else ()
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    EXTERNAL_BZ2_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
    set (H5BZ2_HAVE_BZLIB_H 1)
    set (H5BZ2_HAVE_BZ2 1)
    message (STATUS "Filter BZ2 is built")
  else ()
    message (FATAL_ERROR " bz2 is Required for ${H5BZ2_PACKAGE_NAME} library")
  endif ()
endif ()
message (STATUS "bz2 found: INC=${BZ2_INCLUDE_DIR} LIBS=${BZ2_LIBRARIES}")
set (H5PL_LINK_LIBS ${H5PL_LINK_LIBS} ${BZ2_LIBRARIES})

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (H5PL_BUILD_TESTING "Build h5bz2 Unit Testing" OFF)
if (H5PL_BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE STRING
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing ()
  include (CTest)
  include (${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file (${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

add_subdirectory (src)

option (H5PL_BUILD_EXAMPLES "Build h5bz2 Examples" OFF)
if (H5PL_BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5BZ2)
