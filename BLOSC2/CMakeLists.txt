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
project (H5BLOSC2 C)

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
# Basic H5BLOSC2 stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5BLOSC2_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5BLOSC2_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (${BLOSC2_PACKAGE_NAME})

# parse the full version numbers from blosc2_version.h
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/blosc2_version.h _vers_h_contents)
string(REGEX REPLACE ".*#define[ \t]+BLOSC2_PLUGIN_VERSION_MAJOR[ \t]+([0-9]+).*"
        "\\1" H5BLOSC2_VERS_MAJOR ${_vers_h_contents})
string(REGEX REPLACE ".*#define[ \t]+BLOSC2_PLUGIN_VERSION_MINOR[ \t]+([0-9]+).*"
        "\\1" H5BLOSC2_VERS_MINOR ${_vers_h_contents})
string(REGEX REPLACE ".*#define[ \t]+BLOSC2_PLUGIN_VERSION_INTERFACE[ \t]+([0-9]+).*"
        "\\1" H5BLOSC2_VERS_INTERFACE ${_vers_h_contents})
string(REGEX REPLACE ".*#define[ \t]+BLOSC2_PLUGIN_VERSION_RELEASE[ \t]+([0-9]+).*"
        "\\1" H5BLOSC2_VERS_RELEASE ${_vers_h_contents})
string(REGEX REPLACE ".*#define[ \t]+BLOSC2_PLUGIN_VERSION_STRING[ \t]+\"([-0-9A-Za-z.]+)\".*"
        "\\1" H5BLOSC2_VERSION_STRING ${_vers_h_contents})

