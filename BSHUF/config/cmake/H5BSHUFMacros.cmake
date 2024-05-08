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
macro (EXTERNAL_BSHUF_LIBRARY compress_type)
  if (${compress_type} MATCHES "GIT")
    FetchContent_Declare (BSHUF
        GIT_REPOSITORY ${BSHUF_URL}
        GIT_TAG ${BSHUF_BRANCH}
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (BSHUF
        URL ${BSHUF_URL}
        URL_HASH ""
    )
  endif ()
  FetchContent_GetProperties(BSHUF)
  if(NOT bshuf_POPULATED)
    FetchContent_Populate(BSHUF)

    # Store the old value of the 'BUILD_SHARED_LIBS'
    set (BUILD_SHARED_LIBS_OLD ${BUILD_SHARED_LIBS})
    # Make subproject to use 'BUILD_SHARED_LIBS=OFF' setting.
    set (BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build SHARED libraries" FORCE)
    # Store the old value of the 'H5PL_BUILD_TESTING'
    set (H5PL_BUILD_TESTING_OLD ${H5PL_BUILD_TESTING})
    # Make subproject to use 'H5PL_BUILD_TESTING=OFF' setting.
    set (H5PL_BUILD_TESTING OFF CACHE INTERNAL "Build Unit Testing" FORCE)

    add_subdirectory(${bshuf_SOURCE_DIR} ${bshuf_BINARY_DIR})

    # Restore the old value of the parameter
    set (H5PL_BUILD_TESTING ${H5PL_BUILD_TESTING_OLD} CACHE BOOL "Build Unit Testing" FORCE)
    # Restore the old value of the parameter
    set (BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "Type of libraries to build" FORCE)
  endif()

#  include (${BINARY_DIR}/BSHUF-targets.cmake)
  set (BSHUF_LIBRARY "bshuf")

  set (BSHUF_INCLUDE_DIR_GEN "${bshuf_BINARY_DIR}")
  set (BSHUF_INCLUDE_DIR "${bshuf_SOURCE_DIR}")
  set (BSHUF_FOUND 1)
  set (BSHUF_LIBRARIES ${BSHUF_LIBRARY})
  set (BSHUF_INCLUDE_DIRS ${BSHUF_INCLUDE_DIR_GEN} ${BSHUF_INCLUDE_DIR})
endmacro ()

#-------------------------------------------------------------------------------
macro (PACKAGE_BSHUF_LIBRARY compress_type)
  add_custom_target (BSHUF-GenHeader-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BSHUF_INCLUDE_DIR}/bshuf.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
      COMMENT "Copying ${BSHUF_INCLUDE_DIR}/bshuf.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
  )
  set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bshuf.h)
  if (${compress_type} MATCHES "GIT" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (BSHUF-GenHeader-Copy BSHUF)
  endif ()
endmacro ()

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
  FetchContent_GetProperties(LZ4)
  if(NOT lz4_POPULATED)
    FetchContent_Populate(LZ4)

    add_subdirectory(${lz4_SOURCE_DIR} ${lz4_BINARY_DIR})
  endif()

#  include (${BINARY_DIR}/LZ4-targets.cmake)
  set (LZ4_LIBRARY "lz4")

  set (LZ4_INCLUDE_DIR_GEN "${lz4_BINARY_DIR}")
  set (LZ4_INCLUDE_DIR "${lz4_SOURCE_DIR}/LZ4")
  set (LZ4_FOUND 1)
  set (LZ4_LIBRARIES ${LZ4_LIBRARY})
  set (LZ4_INCLUDE_DIRS ${LZ4_INCLUDE_DIR_GEN} ${LZ4_INCLUDE_DIR})
endmacro ()

#-------------------------------------------------------------------------------
macro (PACKAGE_LZ4_LIBRARY compress_type)
  add_custom_target (LZ4-GenHeader-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LZ4_INCLUDE_DIR}/lz4.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
      COMMENT "Copying ${LZ4_INCLUDE_DIR}/lz4.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
  )
  set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lz4.h)
  if (${compress_type} MATCHES "GIT" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (LZ4-GenHeader-Copy LZ4)
  endif ()
endmacro ()
