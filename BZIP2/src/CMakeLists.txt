cmake_minimum_required (VERSION 2.8.10)
PROJECT (H5BZ2_SRC C)

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${H5BZ2_SRC_SOURCE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR})

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
ADD_DEFINITIONS (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIR} ${BZ2_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Define H5BZ2 Library
#-----------------------------------------------------------------------------
SET (H5BZ2_SRCS
    ${H5BZ2_SRC_SOURCE_DIR}/H5Zbzip2.c
)

ADD_LIBRARY (${H5BZ2_LIB_TARGET} ${LIB_TYPE} ${H5BZ2_SRCS})
TARGET_LINK_LIBRARIES (${H5BZ2_LIB_TARGET} ${BZ2_LIBRARIES})
SET_GLOBAL_VARIABLE (H5BZ2_LIBRARIES_TO_EXPORT ${H5BZ2_LIB_TARGET})
H5BZ2_SET_LIB_OPTIONS (${H5BZ2_LIB_TARGET} ${H5BZ2_LIB_NAME} ${LIB_TYPE})

IF (BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  FILE (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  ADD_CUSTOM_COMMAND (
      TARGET     ${H5BZ2_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different 
        "$<TARGET_FILE:${H5BZ2_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5BZ2_LIB_TARGET}>"
  )
ENDIF (BUILD_TESTING)

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
IF (H5BZ2_EXPORTED_TARGETS)
  INSTALL (
      TARGETS
          ${H5BZ2_LIB_TARGET}
      EXPORT
          ${H5BZ2_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5BZ2_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5BZ2_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5BZ2_INSTALL_BIN_DIR} COMPONENT libraries
  )
ENDIF (H5BZ2_EXPORTED_TARGETS)
  