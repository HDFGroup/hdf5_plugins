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
project (H5SZ C)

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
# Basic H5SZ stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5SZ_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5SZ_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (${SZ_PACKAGE_NAME})

set (H5SZ_VERS_MAJOR 1)
set (H5SZ_VERS_MINOR 0)
set (H5SZ_VERS_RELEASE 0)

set (H5SZ_SOVERS_MAJOR 1)
set (H5SZ_SOVERS_MINOR 0)
set (H5SZ_SOVERS_INTERFACE 1)
set (H5SZ_SOVERS_RELEASE 0)
math (EXPR H5SZ_SOVERS_MAJOR ${H5SZ_SOVERS_INTERFACE}-${H5SZ_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5SZ_PACKAGE_VERSION "${H5SZ_VERS_MAJOR}.${H5SZ_VERS_MINOR}")
set (H5SZ_PACKAGE_VERSION_STRING "${H5SZ_PACKAGE_VERSION}")
set (H5SZ_PACKAGE_VERSION_MAJOR "${H5SZ_VERS_MAJOR}")
set (H5SZ_PACKAGE_VERSION_MINOR "${H5SZ_VERS_MINOR}")
set (H5SZ_PACKAGE_STRING "${H5SZ_PACKAGE_NAME} ${H5SZ_PACKAGE_VERSION}")
set (H5SZ_PACKAGE_TARNAME "${H5SZ_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5SZ_PACKAGE_URL "http://www.hdfgroup.org")
set (H5SZ_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5SZ_PACKAGE_SOVERSION "${H5SZ_SOVERS_MAJOR}.${H5SZ_SOVERS_RELEASE}.${H5SZ_SOVERS_MINOR}")
set (H5SZ_PACKAGE_SOVERSION_MAJOR "${H5SZ_SOVERS_MAJOR}")
message("Configuring for SZ HDF5 Plugin version: " ${H5SZ_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5SZ_RESOURCES_DIR}/H5SZMacros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5SZ_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5SZ link libs: ${LINK_LIBS}")

#-----------------------------------------------------------------------------
# Generate the SZ_config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5SZ_RESOURCES_DIR}/config.h.in ${H5SZ_BINARY_DIR}/SZ_config.h @ONLY)

include (ExternalProject)
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  option (SZ_USE_EXTERNAL "Use External Library Building for SZ" 1)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (SZ_URL ${SZ_GIT_URL} CACHE STRING "Path to SZ git repository")
    set (SZ_BRANCH ${SZ_GIT_BRANCH})
  elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${H5SZ_SOURCE_DIR})
    endif ()
    set (SZ_URL ${TGZPATH}/${SZ_TGZ_NAME})
  else ()
    set (SZ_USE_EXTERNAL 0)
  endif ()
endif ()
#-----------------------------------------------------------------------------
# SZ Library Settings
#-----------------------------------------------------------------------------
if (NOT H5_SZ_HEADER)
  if (NOT SZ_USE_EXTERNAL)
    find_package (SZ NAMES ${SZ_PACKAGE_NAME}${HDF_PACKAGE_EXT} COMPONENTS static)
    if (NOT SZ_FOUND)
      find_package (SZ) # Legacy find
      if (SZ_FOUND)
        set (LINK_LIBS ${LINK_LIBS} ${SZ_LIBRARIES})
      endif ()
    endif ()
  endif ()
  if (SZ_FOUND)
    set (H5_HAVE_SZ_H 1)
    set (H5_HAVE_SZ 1)
    set (H5_SZ_HEADER "sz.h")
    set (SZ_INCLUDE_DIR_GEN ${SZ_INCLUDE_DIR})
    set (SZ_INCLUDE_DIRS ${SZ_INCLUDE_DIRS} ${SZ_INCLUDE_DIR})
  else ()
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      EXTERNAL_SZ_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT} STATIC)
      set (H5_HAVE_SZ_H 1)
      set (H5_HAVE_SZ 1)
      message (STATUS "Filter SZ is built")
    else ()
      message (FATAL_ERROR " SZ is Required for ${H5SZ_PACKAGE_NAME} library")
    endif ()
  endif ()
else ()
  # This project is being called from within another and SZ is already configured
  set (H5_HAVE_SZ_H 1)
endif ()
message (STATUS "SZ found: INC=${SZ_INCLUDE_DIR} TOOLS=${SZ_LIBRARIES}")
set (LINK_LIBS ${LINK_LIBS} ${SZ_LIBRARIES})

if (SZ_FOUND)
  PACKAGE_SZ_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
endif ()
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5SZ Unit Testing" OFF)
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
  if (SZ_FOUND AND SZ_USE_EXTERNAL)
    add_dependencies (${H5SZ_LIB_TARGET} SZ)
  endif ()
endif ()

option (BUILD_EXAMPLES "Build h5SZ Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5SZ)
