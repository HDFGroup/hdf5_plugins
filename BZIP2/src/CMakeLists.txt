cmake_minimum_required (VERSION 2.8.10)
project (H5BZ2_SRC C)

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${H5BZ2_SRC_SOURCE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR})

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIR} ${BZ2_INCLUDE_DIRS})

#-----------------------------------------------------------------------------
# Define H5BZ2 Library
#-----------------------------------------------------------------------------
set (H5BZ2_SRCS
    ${H5BZ2_SRC_SOURCE_DIR}/H5Zbzip2.c
)

add_library (${H5BZ2_LIB_TARGET} ${LIB_TYPE} ${H5BZ2_SRCS})
target_link_libraries (${H5BZ2_LIB_TARGET} ${LINK_LIBS})
SET_GLOBAL_VARIABLE (H5BZ2_LIBRARIES_TO_EXPORT ${H5BZ2_LIB_TARGET})
H5BZ2_SET_LIB_OPTIONS (${H5BZ2_LIB_TARGET} ${H5BZ2_LIB_NAME} ${LIB_TYPE})

if (BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5BZ2_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different 
        "$<TARGET_FILE:${H5BZ2_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5BZ2_LIB_TARGET}>"
  )
endif (BUILD_TESTING)

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5BZ2_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5BZ2_LIB_TARGET}
      EXPORT
          ${H5BZ2_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5BZ2_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5BZ2_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5BZ2_INSTALL_BIN_DIR} COMPONENT libraries
  )
endif (H5BZ2_EXPORTED_TARGETS)
 