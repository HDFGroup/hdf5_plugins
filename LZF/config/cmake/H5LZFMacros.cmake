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
macro (EXTERNAL_LZF_LIBRARY compress_type)
  if (${compress_type} MATCHES "GIT")
    FetchContent_Declare (LZF
        GIT_REPOSITORY ${LZF_URL}
        GIT_TAG ${LZF_BRANCH}
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy
            ${H5LZF_SOURCE_DIR}/config/CMakeLists.txt
            <SOURCE_DIR>/CMakeLists.txt
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (LZF
        URL ${LZF_URL}
        URL_HASH ""
        PATCH_COMMAND ${CMAKE_COMMAND} -E copy
            ${H5LZF_SOURCE_DIR}/config/CMakeLists.txt
            <SOURCE_DIR>/CMakeLists.txt
    )
  endif ()

  set (LZF_EXTERNALLY_CONFIGURED OFF CACHE INTERNAL "No package" FORCE)
  set (LZF_INSTALL_NO_DEVELOPMENT OFF CACHE INTERNAL "No package" FORCE)

  # Store the old value of the 'BUILD_SHARED_LIBS'
  set (BUILD_SHARED_LIBS_OLD ${BUILD_SHARED_LIBS})
  # Make subproject to use 'BUILD_SHARED_LIBS=OFF' setting.
  set (BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build SHARED libraries" FORCE)
  # Store the old value of the 'BUILD_TESTING'
  set (BUILD_TESTING_OLD ${BUILD_TESTING})
  # Make subproject to use 'BUILD_TESTING=OFF' setting.
  set (BUILD_TESTING OFF CACHE INTERNAL "Build Unit Testing" FORCE)

  FetchContent_MakeAvailable (LZF)

  # Restore the old value of the parameter
  set (BUILD_TESTING ${BUILD_TESTING_OLD} CACHE BOOL "Build Unit Testing" FORCE)
  # Restore the old value of the parameter
  set (BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_OLD} CACHE BOOL "Type of libraries to build" FORCE)

#  include (${BINARY_DIR}/LZF-targets.cmake)
  set (LZF_LIBRARY "lzf-static")

  set (LZF_INCLUDE_DIR_GEN "${lzf_BINARY_DIR}")
  set (LZF_INCLUDE_DIR "${lzf_SOURCE_DIR}")
  set (LZF_FOUND 1)
  set (LZF_LIBRARIES ${LZF_LIBRARY})
  set (LZF_INCLUDE_DIRS ${LZF_INCLUDE_DIR_GEN} ${LZF_INCLUDE_DIR})
endmacro ()
