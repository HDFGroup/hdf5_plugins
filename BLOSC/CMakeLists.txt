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
project (H5BLOSC C)

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
# Basic H5BLOSC stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5BLOSC_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5BLOSC_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (${BLOSC_PACKAGE_NAME})

# parse the full version numbers from blosc_version.h
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/blosc_version.h _vers_h_contents)
string(REGEX REPLACE ".*#define[ \t]+BLOSC_PLUGIN_VERSION_MAJOR[ \t]+([0-9]+).*"
        "\\1" H5BLOSC_VERS_MAJOR ${_vers_h_contents})
string(REGEX REPLACE ".*#define[ \t]+BLOSC_PLUGIN_VERSION_MINOR[ \t]+([0-9]+).*"
        "\\1" H5BLOSC_VERS_MINOR ${_vers_h_contents})
string(REGEX REPLACE ".*#define[ \t]+BLOSC_PLUGIN_VERSION_INTERFACE[ \t]+([0-9]+).*"
        "\\1" H5BLOSC_VERS_INTERFACE ${_vers_h_contents})
string(REGEX REPLACE ".*#define[ \t]+BLOSC_PLUGIN_VERSION_RELEASE[ \t]+([0-9]+).*"
        "\\1" H5BLOSC_VERS_RELEASE ${_vers_h_contents})
string(REGEX REPLACE ".*#define[ \t]+BLOSC_PLUGIN_VERSION_STRING[ \t]+\"([-0-9A-Za-z.]+)\".*"
        "\\1" H5BLOSC_VERSION_STRING ${_vers_h_contents})

set (H5BLOSC_SOVERS_MAJOR 3)
set (H5BLOSC_SOVERS_MINOR 1)
set (H5BLOSC_SOVERS_INTERFACE 3)
set (H5BLOSC_SOVERS_RELEASE 0)
math (EXPR H5BLOSC_SOVERS_MAJOR ${H5BLOSC_SOVERS_INTERFACE}-${H5BLOSC_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5BLOSC_PACKAGE_VERSION "${H5BLOSC_VERS_MAJOR}.${H5BLOSC_VERS_MINOR}.${H5BLOSC_VERS_RELEASE}")
set (H5BLOSC_PACKAGE_VERSION_STRING "${H5BLOSC_PACKAGE_VERSION}")
set (H5BLOSC_PACKAGE_VERSION_MAJOR "${H5BLOSC_VERS_MAJOR}")
set (H5BLOSC_PACKAGE_VERSION_MINOR "${H5BLOSC_VERS_MINOR}")
set (H5BLOSC_PACKAGE_STRING "${H5BLOSC_PACKAGE_NAME} ${H5BLOSC_PACKAGE_VERSION}")
set (H5BLOSC_PACKAGE_TARNAME "${H5BLOSC_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5BLOSC_PACKAGE_URL "http://www.hdfgroup.org")
set (H5BLOSC_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5BLOSC_PACKAGE_SOVERSION "${H5BLOSC_SOVERS_MAJOR}.${H5BLOSC_SOVERS_RELEASE}.${H5BLOSC_SOVERS_MINOR}")
set (H5BLOSC_PACKAGE_SOVERSION_MAJOR "${H5BLOSC_SOVERS_MAJOR}")
message("Configuring for blosc HDF5 Plugin version: " ${H5BLOSC_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5BLOSC_RESOURCES_DIR}/H5BLOSCMacros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5BLOSC_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5BLOSC link libs: ${LINK_LIBS}")

set (CMAKE_THREAD_PREFER_PTHREAD TRUE)
if(WIN32)
    # try to use the system library
    find_package(Threads)
    if(Threads_FOUND)
        set (LIBS ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
    endif()
else()
    find_package(Threads REQUIRED)
    set (LINK_LIBS ${LINK_LIBS} ${CMAKE_THREAD_LIBS_INIT})
endif()

include (ExternalProject)
option (H5PL_ALLOW_EXTERNAL_SUPPORT "Allow External Library Building (NO GIT TGZ)" "NO")
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  option (BLOSC_ZLIB_USE_EXTERNAL "Use External Library Building for BLOSC_ZLIB" 1)
  option (BLOSC_USE_EXTERNAL "Use External Library Building for BLOSC" 1)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (BLOSC_ZLIB_URL ${BLOSC_ZLIB_GIT_URL} CACHE STRING "Path to zlib git repository")
    set (BLOSC_ZLIB_BRANCH ${BLOSC_ZLIB_GIT_BRANCH})
    set (BLOSC_URL ${BLOSC_GIT_URL} CACHE STRING "Path to blosc git repository")
    set (BLOSC_BRANCH ${BLOSC_GIT_BRANCH})
  elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${H5BLOSC_SOURCE_DIR})
    endif ()
    set (BLOSC_ZLIB_URL ${TGZPATH}/${BLOSC_ZLIB_TGZ_NAME})
    set (BLOSC_URL ${TGZPATH}/${BLOSC_TGZ_NAME})
  else (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (BLOSC_ZLIB_USE_EXTERNAL 0)
    set (BLOSC_USE_EXTERNAL 0)
  endif ()
endif ()

#-----------------------------------------------------------------------------
# Option for ZLib support
#-----------------------------------------------------------------------------
option (HDF_ENABLE_BLOSC_ZLIB_SUPPORT "Enable BLOSC Zlib Filters" OFF)
if (HDF_ENABLE_BLOSC_ZLIB_SUPPORT)
  if (NOT H5_ZLIB_HEADER)
    if (NOT BLOSC_ZLIB_USE_EXTERNAL)
      find_package (ZLIB NAMES ${BLOSC_ZLIB_PACKAGE_NAME}${HDF_PACKAGE_EXT} COMPONENTS static)
      if (NOT ZLIB_FOUND)
        find_package (ZLIB) # Legacy find
        if (ZLIB_FOUND)
          set (LINK_LIBS ${LINK_LIBS} ${ZLIB_LIBRARIES})
        endif ()
      endif ()
    endif ()
    if (ZLIB_FOUND)
      set (H5_HAVE_FILTER_DEFLATE 1)
      set (H5_HAVE_ZLIB_H 1)
      set (H5_HAVE_LIBZ 1)
      set (H5_ZLIB_HEADER "zlib.h")
      set (BLOSC_ZLIB_INCLUDE_DIR_GEN ${ZLIB_INCLUDE_DIR})
      set (BLOSC_ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIR})
    else ()
      if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
        EXTERNAL_BLOSC_ZLIB_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT} STATIC)
        set (H5_HAVE_FILTER_DEFLATE 1)
        set (H5_HAVE_ZLIB_H 1)
        set (H5_HAVE_LIBZ 1)
        message (STATUS "Filter BLOSC_ZLIB is built")
      else ()
        message (FATAL_ERROR " BLOSC_ZLib is Required for BLOSC_ZLib support in blosc")
      endif ()
    endif ()
  else (NOT H5_ZLIB_HEADER)
    # This project is being called from within another and ZLib is already configured
    set (H5_HAVE_FILTER_DEFLATE 1)
    set (H5_HAVE_ZLIB_H 1)
    set (H5_HAVE_LIBZ 1)
  endif ()
  if (H5_HAVE_FILTER_DEFLATE)
    set (EXTERNAL_FILTERS "${EXTERNAL_FILTERS} DEFLATE")
  endif ()
  set (LINK_LIBS ${LINK_LIBS} ${ZLIB_LIBRARY})
  set (H5BLOSC_INCLUDE_DIRS ${H5BLOSC_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS})
  message (STATUS "Filter BLOSC_ZLIB is ON")
endif ()

#-----------------------------------------------------------------------------
# Option for Blosc support
#-----------------------------------------------------------------------------
option (HDF_ENABLE_BLOSC_SUPPORT "Enable blosc" ON)
if (HDF_ENABLE_BLOSC_SUPPORT)
  if (NOT H5_BLOSC_HEADER)
    if (NOT BLOSC_USE_EXTERNAL)
      find_package (BLOSC NAMES ${BLOSC_PACKAGE_NAME}${HDF_PACKAGE_EXT} COMPONENTS static)
      if (NOT BLOSC_FOUND)
        find_package (BLOSC) # Legacy find
        if (BLOSC_FOUND)
          set (LINK_LIBS ${LINK_LIBS} ${BLOSC_LIBRARIES})
        endif ()
      endif ()
    endif ()
    if (BLOSC_FOUND)
      set (H5_HAVE_BLOSC_H 1)
      set (H5_HAVE_BLOSC 1)
      set (H5_BLOSC_HEADER "blosc.h")
      set (BLOSC_INCLUDE_DIR ${BLOSC_INCLUDE_DIR})
      set (BLOSC_INCLUDE_DIRS ${BLOSC_INCLUDE_DIRS} ${BLOSC_INCLUDE_DIR})
    else ()
      if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
        EXTERNAL_BLOSC_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT} STATIC)
        set (H5_HAVE_BLOSC_H 1)
        set (H5_HAVE_BLOSC 1)
        set (H5_BLOSC_HEADER "blosc.h")
        message (STATUS "BLOSC is built")
      else ()
        message (FATAL_ERROR " BLOSC is Required for BLOSC support in plugin")
      endif ()
    endif ()
  else ()
    # This project is being called from within another and BLOSC is already configured
    set (H5_HAVE_BLOSC_H 1)
  endif ()
  set (LINK_LIBS ${LINK_LIBS} ${BLOSC_LIBRARY})
  set (H5BLOSC_INCLUDE_DIRS ${H5BLOSC_INCLUDE_DIRS} ${BLOSC_INCLUDE_DIRS})
  message (STATUS "BLOSC is ON")
