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

# Wrapper CMakeLists.txt for H5Z-ZFP subtree integration
# This integrates the upstream H5Z-ZFP filter (via git subtree) with
# the HDF Group's hdf5_plugins build system and testing infrastructure.

cmake_minimum_required(VERSION 3.18)
project(H5ZFP C)

if (POLICY CMP0074)
  # find_package() uses <PackageName>_ROOT variables.
  cmake_policy(SET CMP0074 NEW)
endif ()

if (POLICY CMP0083)
  # To control generation of Position Independent Executable (PIE) or not,
  # some flags are required at link time.
  cmake_policy(SET CMP0083 NEW)
endif ()

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

#-----------------------------------------------------------------------------
# Verify H5Z-ZFP subtree exists
#-----------------------------------------------------------------------------
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/H5Z-ZFP/CMakeLists.txt")
    message(FATAL_ERROR
        "H5Z-ZFP not found at ${CMAKE_CURRENT_SOURCE_DIR}/H5Z-ZFP\n"
        "The upstream H5Z-ZFP code should be present via git subtree.")
endif()

message(STATUS "Using H5Z-ZFP from subtree at ${CMAKE_CURRENT_SOURCE_DIR}/H5Z-ZFP")

#-----------------------------------------------------------------------------
# Basic H5ZFP stuff - integrate with HDF Group build system
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include(${H5ZFP_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include(${H5ZFP_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)
  include(${H5ZFP_SOURCE_DIR}/config/CacheURLs.cmake)

  SET_HDF_BUILD_TYPE()
endif ()

# Set up package name for HDF Group infrastructure
if (NOT H5ZFP_PACKAGE_NAME)
  set(H5ZFP_PACKAGE_NAME "zfp")
endif()

BASIC_SETTINGS(${H5ZFP_PACKAGE_NAME})

#-----------------------------------------------------------------------------
# Version information (read from H5Z-ZFP)
#-----------------------------------------------------------------------------
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/H5Z-ZFP/src/H5Zzfp_version.h" H5Z_ZFP_H REGEX "^\#define H5Z_FILTER_ZFP_VERSION_MAJOR")
string(REGEX REPLACE "^.*MAJOR " "" H5ZFP_VERS_MAJOR "${H5Z_ZFP_H}")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/H5Z-ZFP/src/H5Zzfp_version.h" H5Z_ZFP_H REGEX "^\#define H5Z_FILTER_ZFP_VERSION_MINOR")
string(REGEX REPLACE "^.*MINOR " "" H5ZFP_VERS_MINOR "${H5Z_ZFP_H}")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/H5Z-ZFP/src/H5Zzfp_version.h" H5Z_ZFP_H REGEX "^\#define H5Z_FILTER_ZFP_VERSION_PATCH")
string(REGEX REPLACE "^.*PATCH " "" H5ZFP_VERS_RELEASE "${H5Z_ZFP_H}")

set(H5ZFP_PACKAGE_VERSION "${H5ZFP_VERS_MAJOR}.${H5ZFP_VERS_MINOR}.${H5ZFP_VERS_RELEASE}")
set(H5ZFP_PACKAGE_VERSION_STRING "${H5ZFP_PACKAGE_VERSION}")
set(H5ZFP_PACKAGE_VERSION_MAJOR "${H5ZFP_VERS_MAJOR}")
set(H5ZFP_PACKAGE_VERSION_MINOR "${H5ZFP_VERS_MINOR}")
set(H5ZFP_PACKAGE_STRING "${H5ZFP_PACKAGE_NAME} ${H5ZFP_PACKAGE_VERSION}")
set(H5ZFP_PACKAGE_URL "http://www.hdfgroup.org")
set(H5ZFP_PACKAGE_BUGREPORT "help@hdfgroup.org")

message(STATUS "Configuring ZFP HDF5 Plugin version: ${H5ZFP_PACKAGE_STRING} (from upstream H5Z-ZFP)")

#-----------------------------------------------------------------------------
# Include HDF Group macros
#-----------------------------------------------------------------------------
include(${H5ZFP_RESOURCES_DIR}/H5ZFPMacros.cmake)
include(${H5ZFP_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT(TRUE)
message(STATUS "H5ZFP link libs: ${H5PL_LINK_LIBS}")

#-----------------------------------------------------------------------------
# ZFP compression library handling
#-----------------------------------------------------------------------------
include(ExternalProject)
include(FetchContent)
option(H5PL_ALLOW_EXTERNAL_SUPPORT "Allow External Library Building (NO GIT TGZ)" "NO")
set(H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property(CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)

if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  option(ZFP_USE_EXTERNAL "Use External Library Building for ZFP" 1)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set(ZFP_URL ${ZFP_GIT_URL} CACHE STRING "Path to zfp git repository")
    set(ZFP_BRANCH ${ZFP_GIT_BRANCH})
  elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT H5PL_COMP_TGZPATH)
      set(H5PL_COMP_TGZPATH ${H5ZFP_SOURCE_DIR})
    endif ()
    set(ZFP_URL ${H5PL_COMP_TGZPATH}/${ZFP_TGZ_NAME})
  else ()
    set(ZFP_USE_EXTERNAL 0)
  endif ()
endif ()

#-----------------------------------------------------------------------------
# Find or build ZFP compression library (not the filter)
#-----------------------------------------------------------------------------
if (NOT ZFP_USE_EXTERNAL)
  find_package(ZFP NAMES ${ZFP_PACKAGE_NAME})
  if (NOT ZFP_FOUND)
    find_package(ZFP) # Legacy find
  endif ()
endif ()

if (ZFP_FOUND)
  set(H5_HAVE_ZFP_H 1)
  set(H5_HAVE_ZFP 1)
  set(H5_ZFP_HEADER "zfp.h")
  set(ZFP_INCLUDE_DIR_GEN ${ZFP_INCLUDE_DIR})
  set(ZFP_INCLUDE_DIRS ${ZFP_INCLUDE_DIR})
  message(STATUS "Found ZFP library: ${ZFP_LIBRARIES}")
else ()
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    EXTERNAL_ZFP_LIBRARY(${H5PL_ALLOW_EXTERNAL_SUPPORT})
    set(H5_HAVE_ZFP_H 1)
    set(H5_HAVE_ZFP 1)
    set(ZFP_FOUND 1)
    message(STATUS "ZFP library built from external source")
    message(STATUS "  ZFP_INCLUDE_DIRS: ${ZFP_INCLUDE_DIRS}")
    message(STATUS "  ZFP_LIBRARIES: ${ZFP_LIBRARIES}")
  else ()
    message(FATAL_ERROR "ZFP library is required for ${H5ZFP_PACKAGE_NAME} filter plugin")
  endif ()
endif ()

set(H5PL_LINK_LIBS ${H5PL_LINK_LIBS} ${ZFP_LIBRARIES})

# Create a ZFPConfig.cmake file for H5Z-ZFP to find
# H5Z-ZFP's CMakeLists.txt calls find_package(ZFP REQUIRED CONFIG)
# If we built ZFP via FetchContent, create a minimal config file
if (TARGET zfp)
  # ZFP was built via FetchContent, the target is available
  # Create a minimal ZFPConfig.cmake file
  set(ZFP_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/zfp-config")
  file(MAKE_DIRECTORY "${ZFP_CONFIG_DIR}")

  file(WRITE "${ZFP_CONFIG_DIR}/ZFPConfig.cmake" "
# Generated ZFPConfig.cmake for FetchContent-built ZFP
set(ZFP_FOUND TRUE)
set(ZFP_VERSION \"${zfp_VERSION}\")
set(ZFP_INCLUDE_DIRS \"${ZFP_INCLUDE_DIRS}\")
set(ZFP_LIBRARIES \"${ZFP_LIBRARIES}\")

# The zfp target is already available from FetchContent
if(NOT TARGET zfp AND NOT TARGET ZFP::zfp)
  message(FATAL_ERROR \"ZFP target not found\")
endif()

# Create alias if needed
if(TARGET zfp AND NOT TARGET ZFP::zfp)
  add_library(ZFP::zfp ALIAS zfp)
endif()
")

  set(ZFP_DIR "${ZFP_CONFIG_DIR}" CACHE PATH "Path to ZFP CMake config" FORCE)
  set(ZFP_ROOT "${zfp_BINARY_DIR}" CACHE PATH "Path to ZFP installation" FORCE)
  message(STATUS "Created ZFPConfig.cmake at ${ZFP_CONFIG_DIR} for H5Z-ZFP")
  message(STATUS "  ZFP_DIR: ${ZFP_DIR}")
  message(STATUS "  ZFP_INCLUDE_DIRS: ${ZFP_INCLUDE_DIRS}")
endif ()

#-----------------------------------------------------------------------------
# Add the upstream H5Z-ZFP filter source via subdirectory
# Note: H5Z-ZFP's CMakeLists.txt will build the filter plugin
#-----------------------------------------------------------------------------
message(STATUS "Building H5Z-ZFP filter from subtree")
add_subdirectory(H5Z-ZFP)

#-----------------------------------------------------------------------------
# Export the library for parent build system
#-----------------------------------------------------------------------------
# The H5Z-ZFP CMakeLists.txt creates the 'h5z_zfp' target
# We need to export it for the parent build system
set(H5ZFP_LIBRARIES_TO_EXPORT "h5z_zfp" CACHE STRING "H5ZFP libraries to export" FORCE)

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option(H5PL_BUILD_TESTING "Build h5zfp Unit Testing" OFF)
if (H5PL_BUILD_TESTING)
  set(DART_TESTING_TIMEOUT 1200 CACHE STRING
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing()
  include(CTest)
  include(${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file(${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

#-----------------------------------------------------------------------------
# Add HDF Group examples
#-----------------------------------------------------------------------------
option(H5PL_BUILD_EXAMPLES "Build h5zfp Examples" OFF)
if (H5PL_BUILD_EXAMPLES)
  add_subdirectory(example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT(H5ZFP)
