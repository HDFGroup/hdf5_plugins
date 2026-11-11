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
cmake_minimum_required (VERSION 3.18)
project (H5BLOSC2_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
# The filter sources come from the upstream Blosc/HDF5-Blosc2 repository,
# included as a git subtree in ../HDF5-Blosc2 (see ../UPDATING_BLOSC2_SUBTREE.md).
# Only its src/ files are used; its own CMakeLists.txt downloads c-blosc2 and
# is not part of this build.
#-----------------------------------------------------------------------------
set (H5BLOSC2_UPSTREAM_SRC_DIR ${H5BLOSC2_SRC_SOURCE_DIR}/../HDF5-Blosc2/src)

set (H5BLOSC2_INCLUDE_DIRS ${H5BLOSC2_INCLUDE_DIRS}
    ${H5PL_HDF5_INCLUDE_DIRS} ${H5BLOSC2_BINARY_DIR}
    ${H5BLOSC2_SRC_SOURCE_DIR}
    ${H5BLOSC2_UPSTREAM_SRC_DIR}
)

#-----------------------------------------------------------------------------
# Define H5BLOSC2 Library
#-----------------------------------------------------------------------------
set (H5BLOSC2_SRCS
    ${H5BLOSC2_UPSTREAM_SRC_DIR}/blosc2_filter.c
    ${H5BLOSC2_UPSTREAM_SRC_DIR}/blosc2_plugin.c
)

add_library (${H5BLOSC2_LIB_TARGET} MODULE ${H5BLOSC2_SRCS})
if (DISABLE_H5BLOSC2_ENCODER)
  message (WARNING "DISABLE_H5BLOSC2_ENCODER has no effect: the upstream Blosc/HDF5-Blosc2 filter always includes the encoder")
endif ()
target_include_directories(${H5BLOSC2_LIB_TARGET} PRIVATE ${H5BLOSC2_INCLUDE_DIRS})
target_link_libraries (${H5BLOSC2_LIB_TARGET} PRIVATE ${H5PL_LINK_LIBS})
set_global_variable (H5BLOSC2_LIBRARIES_TO_EXPORT ${H5BLOSC2_LIB_TARGET})
HDF_SET_LIB_VERSIONS ("H5BLOSC2" ${H5BLOSC2_LIB_TARGET} ${H5BLOSC2_LIB_NAME} MODULE)

if (H5PL_BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5BLOSC2_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "$<TARGET_FILE:${H5BLOSC2_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5BLOSC2_LIB_TARGET}>"
  )
endif ()

#-----------------------------------------------------------------------------
# The upstream HDF5-Blosc2 licenses ask binary redistributions to carry their notices
#-----------------------------------------------------------------------------
install (
    FILES
        ${H5BLOSC2_SRC_SOURCE_DIR}/../HDF5-Blosc2/LICENSE.txt
        ${H5BLOSC2_SRC_SOURCE_DIR}/../HDF5-Blosc2/LICENSES/BLOSC.txt
        ${H5BLOSC2_SRC_SOURCE_DIR}/../HDF5-Blosc2/LICENSES/H5PY.txt
    DESTINATION ${H5BLOSC2_INSTALL_DOC_DIR}/BLOSC2
    COMPONENT libraries
)

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5BLOSC2_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5BLOSC2_LIB_TARGET}
      EXPORT
          ${H5BLOSC2_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5BLOSC2_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5BLOSC2_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5BLOSC2_INSTALL_BIN_DIR} COMPONENT libraries
  )
endif ()
