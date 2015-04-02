#-------------------------------------------------------------------------------
MACRO (H5BZ2_SET_LIB_OPTIONS libtarget defaultlibname libtype)
  set (libname "${defaultlibname}")
  H5BZ2_SET_BASE_OPTIONS (${libtarget} ${libname} ${libtype})

  if (${libtype} MATCHES "SHARED")
    if (WIN32)
      set (LIBH5BZ2_VERSION ${H5BZ2_PACKAGE_VERSION_MAJOR})
    else (WIN32)
      set (LIBH5BZ2_VERSION ${H5BZ2_PACKAGE_VERSION})
    endif (WIN32)
    set_target_properties (${libtarget} PROPERTIES VERSION ${LIBH5BZ2_VERSION})
    if (WIN32)
        set (${libname} "${libname}-${H5BZ2_PACKAGE_SOVERSION}")
    else (WIN32)
        set_target_properties (${libtarget} PROPERTIES SOVERSION ${H5BZ2_PACKAGE_SOVERSION})
    endif (WIN32)
  endif (${libtype} MATCHES "SHARED")

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
MACRO (SET_GLOBAL_VARIABLE name value)
  set (${name} ${value} CACHE INTERNAL "Used to pass variables between directories" FORCE)
ENDMACRO (SET_GLOBAL_VARIABLE)

#-------------------------------------------------------------------------------
MACRO (SET_GLOBAL_VARIABLE name value)
  set (${name} ${value} CACHE INTERNAL "Used to pass variables between directories" FORCE)
ENDMACRO (SET_GLOBAL_VARIABLE)

