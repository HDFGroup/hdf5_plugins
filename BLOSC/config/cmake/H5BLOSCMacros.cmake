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
macro (EXTERNAL_BLOSC_LIBRARY compress_type)
  if (${compress_type} MATCHES "GIT")
    FetchContent_Declare (BLOSC
        GIT_REPOSITORY ${BLOSC_URL}
        GIT_TAG ${BLOSC_BRANCH}
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (BLOSC
        URL ${BLOSC_URL}
        URL_HASH ""
    )
  endif ()

  set (BUILD_SHARED OFF CACHE BOOL "" FORCE)
  set (BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set (BUILD_FUZZERS OFF CACHE BOOL "" FORCE)
  set (BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
  set (PREFER_EXTERNAL_SNAPPY OFF CACHE BOOL "" FORCE)
  set (PREFER_EXTERNAL_ZLIB OFF CACHE BOOL "" FORCE)
  set (PREFER_EXTERNAL_ZSTD OFF CACHE BOOL "" FORCE)
  set (DEACTIVATE_SNAPPY ON CACHE BOOL "" FORCE)
  set (BLOSC_IS_SUBPROJECT ON CACHE BOOL "" FORCE)
  set (BLOSC_INSTALL OFF CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(BLOSC)

##include (${BINARY_DIR}/${BLOSC_PACKAGE_NAME}${H5BLOSC_PACKAGE_EXT}-targets.cmake)
  set (BLOSC_STATIC_LIBRARY "blosc_static")
  set (BLOSC_LIBRARIES ${BLOSC_STATIC_LIBRARY})

  set (BLOSC_INCLUDE_DIR_GEN "${blosc_BINARY_DIR}")
  set (BLOSC_INCLUDE_DIR "${blosc_SOURCE_DIR}/blosc")
  set (BLOSC_FOUND 1)
  set (BLOSC_INCLUDE_DIRS ${BLOSC_INCLUDE_DIR_GEN} ${BLOSC_INCLUDE_DIR})
endmacro ()

#-------------------------------------------------------------------------------
macro (PACKAGE_BLOSC_LIBRARY compress_type)
  add_custom_target (BLOSC-GenHeader-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BLOSC_INCLUDE_DIR}/blosc.h ${BLOSC_INCLUDE_DIR}/blosc-export.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
      COMMENT "Copying ${BLOSC_INCLUDE_DIR_}/blosc.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
  )
  set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/blosc.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/blosc-export.h)
  if (${compress_type} MATCHES "GIT" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (BLOSC-GenHeader-Copy BLOSC)
  endif ()
endmacro ()

#-------------------------------------------------------------------------------
macro (EXTERNAL_BLOSC_ZLIB_LIBRARY compress_type)
  if (${compress_type} MATCHES "GIT")
    FetchContent_Declare (BLOSC_ZLIB
        GIT_REPOSITORY ${BLOSC_ZLIB_URL}
        GIT_TAG ${BLOSC_ZLIB_BRANCH}
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy
            ${H5BLOSC_SOURCE_DIR}/config/zlibdev-CMakeLists.txt
            <SOURCE_DIR>/CMakeLists.txt
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (BLOSC_ZLIB
        URL ${BLOSC_ZLIB_URL}
        URL_HASH ""
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy
            ${H5BLOSC_SOURCE_DIR}/config/zlib-CMakeLists.txt
            <SOURCE_DIR>/CMakeLists.txt
    )
  endif ()
  FetchContent_MakeAvailable(BLOSC_ZLIB)

  set (BLOSC_ZLIB_STATIC_LIBRARY "zlibstat")
  set (BLOSC_ZLIB_LIBRARY ${BLOSC_ZLIB_STATIC_LIBRARY})
  set (BLOSC_ZLIB_LIBRARIES ${BLOSC_ZLIB_LIBRARY})

  set (BLOSC_ZLIB_INCLUDE_DIR_GEN "${blosc_zlib_BINARY_DIR}")
  set (BLOSC_ZLIB_INCLUDE_DIR "${blosc_zlib_SOURCE_DIR}")
  set (BLOSC_ZLIB_FOUND 1)
  set (BLOSC_ZLIB_INCLUDE_DIRS ${BLOSC_ZLIB_INCLUDE_DIR_GEN} ${BLOSC_ZLIB_INCLUDE_DIR})
  message (VERBOSE "BLOSC_ZLIB Includes: ${BLOSC_ZLIB_INCLUDE_DIRS}")
endmacro ()
