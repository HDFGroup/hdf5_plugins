cmake_minimum_required (VERSION 3.2.2)
project (H5LZF_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
set(H5LZF_INCLUDE_DIRS ${H5LZF_INCLUDE_DIRS}
    ${HDF5_INCLUDE_DIR} ${H5LZF_BINARY_DIR}
    ${H5LZF_SRC_SOURCE_DIR}
    ${LZF_INCLUDE_DIRS}
)

#-----------------------------------------------------------------------------
# Define H5LZF Library
#-----------------------------------------------------------------------------
set (H5LZF_SRCS
    ${H5LZF_SRC_SOURCE_DIR}/H5Zlzf.c
)

add_library (${H5LZF_LIB_TARGET} MODULE ${H5LZF_SRCS})
target_include_directories(${H5LZF_LIB_TARGET} PRIVATE ${H5LZF_INCLUDE_DIRS})
#set_property (TARGET ${H5LZF_LIB_TARGET} APPEND PROPERTY COMPILE_DEFINITIONS H5PY_LZF_DEBUG)
target_link_libraries (${H5LZF_LIB_TARGET} PRIVATE ${LINK_LIBS})
set_global_variable (H5LZF_LIBRARIES_TO_EXPORT ${H5LZF_LIB_TARGET})
HDF_SET_LIB_VERSIONS ("H5LZF" ${H5LZF_LIB_TARGET} ${H5LZF_LIB_NAME} MODULE)

if (BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5LZF_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "$<TARGET_FILE:${H5LZF_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5LZF_LIB_TARGET}>"
  )
endif ()

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5LZF_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5LZF_LIB_TARGET}
      EXPORT
          ${H5LZF_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5LZF_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5LZF_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5LZF_INSTALL_BIN_DIR} COMPONENT libraries
  )
endif ()
