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
macro (EXTERNAL_SZ_LIBRARY compress_type)
  if (${compress_type} MATCHES "GIT")
    FetchContent_Declare (SZF
        GIT_REPOSITORY ${SZ_URL}
        GIT_TAG ${SZ_BRANCH}
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (SZF
        URL ${SZ_URL}
        URL_HASH ""
    )
  endif ()
  FetchContent_GetProperties (SZF)
  if (NOT szf_POPULATED)
    FetchContent_Populate (SZF)

    # Store the old value of the 'BUILD_SHARED_LIBS'
    set (BUILD_SHARED_LIBS_OLD ${BUILD_SHARED_LIBS})
    # Make subproject to use 'BUILD_SHARED_LIBS=OFF' setting.
    set (BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build SHARED libraries")
    # Store the old value of the 'BUILD_TESTING'
    set (BUILD_TESTING_OLD ${BUILD_TESTING})
    # Make subproject to use 'BUILD_TESTING=OFF' setting.
    set (BUILD_TESTING OFF CACHE INTERNAL "Build Unit Testing")

    add_subdirectory (${szf_SOURCE_DIR} ${szf_BINARY_DIR})

    # Restore the old value of the parameter
    set (BUILD_TESTING ${BUILD_TESTING_OLD} CACHE BOOL "Build Unit Testing" FORCE)
    # Restore the old value of the parameter
    set (BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "Type of libraries to build" FORCE)
  endif ()

#  include (${BINARY_DIR}/SZ-targets.cmake)
  set (SZ_LIBRARY "SZ;zstd;ZLIB")

  set (SZ_INCLUDE_DIR_GEN "${szf_BINARY_DIR}")
  set (SZ_INCLUDE_DIR "${szf_SOURCE_DIR}/sz/include")
  set (SZ_FOUND 1)
  set (SZ_LIBRARIES ${SZ_LIBRARY})
  set (SZ_INCLUDE_DIRS ${SZ_INCLUDE_DIR_GEN} ${SZ_INCLUDE_DIR})
endmacro ()
