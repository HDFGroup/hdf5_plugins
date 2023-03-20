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
project (H5JPEG C)

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
# Basic H5JPEG stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5JPEG_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5JPEG_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (${JPEG_PACKAGE_NAME})

set (H5JPEG_VERS_MAJOR 1)
set (H5JPEG_VERS_MINOR 0)
set (H5JPEG_VERS_RELEASE 0)

set (H5JPEG_SOVERS_MAJOR 1)
set (H5JPEG_SOVERS_MINOR 0)
set (H5JPEG_SOVERS_INTERFACE 1)
set (H5JPEG_SOVERS_RELEASE 0)
math (EXPR H5JPEG_SOVERS_MAJOR ${H5JPEG_SOVERS_INTERFACE}-${H5JPEG_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5JPEG_PACKAGE_VERSION "${H5JPEG_VERS_MAJOR}.${H5JPEG_VERS_MINOR}.${H5JPEG_VERS_RELEASE}")
set (H5JPEG_PACKAGE_VERSION_STRING "${H5JPEG_PACKAGE_VERSION}")
set (H5JPEG_PACKAGE_VERSION_MAJOR "${H5JPEG_VERS_MAJOR}")
set (H5JPEG_PACKAGE_VERSION_MINOR "${H5JPEG_VERS_MINOR}")
set (H5JPEG_PACKAGE_STRING "${H5JPEG_PACKAGE_NAME} ${H5JPEG_PACKAGE_VERSION}")
set (H5JPEG_PACKAGE_TARNAME "${H5JPEG_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5JPEG_PACKAGE_URL "http://www.hdfgroup.org")
set (H5JPEG_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5JPEG_PACKAGE_SOVERSION "${H5JPEG_SOVERS_MAJOR}.${H5JPEG_SOVERS_RELEASE}.${H5JPEG_SOVERS_MINOR}")
set (H5JPEG_PACKAGE_SOVERSION_MAJOR "${H5JPEG_SOVERS_MAJOR}")
message("Configuring for jpeg HDF5 Plugin version: " ${H5JPEG_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5JPEG_RESOURCES_DIR}/H5JPEGMacros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5JPEG_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5JPEG link libs: ${LINK_LIBS}")

include (ExternalProject)
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  option (JPEG_USE_EXTERNAL "Use External Library Building for JPEG" 1)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (JPEG_URL ${JPEG_GIT_URL} CACHE STRING "Path to jpeg git repository")
    set (JPEG_BRANCH ${JPEG_GIT_BRANCH})
  elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${H5JPEG_SOURCE_DIR})
    endif ()
    set (JPEG_URL ${TGZPATH}/${JPEG_TGZ_NAME})
  else ()
    set (JPEG_USE_EXTERNAL 0)
  endif ()
endif ()
#-----------------------------------------------------------------------------
# jpeg Library Settings
#-----------------------------------------------------------------------------
if (NOT JPEG_USE_EXTERNAL)
  set (FIND_JPEG_COMPONENTS static)
  find_package (JPEG NAMES ${JPEG_PACKAGE_NAME} COMPONENTS ${FIND_JPEG_COMPONENTS})
  message (STATUS "JPEG C libs: static:${JPEG_static_FOUND}")
  if (NOT JPEG_FOUND)
    find_package (JPEG) # Legacy find
  endif ()
endif ()
if (JPEG_FOUND)
  set (H5JPEG_HAVE_BZLIB_H 1)
  set (H5JPEG_HAVE_JPEG 1)
  set (H5JPEG_JPEG_HEADER "bzlib.h")
  set (JPEG_INCLUDE_DIR_GEN ${JPEG_INCLUDE_DIR})
  set (JPEG_INCLUDE_DIRS ${JPEG_INCLUDE_DIR})
else ()
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    EXTERNAL_JPEG_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT} STATIC)
    set (H5JPEG_HAVE_BZLIB_H 1)
    set (H5JPEG_HAVE_JPEG 1)
    message (STATUS "Filter JPEG is built")
  else ()
    message (FATAL_ERROR " jpeg is Required for ${H5JPEG_PACKAGE_NAME} library")
  endif ()
endif ()
message (STATUS "jpeg found: INC=${JPEG_INCLUDE_DIR} LIBS=${JPEG_LIBRARIES}")
set (LINK_LIBS ${LINK_LIBS} ${JPEG_LIBRARIES})

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5jpeg Unit Testing" OFF)
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
  if (JPEG_FOUND AND JPEG_USE_EXTERNAL)
    add_dependencies (${H5JPEG_LIB_TARGET} JPEG)
  endif ()
endif ()

option (BUILD_EXAMPLES "Build h5jpeg Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5JPEG)
