cmake_minimum_required (VERSION 3.1.0)
project (H5MAFISC_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
include_directories (${H5MAFISC_SRC_SOURCE_DIR})
include_directories (${H5MAFISC_SRC_LIBLZMA_DIR}/api)
include_directories (${CMAKE_BINARY_DIR})
include_directories (${HDF5_INCLUDE_DIR} ${MAFISC_INCLUDE_DIRS})

add_subdirectory (${H5MAFISC_SRC_SOURCE_DIR}/liblzma ${PROJECT_BINARY_DIR}/liblzma)

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
endif (BUILD_MAFISC_LIBRARY_SOURCE)

add_library (${H5MAFISC_LIB_TARGET} ${LIB_TYPE} ${H5MAFISC_SRCS} $<TARGET_OBJECTS:${LZMA_LIB_TARGET}>)
target_link_libraries (${H5MAFISC_LIB_TARGET} ${LINK_LIBS})
  if (WIN32)
    target_link_libraries (${H5MAFISC_LIB_TARGET} "ws2_32.lib")
  endif ()
SET_GLOBAL_VARIABLE (H5MAFISC_LIBRARIES_TO_EXPORT ${H5MAFISC_LIB_TARGET})
H5MAFISC_SET_LIB_OPTIONS (${H5MAFISC_LIB_TARGET} ${H5MAFISC_LIB_NAME} ${LIB_TYPE})

if (BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  HDFTEST_COPY_FILE("$<TARGET_FILE:${H5MAFISC_LIB_TARGET}>" "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5MAFISC_LIB_TARGET}>" "plugin_files")
  add_custom_target(plugin_files ALL COMMENT "Copying plugin files needed by tests" DEPENDS ${plugin_files_list})
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
endif (H5MAFISC_EXPORTED_TARGETS)
