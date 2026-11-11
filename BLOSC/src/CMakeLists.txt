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
project (H5BLOSC_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})
#-----------------------------------------------------------------------------
# Compiler specific flags : Shouldn't there be compiler tests for these
#-----------------------------------------------------------------------------
  if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR (CMAKE_CXX_COMPILER_LOADED AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
    add_definitions ("-Wno-incompatible-pointer-types")
  endif ()

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
# The filter sources come from the upstream Blosc/hdf5-blosc repository,
# included as a git subtree in ../hdf5-blosc (see ../UPDATING_BLOSC_SUBTREE.md).
# Only its src/ files are used; its own CMakeLists.txt downloads c-blosc and
# is not part of this build.
#-----------------------------------------------------------------------------
set (H5BLOSC_UPSTREAM_SRC_DIR ${H5BLOSC_SRC_SOURCE_DIR}/../hdf5-blosc/src)

set (H5BLOSC_INCLUDE_DIRS ${H5BLOSC_INCLUDE_DIRS}
    ${H5PL_HDF5_INCLUDE_DIRS} ${H5BLOSC_BINARY_DIR}
    ${H5BLOSC_SRC_SOURCE_DIR}
    ${H5BLOSC_UPSTREAM_SRC_DIR}
)

#-----------------------------------------------------------------------------
# Define H5BLOSC Library
#-----------------------------------------------------------------------------
set (H5BLOSC_SRCS
    ${H5BLOSC_UPSTREAM_SRC_DIR}/blosc_filter.c
    ${H5BLOSC_UPSTREAM_SRC_DIR}/blosc_plugin.c
)

add_library (${H5BLOSC_LIB_TARGET} MODULE ${H5BLOSC_SRCS})
if (DISABLE_H5BLOSC_ENCODER)
  message (WARNING "DISABLE_H5BLOSC_ENCODER has no effect: the upstream Blosc/hdf5-blosc filter always includes the encoder")
endif ()
target_include_directories(${H5BLOSC_LIB_TARGET} PRIVATE ${H5BLOSC_INCLUDE_DIRS})
target_link_libraries (${H5BLOSC_LIB_TARGET} PRIVATE ${H5PL_LINK_LIBS})
set_global_variable (H5BLOSC_LIBRARIES_TO_EXPORT ${H5BLOSC_LIB_TARGET})
HDF_SET_LIB_VERSIONS ("H5BLOSC" ${H5BLOSC_LIB_TARGET} ${H5BLOSC_LIB_NAME} MODULE)

if (H5PL_BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5BLOSC_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "$<TARGET_FILE:${H5BLOSC_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5BLOSC_LIB_TARGET}>"
  )
endif ()

#-----------------------------------------------------------------------------
# The upstream hdf5-blosc licenses ask binary redistributions to carry their notices
#-----------------------------------------------------------------------------
install (
    FILES
        ${H5BLOSC_SRC_SOURCE_DIR}/../hdf5-blosc/LICENSES/BLOSC_HDF5.txt
        ${H5BLOSC_SRC_SOURCE_DIR}/../hdf5-blosc/LICENSES/BLOSC.txt
        ${H5BLOSC_SRC_SOURCE_DIR}/../hdf5-blosc/LICENSES/H5PY.txt
    DESTINATION ${H5BLOSC_INSTALL_DOC_DIR}/BLOSC
    COMPONENT libraries
)

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5BLOSC_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5BLOSC_LIB_TARGET}
      EXPORT
          ${H5BLOSC_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5BLOSC_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5BLOSC_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5BLOSC_INSTALL_BIN_DIR} COMPONENT libraries
  )
endif ()
