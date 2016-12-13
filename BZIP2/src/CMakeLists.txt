cmake_minimum_required (VERSION 3.1.0)
project (H5BZ2_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
include_directories (${H5BZ2_SRC_SOURCE_DIR})
include_directories (${CMAKE_BINARY_DIR})
include_directories (${HDF5_INCLUDE_DIR} ${BZ2_INCLUDE_DIRS})

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
  HDFTEST_COPY_FILE("$<TARGET_FILE:${H5BZ2_LIB_TARGET}>" "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5BZ2_LIB_TARGET}>" "plugin_files")
  add_custom_target(plugin_files ALL COMMENT "Copying plugin files needed by tests" DEPENDS ${plugin_files_list})
endif ()

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
