cmake_minimum_required(VERSION 3.12)
project (H5PL C)

#-----------------------------------------------------------------------------
# Basic H5PL stuff here
#-----------------------------------------------------------------------------
include (${H5PL_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
include (${H5PL_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

SET_HDF_BUILD_TYPE()

set (H5PL_EXPORTED_TARGETS "")

BASIC_SETTINGS (${PL_PACKAGE_NAME})

set (H5PL_VERS_MAJOR "1")
set (H5PL_VERS_MINOR "12")
set (H5PL_VERS_RELEASE "1")
set (H5PL_OVERRIDE_VERSION "1.12.1")

set (H5PL_SOVERS_MAJOR 1)
set (H5PL_SOVERS_MINOR 0)
set (H5PL_SOVERS_INTERFACE 1)
set (H5PL_SOVERS_RELEASE 0)
math (EXPR H5PL_SOVERS_MAJOR ${H5PL_SOVERS_INTERFACE}-${H5PL_SOVERS_RELEASE})

set (H5PL_PACKAGE_VERSION "${H5PL_VERS_MAJOR}.${H5PL_VERS_MINOR}.${H5PL_VERS_RELEASE}")
set (H5PL_PACKAGE_VERSION_STRING "${H5PL_PACKAGE_VERSION}")
set (H5PL_PACKAGE_VERSION_MAJOR "${H5PL_VERS_MAJOR}")
set (H5PL_PACKAGE_VERSION_MINOR "${H5PL_VERS_MINOR}")
set (H5PL_PACKAGE_STRING "${H5PL_PACKAGE_NAME}")
set (H5PL_PACKAGE_TARNAME "${H5PL_PACKAGE_NAME}${HDF_PACKAGE_EXT}")
set (H5PL_PACKAGE_URL "https://support.hdfgroup.org/hdf5/")
set (H5PL_PACKAGE_BUGREPORT "help@hdfgroup.org")

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5PL_RESOURCES_DIR}/H5PLMacros.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "HDF5 link libs: ${LINK_LIBS}")

#-----------------------------------------------------------------------------
# External plugin targets for the H5PL super project.
#-----------------------------------------------------------------------------
include (ExternalProject)
set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)

# Bitshuffle filter
if (NOT CMAKE_C_COMPILER_ID STREQUAL "Intel")
  FILTER_OPTION (BSHUF)
endif ()

# BLOSC filter
if (WIN32)
  if (NOT CMAKE_C_COMPILER_ID MATCHES "[Cc]lang" AND MSVC_VERSION GREATER 1600)
    FILTER_OPTION (BLOSC)
  endif ()
elseif (APPLE)
  if (NOT CMAKE_C_COMPILER_ID STREQUAL "Intel")
    FILTER_OPTION (BLOSC)
  endif ()
else ()
  FILTER_OPTION (BLOSC)
endif ()

# BZIP2 filter
FILTER_OPTION (BZIP2)

# FPZIP filter
#FILTER_OPTION (FPZIP)

# JPEG filter
if (NOT WIN32)
  FILTER_OPTION (JPEG)
elseif (NOT CMAKE_C_COMPILER_ID MATCHES "[Cc]lang")
  FILTER_OPTION (JPEG)
endif ()

# LZ4 filter
FILTER_OPTION (LZ4)

# LZF filter
FILTER_OPTION (LZF)

#if (WIN32 AND MSVC_VERSION GREATER 1600)
#  # MAFISC filter
#  FILTER_OPTION (MAFISC)
#elseif (NOT WIN32)
#  FILTER_OPTION (MAFISC)
#endif ()

## SZF filter
#if (NOT WIN32)
#  FILTER_OPTION (SZF)
#endif ()

# ZFP filter
FILTER_OPTION (ZFP)

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (H5PL_BUILD_TESTING "Build h5pl Unit Testing" OFF)
if (H5PL_BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE STRING
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing ()
  include (CTest)
  include (${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file (${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
option (H5PL_CPACK_ENABLE "Enable the CPACK include and components" ON)
INSTALL_SUPPORT (H5PL)

