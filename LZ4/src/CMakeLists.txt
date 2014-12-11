cmake_minimum_required (VERSION 2.8.10)
project (H5LZ4_SRC C)

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${H5LZ4_SRC_SOURCE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR})

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIR} ${LZ4_INCLUDE_DIRS})

#-----------------------------------------------------------------------------
# Define H5LZ4 Library
#-----------------------------------------------------------------------------
set (H5LZ4_SRCS
    ${H5LZ4_SRC_SOURCE_DIR}/H5Zlz4.c
)
if (BUILD_LZ4_LIBRARY_SOURCE)
  set (H5LZ4_SRCS ${H5LZ4_SRCS}
      ${H5LZ4_SRC_SOURCE_DIR}/lz4.c
      ${H5LZ4_SRC_SOURCE_DIR}/lz4.h
      ${H5LZ4_SRC_SOURCE_DIR}/lz4hc.c
      ${H5LZ4_SRC_SOURCE_DIR}/lz4hc.h
  )
endif (BUILD_LZ4_LIBRARY_SOURCE)

add_library (${H5LZ4_LIB_TARGET} ${LIB_TYPE} ${H5LZ4_SRCS})
target_link_libraries (${H5LZ4_LIB_TARGET} ${LINK_LIBS})
  if (WIN32)
    target_link_libraries (${H5LZ4_LIB_TARGET} "ws2_32.lib")
  endif (WIN32)
SET_GLOBAL_VARIABLE (H5LZ4_LIBRARIES_TO_EXPORT ${H5LZ4_LIB_TARGET})
H5LZ4_SET_LIB_OPTIONS (${H5LZ4_LIB_TARGET} ${H5LZ4_LIB_NAME} ${LIB_TYPE})

if (BUILD_TESTING)
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
endif (BUILD_TESTING)

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
endif (H5LZ4_EXPORTED_TARGETS)
