#-------------------------------------------------------------------------------
MACRO (H5BZ2_SET_LIB_OPTIONS libtarget libname libtype)
  # message (STATUS "${libname} libtype: ${libtype}")
  HDF_SET_LIB_OPTIONS (${libtarget} ${libname} ${libtype})

  IF (${libtype} MATCHES "SHARED")
    IF (WIN32)
      SET (LIBH5BZ2_VERSION ${H5BZ2_PACKAGE_VERSION_MAJOR})
    ELSE (WIN32)
      SET (LIBH5BZ2_VERSION ${H5BZ2_PACKAGE_VERSION})
    ENDIF (WIN32)

    IF (NOT CYGWIN)
      # This property causes shared libraries on Linux to have the full version
      # encoded into their final filename.  We disable this on Cygwin because
      # it causes cygh5bz2-${H5BZ2_FULL_VERSION}.dll to be created when cygh5bz2.dll
      # seems to be the default.
      SET_TARGET_PROPERTIES (${libtarget} PROPERTIES VERSION ${LIBH5BZ2_VERSION})
    ENDIF (NOT CYGWIN)
    SET_TARGET_PROPERTIES (${libtarget} PROPERTIES SOVERSION ${LIBH5BZ2_VERSION})
  ENDIF (${libtype} MATCHES "SHARED")

  #-- Apple Specific install_name for libraries
  IF (APPLE)
    OPTION (H5BZ2_BUILD_WITH_INSTALL_NAME "Build with library install_name set to the installation path" OFF)
    IF (H5BZ2_BUILD_WITH_INSTALL_NAME)
      SET_TARGET_PROPERTIES(${libtarget} PROPERTIES
          LINK_FLAGS "-current_version ${H5BZ2_PACKAGE_VERSION} -compatibility_version ${H5BZ2_PACKAGE_VERSION}"
          INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
          BUILD_WITH_INSTALL_RPATH ${H5BZ2_BUILD_WITH_INSTALL_NAME}
      )
    ENDIF (H5BZ2_BUILD_WITH_INSTALL_NAME)
  ENDIF (APPLE)

ENDMACRO (H5BZ2_SET_LIB_OPTIONS)

