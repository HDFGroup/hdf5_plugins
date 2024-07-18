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
macro (EXTERNAL_MAFISC_LIBRARY compress_type)
  if (${compress_type} MATCHES "GIT")
    FetchContent_Declare (MAFISC
        GIT_REPOSITORY ${MAFISC_URL}
        GIT_TAG ${MAFISC_BRANCH}
    )
  elseif (${compress_type} MATCHES "TGZ")
    FetchContent_Declare (MAFISC
        URL ${MAFISC_URL}
        URL_HASH ""
    )
  endif ()
#  FetchContent_GetProperties (MAFISC)
#  if (NOT mafisc_POPULATED)
#    FetchContent_Populate (MAFISC)
#
#    add_subdirectory (${mafisc_SOURCE_DIR} ${mafisc_BINARY_DIR})
#  endif ()
  FetchContent_MakeAvailable (MAFISC)

#  include (${BINARY_DIR}/MAFISC-targets.cmake)
  set (MAFISC_LIBRARY "mafisc-static")

  set (MAFISC_INCLUDE_DIR_GEN "${mafisc_BINARY_DIR}")
  set (MAFISC_INCLUDE_DIR "${mafisc_SOURCE_DIR}/MAFISC")
  set (MAFISC_FOUND 1)
  set (MAFISC_LIBRARIES ${MAFISC_LIBRARY})
  set (MAFISC_INCLUDE_DIRS ${MAFISC_INCLUDE_DIR_GEN} ${MAFISC_INCLUDE_DIR})
endmacro ()
