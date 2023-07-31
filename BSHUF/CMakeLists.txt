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
project (H5BSHUF C)

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
# Basic H5BSHUF stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5BSHUF_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5BSHUF_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (${BSHUF_PACKAGE_NAME})

set (H5BSHUF_VERS_MAJOR 1)
set (H5BSHUF_VERS_MINOR 0)
set (H5BSHUF_VERS_RELEASE 0)

set (H5BSHUF_SOVERS_MAJOR 1)
set (H5BSHUF_SOVERS_MINOR 0)
set (H5BSHUF_SOVERS_INTERFACE 1)
set (H5BSHUF_SOVERS_RELEASE 0)
math (EXPR H5BSHUF_SOVERS_MAJOR ${H5BSHUF_SOVERS_INTERFACE}-${H5BSHUF_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5BSHUF_PACKAGE_VERSION "${H5BSHUF_VERS_MAJOR}.${H5BSHUF_VERS_MINOR}")
set (H5BSHUF_PACKAGE_VERSION_STRING "${H5BSHUF_PACKAGE_VERSION}")
set (H5BSHUF_PACKAGE_VERSION_MAJOR "${H5BSHUF_VERS_MAJOR}")
set (H5BSHUF_PACKAGE_VERSION_MINOR "${H5BSHUF_VERS_MINOR}")
set (H5BSHUF_PACKAGE_STRING "${H5BSHUF_PACKAGE_NAME} ${H5BSHUF_PACKAGE_VERSION}")
set (H5BSHUF_PACKAGE_TARNAME "${H5BSHUF_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5BSHUF_PACKAGE_URL "http://www.hdfgroup.org")
set (H5BSHUF_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5BSHUF_PACKAGE_SOVERSION "${H5BSHUF_SOVERS_MAJOR}.${H5BSHUF_SOVERS_RELEASE}.${H5BSHUF_SOVERS_MINOR}")
set (H5BSHUF_PACKAGE_SOVERSION_MAJOR "${H5BSHUF_SOVERS_MAJOR}")
message("Configuring for bshuf HDF5 Plugin version: " ${H5BSHUF_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5BSHUF_RESOURCES_DIR}/H5BSHUFMacros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5BSHUF_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5BSHUF link libs: ${LINK_LIBS}")

#-----------------------------------------------------------------------------
# Generate the bshuf_config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5BSHUF_RESOURCES_DIR}/config.h.in ${H5BSHUF_BINARY_DIR}/bshuf_config.h @ONLY)

include (ExternalProject)
option (H5PL_ALLOW_EXTERNAL_SUPPORT "Allow External Library Building (NO GIT TGZ)" "NO")
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)

option (BUILD_LZ4_LIBRARY_SOURCE "build the lz4 library within the plugin" ON)
if (NOT BUILD_LZ4_LIBRARY_SOURCE)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    option (LZ4_USE_EXTERNAL "Use External Library Building for LZ4" 1)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (LZ4_URL ${LZ4_GIT_URL} CACHE STRING "Path to lz4 git repository")
      set (LZ4_BRANCH ${LZ4_GIT_BRANCH})
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5LZ4_SOURCE_DIR})
      endif ()
      set (LZ4_URL ${TGZPATH}/${LZ4_TGZ_NAME})
    else ()
      set (LZ4_USE_EXTERNAL 0)
    endif ()
  endif ()
  #-----------------------------------------------------------------------------
  # lz4 Library Settings
  #-----------------------------------------------------------------------------
  if (NOT LZ4_USE_EXTERNAL)
    find_package (LZ4 NAMES ${LZ4_PACKAGE_NAME})
    if (NOT LZ4_FOUND)
      find_package (LZ4) # Legacy find
      if (NOT LZ4_FOUND)
        message (FATAL_ERROR " lz4 is Required for ${H5BSHUF_PACKAGE_NAME} library")
      endif ()
    endif ()
  endif ()
  if (LZ4_FOUND)
    set (H5LZ4_HAVE_LZ4_H 1)
    set (H5LZ4_HAVE_LZ4 1)
    set (H5LZ4_LZ4_HEADER "lz4.h")
    set (H5LZ4_INCLUDE_DIR_GEN ${LZ4_INCLUDE_DIR})
    set (H5LZ4_INCLUDE_DIRS ${LZ4_INCLUDE_DIR})
  else ()
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      EXTERNAL_LZ4_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT} STATIC)
      set (H5LZ4_HAVE_LZ4_H 1)
      set (H5LZ4_HAVE_LZ4 1)
      message (STATUS "Filter LZ4 is built")
    else ()
      message (FATAL_ERROR " LZ4 is Required for ${H5BSHUF_PACKAGE_NAME} library")
    endif ()
  endif ()
  message (STATUS "lz4 found: INC=${LZ4_INCLUDE_DIR} TOOLS=${LZ4_LIBRARIES}")
  set (LINK_LIBS ${LINK_LIBS} ${LZ4_LIBRARIES})

  if (LZ4_FOUND)
    PACKAGE_LZ4_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
  endif ()
