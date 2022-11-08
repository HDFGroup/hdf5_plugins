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
project (H5MAFISC_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
set (H5MAFISC_INCLUDE_DIRS ${H5MAFISC_INCLUDE_DIRS}
    ${HDF5_INCLUDE_DIR}
    ${H5MAFISC_SRC_SOURCE_DIR}
    ${H5MAFISC_SRC_LIBLZMA_DIR}/api
    ${H5MAFISC_BINARY_DIR}
    ${MAFISC_INCLUDE_DIR}
)

add_subdirectory (liblzma)

#-----------------------------------------------------------------------------
# Define H5MAFISC Library
#-----------------------------------------------------------------------------
set (H5MAFISC_SRCS
    ${H5MAFISC_SRC_SOURCE_DIR}/mafisc.c
)
if (BUILD_MAFISC_LIBRARY_SOURCE)
  set (H5MAFISC_SRCS ${H5MAFISC_SRCS}
      ${H5MAFISC_SRC_SOURCE_DIR}/adaptive-filter.c
      ${H5MAFISC_SRC_SOURCE_DIR}/bit-sorter-filter.c
      ${H5MAFISC_SRC_SOURCE_DIR}/combined-filter.c
      ${H5MAFISC_SRC_SOURCE_DIR}/difference-filter.c
      ${H5MAFISC_SRC_SOURCE_DIR}/filter-cascade-filter.c
      ${H5MAFISC_SRC_SOURCE_DIR}/lorenzo-filter.c
      ${H5MAFISC_SRC_SOURCE_DIR}/sign-filter.c
      ${H5MAFISC_SRC_SOURCE_DIR}/time-filter.c
      ${H5MAFISC_SRC_SOURCE_DIR}/variable-filter.c
      ${H5MAFISC_SRC_SOURCE_DIR}/simple-hash.c
  )
endif ()

add_library (${H5MAFISC_LIB_TARGET} MODULE ${H5MAFISC_SRCS} $<TARGET_OBJECTS:${LZMA_LIB_TARGET}>)
if (DISABLE_H5MAFISC_ENCODER)
  set_target_properties(${H5MAFISC_LIB_TARGET} PROPERTIES COMPILE_DEFINITIONS "FILTER_DECODE_ONLY")
endif ()
target_include_directories(${H5MAFISC_LIB_TARGET} PRIVATE ${H5MAFISC_INCLUDE_DIRS})
target_link_libraries (${H5MAFISC_LIB_TARGET} PRIVATE ${LINK_LIBS})
if (WIN32)
  target_link_libraries (${H5MAFISC_LIB_TARGET} PRIVATE "ws2_32.lib")
endif ()
set_global_variable (H5MAFISC_LIBRARIES_TO_EXPORT ${H5MAFISC_LIB_TARGET})
HDF_SET_LIB_VERSIONS ("H5MAFISC" ${H5MAFISC_LIB_TARGET} ${H5MAFISC_LIB_NAME} MODULE)

if (BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5MAFISC_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "$<TARGET_FILE:${H5MAFISC_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5MAFISC_LIB_TARGET}>"
  )
endif ()

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5MAFISC_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5MAFISC_LIB_TARGET}
      EXPORT
          ${H5MAFISC_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5MAFISC_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5MAFISC_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5MAFISC_INSTALL_BIN_DIR} COMPONENT libraries
  )
endif ()
