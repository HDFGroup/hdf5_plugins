cmake_minimum_required (VERSION 3.1.0)
project (H5BLOSC_SRC C CXX)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
include_directories (${H5BLOSC_SRC_SOURCE_DIR})
include_directories (${CMAKE_BINARY_DIR})
include_directories (${HDF5_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Define H5BLOSC Library
#-----------------------------------------------------------------------------
set (H5BLOSC_SRCS
    ${H5BLOSC_SRC_SOURCE_DIR}/H5Zblosc.c
)

add_library (${H5BLOSC_LIB_TARGET} SHARED ${H5BLOSC_SRCS})
target_link_libraries (${H5BLOSC_LIB_TARGET} ${LINK_LIBS})
set_global_variable (H5BLOSC_LIBRARIES_TO_EXPORT ${H5BLOSC_LIB_TARGET})
H5BLOSC_SET_LIB_OPTIONS (${H5BLOSC_LIB_TARGET} ${H5BLOSC_LIB_NAME} ${LIB_TYPE})

if (BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  get_filename_component(fname "$<TARGET_FILE:${H5BLOSC_LIB_TARGET}>" NAME)
  HDFTEST_COPY_FILE("$<TARGET_FILE:${H5BLOSC_LIB_TARGET}>" "${CMAKE_BINARY_DIR}/plugins/${fname}" "plugin_files")
  add_custom_target(plugin_files ALL COMMENT "Copying plugin files needed by tests" DEPENDS ${plugin_files_list})
endif ()

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
