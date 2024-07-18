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
macro (EXTERNAL_JPEG_LIBRARY compress_type)
  if (${compress_type} MATCHES "GIT")
    FetchContent_Declare (JPEG
        GIT_REPOSITORY ${JPEG_URL}
        GIT_TAG ${JPEG_BRANCH}
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy
            ${H5JPEG_SOURCE_DIR}/config/CMakeLists.txt
            ${H5JPEG_SOURCE_DIR}/config/jconfig.h.in
            <SOURCE_DIR>
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (JPEG
        URL ${JPEG_URL}
        URL_HASH ""
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy
            ${H5JPEG_SOURCE_DIR}/config/CMakeLists.txt
            ${H5JPEG_SOURCE_DIR}/config/jconfig.h.in
            <SOURCE_DIR>
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

  FetchContent_MakeAvailable(JPEG)

  # Restore the old value of the parameter
  set (H5PL_BUILD_TESTING ${H5PL_BUILD_TESTING_OLD} CACHE BOOL "Build Unit Testing" FORCE)
  # Restore the old value of the parameter
  set (BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "Type of libraries to build" FORCE)

#  include (${BINARY_DIR}/JPEG-targets.cmake)
  set (JPEG_LIBRARY "jpeg-static")

  set (JPEG_INCLUDE_DIR_GEN "${jpeg_BINARY_DIR}")
  set (JPEG_INCLUDE_DIR "${jpeg_SOURCE_DIR}")
  set (JPEG_FOUND 1)
  set (JPEG_LIBRARIES ${JPEG_LIBRARY})
  set (JPEG_INCLUDE_DIRS ${JPEG_INCLUDE_DIR_GEN} ${JPEG_INCLUDE_DIR})
endmacro ()
