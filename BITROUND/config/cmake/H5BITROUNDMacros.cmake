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
macro (EXTERNAL_BITROUND_LIBRARY compress_type)
  if (${compress_type} MATCHES "GIT")
    FetchContent_Declare (BITROUND
        GIT_REPOSITORY ${BITROUND_URL}
        GIT_TAG ${BITROUND_BRANCH}
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (BITROUND
        URL ${BITROUND_URL}
        URL_HASH ""
    )
  endif ()

  # Store the old value of the 'BUILD_SHARED_LIBS'
  set (BUILD_SHARED_LIBS_OLD ${BUILD_SHARED_LIBS})
  # Make subproject to use 'BUILD_SHARED_LIBS=OFF' setting.
  set (BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build SHARED libraries" FORCE)
  # Store the old value of the 'H5PL_BUILD_TESTING'
  set (H5PL_BUILD_TESTING_OLD ${H5PL_BUILD_TESTING})
  # Make subproject to use 'H5PL_BUILD_TESTING=OFF' setting.
  set (H5PL_BUILD_TESTING OFF CACHE INTERNAL "Build Unit Testing" FORCE)

  FetchContent_MakeAvailable(BITROUND)

  # Restore the old value of the parameter
  set (H5PL_BUILD_TESTING ${H5PL_BUILD_TESTING_OLD} CACHE BOOL "Build Unit Testing" FORCE)
  # Restore the old value of the parameter
  set (BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "Type of libraries to build" FORCE)

#  include (${BINARY_DIR}/BITROUND-targets.cmake)
  set (BITROUND_LIBRARY "bitround")

  set (BITROUND_INCLUDE_DIR_GEN "${bitround_BINARY_DIR}")
  set (BITROUND_INCLUDE_DIR "${bitround_SOURCE_DIR}")
  set (BITROUND_FOUND 1)
  set (BITROUND_LIBRARIES ${BITROUND_LIBRARY})
  set (BITROUND_INCLUDE_DIRS ${BITROUND_INCLUDE_DIR_GEN} ${BITROUND_INCLUDE_DIR})
endmacro ()

#-------------------------------------------------------------------------------
macro (PACKAGE_BITROUND_LIBRARY compress_type)
  if (${compress_type} MATCHES "GIT" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (BITROUND-GenHeader-Copy BITROUND)
  endif ()
endmacro ()
