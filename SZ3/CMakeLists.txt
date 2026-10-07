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

# Wrapper CMakeLists.txt for SZ3 subtree integration
# This integrates the upstream H5Z-SZ3 filter (tools/H5Z-SZ3 of SZ3, via git
# subtree) with the HDF Group's hdf5_plugins build system and testing infrastructure.

cmake_minimum_required(VERSION 3.18)
project(H5SZ3 C CXX)

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
# Verify SZ3 subtree exists
#-----------------------------------------------------------------------------
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SZ3/tools/H5Z-SZ3/CMakeLists.txt")
    message(FATAL_ERROR
        "SZ3 not found at ${CMAKE_CURRENT_SOURCE_DIR}/SZ3\n"
        "The upstream SZ3 code should be present via git subtree.")
endif()

# SZ3's CMake requires CMake 3.19
if (CMAKE_VERSION VERSION_LESS "3.19")
  message(FATAL_ERROR "The SZ3 filter plugin requires CMake 3.19 or newer")
endif ()

message(STATUS "Using SZ3 from subtree at ${CMAKE_CURRENT_SOURCE_DIR}/SZ3")

#-----------------------------------------------------------------------------
# Basic H5SZ3 stuff - integrate with HDF Group build system
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include(${H5SZ3_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include(${H5SZ3_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

  SET_HDF_BUILD_TYPE()
endif ()

# Set up package name for HDF Group infrastructure
if (NOT H5SZ3_PACKAGE_NAME)
  set(H5SZ3_PACKAGE_NAME "sz3")
endif()

BASIC_SETTINGS(H5${H5SZ3_PACKAGE_NAME})

# The H5Z-SZ3 filter has no decode-only build
if (DISABLE_H5SZ3_ENCODER)
  message(FATAL_ERROR "The ${H5SZ3_PACKAGE_NAME} filter cannot be built without its encoder; set ENABLE_SZ3=OFF instead")
endif ()

#-----------------------------------------------------------------------------
# Version information (read from SZ3)
#-----------------------------------------------------------------------------
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/SZ3/CMakeLists.txt" SZ3_PROJECT_LINE REGEX "^project\\(SZ3 VERSION ")
string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+)" SZ3_PROJECT_VERSION "${SZ3_PROJECT_LINE}")
set(H5SZ3_VERS_MAJOR ${CMAKE_MATCH_1})
set(H5SZ3_VERS_MINOR ${CMAKE_MATCH_2})
set(H5SZ3_VERS_RELEASE ${CMAKE_MATCH_3})

set(H5SZ3_PACKAGE_VERSION "${H5SZ3_VERS_MAJOR}.${H5SZ3_VERS_MINOR}.${H5SZ3_VERS_RELEASE}")
set(H5SZ3_PACKAGE_VERSION_STRING "${H5SZ3_PACKAGE_VERSION}")
set(H5SZ3_PACKAGE_VERSION_MAJOR "${H5SZ3_VERS_MAJOR}")
set(H5SZ3_PACKAGE_VERSION_MINOR "${H5SZ3_VERS_MINOR}")
set(H5SZ3_PACKAGE_STRING "${H5SZ3_PACKAGE_NAME} ${H5SZ3_PACKAGE_VERSION}")
set(H5SZ3_PACKAGE_URL "http://www.hdfgroup.org")
set(H5SZ3_PACKAGE_BUGREPORT "help@hdfgroup.org")

message(STATUS "Configuring SZ3 HDF5 Plugin version: ${H5SZ3_PACKAGE_STRING} (from upstream SZ3)")

#-----------------------------------------------------------------------------
# Include HDF Group macros
#-----------------------------------------------------------------------------
include(${H5SZ3_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT(TRUE)
message(STATUS "H5SZ3 link libs: ${H5PL_LINK_LIBS}")

#-----------------------------------------------------------------------------
# Add the upstream H5Z-SZ3 filter via SZ3's own CMake
# Note: SZ3 is a header-only C++17 library; with BUILD_H5Z_FILTER its
# CMakeLists.txt builds the filter plugin (target hdf5sz3) from tools/H5Z-SZ3
#-----------------------------------------------------------------------------
message(STATUS "Building H5Z-SZ3 filter from subtree")

# Build the filter, and none of SZ3's tools or tests
set(BUILD_H5Z_FILTER ON)
set(BUILD_SZ3_BINARY OFF)
set(BUILD_MDZ OFF)
set(BUILD_PARAVIEW_PLUGIN OFF)

# Compile the Zstd bundled in SZ3 (SZ3/tools/zstd) into the plugin, so the
# plugin needs no Zstd library at build or run time
set(SZ3_USE_BUNDLED_ZSTD ON)

# SZ3 installs the filter plugin into H5Z_SZ3_PLUGIN_INSTALL_DIR (default
# lib/plugin) only when SZ3_INSTALL is ON, which it is not by default under a
# parent project. Point it at this project's lib/plugin convention
# (H5SZ3_INSTALL_LIB_DIR, set above by BASIC_SETTINGS).
set(SZ3_INSTALL ON)
set(H5Z_SZ3_PLUGIN_INSTALL_DIR ${H5SZ3_INSTALL_LIB_DIR} CACHE STRING
    "Directory to install the HDF5 filter plugin copy into; empty installs no copy")
add_subdirectory(SZ3)

# Copy the plugin to the global plugins directory for testing
# SZ3 builds it in its own binary dir, but tests expect plugins in build/plugins/
# Use a custom target since add_custom_command(TARGET) only works in same directory
add_custom_target(copy_sz3_plugin ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/plugins"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:hdf5sz3>"
    "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:hdf5sz3>"
  DEPENDS hdf5sz3
  COMMENT "Copying SZ3 plugin to ${CMAKE_BINARY_DIR}/plugins/"
)

#-----------------------------------------------------------------------------
# Create compatibility alias for the HDF Group target name
#-----------------------------------------------------------------------------
# The HDF Group build names plugin targets ${H5SZ3_LIB_TARGET} (h5sz3)
# SZ3 creates hdf5sz3, so create an alias for compatibility
if (TARGET hdf5sz3)
  set(H5SZ3_LIB_CORENAME "h5sz3")
  set(H5SZ3_LIB_TARGET ${H5SZ3_LIB_CORENAME})

  # Create alias so parent build system can find it by the HDF Group name
  add_library(${H5SZ3_LIB_TARGET} ALIAS hdf5sz3)

  message(STATUS "Created alias: ${H5SZ3_LIB_TARGET} -> hdf5sz3")
endif ()

#-----------------------------------------------------------------------------
# Export the library for parent build system
#-----------------------------------------------------------------------------
# Export the expected target name for the parent build system
set(H5SZ3_LIBRARIES_TO_EXPORT "${H5SZ3_LIB_TARGET}" CACHE STRING "H5SZ3 libraries to export" FORCE)

#-----------------------------------------------------------------------------
# The SZ3 and Zstd licenses ask binary redistributions to carry their notices
#-----------------------------------------------------------------------------
install(
  FILES
    ${H5SZ3_SOURCE_DIR}/Additional_Legal/copyright-and-BSD-license.txt
    ${H5SZ3_SOURCE_DIR}/Additional_Legal/zstd-LICENSE
  DESTINATION ${H5SZ3_INSTALL_DOC_DIR}/SZ3
  COMPONENT libraries
)

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option(H5PL_BUILD_TESTING "Build h5sz3 Unit Testing" OFF)
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
# Examples are needed for testing, so enable them if testing is enabled
if (H5PL_BUILD_TESTING AND NOT H5PL_BUILD_EXAMPLES)
  set(H5PL_BUILD_EXAMPLES ON CACHE BOOL "Build h5sz3 Examples (required for testing)" FORCE)
  message(STATUS "Enabling H5PL_BUILD_EXAMPLES (required for H5PL_BUILD_TESTING)")
endif ()

option(H5PL_BUILD_EXAMPLES "Build h5sz3 Examples" OFF)
if (H5PL_BUILD_EXAMPLES)
  add_subdirectory(example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT(H5SZ3)
