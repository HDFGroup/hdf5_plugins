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
project (H5BITROUND_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
set (H5BITROUND_INCLUDE_DIRS ${H5BITROUND_INCLUDE_DIRS}
    ${H5PL_HDF5_INCLUDE_DIRS} ${H5BITROUND_BINARY_DIR}
    ${BITROUND_INCLUDE_DIRS}
)

#-----------------------------------------------------------------------------
# Define H5BITROUND Library
#-----------------------------------------------------------------------------
set (H5BITROUND_SRCS
    ${H5BITROUND_SRC_SOURCE_DIR}/H5Zgranularbr.c
)

add_library (${H5BITROUND_LIB_TARGET} MODULE ${H5BITROUND_SRCS})
if (DISABLE_H5BITROUND_ENCODER)
  set_target_properties(${H5BITROUND_LIB_TARGET} PROPERTIES COMPILE_DEFINITIONS "FILTER_DECODE_ONLY")
endif ()
target_include_directories(${H5BITROUND_LIB_TARGET} PRIVATE ${H5BITROUND_INCLUDE_DIRS})
target_link_libraries (${H5BITROUND_LIB_TARGET} PRIVATE ${H5PL_LINK_LIBS})
set_global_variable (H5BITROUND_LIBRARIES_TO_EXPORT ${H5BITROUND_LIB_TARGET})
HDF_SET_LIB_VERSIONS ("H5BITROUND" ${H5BITROUND_LIB_TARGET} ${H5BITROUND_LIB_NAME} MODULE)

if (H5PL_BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5BITROUND_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "$<TARGET_FILE:${H5BITROUND_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5BITROUND_LIB_TARGET}>"
  )
endif ()

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5BITROUND_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5BITROUND_LIB_TARGET}
      EXPORT
          ${H5BITROUND_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5BITROUND_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5BITROUND_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5BITROUND_INSTALL_BIN_DIR} COMPONENT libraries
  )
endif ()
