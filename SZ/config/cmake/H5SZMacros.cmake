#-------------------------------------------------------------------------------
macro (EXTERNAL_SZ_LIBRARY compress_type libtype)
  if (${libtype} MATCHES "SHARED")
    set (BUILD_EXT_SHARED_LIBS "ON")
  else ()
    set (BUILD_EXT_SHARED_LIBS "OFF")
  endif ()
  if (${compress_type} MATCHES "GIT")
    EXTERNALPROJECT_ADD (SZF
        GIT_REPOSITORY ${SZ_URL}
        GIT_TAG ${SZ_BRANCH}
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_EXT_SHARED_LIBS}
            -DBUILD_SZ_EXAMPLES:BOOL=OFF
            -DSZ_PACKAGE_EXT:STRING=${SZ_PACKAGE_EXT}
            -DSZ_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DSZ_FIND_DEPS:BOOL=OFF
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
    EXTERNALPROJECT_ADD (SZF
        URL ${SZ_URL}
        URL_MD5 ""
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_EXT_SHARED_LIBS}
            -DBUILD_SZ_EXAMPLES:BOOL=OFF
            -DSZ_PACKAGE_EXT:STRING=${SZ_PACKAGE_EXT}
            -DSZ_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DSZ_FIND_DEPS:BOOL=OFF
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
  externalproject_get_property (SZF BINARY_DIR SOURCE_DIR)

  # Create imported target SZ
  add_library (SZ ${libtype} IMPORTED)
  HDF_IMPORT_SET_LIB_OPTIONS (SZ "SZ" ${libtype} "")
  add_dependencies (SZ SZF)

  # Create imported target zstd
  add_library(zstd STATIC IMPORTED)
  HDF_IMPORT_SET_LIB_OPTIONS (zstd "zstd" ${libtype} "")
  add_dependencies (zstd SZF)

  # Create imported target ZLIB
  add_library(ZLIB STATIC IMPORTED)
  HDF_IMPORT_SET_LIB_OPTIONS (ZLIB "ZLIB" ${libtype} "")
  add_dependencies (ZLIB SZF)

#  include (${BINARY_DIR}/SZ-targets.cmake)
  set (SZ_LIBRARY "SZ;zstd;ZLIB")

  set (SZ_INCLUDE_DIR_GEN "${BINARY_DIR}")
  set (SZ_INCLUDE_DIR "${SOURCE_DIR}/sz/include")
  set (SZ_FOUND 1)
  set (SZ_LIBRARIES ${SZ_LIBRARY})
  set (SZ_INCLUDE_DIRS ${SZ_INCLUDE_DIR_GEN} ${SZ_INCLUDE_DIR})
endmacro ()

#-------------------------------------------------------------------------------
macro (PACKAGE_SZ_LIBRARY compress_type)
  add_custom_target (SZF-GenHeader-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SZ_INCLUDE_DIR}/sz.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
      COMMENT "Copying ${SZ_INCLUDE_DIR}/sz.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
  )
  set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sz.h)
  if (${compress_type} MATCHES "GIT" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (SZF-GenHeader-Copy SZF)
  endif ()
endmacro ()
