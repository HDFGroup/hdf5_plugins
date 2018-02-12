#
# Copyright by The HDF Group.
# All rights reserved.
#
# This file is part of HDF5.  The full HDF5 copyright notice, including
# terms governing use, modification, and redistribution, is contained in
# the COPYING file, which can be found at the root of the source code
# distribution tree, or in https://support.hdfgroup.org/ftp/HDF5/releases.
# If you do not have access to either file, you may request a copy from
# help@hdfgroup.org.
#

#-------------------------------------------------------------------------------
macro (SET_HDF_BUILD_TYPE)
  get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if(_isMultiConfig)
    set(HDF_CFG_NAME ${CTEST_CONFIGURATION_TYPE})
    set(HDF_BUILD_TYPE ${CMAKE_CFG_INTDIR})
    set(HDF_CFG_BUILD_TYPE \${CMAKE_INSTALL_CONFIG_NAME})
  else()
    set(HDF_CFG_BUILD_TYPE ".")
    if(CMAKE_BUILD_TYPE)
      set(HDF_CFG_NAME ${CMAKE_BUILD_TYPE})
      set(HDF_BUILD_TYPE ${CMAKE_BUILD_TYPE})
    else()
      set(HDF_CFG_NAME "Release")
      set(HDF_BUILD_TYPE "Release")
    endif()
  endif()
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
  endif()
endmacro ()

#-------------------------------------------------------------------------------
macro (SET_GLOBAL_VARIABLE name value)
  set (${name} ${value} CACHE INTERNAL "Used to pass variables between directories" FORCE)
endmacro ()

