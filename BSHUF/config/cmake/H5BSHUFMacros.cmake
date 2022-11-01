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
macro (EXTERNAL_BSHUF_LIBRARY compress_type libtype)
  if (${libtype} MATCHES "SHARED")
    set (BUILD_EXT_SHARED_LIBS "ON")
  else ()
    set (BUILD_EXT_SHARED_LIBS "OFF")
  endif ()
  if (${compress_type} MATCHES "GIT")
    EXTERNALPROJECT_ADD (BSHUF
        GIT_REPOSITORY ${BSHUF_URL}
        GIT_TAG ${BSHUF_BRANCH}
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_EXT_SHARED_LIBS}
            -DBSHUF_PACKAGE_EXT:STRING=${BSHUF_PACKAGE_EXT}
            -DBSHUF_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_PDB_OUTPUT_DIRECTORY:PATH=${CMAKE_PDB_OUTPUT_DIRECTORY}
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DH5PL_USE_GNU_DIRS:STRING=${H5PL_USE_GNU_DIRS}
            -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    )
  elseif (${compress_type} MATCHES "TGZ")
    EXTERNALPROJECT_ADD (BSHUF
        URL ${BSHUF_URL}
        URL_MD5 ""
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_EXT_SHARED_LIBS}
            -DBSHUF_PACKAGE_EXT:STRING=${BSHUF_PACKAGE_EXT}
            -DBSHUF_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_PDB_OUTPUT_DIRECTORY:PATH=${CMAKE_PDB_OUTPUT_DIRECTORY}
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DH5PL_USE_GNU_DIRS:STRING=${H5PL_USE_GNU_DIRS}
            -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    )
  endif ()
  externalproject_get_property (BSHUF BINARY_DIR SOURCE_DIR)

  # Create imported target BSHUF
  add_library (bshuf ${libtype} IMPORTED)
  HDF_IMPORT_SET_LIB_OPTIONS (bshuf "bshuf" ${libtype} "")
  add_dependencies (bshuf BSHUF)

#  include (${BINARY_DIR}/BSHUF-targets.cmake)
  set (BSHUF_LIBRARY "bshuf")

  set (BSHUF_INCLUDE_DIR_GEN "${BINARY_DIR}")
  set (BSHUF_INCLUDE_DIR "${SOURCE_DIR}")
  set (BSHUF_FOUND 1)
  set (BSHUF_LIBRARIES ${BSHUF_LIBRARY})
  set (BSHUF_INCLUDE_DIRS ${BSHUF_INCLUDE_DIR_GEN} ${BSHUF_INCLUDE_DIR})
endmacro ()

#-------------------------------------------------------------------------------
macro (PACKAGE_BSHUF_LIBRARY compress_type)
  add_custom_target (BSHUF-GenHeader-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BSHUF_INCLUDE_DIR}/bshuf.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
      COMMENT "Copying ${BSHUF_INCLUDE_DIR}/bshuf.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
  )
  set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bshuf.h)
  if (${compress_type} MATCHES "GIT" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (BSHUF-GenHeader-Copy BSHUF)
  endif ()
endmacro ()

#-------------------------------------------------------------------------------
macro (EXTERNAL_LZ4_LIBRARY compress_type libtype)
  if (${libtype} MATCHES "SHARED")
    set (BUILD_EXT_SHARED_LIBS "ON")
  else ()
    set (BUILD_EXT_SHARED_LIBS "OFF")
  endif ()
  if (${compress_type} MATCHES "GIT")
    EXTERNALPROJECT_ADD (LZ4
        GIT_REPOSITORY ${LZ4_URL}
        GIT_TAG ${LZ4_BRANCH}
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_EXT_SHARED_LIBS}
            -DLZ4_PACKAGE_EXT:STRING=${LZ4_PACKAGE_EXT}
            -DLZ4_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_PDB_OUTPUT_DIRECTORY:PATH=${CMAKE_PDB_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DH5PL_USE_GNU_DIRS:STRING=${H5PL_USE_GNU_DIRS}
            -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    )
  elseif (${compress_type} MATCHES "TGZ")
    EXTERNALPROJECT_ADD (LZ4
        URL ${LZ4_URL}
        URL_MD5 ""
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_EXT_SHARED_LIBS}
            -DLZ4_PACKAGE_EXT:STRING=${LZ4_PACKAGE_EXT}
            -DLZ4_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_PDB_OUTPUT_DIRECTORY:PATH=${CMAKE_PDB_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DH5PL_USE_GNU_DIRS:STRING=${H5PL_USE_GNU_DIRS}
            -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    )
  endif ()
  externalproject_get_property (LZ4 BINARY_DIR SOURCE_DIR)

  # Create imported target LZ4
  add_library (lz4 ${libtype} IMPORTED)
  HDF_IMPORT_SET_LIB_OPTIONS (lz4 "lz4" ${libtype} "")
  add_dependencies (lz4 LZ4)

#  include (${BINARY_DIR}/LZ4-targets.cmake)
  set (LZ4_LIBRARY "lz4")

  set (LZ4_INCLUDE_DIR_GEN "${BINARY_DIR}")
  set (LZ4_INCLUDE_DIR "${SOURCE_DIR}/LZ4")
  set (LZ4_FOUND 1)
  set (LZ4_LIBRARIES ${LZ4_LIBRARY})
  set (LZ4_INCLUDE_DIRS ${LZ4_INCLUDE_DIR_GEN} ${LZ4_INCLUDE_DIR})
endmacro ()

#-------------------------------------------------------------------------------
macro (PACKAGE_LZ4_LIBRARY compress_type)
  add_custom_target (LZ4-GenHeader-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LZ4_INCLUDE_DIR}/lz4.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
      COMMENT "Copying ${LZ4_INCLUDE_DIR}/lz4.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
  )
  set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lz4.h)
  if (${compress_type} MATCHES "GIT" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (LZ4-GenHeader-Copy LZ4)
  endif ()
endmacro ()
