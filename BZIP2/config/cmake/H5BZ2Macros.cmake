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
macro (EXTERNAL_BZ2_LIBRARY compress_type libtype)
  if (${libtype} MATCHES "SHARED")
    set (BUILD_EXT_SHARED_LIBS "ON")
  else ()
    set (BUILD_EXT_SHARED_LIBS "OFF")
  endif ()
  if (${compress_type} MATCHES "GIT")
    EXTERNALPROJECT_ADD (BZ2
        GIT_REPOSITORY ${BZ2_URL}
        GIT_TAG ${BZ2_BRANCH}
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_EXT_SHARED_LIBS}
            -DBZ2_PACKAGE_EXT:STRING=${BZ2_PACKAGE_EXT}
            -DBZ2_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_PDB_OUTPUT_DIRECTORY:PATH=${CMAKE_PDB_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    )
  elseif (${compress_type} MATCHES "TGZ")
    EXTERNALPROJECT_ADD (BZ2
        URL ${BZ2_URL}
        URL_MD5 ""
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_EXT_SHARED_LIBS}
            -DBZ2_PACKAGE_EXT:STRING=${BZ2_PACKAGE_EXT}
            -DBZ2_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_PDB_OUTPUT_DIRECTORY:PATH=${CMAKE_PDB_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    )
  endif ()
  externalproject_get_property (BZ2 BINARY_DIR SOURCE_DIR)

  # Create imported target BZ2
  add_library (bz2 ${libtype} IMPORTED)
  HDF_IMPORT_SET_LIB_OPTIONS (bz2 "bz2" ${libtype} "")
  add_dependencies (bz2 BZ2)

#  include (${BINARY_DIR}/BZ2-targets.cmake)
  set (BZ2_LIBRARY "bz2")

  set (BZ2_INCLUDE_DIR_GEN "${BINARY_DIR}")
  set (BZ2_INCLUDE_DIR "${SOURCE_DIR}/src")
  set (BZ2_FOUND 1)
  set (BZ2_LIBRARIES ${BZ2_LIBRARY})
  set (BZ2_INCLUDE_DIRS ${BZ2_INCLUDE_DIR_GEN} ${BZ2_INCLUDE_DIR})
endmacro ()

#-------------------------------------------------------------------------------
macro (PACKAGE_BZ2_LIBRARY compress_type)
  add_custom_target (BZ2-GenHeader-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BZ2_INCLUDE_DIR}/bzlib.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
      COMMENT "Copying ${BZ2_INCLUDE_DIR}/bzlib.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
  )
  set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bzlib.h)
  if (${compress_type} MATCHES "GIT" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (BZ2-GenHeader-Copy bz2)
  endif ()
endmacro ()
