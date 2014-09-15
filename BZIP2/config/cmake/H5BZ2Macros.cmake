#-------------------------------------------------------------------------------
MACRO (H5BZ2_SET_LIB_OPTIONS libtarget libname libtype)
  set (LIB_OUT_NAME "${libname}")
  if (${libtype} MATCHES "SHARED")
    if (WIN32)
      set (LIBH5BZ2_VERSION ${H5BZ2_PACKAGE_VERSION_MAJOR})
    else (WIN32)
      set (LIBH5BZ2_VERSION ${H5BZ2_PACKAGE_VERSION})
    endif (WIN32)
    set_target_properties (${libtarget} PROPERTIES VERSION ${LIBH5BZ2_VERSION})
    if (WIN32)
        set (${LIB_OUT_NAME} "${LIB_OUT_NAME}-${H5BZ2_PACKAGE_SOVERSION}")
    else (WIN32)
        set_target_properties (${libtarget} PROPERTIES SOVERSION ${H5BZ2_PACKAGE_SOVERSION})
    endif (WIN32)
  endif (${libtype} MATCHES "SHARED")
  HDF_SET_LIB_OPTIONS (${libtarget} ${LIB_OUT_NAME} ${libtype})

  #-- Apple Specific install_name for libraries
  if (APPLE)
    option (H5BZ2_BUILD_WITH_INSTALL_NAME "Build with library install_name set to the installation path" OFF)
    if (H5BZ2_BUILD_WITH_INSTALL_NAME)
      set_target_properties(${libtarget} PROPERTIES
          LINK_FLAGS "-current_version ${H5BZ2_PACKAGE_VERSION} -compatibility_version ${H5BZ2_PACKAGE_VERSION}"
          INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
          BUILD_WITH_INSTALL_RPATH ${H5BZ2_BUILD_WITH_INSTALL_NAME}
      )
    endif (H5BZ2_BUILD_WITH_INSTALL_NAME)
  endif (APPLE)

ENDMACRO (H5BZ2_SET_LIB_OPTIONS)

#-------------------------------------------------------------------------------
MACRO (EXTERNAL_BZ2_LIBRARY compress_type libtype)
  if (${compress_type} MATCHES "SVN")
    EXTERNALPROJECT_ADD (BZ2
        SVN_REPOSITORY ${BZ2_URL}
        # [SVN_REVISION rev] 
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
            -DBZ2_PACKAGE_EXT:STRING=${BZ2_PACKAGE_EXT}
            -DBZ2_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
    ) 
  elseif (${compress_type} MATCHES "GIT")
    EXTERNALPROJECT_ADD (BZ2
        GIT_REPOSITORY ${BZ2_URL}
        # [SVN_REVISION rev] 
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
            -DBZ2_PACKAGE_EXT:STRING=${BZ2_PACKAGE_EXT}
            -DBZ2_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
    ) 
  elseif (${compress_type} MATCHES "TGZ")
    EXTERNALPROJECT_ADD (BZ2
        URL ${BZ2_URL}
        URL_MD5 ""
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
            -DBZ2_PACKAGE_EXT:STRING=${BZ2_PACKAGE_EXT}
            -DBZ2_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
    ) 
  endif (${compress_type} MATCHES "SVN")
  externalproject_get_property (BZ2 BINARY_DIR SOURCE_DIR) 

  # Create imported target BZ2
  add_library (bz2 ${libtype} IMPORTED)
  HDF_IMPORT_SET_LIB_OPTIONS (bz2 "bz2" ${libtype} "")
  add_dependencies (BZ2 bz2)

#  include (${BINARY_DIR}/BZ2-targets.cmake)  
  set (BZ2_LIBRARY "bz2")

  set (BZ2_INCLUDE_DIR_GEN "${BINARY_DIR}")
  set (BZ2_INCLUDE_DIR "${SOURCE_DIR}/src")
  set (BZ2_FOUND 1)
  set (BZ2_LIBRARIES ${BZ2_LIBRARY})
  set (BZ2_INCLUDE_DIRS ${BZ2_INCLUDE_DIR_GEN} ${BZ2_INCLUDE_DIR})
ENDMACRO (EXTERNAL_BZ2_LIBRARY)

#-------------------------------------------------------------------------------
MACRO (PACKAGE_BZ2_LIBRARY compress_type)
  add_custom_target (BZ2-GenHeader-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BZ2_INCLUDE_DIR}/bzlib.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
      COMMENT "Copying ${BZ2_INCLUDE_DIR}/bzlib.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
  )
  set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bzlib.h)
  if (${compress_type} MATCHES "SVN" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (BZ2-GenHeader-Copy bz2)
  endif (${compress_type} MATCHES "SVN" OR ${compress_type} MATCHES "TGZ")
ENDMACRO (PACKAGE_BZ2_LIBRARY)
