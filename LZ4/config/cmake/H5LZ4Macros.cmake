#-------------------------------------------------------------------------------
MACRO (H5LZ4_SET_LIB_OPTIONS libtarget libname libtype)
  set (LIB_OUT_NAME "${libname}")
  if (${libtype} MATCHES "SHARED")
    if (WIN32)
      set (LIBH5LZ4_VERSION ${H5LZ4_PACKAGE_VERSION_MAJOR})
    else (WIN32)
      set (LIBH5LZ4_VERSION ${H5LZ4_PACKAGE_VERSION})
    endif (WIN32)
    set_target_properties (${libtarget} PROPERTIES VERSION ${LIBH5LZ4_VERSION})
    if (WIN32)
        set (${LIB_OUT_NAME} "${LIB_OUT_NAME}-${H5LZ4_PACKAGE_SOVERSION}")
    else (WIN32)
        set_target_properties (${libtarget} PROPERTIES SOVERSION ${H5LZ4_PACKAGE_SOVERSION})
    endif (WIN32)
  endif (${libtype} MATCHES "SHARED")
  HDF_SET_LIB_OPTIONS (${libtarget} ${LIB_OUT_NAME} ${libtype})

  #-- Apple Specific install_name for libraries
  if (APPLE)
    option (H5LZ4_BUILD_WITH_INSTALL_NAME "Build with library install_name set to the installation path" OFF)
    if (H5LZ4_BUILD_WITH_INSTALL_NAME)
      set_target_properties(${libtarget} PROPERTIES
          LINK_FLAGS "-current_version ${H5LZ4_PACKAGE_VERSION} -compatibility_version ${H5LZ4_PACKAGE_VERSION}"
          INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
          BUILD_WITH_INSTALL_RPATH ${H5LZ4_BUILD_WITH_INSTALL_NAME}
      )
    endif (H5LZ4_BUILD_WITH_INSTALL_NAME)
  endif (APPLE)

ENDMACRO (H5LZ4_SET_LIB_OPTIONS)

#-------------------------------------------------------------------------------
MACRO (EXTERNAL_LZ4_LIBRARY compress_type libtype)
  if (${libtype} MATCHES "SHARED")
    set (BUILD_EXT_SHARED_LIBS "ON")
  else (${libtype} MATCHES "SHARED")
    set (BUILD_EXT_SHARED_LIBS "OFF")
  endif (${libtype} MATCHES "SHARED")
  if (${compress_type} MATCHES "SVN")
    EXTERNALPROJECT_ADD (LZ4
        SVN_REPOSITORY ${LZ4_URL}
        # [SVN_REVISION rev] 
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
            -DLZ4_PACKAGE_EXT:STRING=${LZ4_PACKAGE_EXT}
            -DLZ4_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
    ) 
  elseif (${compress_type} MATCHES "GIT")
    EXTERNALPROJECT_ADD (LZ4
        GIT_REPOSITORY ${LZ4_URL}
        # [SVN_REVISION rev] 
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
            -DLZ4_PACKAGE_EXT:STRING=${LZ4_PACKAGE_EXT}
            -DLZ4_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
    ) 
  elseif (${compress_type} MATCHES "TGZ")
    EXTERNALPROJECT_ADD (LZ4
        URL ${LZ4_URL}
        URL_MD5 ""
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
            -DLZ4_PACKAGE_EXT:STRING=${LZ4_PACKAGE_EXT}
            -DLZ4_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
    ) 
  endif (${compress_type} MATCHES "SVN")
  externalproject_get_property (LZ4 BINARY_DIR SOURCE_DIR) 

  # Create imported target LZ4
  add_library (lz4 ${libtype} IMPORTED)
  HDF_IMPORT_SET_LIB_OPTIONS (lz4 "lz4" ${libtype} "")
  add_dependencies (LZ4 lz4)

#  include (${BINARY_DIR}/LZ4-targets.cmake)  
  set (LZ4_LIBRARY "lz4")

  set (LZ4_INCLUDE_DIR_GEN "${BINARY_DIR}")
  set (LZ4_INCLUDE_DIR "${SOURCE_DIR}/LZ4")
  set (LZ4_FOUND 1)
  set (LZ4_LIBRARIES ${LZ4_LIBRARY})
  set (LZ4_INCLUDE_DIRS ${LZ4_INCLUDE_DIR_GEN} ${LZ4_INCLUDE_DIR})
ENDMACRO (EXTERNAL_LZ4_LIBRARY)

#-------------------------------------------------------------------------------
MACRO (PACKAGE_LZ4_LIBRARY compress_type)
  if (WIN32)
    add_custom_target (LZ4-GenHeader-Copy ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LZ4_INCLUDE_DIR}/lz4.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
        COMMENT "Copying ${LZ4_INCLUDE_DIR}/lz4.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
    )
    set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lz4.h)
    if (${compress_type} MATCHES "SVN" OR ${compress_type} MATCHES "TGZ")
      add_dependencies (LZ4-GenHeader-Copy LZ4)
    endif (${compress_type} MATCHES "SVN" OR ${compress_type} MATCHES "TGZ")
  endif (WIN32)
ENDMACRO (PACKAGE_LZ4_LIBRARY)
