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
project (H5BITGROOM_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
set (H5BITGROOM_INCLUDE_DIRS ${H5BITGROOM_INCLUDE_DIRS}
    ${H5PL_HDF5_INCLUDE_DIRS} ${H5BITGROOM_BINARY_DIR}
    ${BITGROOM_INCLUDE_DIRS}
)

#-----------------------------------------------------------------------------
# Define H5BITGROOM Library
#-----------------------------------------------------------------------------
set (H5BITGROOM_SRCS
    ${H5BITGROOM_SRC_SOURCE_DIR}/H5Zbitgroom.c
)

add_library (${H5BITGROOM_LIB_TARGET} MODULE ${H5BITGROOM_SRCS})
if (DISABLE_H5BITGROOM_ENCODER)
  set_target_properties(${H5BITGROOM_LIB_TARGET} PROPERTIES COMPILE_DEFINITIONS "FILTER_DECODE_ONLY")
endif ()
target_include_directories(${H5BITGROOM_LIB_TARGET} PRIVATE ${H5BITGROOM_INCLUDE_DIRS})
target_link_libraries (${H5BITGROOM_LIB_TARGET} PRIVATE ${H5PL_LINK_LIBS})
set_global_variable (H5BITGROOM_LIBRARIES_TO_EXPORT ${H5BITGROOM_LIB_TARGET})
HDF_SET_LIB_VERSIONS ("H5BITGROOM" ${H5BITGROOM_LIB_TARGET} ${H5BITGROOM_LIB_NAME} MODULE)

if (H5PL_BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5BITGROOM_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "$<TARGET_FILE:${H5BITGROOM_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5BITGROOM_LIB_TARGET}>"
  )
endif ()

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5BITGROOM_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5BITGROOM_LIB_TARGET}
      EXPORT
          ${H5BITGROOM_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5BITGROOM_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5BITGROOM_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5BITGROOM_INSTALL_BIN_DIR} COMPONENT libraries
  )
endif ()