#-------------------------------------------------------------------------------
macro (IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)
  #set(source_group_path "Source/AIM/${NAME}")
  string (REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH})
  source_group (${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property (SOURCE ${HEADERS}
  #       PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)
endmacro ()

#-------------------------------------------------------------------------------
macro (IDE_SOURCE_PROPERTIES SOURCE_PATH HEADERS SOURCES)
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
endmacro ()

#-------------------------------------------------------------------------------
macro (INSTALL_TARGET_PDB libtarget targetdestination targetcomponent)
  if (WIN32 AND MSVC)
    get_target_property (target_type ${libtarget} TYPE)
    if (${libtype} MATCHES "SHARED")
      set (targetfilename $<TARGET_PDB_FILE:${libtarget}>)
    else ()
      get_property (target_name TARGET ${libtarget} PROPERTY OUTPUT_NAME_RELWITHDEBINFO)
      set (targetfilename $<TARGET_FILE_DIR:${libtarget}>/${target_name}.pdb)
    endif ()
    install (
      FILES
          ${targetfilename}
      DESTINATION
          ${targetdestination}
      CONFIGURATIONS RelWithDebInfo
      COMPONENT ${targetcomponent}
  )
  endif ()
endmacro ()

#-------------------------------------------------------------------------------
macro (INSTALL_PROGRAM_PDB progtarget targetdestination targetcomponent)
  if (WIN32 AND MSVC)
    install (
      FILES
          $<TARGET_PDB_FILE:${progtarget}>
      DESTINATION
          ${targetdestination}
      CONFIGURATIONS RelWithDebInfo
      COMPONENT ${targetcomponent}
  )
  endif ()
endmacro ()

#-------------------------------------------------------------------------------
macro (HDF_SET_BASE_OPTIONS libtarget libname libtype)
  if (WIN32)
    set (LIB_DEBUG_SUFFIX "_D")
  else ()
    set (LIB_DEBUG_SUFFIX "_debug")
  endif ()
  if (${libtype} MATCHES "SHARED")
    set (LIB_RELEASE_NAME "${libname}")
    set (LIB_DEBUG_NAME "${libname}${LIB_DEBUG_SUFFIX}")
  else ()
    if (WIN32)
      set (LIB_RELEASE_NAME "lib${libname}")
      set (LIB_DEBUG_NAME "lib${libname}${LIB_DEBUG_SUFFIX}")
    else ()
      set (LIB_RELEASE_NAME "${libname}")
      set (LIB_DEBUG_NAME "${libname}${LIB_DEBUG_SUFFIX}")
    endif ()
  endif ()

  set_target_properties (${libtarget}
      PROPERTIES
         OUTPUT_NAME
               ${LIB_RELEASE_NAME}
         OUTPUT_NAME_DEBUG
               ${LIB_DEBUG_NAME}
         OUTPUT_NAME_RELEASE
               ${LIB_RELEASE_NAME}
         OUTPUT_NAME_MINSIZEREL
               ${LIB_RELEASE_NAME}
         OUTPUT_NAME_RELWITHDEBINFO
               ${LIB_RELEASE_NAME}
  )
  if (${libtype} MATCHES "STATIC")
    if (WIN32)
      set_target_properties (${libtarget}
          PROPERTIES
          COMPILE_PDB_NAME_DEBUG          ${LIB_DEBUG_NAME}
          COMPILE_PDB_NAME_RELEASE        ${LIB_RELEASE_NAME}
          COMPILE_PDB_NAME_MINSIZEREL     ${LIB_RELEASE_NAME}
          COMPILE_PDB_NAME_RELWITHDEBINFO ${LIB_RELEASE_NAME}
          COMPILE_PDB_OUTPUT_DIRECTORY    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
      )
    endif ()
  endif ()

  #----- Use MSVC Naming conventions for Shared Libraries
  if (MINGW AND ${libtype} MATCHES "SHARED")
    set_target_properties (${libtarget}
        PROPERTIES
        IMPORT_SUFFIX ".lib"
        IMPORT_PREFIX ""
        PREFIX ""
    )
  endif ()
endmacro ()

#-------------------------------------------------------------------------------
macro (HDF_SET_LIB_VERSIONS pkg_name libtarget defaultlibname libtype)
  set (libname "${defaultlibname}")
  HDF_SET_BASE_OPTIONS (${libtarget} ${libname} ${libtype})

  if (${libtype} MATCHES "SHARED")
    set (LIB_PACKAGE_SOVERSION ${${pkg_name}_VERS_MAJOR})
    if (WIN32)
      set (LIB_VERSION ${${pkg_name}_PACKAGE_VERSION_MAJOR})
    else ()
      set (LIB_VERSION ${${pkg_name}_PACKAGE_VERSION})
    endif ()
    set_target_properties (${libtarget} PROPERTIES VERSION ${LIB_VERSION})
    if (WIN32)
        set (${libname} "${libname}-${LIB_PACKAGE_SOVERSION}")
    else ()
        set_target_properties (${libtarget} PROPERTIES SOVERSION ${LIB_PACKAGE_SOVERSION})
    endif ()
  endif ()

  #-- Apple Specific install_name for libraries
  if (APPLE)
    option (${pkg_name}_BUILD_WITH_INSTALL_NAME "Build with library install_name set to the installation path" OFF)
    if (${pkg_name}_BUILD_WITH_INSTALL_NAME)
      set_target_properties(${libtarget} PROPERTIES
          LINK_FLAGS "-current_version ${${pkg_name}_PACKAGE_VERSION} -compatibility_version ${${pkg_name}_PACKAGE_VERSION}"
          INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
          BUILD_WITH_INSTALL_RPATH ${${pkg_name}_BUILD_WITH_INSTALL_NAME}
      )
    endif ()
  endif ()
endmacro ()

#-------------------------------------------------------------------------------
macro (HDF_IMPORT_SET_LIB_OPTIONS libtarget libname libtype libversion)
  HDF_SET_BASE_OPTIONS (${libtarget} ${libname} ${libtype})

  if (${importtype} MATCHES "IMPORT")
    set (importprefix "${CMAKE_STATIC_LIBRARY_PREFIX}")
  endif ()
  if (${HDF_CFG_NAME} MATCHES "Debug")
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
            IMPORTED_IMPLIB "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${HDF_BUILD_TYPE}/${CMAKE_IMPORT_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_IMPORT_LIBRARY_SUFFIX}"
            IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${HDF_BUILD_TYPE}/${CMAKE_IMPORT_LIBRARY_PREFIX}${IMPORT_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
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
          IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${HDF_BUILD_TYPE}/${IMPORT_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
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

#-------------------------------------------------------------------------------
macro (TARGET_C_PROPERTIES wintarget libtype addcompileflags addlinkflags)
  if (MSVC)
    TARGET_MSVC_PROPERTIES (${wintarget} ${libtype} "${addcompileflags} ${WIN_COMPILE_FLAGS}" "${addlinkflags} ${WIN_LINK_FLAGS}")
  else ()
    set_target_properties (${wintarget} PROPERTIES COMPILE_FLAGS "${addcompileflags}" LINK_FLAGS "${addlinkflags}")
  endif ()
endmacro ()

#-------------------------------------------------------------------------------
macro (TARGET_MSVC_PROPERTIES wintarget libtype addcompileflags addlinkflags)
  if (MSVC)
    set_target_properties (${wintarget} PROPERTIES COMPILE_FLAGS "${addcompileflags}" LINK_FLAGS "${addlinkflags}")
  endif ()
endmacro ()

#-----------------------------------------------------------------------------
# Configure the README.txt file for the binary package
#-----------------------------------------------------------------------------
macro (PLUGIN_README_PROPERTIES pkg_name)
  set (BINARY_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
  set (BINARY_PLATFORM "${CMAKE_SYSTEM_NAME}")
  if (WIN32)
    set (BINARY_EXAMPLE_ENDING "zip")
    set (BINARY_INSTALL_ENDING "msi")
    if (CMAKE_CL_64)
      set (BINARY_SYSTEM_NAME "win64")
    else ()
      set (BINARY_SYSTEM_NAME "win32")
    endif ()
    if (${CMAKE_SYSTEM_VERSION} MATCHES "6.1")
      set (BINARY_PLATFORM "${BINARY_PLATFORM} 7")
    elseif (${CMAKE_SYSTEM_VERSION} MATCHES "6.2")
      set (BINARY_PLATFORM "${BINARY_PLATFORM} 8")
    elseif (${CMAKE_SYSTEM_VERSION} MATCHES "6.3")
      set (BINARY_PLATFORM "${BINARY_PLATFORM} 10")
    endif ()
    if (CMAKE_C_COMPILER_ID MATCHES "Intel")
      set (BINARY_PLATFORM "${BINARY_PLATFORM} Intel")
      if (${CMAKE_C_COMPILER_VERSION} MATCHES "^17.*")
        set (BINARY_PLATFORM "${BINARY_PLATFORM}, using Intel 17")
      else ()
        set (BINARY_PLATFORM "${BINARY_PLATFORM}, using Intel ${CMAKE_C_COMPILER_VERSION}")
      endif ()
    elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
      set (BINARY_PLATFORM "${BINARY_PLATFORM} ${MSVC_C_ARCHITECTURE_ID}")
      if (${CMAKE_C_COMPILER_VERSION} MATCHES "^16.*")
        set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2010")
      elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "^15.*")
        set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2008")
      elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "^17.*")
        set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2012")
      elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "^18.*")
        set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2013")
      elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "^19.*")
        set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2015")
      elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "^20.*")
        set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2017")
      else ()
        set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO ${CMAKE_C_COMPILER_VERSION}")
      endif ()
    endif ()
  elseif (APPLE)
    set (BINARY_EXAMPLE_ENDING "tar.gz")
    set (BINARY_INSTALL_ENDING "dmg")
    set (BINARY_PLATFORM "${BINARY_PLATFORM} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR}")
    set (BINARY_PLATFORM "${BINARY_PLATFORM}, using ${CMAKE_C_COMPILER_ID} C ${CMAKE_C_COMPILER_VERSION}")
  else ()
    set (BINARY_EXAMPLE_ENDING "tar.gz")
    set (BINARY_INSTALL_ENDING "sh")
    set (BINARY_PLATFORM "${BINARY_PLATFORM} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR}")
    set (BINARY_PLATFORM "${BINARY_PLATFORM}, using ${CMAKE_C_COMPILER_ID} C ${CMAKE_C_COMPILER_VERSION}")
  endif ()

  configure_file (
      ${${pkg_name}_RESOURCES_DIR}/README.txt.cmake.in
      ${CMAKE_BINARY_DIR}/README.txt @ONLY
  )
endmacro ()

macro (HDFTEST_COPY_FILE src dest target)
    add_custom_command(
        OUTPUT  "${dest}"
        COMMAND "${CMAKE_COMMAND}"
        ARGS     -E copy_if_different "${src}" "${dest}"
        DEPENDS "${src}"
    )
    list (APPEND ${target}_list "${dest}")
endmacro ()
