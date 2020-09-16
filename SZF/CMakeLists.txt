cmake_minimum_required (VERSION 3.12)
project (H5SZF C)

#-----------------------------------------------------------------------------
# Basic H5SZF stuff here
#-----------------------------------------------------------------------------
if (NOT H5PL_RESOURCES_DIR)
  include (${H5SZF_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
  include (${H5SZF_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

  SET_HDF_BUILD_TYPE()
endif ()
BASIC_SETTINGS (${SZF_PACKAGE_NAME})

set (H5SZF_VERS_MAJOR 1)
set (H5SZF_VERS_MINOR 0)
set (H5SZF_VERS_RELEASE 0)

set (H5SZF_SOVERS_MAJOR 1)
set (H5SZF_SOVERS_MINOR 0)
set (H5SZF_SOVERS_INTERFACE 1)
set (H5SZF_SOVERS_RELEASE 0)
math (EXPR H5SZF_SOVERS_MAJOR ${H5SZF_SOVERS_INTERFACE}-${H5SZF_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5SZF_PACKAGE_VERSION "${H5SZF_VERS_MAJOR}.${H5SZF_VERS_MINOR}")
set (H5SZF_PACKAGE_VERSION_STRING "${H5SZF_PACKAGE_VERSION}")
set (H5SZF_PACKAGE_VERSION_MAJOR "${H5SZF_VERS_MAJOR}")
set (H5SZF_PACKAGE_VERSION_MINOR "${H5SZF_VERS_MINOR}")
set (H5SZF_PACKAGE_STRING "${H5SZF_PACKAGE_NAME} ${H5SZF_PACKAGE_VERSION}")
set (H5SZF_PACKAGE_TARNAME "${H5SZF_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5SZF_PACKAGE_URL "http://www.hdfgroup.org")
set (H5SZF_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5SZF_PACKAGE_SOVERSION "${H5SZF_SOVERS_MAJOR}.${H5SZF_SOVERS_RELEASE}.${H5SZF_SOVERS_MINOR}")
message("Configuring for szf HDF5 Plugin version: " ${H5SZF_PACKAGE_STRING})

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5SZF_RESOURCES_DIR}/H5SZFMacros.cmake)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5SZF_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)
message (STATUS "H5SZ link libs: ${LINK_LIBS}")

#-----------------------------------------------------------------------------
# Generate the config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5SZF_RESOURCES_DIR}/config.h.in ${H5SZF_BINARY_DIR}/config.h @ONLY)

option (BUILD_SZF_LIBRARY_SOURCE "build the szf library within the plugin" ON)
if (NOT BUILD_SZF_LIBRARY_SOURCE)
  include (ExternalProject)
  set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
  set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    option (SZF_USE_EXTERNAL "Use External Library Building for SZF" 1)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (SZF_URL ${SZF_GIT_URL} CACHE STRING "Path to szf git repository")
      set (SZF_BRANCH ${SZF_GIT_BRANCH})
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5SZF_SOURCE_DIR})
      endif ()
      set (SZF_URL ${TGZPATH}/${SZF_TGZ_NAME})
    else ()
      set (SZF_USE_EXTERNAL 0)
    endif ()
  endif ()
  #-----------------------------------------------------------------------------
  # szf Library Settings
  #-----------------------------------------------------------------------------
  if (NOT H5_SZF_HEADER)
    if (NOT SZF_USE_EXTERNAL)
      find_package (SZ NAMES ${SZF_PACKAGE_NAME}${HDF_PACKAGE_EXT} COMPONENTS static)
      if (NOT SZF_FOUND)
        find_package (SZ) # Legacy find
        if (SZF_FOUND)
          set (LINK_LIBS ${LINK_LIBS} ${SZF_LIBRARIES})
        endif ()
      endif ()
    endif ()
    if (SZF_FOUND)
      set (H5_HAVE_SZF_H 1)
      set (H5_HAVE_SZ 1)
      set (H5_SZF_HEADER "sz.h")
      set (SZF_INCLUDE_DIR_GEN ${SZF_INCLUDE_DIR})
      set (SZF_INCLUDE_DIRS ${SZF_INCLUDE_DIRS} ${SZF_INCLUDE_DIR})
    else ()
      if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
        EXTERNAL_SZF_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT} STATIC)
        set (H5_HAVE_SZ_H 1)
        set (H5_HAVE_SZ 1)
        message (STATUS "Filter SZF is built")
      else ()
        message (FATAL_ERROR " SZF is Required for ${H5SZF_PACKAGE_NAME} library")
      endif ()
    endif ()
  else ()
    # This project is being called from within another and SZF is already configured
    set (H5_HAVE_SZ_H 1)
  endif ()
  message (STATUS "szf found: INC=${SZF_INCLUDE_DIR} TOOLS=${SZF_LIBRARIES}")
  set (LINK_LIBS ${LINK_LIBS} ${SZF_LIBRARIES})

  if (SZF_FOUND)
    PACKAGE_SZF_LIBRARY (${H5PL_ALLOW_EXTERNAL_SUPPORT})
  endif ()
endif ()
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5szf Unit Testing" OFF)
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
  if (SZF_FOUND AND SZF_USE_EXTERNAL)
    add_dependencies (${H5SZF_LIB_TARGET} SZ)
  endif ()
endif ()

option (BUILD_EXAMPLES "Build h5szf Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------
INSTALL_SUPPORT (H5SZF)
