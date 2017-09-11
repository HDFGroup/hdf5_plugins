cmake_minimum_required (VERSION 3.2.2)
project (H5LZ4 C)

#-----------------------------------------------------------------------------
# Basic H5LZ4 stuff here
#-----------------------------------------------------------------------------
include (${H5LZ4_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
include (${H5LZ4_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)
BASIC_SETTINGS (H5LZ4)

set (H5LZ4_VERS_MAJOR 1)
set (H5LZ4_VERS_MINOR 0)
set (H5LZ4_VERS_RELEASE 0)

set (H5LZ4_SOVERS_MAJOR 1)
set (H5LZ4_SOVERS_MINOR 0)
set (H5LZ4_SOVERS_INTERFACE 1)
set (H5LZ4_SOVERS_RELEASE 0)
math (EXPR H5LZ4_SOVERS_MAJOR ${H5LZ4_SOVERS_INTERFACE}-${H5LZ4_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5LZ4_PACKAGE "h5lz4")
set (H5LZ4_PACKAGE_NAME "h5lz4")
set (H5LZ4_PACKAGE_VERSION "${H5LZ4_VERS_MAJOR}.${H5LZ4_VERS_MINOR}")
set (H5LZ4_PACKAGE_VERSION_STRING "${H5LZ4_PACKAGE_VERSION}")
set (H5LZ4_PACKAGE_VERSION_MAJOR "${H5LZ4_VERS_MAJOR}")
set (H5LZ4_PACKAGE_VERSION_MINOR "${H5LZ4_VERS_MINOR}")
set (H5LZ4_PACKAGE_STRING "${H5LZ4_PACKAGE_NAME} ${H5LZ4_PACKAGE_VERSION}")
set (H5LZ4_PACKAGE_TARNAME "${H5LZ4_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5LZ4_PACKAGE_URL "http://www.hdfgroup.org")
set (H5LZ4_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5LZ4_PACKAGE_SOVERSION "${H5LZ4_SOVERS_MAJOR}.${H5LZ4_SOVERS_RELEASE}.${H5LZ4_SOVERS_MINOR}")

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5LZ4_RESOURCES_DIR}/H5LZ4Macros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5LZ4_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "HDF5 link libs: ${HDF5_LINK_LIBS}")

option (DISABLE_H5LZ4_ENCODER "build the lz4 library without encoding" OFF)
if (NOT DISABLE_H5LZ4_ENCODER)
  set (FILTER_DECODE_ONLY 1)
endif ()

INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIR})

option (BUILD_LZ4_LIBRARY_SOURCE "build the lz4 library within the plugin" OFF)
if (NOT BUILD_LZ4_LIBRARY_SOURCE)
  include (ExternalProject)
  set (H5LZ4_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
  set_property (CACHE H5LZ4_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
  if (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    option (LZ4_USE_EXTERNAL "Use External Library Building for LZ4" 1)
    if (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (LZ4_URL ${LZ4_GIT_URL})
    elseif (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
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
        message (FATAL_ERROR " lz4 is Required for ${H5LZ4_PACKAGE_NAME} library")
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
    if (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      EXTERNAL_LZ4_LIBRARY (${H5LZ4_ALLOW_EXTERNAL_SUPPORT} STATIC)
      set (H5LZ4_HAVE_LZ4_H 1)
      set (H5LZ4_HAVE_LZ4 1)
      message (STATUS "Filter LZ4 is built")
    else ()
      message (FATAL_ERROR " LZ4 is Required for ${H5LZ4_PACKAGE_NAME} library")
    endif ()
  endif ()
  message (STATUS "lz4 found: INC=${LZ4_INCLUDE_DIR} TOOLS=${LZ4_LIBRARIES}")
  set (LINK_LIBS ${LINK_LIBS} ${LZ4_LIBRARIES})
  INCLUDE_DIRECTORIES (${H5LZ4_INCLUDE_DIRS})

  if (LZ4_FOUND)
    PACKAGE_LZ4_LIBRARY (${H5LZ4_ALLOW_EXTERNAL_SUPPORT})
  endif ()
endif ()
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5lz4 Unit Testing" OFF)
if (BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE INTEGER
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing ()
  include (CTest)
  include (${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file (${HDF_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

add_subdirectory (${H5LZ4_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)

if (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  if (LZ4_FOUND AND LZ4_USE_EXTERNAL)
    add_dependencies (${H5LZ4_LIB_TARGET} LZ4)
  endif ()
endif ()

option (BUILD_EXAMPLES "Build h5lz4 Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (${H5LZ4_SOURCE_DIR}/example ${PROJECT_BINARY_DIR}/example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5LZ4)
