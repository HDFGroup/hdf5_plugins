#
# Copyright by The HDF Group.
# All rights reserved.
#
# This file is part of HDF5.  The full HDF5 copyright notice, including
# terms governing use, modification, and redistribution, is contained in
# the COPYING file, which can be found at the root of the source code
# distribution tree, or in https://www.hdfgroup.org/licenses.
# If you do not have access to either file, you may request a copy from
# help@hdfgroup.org.
#
#-------------------------------------------------------------------------------
macro (EXTERNAL_BLOSC_LIBRARY compress_type libtype)
  if (${libtype} MATCHES "SHARED")
    set (BUILD_EXT_SHARED_LIBS "ON")
  else ()
    set (BUILD_EXT_SHARED_LIBS "OFF")
  endif ()
  if (${compress_type} MATCHES "GIT")
    EXTERNALPROJECT_ADD (BLOSC
        GIT_REPOSITORY ${BLOSC_URL}
        GIT_TAG ${BLOSC_BRANCH}
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_TESTS:BOOL=OFF
            -DBUILD_BENCHMARKS:BOOL=OFF
            -DPREFER_EXTERNAL_SNAPPY:BOOL=OFF
            -DPREFER_EXTERNAL_ZLIB:BOOL=OFF
            -DPREFER_EXTERNAL_ZSTD:BOOL=OFF
            -DDEACTIVATE_SNAPPY:BOOL=ON
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_PDB_OUTPUT_DIRECTORY:PATH=${CMAKE_PDB_OUTPUT_DIRECTORY}
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    )
  elseif (${compress_type} MATCHES "TGZ")
    EXTERNALPROJECT_ADD (BLOSC
        URL ${BLOSC_URL}
        URL_MD5 ""
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_TESTS:BOOL=OFF
            -DBUILD_BENCHMARKS:BOOL=OFF
            -DPREFER_EXTERNAL_SNAPPY:BOOL=OFF
            -DPREFER_EXTERNAL_ZLIB:BOOL=OFF
            -DPREFER_EXTERNAL_ZSTD:BOOL=OFF
            -DDEACTIVATE_SNAPPY:BOOL=ON
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_PDB_OUTPUT_DIRECTORY:PATH=${CMAKE_PDB_OUTPUT_DIRECTORY}
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    )
  endif ()
  externalproject_get_property (BLOSC BINARY_DIR SOURCE_DIR)

##include (${BINARY_DIR}/${BLOSC_PACKAGE_NAME}${H5BLOSC_PACKAGE_EXT}-targets.cmake)
# Create imported target blosc_static
  add_library(blosc ${libtype} IMPORTED)
  HDF_IMPORT_SET_LIB_OPTIONS (blosc "blosc" ${libtype} "")
  add_dependencies (blosc BLOSC)
  set (BLOSC_LIBRARY "blosc")

  set (BLOSC_INCLUDE_DIR_GEN "${BINARY_DIR}")
  set (BLOSC_INCLUDE_DIR "${SOURCE_DIR}/blosc")
  set (BLOSC_FOUND 1)
  set (BLOSC_LIBRARIES ${BLOSC_LIBRARY})
  set (BLOSC_INCLUDE_DIRS ${BLOSC_INCLUDE_DIR_GEN} ${BLOSC_INCLUDE_DIR})
endmacro ()

#-------------------------------------------------------------------------------
macro (PACKAGE_BLOSC_LIBRARY compress_type)
  add_custom_target (BLOSC-GenHeader-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BLOSC_INCLUDE_DIR}/blosc.h ${BLOSC_INCLUDE_DIR}/blosc-export.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
      COMMENT "Copying ${BLOSC_INCLUDE_DIR_}/blosc.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
  )
  set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/blosc.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/blosc-export.h)
  if (${compress_type} MATCHES "GIT" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (BLOSC-GenHeader-Copy BLOSC)
  endif ()
endmacro ()

#-------------------------------------------------------------------------------
macro (EXTERNAL_BLOSC_ZLIB_LIBRARY compress_type libtype)
  if (${libtype} MATCHES "SHARED")
    set (BUILD_EXT_SHARED_LIBS "ON")
  else ()
    set (BUILD_EXT_SHARED_LIBS "OFF")
  endif ()
  if (${compress_type} MATCHES "GIT")
    EXTERNALPROJECT_ADD (BLOSC_ZLIB
        GIT_REPOSITORY ${BLOSC_ZLIB_URL}
        GIT_TAG ${BLOSC_ZLIB_BRANCH}
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_EXT_SHARED_LIBS}
            -DZLIB_PACKAGE_EXT:STRING=${H5BLOSC_PACKAGE_EXT}
            -DZLIB_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_PDB_OUTPUT_DIRECTORY:PATH=${CMAKE_PDB_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    )
  elseif (${compress_type} MATCHES "TGZ")
    EXTERNALPROJECT_ADD (BLOSC_ZLIB
        URL ${BLOSC_ZLIB_URL}
        URL_MD5 ""
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_EXT_SHARED_LIBS}
            -DZLIB_PACKAGE_EXT:STRING=${H5BLOSC_PACKAGE_EXT}
            -DZLIB_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_PDB_OUTPUT_DIRECTORY:PATH=${CMAKE_PDB_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    )
  endif ()
  externalproject_get_property (BLOSC_ZLIB BINARY_DIR SOURCE_DIR)

  if (WIN32)
    set (BLOSC_ZLIB_LIB_NAME "zlib")
  else ()
    set (BLOSC_ZLIB_LIB_NAME "z")
  endif ()

# Create imported target zlib-static
  add_library(zlib ${libtype} IMPORTED)
  HDF_IMPORT_SET_LIB_OPTIONS (zlib ${BLOSC_ZLIB_LIB_NAME} ${libtype} "")
  add_dependencies (zlib BLOSC_ZLIB)
  set (BLOSC_ZLIB_STATIC_LIBRARY "zlib")

  set (BLOSC_ZLIB_INCLUDE_DIR_GEN "${BINARY_DIR}")
  set (BLOSC_ZLIB_INCLUDE_DIR "${SOURCE_DIR}")
  set (BLOSC_ZLIB_FOUND 1)
  set (BLOSC_ZLIB_LIBRARIES ${ZLIB_LIBRARY})
  set (BLOSC_ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR_GEN} ${ZLIB_INCLUDE_DIR})
endmacro ()

#-------------------------------------------------------------------------------
macro (PACKAGE_BLOSC_ZLIB_LIBRARY compress_type)
  add_custom_target (BLOSC_ZLIB-GenHeader-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BLOSC_ZLIB_INCLUDE_DIR_GEN}/zconf.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
      COMMENT "Copying ${BLOSC_ZLIB_INCLUDE_DIR_GEN}/zconf.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
  )
  set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/zconf.h)
  if (${compress_type} MATCHES "GIT" OR ${compress_type} MATCHES "SVN" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (BLOSC_ZLIB-GenHeader-Copy BLOSC_ZLIB)
  endif ()
endmacro ()
