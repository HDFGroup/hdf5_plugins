cmake_minimum_required (VERSION 3.12)
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
# Bitshuffle filter
set (BSHUF_AVAILABLE 0)
if (NOT CMAKE_C_COMPILER_ID STREQUAL "Intel")
  set (BSHUF_AVAILABLE 1)
endif ()

# BLOSC filter
if (WIN32 AND MSVC_VERSION GREATER 1600)
  set (BLOSC_AVAILABLE 1)
elseif (NOT WIN32)
  set (BLOSC_AVAILABLE 1)
endif ()

# BZIP2 filter
set (BZIP2_AVAILABLE 1)

# FPZIP filter
#set (FPZIP_AVAILABLE 1)

# JPEG filter
set (JPEG_AVAILABLE 1)

# LZ4 filter
set (LZ4_AVAILABLE 1)

# LZF filter
set (LZF_AVAILABLE 1)

set (MAFISC_AVAILABLE 0)
#if (WIN32 AND MSVC_VERSION GREATER 1600)
#  # MAFISC filter
#  set (MAFISC_AVAILABLE 1)
#elseif (NOT WIN32)
#  set (MAFISC_AVAILABLE 1)
#endif ()

set (SZ_AVAILABLE 0)
#if (NOT WIN32)
#  # SZ filter
#  set (SZ_AVAILABLE 1)
#endif ()

# ZFP filter
set (ZFP_AVAILABLE 1)

add_subdirectory (example)
