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
project (H5SZ_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
set (H5SZ_INCLUDE_DIRS ${H5SZ_INCLUDE_DIRS}
    ${HDF5_INCLUDE_DIR}
    ${H5SZ_SRC_SOURCE_DIR}
    ${H5SZ_BINARY_DIR}
    ${SZ_INCLUDE_DIRS}
)
#-----------------------------------------------------------------------------
# Define H5SZ Library
#-----------------------------------------------------------------------------
set (H5SZ_SRCS
    ${H5SZ_SRC_SOURCE_DIR}/H5Zszf.c
)

add_library (${H5SZ_LIB_TARGET} MODULE ${H5SZ_SRCS})
if (DISABLE_H5SZ_ENCODER)
  set_target_properties(${H5SZ_LIB_TARGET} PROPERTIES COMPILE_DEFINITIONS "FILTER_DECODE_ONLY")
endif ()
target_include_directories(${H5SZ_LIB_TARGET} PRIVATE ${H5SZ_INCLUDE_DIRS})
#set_property (TARGET ${H5SZ_LIB_TARGET} APPEND PROPERTY COMPILE_DEFINITIONS H5_SZ_DEBUG)
target_link_libraries (${H5SZ_LIB_TARGET} PRIVATE ${LINK_LIBS})
if (WIN32)
  target_link_libraries (${H5SZ_LIB_TARGET} PRIVATE "ws2_32.lib")
endif ()
set_global_variable (H5SZ_LIBRARIES_TO_EXPORT ${H5SZ_LIB_TARGET})
HDF_SET_LIB_VERSIONS ("H5SZ" ${H5SZ_LIB_TARGET} ${H5SZ_LIB_NAME} MODULE)

if (BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5SZ_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "$<TARGET_FILE:${H5SZ_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5SZ_LIB_TARGET}>"
  )
  #-----------------------------------------------------------------------------
  # Copy plugin config to a plugins folder
  #-----------------------------------------------------------------------------
  add_custom_command (
      TARGET     ${H5SZ_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "${H5SZ_SRC_SOURCE_DIR}/sz.config"
        "${CMAKE_BINARY_DIR}/plugins/sz.config"
  )
endif ()

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5SZ_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5SZ_LIB_TARGET}
      EXPORT
          ${H5SZ_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5SZ_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5SZ_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5SZ_INSTALL_BIN_DIR} COMPONENT libraries
  )
  install (
      FILES
          ${H5SZ_SRC_SOURCE_DIR}/sz.config
      DESTINATION ${H5SZ_INSTALL_LIB_DIR} COMPONENT libraries
  )
endif ()