set (H5BLOSC2_SOVERS_MAJOR 3)
set (H5BLOSC2_SOVERS_MINOR 1)
set (H5BLOSC2_SOVERS_INTERFACE 3)
set (H5BLOSC2_SOVERS_RELEASE 0)
math (EXPR H5BLOSC2_SOVERS_MAJOR ${H5BLOSC2_SOVERS_INTERFACE}-${H5BLOSC2_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5BLOSC2_PACKAGE_VERSION "${H5BLOSC2_VERS_MAJOR}.${H5BLOSC2_VERS_MINOR}.${H5BLOSC2_VERS_RELEASE}")
set (H5BLOSC2_PACKAGE_VERSION_STRING "${H5BLOSC2_PACKAGE_VERSION}")
set (H5BLOSC2_PACKAGE_VERSION_MAJOR "${H5BLOSC2_VERS_MAJOR}")
set (H5BLOSC2_PACKAGE_VERSION_MINOR "${H5BLOSC2_VERS_MINOR}")
set (H5BLOSC2_PACKAGE_STRING "${H5BLOSC2_PACKAGE_NAME} ${H5BLOSC2_PACKAGE_VERSION}")
set (H5BLOSC2_PACKAGE_TARNAME "${H5BLOSC2_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5BLOSC2_PACKAGE_URL "http://www.hdfgroup.org")
set (H5BLOSC2_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5BLOSC2_PACKAGE_SOVERSION "${H5BLOSC2_SOVERS_MAJOR}.${H5BLOSC2_SOVERS_RELEASE}.${H5BLOSC2_SOVERS_MINOR}")
set (H5BLOSC2_PACKAGE_SOVERSION_MAJOR "${H5BLOSC2_SOVERS_MAJOR}")
message (STATUS "Configuring for blosc2 HDF5 Plugin version: " ${H5BLOSC2_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5BLOSC2_RESOURCES_DIR}/H5BLOSC2Macros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5BLOSC2_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5BLOSC2 link libs: ${H5PL_LINK_LIBS}")

set (CMAKE_THREAD_PREFER_PTHREAD TRUE)
if(WIN32)
    # try to use the system library
    find_package(Threads)
    if(Threads_FOUND)
        set (LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
    endif()
else()
    find_package(Threads REQUIRED)
    set (H5PL_LINK_LIBS ${H5PL_LINK_LIBS} ${CMAKE_THREAD_LIBS_INIT})
endif()

include (ExternalProject)
option (H5PL_ALLOW_EXTERNAL_SUPPORT "Allow External Library Building (NO GIT TGZ)" "NO")
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  option (BLOSC2_USE_EXTERNAL "Use External Library Building for BLOSC2" 1)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (BLOSC2_URL ${BLOSC2_GIT_URL} CACHE STRING "Path to blosc2 git repository")
    set (BLOSC2_BRANCH ${BLOSC2_GIT_BRANCH})
  elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT H5PL_COMP_TGZPATH)
      set (H5PL_COMP_TGZPATH ${H5BLOSC2_SOURCE_DIR})
    endif ()
    set (BLOSC2_URL ${H5PL_COMP_TGZPATH}/${BLOSC2_TGZ_NAME})
  else ()
    set (BLOSC2_USE_EXTERNAL 0)
  endif ()
endif ()

#-----------------------------------------------------------------------------
# Option for Blosc support
#-----------------------------------------------------------------------------
option (HDF_ENABLE_BLOSC2_SUPPORT "Enable blosc2" ON)
if (HDF_ENABLE_BLOSC2_SUPPORT)
  if (NOT H5_BLOSC2_HEADER)
    if (NOT BLOSC2_USE_EXTERNAL)
      find_package (BLOSC2 NAMES ${BLOSC2_PACKAGE_NAME}${HDF_PACKAGE_EXT} COMPONENTS static)
      if (NOT BLOSC2_FOUND)
        find_package (BLOSC2) # Legacy find
        if (BLOSC2_FOUND)
          set (H5PL_LINK_LIBS ${H5PL_LINK_LIBS} ${BLOSC2_LIBRARIES})
        endif ()
      endif ()
    endif ()
    if (BLOSC2_FOUND)
      set (H5_HAVE_BLOSC2_H 1)
      set (H5_HAVE_BLOSC2 1)
      set (H5_BLOSC2_HEADER "blosc2.h")
      set (BLOSC2_INCLUDE_DIR ${BLOSC2_INCLUDE_DIR})
      set (BLOSC2_INCLUDE_DIRS ${BLOSC2_INCLUDE_DIRS} ${BLOSC2_INCLUDE_DIR})
    else ()
      if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
        EXTERNAL_BLOSC2_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
        set (H5_HAVE_BLOSC2_H 1)
        set (H5_HAVE_BLOSC2 1)
        set (H5_BLOSC2_HEADER "blosc2.h")
        message (STATUS "BLOSC2 is built")
      else ()
        message (FATAL_ERROR " BLOSC2 is Required for BLOSC2 support in plugin")
      endif ()
    endif ()
  else ()
    # This project is being called from within another and BLOSC2 is already configured
    set (H5_HAVE_BLOSC2_H 1)
  endif ()
  set (H5PL_LINK_LIBS ${H5PL_LINK_LIBS} ${BLOSC2_LIBRARIES})
  set (H5BLOSC2_INCLUDE_DIRS ${H5BLOSC2_INCLUDE_DIRS} ${BLOSC2_INCLUDE_DIRS})
  message (STATUS "BLOSC2 is ON")
endif ()

#-----------------------------------------------------------------------------
# Generate the blosc2_config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5BLOSC2_RESOURCES_DIR}/config.h.in ${H5BLOSC2_BINARY_DIR}/blosc2_config.h @ONLY)

#-----------------------------------------------------------------------------
# Option for external libraries on windows
#-----------------------------------------------------------------------------
option (HDF_PACKAGE_EXTLIBS "CPACK - include external libraries" OFF)
if (NOT HDF_EXTERNALLY_CONFIGURED)
  if (HDF_PACKAGE_EXTLIBS)
    set (HDF_NO_PACKAGES OFF CACHE BOOL "CPACK - Disable packaging" FORCE)
  endif ()
endif ()

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (H5PL_BUILD_TESTING "Build h5blosc2 Unit Testing" OFF)
if (H5PL_BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE STRING
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing ()
  include (CTest)
  include (${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file (${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

add_subdirectory (src)

option (H5PL_BUILD_EXAMPLES "Build h5blosc2 Examples" OFF)
if (H5PL_BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5BLOSC2)
