cmake_minimum_required(VERSION 3.2.2)
PROJECT ( H5PL C )

#-----------------------------------------------------------------------------
# Basic H5PL stuff here
#-----------------------------------------------------------------------------
include (${H5PL_SOURCE_DIR}/config/cmake/HDFMacros.cmake)
include (${H5PL_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)

set (H5PL_EXPORTED_TARGETS "")

BASIC_SETTINGS (${PL_PACKAGE_NAME})

set (H5PL_VERS_MAJOR "1")
set (H5PL_VERS_MINOR "0")
set (H5PL_VERS_RELEASE "0")
set (H5PL_OVERRIDE_VERSION "1.10.2")

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

# BLOSC filter
option (ENABLE_BLOSC "Enable Library Building for BLOSC plugin" ON)
if (ENABLE_BLOSC)
  option (HDF_BLOSC_USE_EXTERNAL "Use External Library Building for blosc plugin" 0)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    set (HDF_BLOSC_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for blosc plugin" FORCE)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (HDF_BLOSC_URL ${HDF_BLOSC_GIT_URL})
      set (HDF_BLOSC_BRANCH ${HDF_BLOSC_GIT_BRANCH})
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5PL_SOURCE_DIR})
      endif ()
      set (HDF_BLOSC_URL ${TGZPATH}/${HDF_BLOSC_TGZ_NAME})
    endif ()
  endif ()
  add_subdirectory (${H5PL_SOURCE_DIR}/BLOSC ${PROJECT_BINARY_DIR}/BLOSC)
  set_global_variable (H5PL_LIBRARIES_TO_EXPORT "${H5PL_LIBRARIES_TO_EXPORT};${H5BLOSC_LIBRARIES_TO_EXPORT}")
endif ()

# BZIP2 filter
option (ENABLE_BZIP2 "Enable Library Building for BZIP2 plugin" ON)
if (ENABLE_BZIP2)
  option (HDF_BZIP2_USE_EXTERNAL "Use External Library Building for bzip2 plugin" 0)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    set (HDF_BZIP2_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for bzip2 plugin" FORCE)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (HDF_BZIP2_URL ${HDF_BZIP2_GIT_URL})
      set (HDF_BZIP2_BRANCH ${HDF_BZIP2_GIT_BRANCH})
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5PL_SOURCE_DIR})
      endif ()
      set (HDF_BZIP2_URL ${TGZPATH}/${HDF_BZIP2_TGZ_NAME})
    endif ()
  endif ()
  add_subdirectory (${H5PL_SOURCE_DIR}/BZIP2 ${PROJECT_BINARY_DIR}/BZIP2)
  set_global_variable (H5PL_LIBRARIES_TO_EXPORT "${H5PL_LIBRARIES_TO_EXPORT};${H5BZ2_LIBRARIES_TO_EXPORT}")
endif ()

# LZ4 filter
option (ENABLE_LZ4 "Enable Library Building for LZ4 plugin" ON)
if (ENABLE_LZ4)
  option (HDF_LZ4_USE_EXTERNAL "Use External Library Building for lz4 plugin" 0)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    set (HDF_LZ4_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for lz4 plugin" FORCE)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (HDF_LZ4_URL ${HDF_LZ4_GIT_URL})
      set (HDF_LZ4_BRANCH ${HDF_LZ4_GIT_BRANCH})
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5PL_SOURCE_DIR})
      endif ()
      set (HDF_LZ4_URL ${TGZPATH}/${HDF_LZ4_TGZ_NAME})
    endif ()
  endif ()
  add_subdirectory (${H5PL_SOURCE_DIR}/LZ4 ${PROJECT_BINARY_DIR}/LZ4)
  set_global_variable (H5PL_LIBRARIES_TO_EXPORT "${H5PL_LIBRARIES_TO_EXPORT};${H5LZ4_LIBRARIES_TO_EXPORT}")
endif ()

# LZF filter
option (ENABLE_LZF "Enable Library Building for LZF plugin" ON)
if (ENABLE_LZF)
  option (HDF_LZF_USE_EXTERNAL "Use External Library Building for lzf plugin" 0)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    set (HDF_LZF_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for lzf plugin" FORCE)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (HDF_LZF_URL ${HDF_LZF_GIT_URL})
      set (HDF_LZF_BRANCH ${HDF_LZF_GIT_BRANCH})
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5PL_SOURCE_DIR})
      endif ()
      set (HDF_LZF_URL ${TGZPATH}/${HDF_LZF_TGZ_NAME})
    endif ()
  endif ()
  add_subdirectory (${H5PL_SOURCE_DIR}/LZF ${PROJECT_BINARY_DIR}/LZF)
  set_global_variable (H5PL_LIBRARIES_TO_EXPORT "${H5PL_LIBRARIES_TO_EXPORT};${H5LZF_LIBRARIES_TO_EXPORT}")
endif ()

# MAFISC filter
option (ENABLE_MAFISC "Enable Library Building for MAFISC plugin" ON)
if (ENABLE_MAFISC)
  option (HDF_MAFISC_USE_EXTERNAL "Use External Library Building for mafisc plugin" 0)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    set (HDF_MAFISC_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for mafisc plugin" FORCE)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (HDF_MAFISC_URL ${HDF_MAFISC_GIT_URL})
      set (HDF_MAFISC_BRANCH ${HDF_MAFISC_GIT_BRANCH})
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5PL_SOURCE_DIR})
      endif ()
      set (HDF_MAFISC_URL ${TGZPATH}/${HDF_MAFISC_TGZ_NAME})
    endif ()
  endif ()
  add_subdirectory (${H5PL_SOURCE_DIR}/MAFISC ${PROJECT_BINARY_DIR}/MAFISC)
  set_global_variable (H5PL_LIBRARIES_TO_EXPORT "${H5PL_LIBRARIES_TO_EXPORT};${H5MAFISC_LIBRARIES_TO_EXPORT}")
endif ()

# ZFP filter
option (ENABLE_ZFP "Enable Library Building for ZFP plugin" ON)
if (ENABLE_ZFP)
  option (HDF_ZFP_USE_EXTERNAL "Use External Library Building for zfp plugin" 0)
  if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    set (HDF_ZFP_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for zfp plugin" FORCE)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (HDF_ZFP_URL ${HDF_ZFP_GIT_URL})
      set (HDF_ZFP_BRANCH ${HDF_ZFP_GIT_BRANCH})
    elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5PL_SOURCE_DIR})
      endif ()
      set (HDF_ZFP_URL ${TGZPATH}/${HDF_ZFP_TGZ_NAME})
    endif ()
  endif ()
  add_subdirectory (${H5PL_SOURCE_DIR}/ZFP ${PROJECT_BINARY_DIR}/ZFP)
  set_global_variable (H5PL_LIBRARIES_TO_EXPORT "${H5PL_LIBRARIES_TO_EXPORT};${H5ZFP_LIBRARIES_TO_EXPORT}")
endif ()

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (H5PL_BUILD_TESTING "Build h5pl Unit Testing" OFF)
if (H5PL_BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE INTEGER
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

