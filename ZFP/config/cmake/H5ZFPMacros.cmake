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
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy
            ${H5ZFP_SOURCE_DIR}/config/CMakeLists.txt
            <SOURCE_DIR>/CMakeLists.txt
            && ${CMAKE_COMMAND} -E copy
            ${H5ZFP_SOURCE_DIR}/config/srcCMakeLists.txt
            <SOURCE_DIR>/src/CMakeLists.txt
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (ZFP
        URL ${ZFP_URL}
        URL_HASH ""
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy
            ${H5ZFP_SOURCE_DIR}/config/CMakeLists.txt
            <SOURCE_DIR>/CMakeLists.txt
            && ${CMAKE_COMMAND} -E copy
            ${H5ZFP_SOURCE_DIR}/config/srcCMakeLists.txt
            <SOURCE_DIR>/src/CMakeLists.txt
    )
  endif ()
#  FetchContent_GetProperties (ZFP)
#  if (NOT zfp_POPULATED)
#    FetchContent_Populate (ZFP)

    # Copy an additional/replacement files into the populated source
#    file(COPY ${H5ZFP_SOURCE_DIR}/config/CMakeLists.txt DESTINATION ${zfp_SOURCE_DIR})
#    file(COPY ${H5ZFP_SOURCE_DIR}/config/srcCMakeLists.txt DESTINATION ${zfp_SOURCE_DIR}/src)
#    file(RENAME ${zfp_SOURCE_DIR}/src/srcCMakeLists.txt ${zfp_SOURCE_DIR}/src/CMakeLists.txt)

    set (ZFP_WITH_OPENMP OFF CACHE BOOL "")
    set (ZFP_BIT_STREAM_WORD_SIZE 8 CACHE STRING "")
    set (ZFP_LIBRARY_PREFIX ${ZFP_LIBRARY_PREFIX} CACHE STRING "")

    # Store the old value of the 'BUILD_SHARED_LIBS'
    set (BUILD_SHARED_LIBS_OLD ${BUILD_SHARED_LIBS})
    # Make subproject to use 'BUILD_SHARED_LIBS=OFF' setting.
    set (BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build SHARED libraries" FORCE)

#    add_subdirectory (${zfp_SOURCE_DIR} ${zfp_BINARY_DIR})
  FetchContent_MakeAvailable (ZFP)

    # Restore the old value of the parameter
    set (BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "Type of libraries to build" FORCE)
#  endif ()

#  include (${BINARY_DIR}/ZFP-targets.cmake)
  set (ZFP_LIBRARY "zfp")

  set (ZFP_INCLUDE_DIR_GEN "${zfp_BINARY_DIR}")
  set (ZFP_INCLUDE_DIR "${zfp_SOURCE_DIR}/include")
  set (ZFP_FOUND 1)
  set (ZFP_LIBRARIES ${ZFP_LIBRARY})
  set (ZFP_INCLUDE_DIRS ${ZFP_INCLUDE_DIR_GEN} ${ZFP_INCLUDE_DIR})
endmacro ()
