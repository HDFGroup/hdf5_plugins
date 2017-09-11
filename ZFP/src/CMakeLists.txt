cmake_minimum_required (VERSION 3.2.2)
project (H5ZFP_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
include_directories (${H5ZFP_SRC_SOURCE_DIR})
include_directories (${H5ZFP_SRC_LIBZFP_DIR}/inc)
include_directories (${CMAKE_BINARY_DIR})
include_directories (${HDF5_INCLUDE_DIR} ${ZFP_INCLUDE_DIRS})

add_subdirectory (${H5ZFP_SRC_SOURCE_DIR}/libzfp ${PROJECT_BINARY_DIR}/libzfp)

#-----------------------------------------------------------------------------
# Define H5ZFP Library
#-----------------------------------------------------------------------------
set (H5ZFP_SRCS
    ${H5ZFP_SRC_SOURCE_DIR}/H5Zzfp.c
)

add_library (${H5ZFP_LIB_TARGET} ${LIB_TYPE} ${H5ZFP_SRCS} $<TARGET_OBJECTS:${ZFP_LIB_TARGET}>)
target_link_libraries (${H5ZFP_LIB_TARGET} ${LINK_LIBS})
  if (WIN32)
    target_link_libraries (${H5ZFP_LIB_TARGET} "ws2_32.lib")
  endif ()
SET_GLOBAL_VARIABLE (H5ZFP_LIBRARIES_TO_EXPORT ${H5ZFP_LIB_TARGET})
H5ZFP_SET_LIB_OPTIONS (${H5ZFP_LIB_TARGET} ${H5ZFP_LIB_NAME} ${LIB_TYPE})

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