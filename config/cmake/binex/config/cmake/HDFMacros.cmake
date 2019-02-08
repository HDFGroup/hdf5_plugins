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
    set(HDF_CFG_NAME ${CMAKE_BUILD_TYPE})
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
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
      "MinSizeRel" "RelWithDebInfo")
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
macro (TARGET_C_PROPERTIES wintarget libtype)
  target_compile_options(${wintarget} PRIVATE
      $<$<C_COMPILER_ID:MSVC>:${WIN_COMPILE_FLAGS}>
      $<$<CXX_COMPILER_ID:MSVC>:${WIN_COMPILE_FLAGS}>
  )
  target_link_libraries(${wintarget} INTERFACE
      $<$<C_COMPILER_ID:MSVC>:${WIN_LINK_FLAGS}>
      $<$<CXX_COMPILER_ID:MSVC>:${WIN_LINK_FLAGS}>
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

macro (HDF_DIR_PATHS package_prefix)
  if (APPLE)
    option (${package_prefix}_BUILD_FRAMEWORKS "TRUE to build as frameworks libraries, FALSE to build according to BUILD_SHARED_LIBS" FALSE)
  endif ()

  if (NOT ${package_prefix}_INSTALL_BIN_DIR)
    set (${package_prefix}_INSTALL_BIN_DIR bin)
  endif ()
  if (NOT ${package_prefix}_INSTALL_LIB_DIR)
    if (APPLE)
      if (${package_prefix}_BUILD_FRAMEWORKS)
        set (${package_prefix}_INSTALL_JAR_DIR ../Java)
      else ()
        set (${package_prefix}_INSTALL_JAR_DIR lib)
      endif ()
      set (${package_prefix}_INSTALL_FMWK_DIR ${CMAKE_INSTALL_FRAMEWORK_PREFIX})
    else ()
      set (${package_prefix}_INSTALL_JAR_DIR lib)
    endif ()
    set (${package_prefix}_INSTALL_LIB_DIR lib)
  endif ()
  if (NOT ${package_prefix}_INSTALL_INCLUDE_DIR)
    set (${package_prefix}_INSTALL_INCLUDE_DIR include)
  endif ()
  if (NOT ${package_prefix}_INSTALL_DATA_DIR)
    if (NOT WIN32)
      if (APPLE)
        if (${package_prefix}_BUILD_FRAMEWORKS)
          set (${package_prefix}_INSTALL_EXTRA_DIR ../SharedSupport)
        else ()
          set (${package_prefix}_INSTALL_EXTRA_DIR share)
        endif ()
        set (${package_prefix}_INSTALL_FWRK_DIR ${CMAKE_INSTALL_FRAMEWORK_PREFIX})
      endif ()
      set (${package_prefix}_INSTALL_DATA_DIR share)
      set (${package_prefix}_INSTALL_CMAKE_DIR share/cmake)
    else ()
      set (${package_prefix}_INSTALL_DATA_DIR ".")
      set (${package_prefix}_INSTALL_CMAKE_DIR cmake)
    endif ()
  endif ()

  set (CMAKE_SKIP_BUILD_RPATH  FALSE)
  set (CMAKE_INSTALL_RPATH_USE_LINK_PATH  FALSE)
  set (CMAKE_BUILD_WITH_INSTALL_RPATH ON)
  if (APPLE)
    set (CMAKE_INSTALL_NAME_DIR "@rpath")
    set (CMAKE_INSTALL_RPATH
        "@executable_path/../${${package_prefix}_INSTALL_LIB_DIR}"
        "@executable_path/"
        "@loader_path/../${${package_prefix}_INSTALL_LIB_DIR}"
        "@loader_path/"
    )
  else ()
    set (CMAKE_INSTALL_RPATH "\$ORIGIN/../${${package_prefix}_INSTALL_LIB_DIR}:\$ORIGIN/")
  endif ()

  if (DEFINED ADDITIONAL_CMAKE_PREFIX_PATH AND EXISTS "${ADDITIONAL_CMAKE_PREFIX_PATH}")
    set (CMAKE_PREFIX_PATH ${ADDITIONAL_CMAKE_PREFIX_PATH} ${CMAKE_PREFIX_PATH})
  endif ()

  SET_HDF_BUILD_TYPE()

#-----------------------------------------------------------------------------
# Setup output Directories
#-----------------------------------------------------------------------------
  if (NOT ${package_prefix}_EXTERNALLY_CONFIGURED)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
    )
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries"
    )
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries."
    )
    set (CMAKE_Fortran_MODULE_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all fortran modules."
    )
    get_property(_isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if(_isMultiConfig)
      set (CMAKE_TEST_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
      set (CMAKE_PDB_OUTPUT_DIRECTORY
          ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all pdb files."
      )
    else ()
      set (CMAKE_TEST_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif ()
  else ()
    # if we are externally configured, but the project uses old cmake scripts
    # this may not be set and utilities like H5detect will fail
    if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
      set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
    endif ()
  endif ()
endmacro ()

