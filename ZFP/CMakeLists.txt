cmake_minimum_required (VERSION 3.2.2)
project (H5ZFP C CXX)

#-----------------------------------------------------------------------------
# Basic H5ZFP stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5ZFP_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5ZFP_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)
endif ()
BASIC_SETTINGS (${ZFP_PACKAGE_NAME})

#-----------------------------------------------------------------------------
# Specific H5ZFP here
#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
set (H5ZFP_SRC_LIBZFP_DIR           ${H5ZFP_SOURCE_DIR}/src/libzfp)
#-----------------------------------------------------------------------------
# Set the core names of all the libraries
#-----------------------------------------------------------------------------
set (ZFP_LIB_CORENAME              "zfp")
#-----------------------------------------------------------------------------
# Set the true names of all the libraries if customized by external project
#-----------------------------------------------------------------------------
set (ZFP_LIB_NAME              "${H5ZFP_EXTERNAL_LIB_PREFIX}${ZFP_LIB_CORENAME}")
#-----------------------------------------------------------------------------
# Set the target names of all the libraries
#-----------------------------------------------------------------------------
set (ZFP_LIB_TARGET              ${ZFP_LIB_CORENAME})
#-----------------------------------------------------------------------------

set (H5ZFP_VERS_MAJOR 1)
set (H5ZFP_VERS_MINOR 0)
set (H5ZFP_VERS_RELEASE 0)

set (H5ZFP_SOVERS_MAJOR 1)
set (H5ZFP_SOVERS_MINOR 0)
set (H5ZFP_SOVERS_INTERFACE 1)
set (H5ZFP_SOVERS_RELEASE 0)
math (EXPR H5ZFP_SOVERS_MAJOR ${H5ZFP_SOVERS_INTERFACE}-${H5ZFP_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5ZFP_PACKAGE_VERSION "${H5ZFP_VERS_MAJOR}.${H5ZFP_VERS_MINOR}")
set (H5ZFP_PACKAGE_VERSION_STRING "${H5ZFP_PACKAGE_VERSION}")
set (H5ZFP_PACKAGE_VERSION_MAJOR "${H5ZFP_VERS_MAJOR}")
set (H5ZFP_PACKAGE_VERSION_MINOR "${H5ZFP_VERS_MINOR}")
set (H5ZFP_PACKAGE_STRING "${H5ZFP_PACKAGE_NAME} ${H5ZFP_PACKAGE_VERSION}")
set (H5ZFP_PACKAGE_TARNAME "${H5ZFP_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5ZFP_PACKAGE_URL "http://www.hdfgroup.org")
set (H5ZFP_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5ZFP_PACKAGE_SOVERSION "${H5ZFP_SOVERS_MAJOR}.${H5ZFP_SOVERS_RELEASE}.${H5ZFP_SOVERS_MINOR}")
message("Configuring for zfp HDF5 Plugin version: " ${H5ZFP_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5ZFP_RESOURCES_DIR}/H5ZFPMacros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5ZFP_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5ZFP link libs: ${LINK_LIBS}")

option (DISABLE_H5ZFP_ENCODER "build the zfp library without encoding" OFF)
if (NOT DISABLE_H5ZFP_ENCODER)
  set (FILTER_DECODE_ONLY 1)
endif ()

option (BUILD_ZFP_LIBRARY_SOURCE "build the zfp library within the plugin" OFF)
if (NOT BUILD_ZFP_LIBRARY_SOURCE)
  include (ExternalProject)
  option (H5PL_ALLOW_EXTERNAL_SUPPORT "Allow External Library Building (NO GIT TGZ)" "NO")
  set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
  set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    option (ZFP_USE_EXTERNAL "Use External Library Building for ZFP" 1)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (ZFP_URL ${ZFP_GIT_URL})
      set (ZFP_BRANCH ${ZFP_GIT_BRANCH})
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5ZFP_SOURCE_DIR})
      endif ()
      set (ZFP_URL ${TGZPATH}/${ZFP_TGZ_NAME})
    else ()
      set (ZFP_USE_EXTERNAL 0)
    endif ()
  endif ()
  #-----------------------------------------------------------------------------
  # zfp Library Settings
  #-----------------------------------------------------------------------------
  if (NOT ZFP_USE_EXTERNAL)
    find_package (ZFP NAMES ${ZFP_PACKAGE_NAME})
    if (NOT ZFP_FOUND)
      find_package (ZFP) # Legacy find
      if (NOT ZFP_FOUND)
        message (FATAL_ERROR " zfp is Required for ${H5ZFP_PACKAGE_NAME} library")
      endif ()
    endif ()
  endif ()
  if (ZFP_FOUND)
    set (H5_HAVE_ZFP_H 1)
    set (H5_HAVE_ZFP 1)
    set (H5_ZFP_HEADER "zfp.h")
    set (ZFP_INCLUDE_DIR_GEN ${ZFP_INCLUDE_DIR})
    set (ZFP_INCLUDE_DIRS ${ZFP_INCLUDE_DIR})
  else ()
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      EXTERNAL_ZFP_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT} STATIC)
      set (H5_HAVE_ZFP_H 1)
      set (H5_HAVE_ZFP 1)
      message (STATUS "Filter ZFP is built")
    else ()
      message (FATAL_ERROR " ZFP is Required for ${H5ZFP_PACKAGE_NAME} library")
    endif ()
  endif ()
  message (STATUS "zfp found: INC=${ZFP_INCLUDE_DIR} TOOLS=${ZFP_LIBRARIES}")
  set (LINK_LIBS ${LINK_LIBS} ${ZFP_LIBRARIES})

  if (ZFP_FOUND)
    PACKAGE_ZFP_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
  endif ()
endif ()

#-----------------------------------------------------------------------------
# Generate the config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5ZFP_RESOURCES_DIR}/config.h.in ${H5ZFP_BINARY_DIR}/config.h @ONLY)

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5zfp Unit Testing" OFF)
if (BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE INTEGER
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing ()
  include (CTest)
  include (${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file (${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

add_subdirectory (${H5ZFP_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)

if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  if (ZFP_FOUND AND ZFP_USE_EXTERNAL)
    add_dependencies (${H5ZFP_LIB_TARGET} ZFP)
  endif ()
endif ()

option (BUILD_EXAMPLES "Build h5zfp Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (${H5ZFP_SOURCE_DIR}/example ${PROJECT_BINARY_DIR}/example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5ZFP)
