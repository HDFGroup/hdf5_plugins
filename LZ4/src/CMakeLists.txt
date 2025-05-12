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
project (H5LZ4_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
set (H5LZ4_INCLUDE_DIRS ${H5LZ4_INCLUDE_DIRS}
    ${H5PL_HDF5_INCLUDE_DIRS} ${H5LZ4_BINARY_DIR}
    ${H5LZ4_SRC_SOURCE_DIR}
    ${LZ4_INCLUDE_DIRS}
)

#-----------------------------------------------------------------------------
# Define H5LZ4 Library
#-----------------------------------------------------------------------------
set (H5LZ4_SRCS
    ${H5LZ4_SRC_SOURCE_DIR}/H5Zlz4.c
)

add_library (${H5LZ4_LIB_TARGET} MODULE ${H5LZ4_SRCS})
if (DISABLE_H5LZ4_ENCODER)
  set_target_properties(${H5LZ4_LIB_TARGET} PROPERTIES COMPILE_DEFINITIONS "FILTER_DECODE_ONLY")
endif ()
target_include_directories(${H5LZ4_LIB_TARGET} PRIVATE ${H5LZ4_INCLUDE_DIRS})
target_link_libraries (${H5LZ4_LIB_TARGET} PRIVATE ${H5PL_LINK_LIBS})
  if (WIN32)
    target_link_libraries (${H5LZ4_LIB_TARGET} PRIVATE "ws2_32.lib")
  endif ()
set_global_variable (H5LZ4_LIBRARIES_TO_EXPORT ${H5LZ4_LIB_TARGET})
HDF_SET_LIB_VERSIONS ("H5LZ4" ${H5LZ4_LIB_TARGET} ${H5LZ4_LIB_NAME} MODULE)

if (H5PL_BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5LZ4_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "$<TARGET_FILE:${H5LZ4_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5LZ4_LIB_TARGET}>"
  )
endif ()

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5LZ4_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5LZ4_LIB_TARGET}
      EXPORT
          ${H5LZ4_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5LZ4_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5LZ4_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5LZ4_INSTALL_BIN_DIR} COMPONENT libraries
  )
endif ()
