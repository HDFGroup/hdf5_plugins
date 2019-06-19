cmake_minimum_required (VERSION 3.10)
project (H5PLExamples C)

#-----------------------------------------------------------------------------
# For standard build of HDF5  plugin examples.
#-----------------------------------------------------------------------------
include (${PROJECT_SOURCE_DIR}/config/cmake/HDFPluginMacros.cmake)
BASIC_SETTINGS (H5PLExamples)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
HDF5_SUPPORT (TRUE)

message (STATUS "HDF5 link libs: ${LINK_LIBS}")

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build H5PL Example Testing" OFF)
if (BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200
      CACHE STRING
      "Timeout in seconds for each test (default 1200=20minutes)"
  )
  enable_testing ()
  include (CTest)
  include (${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file (${H5PL_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

#-----------------------------------------------------------------------------
# Build examples
#-----------------------------------------------------------------------------
set (MAFISC_AVAILABLE 0)
if (WIN32 AND MSVC_VERSION GREATER 1600)
  # MAFISC filter
  set (MAFISC_AVAILABLE 1)
elseif (NOT WIN32)
  set (MAFISC_AVAILABLE 1)
endif ()

add_subdirectory (example)
