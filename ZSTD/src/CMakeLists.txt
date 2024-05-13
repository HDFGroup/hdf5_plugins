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
cmake_minimum_required (VERSION 3.12)
project (H5ZSTD_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
set (H5ZSTD_INCLUDE_DIRS ${H5ZSTD_INCLUDE_DIRS}
    ${H5PL_HDF5_INCLUDE_DIRS} ${H5ZSTD_BINARY_DIR}
    ${H5ZSTD_SRC_SOURCE_DIR}
    ${ZSTD_INCLUDE_DIRS}
)
#-----------------------------------------------------------------------------
# Define H5ZSTD Library
#-----------------------------------------------------------------------------
set (H5ZSTD_SRCS
    ${H5ZSTD_SRC_SOURCE_DIR}/H5Zzstd.c
)

add_library (${H5ZSTD_LIB_TARGET} MODULE ${H5ZSTD_SRCS})
if (DISABLE_H5ZSTD_ENCODER)
  set_target_properties(${H5ZSTD_LIB_TARGET} PROPERTIES COMPILE_DEFINITIONS "FILTER_DECODE_ONLY")
endif ()
target_include_directories(${H5ZSTD_LIB_TARGET} PRIVATE ${H5ZSTD_INCLUDE_DIRS})
target_link_libraries (${H5ZSTD_LIB_TARGET} PRIVATE ${H5PL_LINK_LIBS})
set_global_variable (H5ZSTD_LIBRARIES_TO_EXPORT ${H5ZSTD_LIB_TARGET})
HDF_SET_LIB_VERSIONS ("H5ZSTD" ${H5ZSTD_LIB_TARGET} ${H5ZSTD_LIB_NAME} MODULE)

if (H5PL_BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5ZSTD_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "$<TARGET_FILE:${H5ZSTD_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5ZSTD_LIB_TARGET}>"
  )
endif ()

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5ZSTD_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5ZSTD_LIB_TARGET}
      EXPORT
          ${H5ZSTD_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5ZSTD_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5ZSTD_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5ZSTD_INSTALL_BIN_DIR} COMPONENT libraries
  )
endif ()