endif ()
#-----------------------------------------------------------------------------
option (BUILD_BSHUF_LIBRARY_SOURCE "build the bitshuffle library within the plugin" ON)
if (NOT BUILD_BSHUF_LIBRARY_SOURCE)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    option (BSHUF_USE_EXTERNAL "Use External Library Building for BSHUF" 1)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (BSHUF_URL ${BSHUF_GIT_URL} CACHE STRING "Path to bshuf git repository")
      set (BSHUF_BRANCH ${BSHUF_GIT_BRANCH})
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5BSHUF_SOURCE_DIR})
      endif ()
      set (BSHUF_URL ${TGZPATH}/${BSHUF_TGZ_NAME})
    else ()
      set (BSHUF_USE_EXTERNAL 0)
    endif ()
  endif ()
  #-----------------------------------------------------------------------------
  # bshuf Library Settings
  #-----------------------------------------------------------------------------
  if (NOT H5_BSHUF_HEADER)
    if (NOT BSHUF_USE_EXTERNAL)
      find_package (BSHUF NAMES ${BSHUF_PACKAGE_NAME}${HDF_PACKAGE_EXT} COMPONENTS static)
      if (NOT BSHUF_FOUND)
        find_package (BSHUF) # Legacy find
        if (BSHUF_FOUND)
          set (LINK_LIBS ${LINK_LIBS} ${BSHUF_LIBRARIES})
        endif ()
      endif ()
    endif ()
    if (BSHUF_FOUND)
      set (H5_HAVE_BSHUF_H 1)
      set (H5_HAVE_BSHUF 1)
      set (H5_BSHUF_HEADER "bshuf.h")
      set (BSHUF_INCLUDE_DIR_GEN ${BSHUF_INCLUDE_DIR})
      set (BSHUF_INCLUDE_DIRS ${BSHUF_INCLUDE_DIRS} ${BSHUF_INCLUDE_DIR})
    else ()
      if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
        EXTERNAL_BSHUF_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT} STATIC)
        set (H5_HAVE_BSHUF_H 1)
        set (H5_HAVE_BSHUF 1)
        message (STATUS "Filter BSHUF is built")
      else ()
        message (FATAL_ERROR " BSHUF is Required for ${H5BSHUF_PACKAGE_NAME} library")
      endif ()
    endif ()
  else ()
    # This project is being called from within another and BSHUF is already configured
    set (H5_HAVE_BSHUF_H 1)
  endif ()
  message (STATUS "bshuf found: INC=${BSHUF_INCLUDE_DIR} TOOLS=${BSHUF_LIBRARIES}")
  set (LINK_LIBS ${LINK_LIBS} ${BSHUF_LIBRARIES})

  if (BSHUF_FOUND)
    PACKAGE_BSHUF_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
  endif ()
endif ()
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5bshuf Unit Testing" OFF)
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
  if (BSHUF_FOUND AND BSHUF_USE_EXTERNAL)
    add_dependencies (${H5BSHUF_LIB_TARGET} BSHUF)
  endif ()
  if (LZ4_FOUND AND LZ4_USE_EXTERNAL)
    add_dependencies (${H5BSHUF_LIB_TARGET} LZ4)
  endif ()
endif ()

option (BUILD_EXAMPLES "Build h5bshuf Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5BSHUF)