#-------------------------------------------------------------------------------
MACRO (IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)
  #set(source_group_path "Source/AIM/${NAME}")
  string (REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH})
  source_group (${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property (SOURCE ${HEADERS}
  #       PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)
ENDMACRO (IDE_GENERATED_PROPERTIES)

#-------------------------------------------------------------------------------
MACRO (IDE_SOURCE_PROPERTIES SOURCE_PATH HEADERS SOURCES)
  #  install (FILES ${HEADERS}
  #       DESTINATION include/R3D/${NAME}
  #       COMPONENT Headers       
  #  )

  string (REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH}  )
  source_group (${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property (SOURCE ${HEADERS}
  #       PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)
ENDMACRO (IDE_SOURCE_PROPERTIES)

#-------------------------------------------------------------------------------
MACRO (TARGET_NAMING libtarget libtype)
  if (WIN32)
    if (${libtype} MATCHES "SHARED")
      set_target_properties (${libtarget} PROPERTIES OUTPUT_NAME "${libtarget}dll")
    endif (${libtype} MATCHES "SHARED")
  else (WIN32)
    if (${libtype} MATCHES "SHARED")
      set_target_properties (${libtarget} PROPERTIES OUTPUT_NAME "${libtarget}${ARGN}")
    endif (${libtype} MATCHES "SHARED")
  endif (WIN32)
ENDMACRO (TARGET_NAMING)

#-------------------------------------------------------------------------------
MACRO (INSTALL_TARGET_PDB libtarget targetdestination targetcomponent)
  if (WIN32 AND MSVC)
    get_target_property (target_name ${libtarget} OUTPUT_NAME_RELWITHDEBINFO)
    install (
      FILES
          ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/${CMAKE_IMPORT_LIBRARY_PREFIX}${target_name}.pdb
      DESTINATION
          ${targetdestination}
      CONFIGURATIONS RelWithDebInfo
      COMPONENT ${targetcomponent}
  )
  endif (WIN32 AND MSVC)
ENDMACRO (INSTALL_TARGET_PDB)

#-------------------------------------------------------------------------------
MACRO (INSTALL_PROGRAM_PDB progtarget targetdestination targetcomponent)
  if (WIN32 AND MSVC)
    get_target_property (target_name ${progtarget} OUTPUT_NAME_RELWITHDEBINFO)
    get_target_property (target_prefix ${progtarget} PREFIX)
    install (
      FILES
          ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/${target_prefix}${target_name}.pdb
      DESTINATION
          ${targetdestination}
      CONFIGURATIONS RelWithDebInfo
      COMPONENT ${targetcomponent}
  )
  endif (WIN32 AND MSVC)
ENDMACRO (INSTALL_PROGRAM_PDB)

#-------------------------------------------------------------------------------
MACRO (H5BZ2_SET_BASE_OPTIONS libtarget libname libtype)
  # message (STATUS "${libname} libtype: ${libtype}")
  if (${libtype} MATCHES "SHARED")
    if (WIN32)
      set (LIB_RELEASE_NAME "${libname}")
      set (LIB_DEBUG_NAME "${libname}_D")
    else (WIN32)
      set (LIB_RELEASE_NAME "${libname}")
      set (LIB_DEBUG_NAME "${libname}_debug")
    endif (WIN32)
  else (${libtype} MATCHES "SHARED")
    if (WIN32)
      set (LIB_RELEASE_NAME "lib${libname}")
      set (LIB_DEBUG_NAME "lib${libname}_D")
    else (WIN32)
      # if the generator supports configuration types or if the CMAKE_BUILD_TYPE has a value
      if (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
        set (LIB_RELEASE_NAME "${libname}")
        set (LIB_DEBUG_NAME "${libname}_debug")
      else (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
        set (LIB_RELEASE_NAME "lib${libname}")
        set (LIB_DEBUG_NAME "lib${libname}_debug")
      endif (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
    endif (WIN32)
  endif (${libtype} MATCHES "SHARED")
  
  set_target_properties (${libtarget}
      PROPERTIES
      OUTPUT_NAME_DEBUG          ${LIB_DEBUG_NAME}
      OUTPUT_NAME_RELEASE        ${LIB_RELEASE_NAME}
      OUTPUT_NAME_MINSIZEREL     ${LIB_RELEASE_NAME}
      OUTPUT_NAME_RELWITHDEBINFO ${LIB_RELEASE_NAME}
  )
  
  #----- Use MSVC Naming conventions for Shared Libraries
  if (MINGW AND ${libtype} MATCHES "SHARED")
    set_target_properties (${libtarget}
        PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
    )
  endif (MINGW AND ${libtype} MATCHES "SHARED")

ENDMACRO (H5BZ2_SET_BASE_OPTIONS)

#-------------------------------------------------------------------------------
MACRO (H5BZ2_IMPORT_SET_LIB_OPTIONS libtarget libname libtype libversion)
  H5BZ2_SET_BASE_OPTIONS (${libtarget} ${libname} ${libtype})

  if (${importtype} MATCHES "IMPORT")
    set (importprefix "${CMAKE_STATIC_LIBRARY_PREFIX}")
  endif (${importtype} MATCHES "IMPORT")
  if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set (IMPORT_LIB_NAME ${LIB_DEBUG_NAME})
  else (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set (IMPORT_LIB_NAME ${LIB_RELEASE_NAME})
  endif (${CMAKE_BUILD_TYPE} MATCHES "Debug")

  if (${libtype} MATCHES "SHARED")
    if (WIN32)
      if (MINGW)
        set_target_properties (${libtarget} PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${IMPORT_LIB_NAME}.lib"
            IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${IMPORT_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
        )
      else (MINGW)
        set_target_properties (${libtarget} PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/${CMAKE_IMPORT_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_IMPORT_LIBRARY_SUFFIX}"
            IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/${CMAKE_IMPORT_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
        )
      endif (MINGW)
    else (WIN32)
      if (CYGWIN)
        set_target_properties (${libtarget} PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_IMPORT_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_IMPORT_LIBRARY_SUFFIX}"
            IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_IMPORT_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
        )
      else (CYGWIN)
        set_target_properties (${libtarget} PROPERTIES
            IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
            IMPORTED_SONAME "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}.${libversion}"
            SOVERSION "${libversion}"
        )
      endif (CYGWIN)
    endif (WIN32)
  else (${libtype} MATCHES "SHARED")
    if (WIN32 AND NOT MINGW)
      set_target_properties (${libtarget} PROPERTIES
          IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/${IMPORT_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
          IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      )
    else (WIN32 AND NOT MINGW)
      set_target_properties (${libtarget} PROPERTIES
          IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_STATIC_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
          IMPORTED_LINK_INTERFACE_LANGUAGES "C"
      )
    endif (WIN32 AND NOT MINGW)
  endif (${libtype} MATCHES "SHARED")

ENDMACRO (H5BZ2_IMPORT_SET_LIB_OPTIONS)

#-------------------------------------------------------------------------------
MACRO (TARGET_C_PROPERTIES wintarget libtype addcompileflags addlinkflags)
  if (MSVC)
    TARGET_MSVC_PROPERTIES (${wintarget} ${libtype} "${addcompileflags} ${WIN_COMPILE_FLAGS}" "${addlinkflags} ${WIN_LINK_FLAGS}")
  else (MSVC)
    set_target_properties (${wintarget}
        PROPERTIES
            COMPILE_FLAGS "${addcompileflags}"
            LINK_FLAGS "${addlinkflags}"
    ) 
  endif (MSVC)
ENDMACRO (TARGET_C_PROPERTIES)

#-------------------------------------------------------------------------------
MACRO (TARGET_MSVC_PROPERTIES wintarget libtype addcompileflags addlinkflags)
  if (MSVC)
    set_target_properties (${wintarget}
        PROPERTIES
            COMPILE_FLAGS "${addcompileflags}"
            LINK_FLAGS "${addlinkflags}"
    ) 
  endif (MSVC)
ENDMACRO (TARGET_MSVC_PROPERTIES)

#-----------------------------------------------------------------------------
# Configure the README.txt file for the binary package
#-----------------------------------------------------------------------------
MACRO (H5BZ2_README_PROPERTIES)
  set (BINARY_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
  set (BINARY_PLATFORM "${CMAKE_SYSTEM_NAME}")
  if (WIN32)
    set (BINARY_EXAMPLE_ENDING "zip")
    set (BINARY_INSTALL_ENDING "exe")
    if (CMAKE_CL_64)
      set (BINARY_SYSTEM_NAME "win64")
    else (CMAKE_CL_64)
      set (BINARY_SYSTEM_NAME "win32")
    endif (CMAKE_CL_64)
    if (${CMAKE_SYSTEM_VERSION} MATCHES "6.1")
      set (BINARY_PLATFORM "${BINARY_PLATFORM} 7")
    elseif (${CMAKE_SYSTEM_VERSION} MATCHES "6.2")
      set (BINARY_PLATFORM "${BINARY_PLATFORM} 8")
    endif (${CMAKE_SYSTEM_VERSION} MATCHES "6.1")
    set (BINARY_PLATFORM "${BINARY_PLATFORM} ${MSVC_C_ARCHITECTURE_ID}")
    if (${CMAKE_C_COMPILER_VERSION} MATCHES "16.*")
      set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2010")
    elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "15.*")
      set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2008")
    elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "17.*")
      set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2012")
    elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "18.*")
      set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2013")
    else (${CMAKE_C_COMPILER_VERSION} MATCHES "16.*")
      set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO ${CMAKE_C_COMPILER_VERSION}")
    endif (${CMAKE_C_COMPILER_VERSION} MATCHES "16.*")
  elseif (APPLE)
    set (BINARY_EXAMPLE_ENDING "tar.gz")
    set (BINARY_INSTALL_ENDING "dmg")
    set (BINARY_PLATFORM "${BINARY_PLATFORM} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR}")
    set (BINARY_PLATFORM "${BINARY_PLATFORM}, using ${CMAKE_C_COMPILER_ID} C ${CMAKE_C_COMPILER_VERSION}")
  else (WIN32)
    set (BINARY_EXAMPLE_ENDING "tar.gz")
    set (BINARY_INSTALL_ENDING "sh")
    set (BINARY_PLATFORM "${BINARY_PLATFORM} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR}")
    set (BINARY_PLATFORM "${BINARY_PLATFORM}, using ${CMAKE_C_COMPILER_ID} C ${CMAKE_C_COMPILER_VERSION}")
  endif (WIN32)
    
  configure_file (
      ${H5BZ2_RESOURCES_DIR}/README.txt.cmake.in 
      ${CMAKE_BINARY_DIR}/README.txt @ONLY
  )
ENDMACRO (H5BZ2_README_PROPERTIES)

#-------------------------------------------------------------------------------
MACRO (EXTERNAL_BZ2_LIBRARY compress_type libtype)
  if (${libtype} MATCHES "SHARED")
    set (BUILD_EXT_SHARED_LIBS "ON")
  else (${libtype} MATCHES "SHARED")
    set (BUILD_EXT_SHARED_LIBS "OFF")
  endif (${libtype} MATCHES "SHARED")
  if (${compress_type} MATCHES "SVN")
    EXTERNALPROJECT_ADD (BZ2
        SVN_REPOSITORY ${BZ2_URL}
        # [SVN_REVISION rev] 
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
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
    ) 
  elseif (${compress_type} MATCHES "GIT")
    EXTERNALPROJECT_ADD (BZ2
        GIT_REPOSITORY ${BZ2_URL}
        # [SVN_REVISION rev] 
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
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
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
            -DCMAKE_ANSI_CFLAGS:STRING=${CMAKE_ANSI_CFLAGS}
    ) 
  endif (${compress_type} MATCHES "SVN")
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
ENDMACRO (EXTERNAL_BZ2_LIBRARY)

#-------------------------------------------------------------------------------
MACRO (PACKAGE_BZ2_LIBRARY compress_type)
  if (WIN32)
    add_custom_target (BZ2-GenHeader-Copy ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BZ2_INCLUDE_DIR}/bzlib.h ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
        COMMENT "Copying ${BZ2_INCLUDE_DIR}/bzlib.h to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
    )
    set (EXTERNAL_HEADER_LIST ${EXTERNAL_HEADER_LIST} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bzlib.h)
    if (${compress_type} MATCHES "SVN" OR ${compress_type} MATCHES "TGZ")
      add_dependencies (BZ2-GenHeader-Copy bz2)
    endif (${compress_type} MATCHES "SVN" OR ${compress_type} MATCHES "TGZ")
  endif (WIN32)
ENDMACRO (PACKAGE_BZ2_LIBRARY)