endif ()

#-----------------------------------------------------------------------------
# Generate the blosc_config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5BLOSC_RESOURCES_DIR}/config.h.in ${H5BLOSC_BINARY_DIR}/blosc_config.h @ONLY)

#-----------------------------------------------------------------------------
# Option for external libraries on windows
#-----------------------------------------------------------------------------
option (HDF_PACKAGE_EXTLIBS "CPACK - include external libraries" OFF)
if (NOT HDF_EXTERNALLY_CONFIGURED)
  if (HDF_PACKAGE_EXTLIBS)
    set (HDF_NO_PACKAGES OFF CACHE BOOL "CPACK - Disable packaging" FORCE)
    if (HDF_ENABLE_BLOSC_ZLIB_SUPPORT AND ZLIB_FOUND)
      PACKAGE_BLOSC_ZLIB_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
    endif ()

    if (HDF_ENABLE_BLOSC_SUPPORT AND BLOSC_FOUND)
      PACKAGE_BLOSC_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
    endif ()
  endif ()
endif ()

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5blosc Unit Testing" OFF)
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
  if (HDF_ENABLE_BLOSC_ZLIB_SUPPORT)
    if (ZLIB_FOUND AND BLOSC_ZLIB_USE_EXTERNAL)
      add_dependencies (${H5BLOSC_LIB_TARGET} BLOSC_ZLIB)
    endif ()
  endif ()
  if (BLOSC_FOUND AND BLOSC_USE_EXTERNAL)
    add_dependencies (${H5BLOSC_LIB_TARGET} BLOSC)
  endif ()
endif ()

option (BUILD_EXAMPLES "Build h5blosc Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5BLOSC)
