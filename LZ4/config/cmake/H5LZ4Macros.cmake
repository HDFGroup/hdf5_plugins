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
macro (EXTERNAL_LZ4_LIBRARY compress_type)
  if (${compress_type} MATCHES "GIT")
    FetchContent_Declare (LZ4
        GIT_REPOSITORY ${LZ4_URL}
        GIT_TAG ${LZ4_BRANCH}
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (LZ4
        URL ${LZ4_URL}
        URL_HASH ""
    )
  endif ()

  # Store the old value of the 'BUILD_SHARED_LIBS'
  set (BUILD_SHARED_LIBS_OLD ${BUILD_SHARED_LIBS})
  # Make subproject to use 'BUILD_SHARED_LIBS=OFF' setting.
  set (BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build SHARED libraries" FORCE)
  # Store the old value of the 'BUILD_TESTING'
  set (BUILD_TESTING_OLD ${BUILD_TESTING})
  # Make subproject to use 'BUILD_TESTING=OFF' setting.
  set (BUILD_TESTING OFF CACHE INTERNAL "Build Unit Testing" FORCE)

  FetchContent_MakeAvailable(LZ4)

  # Restore the old value of the parameter
  set (BUILD_TESTING ${BUILD_TESTING_OLD} CACHE BOOL "Build Unit Testing" FORCE)
  # Restore the old value of the parameter
  set (BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "Type of libraries to build" FORCE)

#  include (${BINARY_DIR}/LZ4-targets.cmake)
  set (LZ4_LIBRARY "lz4-static")

  set (LZ4_INCLUDE_DIR_GEN "${lz4_BINARY_DIR}")
  set (LZ4_INCLUDE_DIR "${lz4_SOURCE_DIR}/src")
  set (LZ4_FOUND 1)
  set (LZ4_LIBRARIES ${JPEG_LIBRARY})
  set (LZ4_INCLUDE_DIRS ${LZ4_INCLUDE_DIR_GEN} ${LZ4_INCLUDE_DIR})
endmacro ()
