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
project (H5BITGROOM C)

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
# Basic H5BITGROOM stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5BITGROOM_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5BITGROOM_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)
  include (${H5BITGROOM_SOURCE_DIR}/config/CacheURLs.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (${BITGROOM_PACKAGE_NAME})

set (H5BITGROOM_VERS_MAJOR 1)
set (H5BITGROOM_VERS_MINOR 0)
set (H5BITGROOM_VERS_RELEASE 0)

set (H5BITGROOM_SOVERS_MAJOR 1)
set (H5BITGROOM_SOVERS_MINOR 0)
set (H5BITGROOM_SOVERS_INTERFACE 1)
set (H5BITGROOM_SOVERS_RELEASE 0)
math (EXPR H5BITGROOM_SOVERS_MAJOR ${H5BITGROOM_SOVERS_INTERFACE}-${H5BITGROOM_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5BITGROOM_PACKAGE_VERSION "${H5BITGROOM_VERS_MAJOR}.${H5BITGROOM_VERS_MINOR}")
set (H5BITGROOM_PACKAGE_VERSION_STRING "${H5BITGROOM_PACKAGE_VERSION}")
set (H5BITGROOM_PACKAGE_VERSION_MAJOR "${H5BITGROOM_VERS_MAJOR}")
set (H5BITGROOM_PACKAGE_VERSION_MINOR "${H5BITGROOM_VERS_MINOR}")
set (H5BITGROOM_PACKAGE_STRING "${H5BITGROOM_PACKAGE_NAME} ${H5BITGROOM_PACKAGE_VERSION}")
set (H5BITGROOM_PACKAGE_TARNAME "${H5BITGROOM_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5BITGROOM_PACKAGE_URL "http://www.hdfgroup.org")
set (H5BITGROOM_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5BITGROOM_PACKAGE_SOVERSION "${H5BITGROOM_SOVERS_MAJOR}.${H5BITGROOM_SOVERS_RELEASE}.${H5BITGROOM_SOVERS_MINOR}")
set (H5BITGROOM_PACKAGE_SOVERSION_MAJOR "${H5BITGROOM_SOVERS_MAJOR}")
message(STATUS "Configuring for bitgroom HDF5 Plugin version: " ${H5BITGROOM_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5BITGROOM_RESOURCES_DIR}/H5BITGROOMMacros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5BITGROOM_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5BITGROOM link libs: ${H5PL_LINK_LIBS}")

#-----------------------------------------------------------------------------
# Generate the bitgroom_config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5BITGROOM_RESOURCES_DIR}/config.h.in ${H5BITGROOM_BINARY_DIR}/bitgroom_config.h @ONLY)

include (ExternalProject)
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
#-----------------------------------------------------------------------------
option (BUILD_BITGROOM_LIBRARY_SOURCE "build the bitgroom library within the plugin" ON)
if (NOT BUILD_BITGROOM_LIBRARY_SOURCE)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    option (BITGROOM_USE_EXTERNAL "Use External Library Building for BITGROOM" 1)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT") 
      message (FATAL_ERROR " BITGROOM has no compression only library repository")
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT H5PL_COMP_TGZPATH)
        set (H5PL_COMP_TGZPATH ${H5BITGROOM_SOURCE_DIR})
      endif ()
      set (BITGROOM_URL ${H5PL_COMP_TGZPATH}/${BITGROOM_TGZ_NAME})
    else ()
      set (BITGROOM_USE_EXTERNAL 0)
    endif ()
  endif ()
  #-----------------------------------------------------------------------------
  # bitgroom Library Settings
  #-----------------------------------------------------------------------------
  if (NOT H5_BITGROOM_HEADER)
    if (NOT BITGROOM_USE_EXTERNAL)
      find_package (BITGROOM NAMES ${BITGROOM_PACKAGE_NAME}${HDF_PACKAGE_EXT} COMPONENTS static)
      if (NOT BITGROOM_FOUND)
        find_package (BITGROOM) # Legacy find
        if (BITGROOM_FOUND)
          set (H5PL_LINK_LIBS ${H5PL_LINK_LIBS} ${BITGROOM_LIBRARIES})
        endif ()
      endif ()
    endif ()
    if (BITGROOM_FOUND)
      set (H5_HAVE_BITGROOM 1)
      set (BITGROOM_INCLUDE_DIR_GEN ${BITGROOM_INCLUDE_DIR})
      set (BITGROOM_INCLUDE_DIRS ${BITGROOM_INCLUDE_DIRS} ${BITGROOM_INCLUDE_DIR})
    else ()
      if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
        EXTERNAL_BITGROOM_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
        set (H5_HAVE_BITGROOM 1)
        message (STATUS "Filter BITGROOM is built")
      else ()
        message (FATAL_ERROR " BITGROOM is Required for ${H5BITGROOM_PACKAGE_NAME} library")
      endif ()
    endif ()
  endif ()
  message (STATUS "bitgroom found: INC=${BITGROOM_INCLUDE_DIR} TOOLS=${BITGROOM_LIBRARIES}")
  set (H5PL_LINK_LIBS ${H5PL_LINK_LIBS} ${BITGROOM_LIBRARIES})

  if (BITGROOM_FOUND)
    PACKAGE_BITGROOM_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
  endif ()
endif ()
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (H5PL_BUILD_TESTING "Build h5bitgroom Unit Testing" OFF)
if (H5PL_BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE STRING
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing ()
  include (CTest)
  include (${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file (${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

add_subdirectory (src)

option (H5PL_BUILD_EXAMPLES "Build h5bitgroom Examples" OFF)
if (H5PL_BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5BITGROOM)
