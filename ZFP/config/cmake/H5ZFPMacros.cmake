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
macro (EXTERNAL_ZFP_LIBRARY compress_type)
  if (WIN32)
    set (ZFP_LIBRARY_PREFIX "lib")
  endif ()
  if (${compress_type} MATCHES "GIT")
    FetchContent_Declare (ZFP
        GIT_REPOSITORY ${ZFP_URL}
        GIT_TAG ${ZFP_BRANCH}
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (ZFP
        URL ${ZFP_URL}
        URL_HASH ""
    )
  endif ()
  FetchContent_GetProperties (ZFP)
  if (NOT zfp_POPULATED)
    FetchContent_Populate (ZFP)

    set (ZFP_WITH_OPENMP OFF CACHE BOOL "")
    set (ZFP_BIT_STREAM_WORD_SIZE 8 CACHE STRING "")
    set (ZFP_LIBRARY_PREFIX ${ZFP_LIBRARY_PREFIX} CACHE STRING "")

    # Store the old value of the 'BUILD_SHARED_LIBS'
    set (BUILD_SHARED_LIBS_OLD ${BUILD_SHARED_LIBS})
    # Make subproject to use 'BUILD_SHARED_LIBS=OFF' setting.
    set (BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build SHARED libraries")
    # Store the old value of the 'BUILD_TESTING'
    set (BUILD_TESTING_OLD ${BUILD_TESTING})
    # Make subproject to use 'BUILD_TESTING=OFF' setting.
    set (BUILD_TESTING OFF CACHE INTERNAL "Build Unit Testing")
    # Store the old value of the 'BUILD_UTILITIES'
    set (BUILD_UTILITIES_OLD ${BUILD_UTILITIES})
    # Make subproject to use 'BUILD_UTILITIES=OFF' setting.
    set (BUILD_UTILITIES OFF CACHE INTERNAL "Build Utilities")
    # Store the old value of the 'BUILD_EXAMPLES'
    set (BUILD_EXAMPLES_OLD ${BUILD_EXAMPLES})
    # Make subproject to use 'BUILD_EXAMPLES=OFF' setting.
    set (BUILD_EXAMPLES OFF CACHE INTERNAL "Build Examples")

    add_subdirectory (${zfp_SOURCE_DIR} ${zfp_BINARY_DIR})

    # Restore the old value of the parameter
    set (BUILD_EXAMPLES ${BUILD_EXAMPLES_OLD} CACHE BOOL "Build Examples" FORCE)
    # Restore the old value of the parameter
    set (BUILD_UTILITIES ${BUILD_UTILITIES_OLD} CACHE BOOL "Build Utilities" FORCE)
    # Restore the old value of the parameter
    set (BUILD_TESTING ${BUILD_TESTING_OLD} CACHE BOOL "Build Unit Testing" FORCE)
    # Restore the old value of the parameter
    set (BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "Type of libraries to build" FORCE)
  endif ()

#  include (${BINARY_DIR}/ZFP-targets.cmake)
  set (ZFP_LIBRARY "zfp")

  set (ZFP_INCLUDE_DIR_GEN "${zfp_BINARY_DIR}")
  set (ZFP_INCLUDE_DIR "${zfp_SOURCE_DIR}/include")
  set (ZFP_FOUND 1)
  set (ZFP_LIBRARIES ${ZFP_LIBRARY})
  set (ZFP_INCLUDE_DIRS ${ZFP_INCLUDE_DIR_GEN} ${ZFP_INCLUDE_DIR})
endmacro ()
