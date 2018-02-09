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
    )
  endif ()
  externalproject_get_property (BZ2 BINARY_DIR SOURCE_DIR)

  # Create imported target BZ2
  add_library (bz2 ${libtype} IMPORTED)
  H5BZ2_IMPORT_SET_LIB_OPTIONS (bz2 "bz2" ${libtype} "")
  add_dependencies (BZ2 bz2)

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

#-------------------------------------------------------------------------------
macro (H5BZ2_SET_LIB_OPTIONS libtarget defaultlibname libtype)
  set (libname "${defaultlibname}")
  HDF_SET_BASE_OPTIONS (${libtarget} ${libname} ${libtype})

  if (${libtype} MATCHES "SHARED")
    set (LIB_PACKAGE_SOVERSION ${H5BZ2_SOVERS_MAJOR})
    if (WIN32)
      set (LIB_VERSION ${H5BZ2_PACKAGE_VERSION_MAJOR})
    else ()
      set (LIB_VERSION ${H5BZ2_PACKAGE_SOVERSION})
    endif ()
    set_target_properties (${libtarget} PROPERTIES VERSION ${LIB_VERSION})
    if (WIN32)
        set (${libname} "${libname}-${LIB_PACKAGE_SOVERSION}")
    else ()
        set_target_properties (${libtarget} PROPERTIES SOVERSION ${H5BZ2_PACKAGE_SOVERSION})
    endif ()
  endif ()

  #-- Apple Specific install_name for libraries
  if (APPLE)
    option (H5BZ2_BUILD_WITH_INSTALL_NAME "Build with library install_name set to the installation path" OFF)
    if (H5BZ2_BUILD_WITH_INSTALL_NAME)
      set_target_properties(${libtarget} PROPERTIES
          LINK_FLAGS "-current_version ${H5BZ2_PACKAGE_VERSION} -compatibility_version ${H5BZ2_PACKAGE_VERSION}"
          INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
          BUILD_WITH_INSTALL_RPATH ${H5BZ2_BUILD_WITH_INSTALL_NAME}
      )
    endif ()
  endif ()
endmacro ()

#-------------------------------------------------------------------------------
macro (H5BZ2_IMPORT_SET_LIB_OPTIONS libtarget libname libtype libversion)
  HDF_SET_BASE_OPTIONS (${libtarget} ${libname} ${libtype})

  if (${importtype} MATCHES "IMPORT")
    set (importprefix "${CMAKE_STATIC_LIBRARY_PREFIX}")
  endif ()
  if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set (IMPORT_LIB_NAME ${LIB_DEBUG_NAME})
  else ()
    set (IMPORT_LIB_NAME ${LIB_RELEASE_NAME})
  endif ()

  if (${libtype} MATCHES "SHARED")
    if (WIN32)
      if (MINGW)
        set_target_properties (${libtarget} PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${IMPORT_LIB_NAME}.lib"
            IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${IMPORT_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
        )
      else ()
        set_target_properties (${libtarget} PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/${CMAKE_IMPORT_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_IMPORT_LIBRARY_SUFFIX}"
            IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/${CMAKE_IMPORT_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
        )
      endif ()
    else ()
      if (CYGWIN)
        set_target_properties (${libtarget} PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_IMPORT_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_IMPORT_LIBRARY_SUFFIX}"
            IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_IMPORT_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
        )
      else ()
        set_target_properties (${libtarget} PROPERTIES
            IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
            IMPORTED_SONAME "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}.${libversion}"
            SOVERSION "${libversion}"
        )
      endif ()
    endif ()
  else ()
    if (WIN32 AND NOT MINGW)
      set_target_properties (${libtarget} PROPERTIES
          IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/${IMPORT_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
          IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      )
    else ()
      set_target_properties (${libtarget} PROPERTIES
          IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_STATIC_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
          IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      )
    endif ()
  endif ()
endmacro ()
