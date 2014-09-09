cmake_minimum_required (VERSION 2.8.10)
project (H5BLOSC_SRC C)

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${H5BLOSC_SRC_SOURCE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR})

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIR} ${BLOSC_INCLUDE_DIRS})

#-----------------------------------------------------------------------------
# Define H5BLOSC Library
#-----------------------------------------------------------------------------
set (H5BLOSC_SRCS
    ${H5BLOSC_SRC_SOURCE_DIR}/H5Zblosc.c
)

add_library (${H5BLOSC_LIB_TARGET} ${LIB_TYPE} ${H5BLOSC_SRCS})
target_link_libraries (${H5BLOSC_LIB_TARGET} ${LINK_LIBS} ${CMAKE_THREAD_LIBS_INIT})
SET_GLOBAL_VARIABLE (H5BLOSC_LIBRARIES_TO_EXPORT ${H5BLOSC_LIB_TARGET})
H5BLOSC_SET_LIB_OPTIONS (${H5BLOSC_LIB_TARGET} ${H5BLOSC_LIB_NAME} ${LIB_TYPE})

if (BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5BLOSC_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different 
        "$<TARGET_FILE:${H5BLOSC_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5BLOSC_LIB_TARGET}>"
  )
endif (BUILD_TESTING)

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5BLOSC_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5BLOSC_LIB_TARGET}
      EXPORT
          ${H5BLOSC_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5BLOSC_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5BLOSC_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5BLOSC_INSTALL_BIN_DIR} COMPONENT libraries
  )
endif (H5BLOSC_EXPORTED_TARGETS)
 