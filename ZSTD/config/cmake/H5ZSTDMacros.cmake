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

include (FetchContent)
#-------------------------------------------------------------------------------
macro (EXTERNAL_ZSTD_LIBRARY compress_type)
  if (${compress_type} MATCHES "GIT")
    FetchContent_Declare (ZSTD
        GIT_REPOSITORY ${ZSTD_URL}
        GIT_TAG ${ZSTD_BRANCH}
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (ZSTD
        URL ${ZSTD_URL}
        URL_HASH ""
    )
  endif ()
  FetchContent_GetProperties (ZSTD)
  if (NOT zstd_POPULATED)
    FetchContent_Populate (ZSTD)

    set (ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "")
    set (ZSTD_BUILD_SHARED OFF CACHE BOOL "")
    set (ZSTD_LEGACY_SUPPORT OFF CACHE BOOL "")

    # Store the old value of the 'BUILD_SHARED_LIBS'
    set (BUILD_SHARED_LIBS_OLD ${BUILD_SHARED_LIBS})
    # Make subproject to use 'BUILD_SHARED_LIBS=OFF' setting.
    set (BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build SHARED libraries")
    # Store the old value of the 'BUILD_TESTING'
    set (BUILD_TESTING_OLD ${BUILD_TESTING})
    # Make subproject to use 'BUILD_TESTING=OFF' setting.
    set (BUILD_TESTING OFF CACHE INTERNAL "Build Unit Testing")

    add_subdirectory (${zstd_SOURCE_DIR}/build/cmake ${zstd_BINARY_DIR})

    # Restore the old value of the parameter
    set (BUILD_TESTING ${BUILD_TESTING_OLD} CACHE BOOL "Build Unit Testing" FORCE)
    # Restore the old value of the parameter
    set (BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "Type of libraries to build" FORCE)
  endif ()

#  include (${BINARY_DIR}/ZSTD-targets.cmake)
  set (ZSTD_LIBRARY "libzstd_static")

  set (ZSTD_INCLUDE_DIR_GEN "${zstd_BINARY_DIR}")
  set (ZSTD_INCLUDE_DIR "${zstd_SOURCE_DIR}/lib")
  set (ZSTD_FOUND 1)
  set (ZSTD_LIBRARIES ${ZSTD_LIBRARY})
  set (ZSTD_INCLUDE_DIRS ${ZSTD_INCLUDE_DIR_GEN} ${ZSTD_INCLUDE_DIR})
endmacro ()
