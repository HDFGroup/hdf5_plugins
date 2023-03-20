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
project (H5LZF C)

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
# Basic H5LZF stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5LZF_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5LZF_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (${LZF_PACKAGE_NAME})

set (H5LZF_VERS_MAJOR 1)
set (H5LZF_VERS_MINOR 0)
set (H5LZF_VERS_RELEASE 0)

set (H5LZF_SOVERS_MAJOR 1)
set (H5LZF_SOVERS_MINOR 0)
set (H5LZF_SOVERS_INTERFACE 1)
set (H5LZF_SOVERS_RELEASE 0)
math (EXPR H5LZF_SOVERS_MAJOR ${H5LZF_SOVERS_INTERFACE}-${H5LZF_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5LZF_PACKAGE_VERSION "${H5LZF_VERS_MAJOR}.${H5LZF_VERS_MINOR}")
set (H5LZF_PACKAGE_VERSION_STRING "${H5LZF_PACKAGE_VERSION}")
set (H5LZF_PACKAGE_VERSION_MAJOR "${H5LZF_VERS_MAJOR}")
set (H5LZF_PACKAGE_VERSION_MINOR "${H5LZF_VERS_MINOR}")
set (H5LZF_PACKAGE_STRING "${H5LZF_PACKAGE_NAME} ${H5LZF_PACKAGE_VERSION}")
set (H5LZF_PACKAGE_TARNAME "${H5LZF_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5LZF_PACKAGE_URL "http://www.hdfgroup.org")
set (H5LZF_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5LZF_PACKAGE_SOVERSION "${H5LZF_SOVERS_MAJOR}.${H5LZF_SOVERS_RELEASE}.${H5LZF_SOVERS_MINOR}")
set (H5LZF_PACKAGE_SOVERSION_MAJOR "${H5LZF_SOVERS_MAJOR}")
message("Configuring for lzf HDF5 Plugin version: " ${H5LZF_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5LZF_RESOURCES_DIR}/H5LZFMacros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5LZF_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5LZF link libs: ${LINK_LIBS}")

#-----------------------------------------------------------------------------
# Generate the lzf_config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5LZF_RESOURCES_DIR}/config.h.in ${H5LZF_BINARY_DIR}/lzf_config.h @ONLY)

include (ExternalProject)
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  option (LZF_USE_EXTERNAL "Use External Library Building for LZF" 1)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (LZF_URL ${LZF_GIT_URL} CACHE STRING "Path to lzf git repository")
    set (LZF_BRANCH ${LZF_GIT_BRANCH})
  elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${H5LZF_SOURCE_DIR})
    endif ()
    set (LZF_URL ${TGZPATH}/${LZF_TGZ_NAME})
  else ()
    set (LZF_USE_EXTERNAL 0)
  endif ()
endif ()
#-----------------------------------------------------------------------------
# lzf Library Settings
#-----------------------------------------------------------------------------
if (NOT H5_LZF_HEADER)
  if (NOT LZF_USE_EXTERNAL)
    find_package (LZF NAMES ${LZF_PACKAGE_NAME}${HDF_PACKAGE_EXT} COMPONENTS static)
    if (NOT LZF_FOUND)
      find_package (LZF) # Legacy find
      if (LZF_FOUND)
        set (LINK_LIBS ${LINK_LIBS} ${LZF_LIBRARIES})
      endif ()
    endif ()
  endif ()
  if (LZF_FOUND)
    set (H5_HAVE_LZF_H 1)
    set (H5_HAVE_LZF 1)
    set (H5_LZF_HEADER "lzf.h")
    set (LZF_INCLUDE_DIR_GEN ${LZF_INCLUDE_DIR})
    set (LZF_INCLUDE_DIRS ${LZF_INCLUDE_DIRS} ${LZF_INCLUDE_DIR})
  else ()
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      EXTERNAL_LZF_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT} STATIC)
      set (H5_HAVE_LZF_H 1)
      set (H5_HAVE_LZF 1)
      message (STATUS "Filter LZF is built")
    else ()
      message (FATAL_ERROR " LZF is Required for ${H5LZF_PACKAGE_NAME} library")
    endif ()
  endif ()
else ()
  # This project is being called from within another and LZF is already configured
  set (H5_HAVE_LZF_H 1)
endif ()
message (STATUS "lzf found: INC=${LZF_INCLUDE_DIR} TOOLS=${LZF_LIBRARIES}")
set (LINK_LIBS ${LINK_LIBS} ${LZF_LIBRARIES})

if (LZF_FOUND)
  PACKAGE_LZF_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
endif ()
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5lzf Unit Testing" OFF)
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
  if (LZF_FOUND AND LZF_USE_EXTERNAL)
    add_dependencies (${H5LZF_LIB_TARGET} LZF)
  endif ()
endif ()

option (BUILD_EXAMPLES "Build h5lzf Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5LZF)
