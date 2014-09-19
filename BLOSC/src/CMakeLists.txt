cmake_minimum_required (VERSION 2.8.10)
project (H5BLOSC_SRC C CXX)

set(INTERNAL_LIBS ${H5BLOSC_SRC_SOURCE_DIR}/internal-complibs)

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${H5BLOSC_SRC_SOURCE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR})

if (BLOSC_FOUND)
    include_directories( ${BLOSC_INCLUDE_DIR} )
else(BLOSC_FOUND)
    set(BLOSC_LOCAL_DIR ${INTERNAL_LIBS}/blosc-1.4.1)
    include_directories( ${BLOSC_LOCAL_DIR} )
endif(BLOSC_FOUND)

if(NOT DEACTIVATE_LZ4)
    if (LZ4_FOUND)
        include_directories( ${LZ4_INCLUDE_DIR} )
    else(LZ4_FOUND)
        set(LZ4_LOCAL_DIR ${INTERNAL_LIBS}/lz4-r119)
        include_directories( ${LZ4_LOCAL_DIR} )
    endif(LZ4_FOUND)
endif(NOT DEACTIVATE_LZ4)

if(NOT DEACTIVATE_SNAPPY)
    if (SNAPPY_FOUND)
        include_directories( ${SNAPPY_INCLUDE_DIR} )
    else(SNAPPY_FOUND)
        set(SNAPPY_LOCAL_DIR ${INTERNAL_LIBS}/snappy-1.1.1)
        include_directories( ${SNAPPY_LOCAL_DIR} )
    endif(SNAPPY_FOUND)
endif(NOT DEACTIVATE_SNAPPY)

if(NOT DEACTIVATE_ZLIB)
    if (ZLIB_FOUND)
        include_directories( ${ZLIB_INCLUDE_DIR} )
    else(ZLIB_FOUND)
        set(ZLIB_LOCAL_DIR ${INTERNAL_LIBS}/zlib-1.2.8)
        include_directories( ${ZLIB_LOCAL_DIR} )
    endif(ZLIB_FOUND)
endif(NOT DEACTIVATE_ZLIB)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Define H5BLOSC Library
#-----------------------------------------------------------------------------
set (H5BLOSC_SRCS
    ${H5BLOSC_SRC_SOURCE_DIR}/H5Zblosc.c
)

if(WIN32)
  if(NOT Threads_FOUND)
    message(STATUS "using the internal pthread library for win32 systems.")
    set(H5BLOSC_SRCS ${H5BLOSC_SRCS} win32/pthread.c)
  endif(NOT Threads_FOUND)
endif(WIN32)

if(NOT BLOSC_FOUND)
    set(BLOSC_FILES 
        ${BLOSC_LOCAL_DIR}/blosc.c 
        ${BLOSC_LOCAL_DIR}/blosclz.c 
        ${BLOSC_LOCAL_DIR}/shuffle.c
    )
    set(H5BLOSC_SRCS ${H5BLOSC_SRCS} ${BLOSC_FILES})
endif(NOT BLOSC_FOUND)

if(NOT DEACTIVATE_LZ4)
    if(NOT LZ4_FOUND)
        set(LZ4_FILES
            ${LZ4_LOCAL_DIR}/lz4.c
            ${LZ4_LOCAL_DIR}/lz4hc.c
        )
        set(H5BLOSC_SRCS ${H5BLOSC_SRCS} ${LZ4_FILES})
    endif(NOT LZ4_FOUND)
endif(NOT DEACTIVATE_LZ4)

if(NOT DEACTIVATE_SNAPPY)
    if(NOT SNAPPY_FOUND)
        set(SNAPPY_FILES
            ${SNAPPY_LOCAL_DIR}/snappy.cc
            ${SNAPPY_LOCAL_DIR}/snappy-c.cc
            ${SNAPPY_LOCAL_DIR}/snappy-sinksource.cc
            ${SNAPPY_LOCAL_DIR}/snappy-stubs-internal.cc
        )
        set(H5BLOSC_SRCS ${H5BLOSC_SRCS} ${SNAPPY_FILES})
    endif(NOT SNAPPY_FOUND)
endif(NOT DEACTIVATE_SNAPPY)

if(NOT DEACTIVATE_ZLIB)
    if(NOT ZLIB_FOUND)
        set(ZLIB_FILES
            ${ZLIB_LOCAL_DIR}/adler32.c
            ${ZLIB_LOCAL_DIR}/crc32.c
            ${ZLIB_LOCAL_DIR}/deflate.c
            ${ZLIB_LOCAL_DIR}/gzclose.c
            ${ZLIB_LOCAL_DIR}/gzlib.c
            ${ZLIB_LOCAL_DIR}/gzread.c
            ${ZLIB_LOCAL_DIR}/gzwrite.c
            ${ZLIB_LOCAL_DIR}/infback.c
            ${ZLIB_LOCAL_DIR}/inffast.c
            ${ZLIB_LOCAL_DIR}/inflate.c
            ${ZLIB_LOCAL_DIR}/inftrees.c
            ${ZLIB_LOCAL_DIR}/trees.c
            ${ZLIB_LOCAL_DIR}/uncompr.c
            ${ZLIB_LOCAL_DIR}/zutil.c
        )
        set(H5BLOSC_SRCS ${H5BLOSC_SRCS} ${ZLIB_FILES})
    endif(NOT ZLIB_FOUND)
endif(NOT DEACTIVATE_ZLIB)

add_library (${H5BLOSC_LIB_TARGET} ${LIB_TYPE} ${H5BLOSC_SRCS})
target_link_libraries (${H5BLOSC_LIB_TARGET} ${LINK_LIBS})
#if (NOT USE_SHARED_LIBS)
#  target_link_libraries (${H5BLOSC_LIB_TARGET} ${HDF5_LIBRARIES})
#endif (NOT USE_SHARED_LIBS)
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
 