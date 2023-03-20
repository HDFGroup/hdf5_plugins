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
project (H5ZFP_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
set (H5ZFP_INCLUDE_DIRS ${H5ZFP_INCLUDE_DIRS}
    ${HDF5_INCLUDE_DIR}
    ${H5ZFP_SRC_SOURCE_DIR}
    ${H5ZFP_BINARY_DIR}
    ${ZFP_INCLUDE_DIRS}
)
#-----------------------------------------------------------------------------
# Define H5ZFP Library
#-----------------------------------------------------------------------------
set (H5ZFP_HDRS
    ${H5ZFP_SRC_SOURCE_DIR}/H5Zzfp.h
    ${H5ZFP_SRC_SOURCE_DIR}/H5Zzfp_lib.h
    ${H5ZFP_SRC_SOURCE_DIR}/H5Zzfp_plugin.h
    ${H5ZFP_SRC_SOURCE_DIR}/H5Zzfp_props.h
)

set (H5ZFP_SRCS
    ${H5ZFP_SRC_SOURCE_DIR}/H5Zzfp.c
    ${H5ZFP_SRC_SOURCE_DIR}/H5Zzfp_props_private.h
    ${H5ZFP_SRC_SOURCE_DIR}/H5Zzfp_props.c
)

add_library (${H5ZFP_LIB_TARGET} MODULE ${H5ZFP_SRCS} ${H5ZFP_HDRS})
if (DISABLE_H5ZFP_ENCODER)
  set_target_properties(${H5ZFP_LIB_TARGET} PROPERTIES COMPILE_DEFINITIONS "FILTER_DECODE_ONLY")
endif ()
target_include_directories(${H5ZFP_LIB_TARGET} PRIVATE ${H5ZFP_INCLUDE_DIRS})
target_link_libraries (${H5ZFP_LIB_TARGET} PRIVATE ${LINK_LIBS})
if (WIN32)
  target_link_libraries (${H5ZFP_LIB_TARGET} PRIVATE "ws2_32.lib")
endif ()
set_global_variable (H5ZFP_LIBRARIES_TO_EXPORT ${H5ZFP_LIB_TARGET})
HDF_SET_LIB_VERSIONS ("H5ZFP" ${H5ZFP_LIB_TARGET} ${H5ZFP_LIB_NAME} MODULE)

if (BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5ZFP_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "$<TARGET_FILE:${H5ZFP_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5ZFP_LIB_TARGET}>"
  )
endif ()

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5ZFP_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5ZFP_LIB_TARGET}
      EXPORT
          ${H5ZFP_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5ZFP_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5ZFP_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5ZFP_INSTALL_BIN_DIR} COMPONENT libraries
  )
endif ()
