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
project (H5GRANULAR_BITROUND C)

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
# Basic H5GRANULAR_BITROUND stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5GRANULAR_BITROUND_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5GRANULAR_BITROUND_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)
  include (${H5GRANULAR_BITROUND_SOURCE_DIR}/config/CacheURLs.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (H5${GRANULAR_BITROUND_PACKAGE_NAME})

set (H5GRANULAR_BITROUND_VERS_MAJOR 1)
set (H5GRANULAR_BITROUND_VERS_MINOR 0)
set (H5GRANULAR_BITROUND_VERS_RELEASE 0)

set (H5GRANULAR_BITROUND_SOVERS_MAJOR 1)
set (H5GRANULAR_BITROUND_SOVERS_MINOR 0)
set (H5GRANULAR_BITROUND_SOVERS_INTERFACE 1)
set (H5GRANULAR_BITROUND_SOVERS_RELEASE 0)
math (EXPR H5GRANULAR_BITROUND_SOVERS_MAJOR ${H5GRANULAR_BITROUND_SOVERS_INTERFACE}-${H5GRANULAR_BITROUND_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5GRANULAR_BITROUND_PACKAGE_VERSION "${H5GRANULAR_BITROUND_VERS_MAJOR}.${H5GRANULAR_BITROUND_VERS_MINOR}")
set (H5GRANULAR_BITROUND_PACKAGE_VERSION_STRING "${H5GRANULAR_BITROUND_PACKAGE_VERSION}")
set (H5GRANULAR_BITROUND_PACKAGE_VERSION_MAJOR "${H5GRANULAR_BITROUND_VERS_MAJOR}")
set (H5GRANULAR_BITROUND_PACKAGE_VERSION_MINOR "${H5GRANULAR_BITROUND_VERS_MINOR}")
set (H5GRANULAR_BITROUND_PACKAGE_STRING "${H5GRANULAR_BITROUND_PACKAGE_NAME} ${H5GRANULAR_BITROUND_PACKAGE_VERSION}")
set (H5GRANULAR_BITROUND_PACKAGE_TARNAME "${H5GRANULAR_BITROUND_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5GRANULAR_BITROUND_PACKAGE_URL "http://www.hdfgroup.org")
set (H5GRANULAR_BITROUND_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5GRANULAR_BITROUND_PACKAGE_SOVERSION "${H5GRANULAR_BITROUND_SOVERS_MAJOR}.${H5GRANULAR_BITROUND_SOVERS_RELEASE}.${H5GRANULAR_BITROUND_SOVERS_MINOR}")
set (H5GRANULAR_BITROUND_PACKAGE_SOVERSION_MAJOR "${H5GRANULAR_BITROUND_SOVERS_MAJOR}")
message(STATUS "Configuring for granularbr HDF5 Plugin version: " ${H5GRANULAR_BITROUND_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5GRANULAR_BITROUND_RESOURCES_DIR}/H5GRANULAR_BITROUNDMacros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5GRANULAR_BITROUND_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5GRANULAR_BITROUND link libs: ${H5PL_LINK_LIBS}")

#-----------------------------------------------------------------------------
# Generate the granularbr_config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5GRANULAR_BITROUND_RESOURCES_DIR}/config.h.in ${H5GRANULAR_BITROUND_BINARY_DIR}/granularbr_config.h @ONLY)

include (ExternalProject)
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
#-----------------------------------------------------------------------------
option (BUILD_GRANULAR_BITROUND_LIBRARY_SOURCE "build the granularbr library within the plugin" ON)
if (NOT BUILD_GRANULAR_BITROUND_LIBRARY_SOURCE)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    option (GRANULAR_BITROUND_USE_EXTERNAL "Use External Library Building for GRANULAR_BITROUND" 1)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT") 
      message (FATAL_ERROR " GRANULAR_BITROUND has no compression only library repository")
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT H5PL_COMP_TGZPATH)
        set (H5PL_COMP_TGZPATH ${H5GRANULAR_BITROUND_SOURCE_DIR})
      endif ()
      set (GRANULAR_BITROUND_URL ${H5PL_COMP_TGZPATH}/${GRANULAR_BITROUND_TGZ_NAME})
    else ()
      set (GRANULAR_BITROUND_USE_EXTERNAL 0)
    endif ()
  endif ()
  #-----------------------------------------------------------------------------
  # granularbr Library Settings
  #-----------------------------------------------------------------------------
  if (NOT H5_GRANULAR_BITROUND_HEADER)
    if (NOT GRANULAR_BITROUND_USE_EXTERNAL)
      find_package (GRANULAR_BITROUND NAMES ${GRANULAR_BITROUND_PACKAGE_NAME}${HDF_PACKAGE_EXT} COMPONENTS static)
      if (NOT GRANULAR_BITROUND_FOUND)
        find_package (GRANULAR_BITROUND) # Legacy find
        if (GRANULAR_BITROUND_FOUND)
          set (H5PL_LINK_LIBS ${H5PL_LINK_LIBS} ${GRANULAR_BITROUND_LIBRARIES})
        endif ()
      endif ()
    endif ()
    if (GRANULAR_BITROUND_FOUND)
      set (H5_HAVE_GRANULAR_BITROUND 1)
      set (GRANULAR_BITROUND_INCLUDE_DIR_GEN ${GRANULAR_BITROUND_INCLUDE_DIR})
      set (GRANULAR_BITROUND_INCLUDE_DIRS ${GRANULAR_BITROUND_INCLUDE_DIRS} ${GRANULAR_BITROUND_INCLUDE_DIR})
    else ()
      if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
        EXTERNAL_GRANULAR_BITROUND_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
        set (H5_HAVE_GRANULAR_BITROUND 1)
        message (STATUS "Filter GRANULAR_BITROUND is built")
      else ()
        message (FATAL_ERROR " GRANULAR_BITROUND is Required for ${H5GRANULAR_BITROUND_PACKAGE_NAME} library")
      endif ()
    endif ()
  endif ()
  message (STATUS "granularbr found: INC=${GRANULAR_BITROUND_INCLUDE_DIR} TOOLS=${GRANULAR_BITROUND_LIBRARIES}")
  set (H5PL_LINK_LIBS ${H5PL_LINK_LIBS} ${GRANULAR_BITROUND_LIBRARIES})

  if (GRANULAR_BITROUND_FOUND)
    PACKAGE_GRANULAR_BITROUND_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
  endif ()
endif ()
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (H5PL_BUILD_TESTING "Build h5granularbr Unit Testing" OFF)
if (H5PL_BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE STRING
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing ()
  include (CTest)
  include (${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file (${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

add_subdirectory (src)

option (H5PL_BUILD_EXAMPLES "Build h5granularbr Examples" OFF)
if (H5PL_BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5GRANULAR_BITROUND)
