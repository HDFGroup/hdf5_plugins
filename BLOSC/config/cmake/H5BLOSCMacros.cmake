#-------------------------------------------------------------------------------
MACRO (H5BLOSC_SET_LIB_OPTIONS libtarget libname libtype)
  set (LIB_OUT_NAME "${libname}")
  if (${libtype} MATCHES "SHARED")
    if (WIN32)
      set (LIBH5BLOSC_VERSION ${H5BLOSC_PACKAGE_VERSION_MAJOR})
    else (WIN32)
      set (LIBH5BLOSC_VERSION ${H5BLOSC_PACKAGE_VERSION})
    endif (WIN32)
    set_target_properties (${libtarget} PROPERTIES VERSION ${LIBH5BLOSC_VERSION})
    if (WIN32)
        set (${LIB_OUT_NAME} "${LIB_OUT_NAME}-${H5BLOSC_PACKAGE_SOVERSION}")
    else (WIN32)
        set_target_properties (${libtarget} PROPERTIES SOVERSION ${H5BLOSC_PACKAGE_SOVERSION})
    endif (WIN32)
  endif (${libtype} MATCHES "SHARED")
  HDF_SET_LIB_OPTIONS (${libtarget} ${LIB_OUT_NAME} ${libtype})

  #-- Apple Specific install_name for libraries
  if (APPLE)
    option (H5BLOSC_BUILD_WITH_INSTALL_NAME "Build with library install_name set to the installation path" OFF)
    if (H5BLOSC_BUILD_WITH_INSTALL_NAME)
      set_target_properties(${libtarget} PROPERTIES
          LINK_FLAGS "-current_version ${H5BLOSC_PACKAGE_VERSION} -compatibility_version ${H5BLOSC_PACKAGE_VERSION}"
          INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
          BUILD_WITH_INSTALL_RPATH ${H5BLOSC_BUILD_WITH_INSTALL_NAME}
      )
    endif (H5BLOSC_BUILD_WITH_INSTALL_NAME)
  endif (APPLE)

ENDMACRO (H5BLOSC_SET_LIB_OPTIONS)

#-------------------------------------------------------------------------------
MACRO (EXTERNAL_BLOSC_LIBRARY compress_type libtype)
  if (${compress_type} MATCHES "SVN")
    EXTERNALPROJECT_ADD (BLOSC
        SVN_REPOSITORY ${BLOSC_URL}
        # [SVN_REVISION rev] 
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
            -DBLOSC_PACKAGE_EXT:STRING=${BLOSC_PACKAGE_EXT}
            -DBLOSC_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
    ) 
  elseif (${compress_type} MATCHES "GIT")
    EXTERNALPROJECT_ADD (BLOSC
        GIT_REPOSITORY ${BLOSC_URL}
        # [SVN_REVISION rev] 
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
            -DBLOSC_PACKAGE_EXT:STRING=${BLOSC_PACKAGE_EXT}
            -DBLOSC_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
    ) 
  elseif (${compress_type} MATCHES "TGZ")
    EXTERNALPROJECT_ADD (BLOSC
        URL ${BLOSC_URL}
        URL_MD5 ""
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
            -DBLOSC_PACKAGE_EXT:STRING=${BLOSC_PACKAGE_EXT}
            -DBLOSC_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
    ) 
  endif (${compress_type} MATCHES "SVN")
  externalproject_get_property (BLOSC BINARY_DIR SOURCE_DIR) 

  # Create imported target BLOSC
  add_library (blosc ${libtype} IMPORTED)
  HDF_IMPORT_SET_LIB_OPTIONS (blosc "blosc" ${libtype} "")
  add_dependencies (BLOSC blosc)

#  include (${BINARY_DIR}/BLOSC-targets.cmake)  
  set (BLOSC_LIBRARY "blosc")

  set (BLOSC_INCLUDE_DIR_GEN "${BINARY_DIR}")
  set (BLOSC_INCLUDE_DIR "${SOURCE_DIR}/blosc")
  set (BLOSC_FOUND 1)
  set (BLOSC_LIBRARIES ${BLOSC_LIBRARY})
  set (BLOSC_INCLUDE_DIRS ${BLOSC_INCLUDE_DIR_GEN} ${BLOSC_INCLUDE_DIR})
ENDMACRO (EXTERNAL_BLOSC_LIBRARY)

#-------------------------------------------------------------------------------
MACRO (PACKAGE_BLOSC_LIBRARY compress_type)
  add_custom_target (BLOSC-GenHeader-Copy ALL
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BLOSC_INCLUDE_DIR}/blosc.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
      COMMENT "Copying ${BLOSC_INCLUDE_DIR}/blosc.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
  )
  set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/blosc.h)
  if (${compress_type} MATCHES "SVN" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (BLOSC-GenHeader-Copy blosc)
  endif (${compress_type} MATCHES "SVN" OR ${compress_type} MATCHES "TGZ")
ENDMACRO (PACKAGE_BLOSC_LIBRARY)
