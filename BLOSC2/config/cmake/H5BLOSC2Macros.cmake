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
macro (EXTERNAL_BLOSC2_LIBRARY compress_type)
  if (${compress_type} MATCHES "GIT")
    FetchContent_Declare (BLOSC2
        GIT_REPOSITORY ${BLOSC2_URL}
        GIT_TAG ${BLOSC2_BRANCH}
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (BLOSC2
        URL ${BLOSC2_URL}
        URL_HASH ""
    )
  endif ()
  FetchContent_GetProperties(BLOSC2)
  if(NOT blosc2_POPULATED)
    FetchContent_Populate(BLOSC2)
    set (BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set (BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set (BUILD_FUZZERS OFF CACHE BOOL "" FORCE)
    set (BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
    # Store the old value of the 'BUILD_EXAMPLES'
    set (BUILD_EXAMPLES_OLD ${BUILD_EXAMPLES})
    set (BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set (BUILD_PLUGINS OFF CACHE BOOL "" FORCE)
    set (BLOSC_IS_SUBPROJECT ON CACHE BOOL "" FORCE)
    set (BLOSC_INSTALL OFF CACHE BOOL "" FORCE)

    add_subdirectory(${blosc2_SOURCE_DIR} ${blosc2_BINARY_DIR})
    target_compile_options(blosc2_static PRIVATE $<$<CMAKE_C_COMPILER_ID>:IntelLLVM>:-Wno-implicit-function-declaration>)

    # Restore the old value of the parameter
    set (BUILD_EXAMPLES ${BUILD_EXAMPLES_OLD} CACHE BOOL "Build EXAMPLES" FORCE)
  endif()

##include (${BINARY_DIR}/${BLOSC2_PACKAGE_NAME}${H5BLOSC2_PACKAGE_EXT}-targets.cmake)
  set (BLOSC2_STATIC_LIBRARY "blosc2_static")
  set (BLOSC2_LIBRARIES ${BLOSC2_STATIC_LIBRARY})

  set (BLOSC2_INCLUDE_DIR_GEN "${blosc2_BINARY_DIR}")
  set (BLOSC2_INCLUDE_DIR "${blosc2_SOURCE_DIR}/include")
  set (BLOSC2_FOUND 1)
  set (BLOSC2_INCLUDE_DIRS ${BLOSC2_INCLUDE_DIR_GEN} ${BLOSC2_INCLUDE_DIR})
endmacro ()

#-------------------------------------------------------------------------------
macro (PACKAGE_BLOSC2_LIBRARY compress_type)
  add_custom_target (BLOSC2-GenHeader-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BLOSC2_INCLUDE_DIR}/blosc2.h ${BLOSC2_INCLUDE_DIR}/blosc2-export.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
      COMMENT "Copying ${BLOSC2_INCLUDE_DIR_}/blosc2.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
  )
  set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/blosc2.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/blosc2-export.h)
  if (${compress_type} MATCHES "GIT" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (BLOSC2-GenHeader-Copy BLOSC2)
  endif ()
endmacro ()
