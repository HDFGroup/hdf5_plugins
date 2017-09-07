#-------------------------------------------------------------------------------

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
macro (TARGET_NAMING libtarget libtype)
  if (${libtype} MATCHES "SHARED")
    set_target_properties (${libtarget} PROPERTIES OUTPUT_NAME "${libtarget}${ARGN}")
  endif ()
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
  if (${libtype} MATCHES "SHARED")
    if (WIN32)
      set (LIB_RELEASE_NAME "${libname}")
      set (LIB_DEBUG_NAME "${libname}_D")
    else ()
      set (LIB_RELEASE_NAME "${libname}")
      set (LIB_DEBUG_NAME "${libname}_debug")
    endif ()
  else ()
    if (WIN32)
      set (LIB_RELEASE_NAME "lib${libname}")
      set (LIB_DEBUG_NAME "lib${libname}_D")
    else ()
      set (LIB_RELEASE_NAME "${libname}")
      set (LIB_DEBUG_NAME "${libname}_debug")
    endif ()
  endif ()

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
    set (BINARY_PLATFORM "${BINARY_PLATFORM} ${MSVC_C_ARCHITECTURE_ID}")
    if (${CMAKE_C_COMPILER_VERSION} MATCHES "16.*")
      set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2010")
    elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "15.*")
      set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2008")
    elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "17.*")
      set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2012")
    elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "18.*")
      set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2013")
    elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "19.*")
      set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2015")
    else ()
      set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO ${CMAKE_C_COMPILER_VERSION}")
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

  if (BUILD_SHARED_LIBS)
    set (LIB_TYPE "Static and Shared")
  else ()
    set (LIB_TYPE "Static")
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
