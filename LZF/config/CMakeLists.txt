cmake_minimum_required (VERSION 3.14)
PROJECT (LZF C)

#-----------------------------------------------------------------------------
# Basic LZF stuff here
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
set (LZF_RESOURCES_DIR          ${LZF_SOURCE_DIR})
set (LZF_SRC_DIR                ${LZF_SOURCE_DIR})

#-----------------------------------------------------------------------------
# Set the core names of all the libraries
#-----------------------------------------------------------------------------
set (LZF_LIB_CORENAME           "lzf")

#-----------------------------------------------------------------------------
# Set the true names of all the libraries if customized by external project
#-----------------------------------------------------------------------------
set (LZF_LIB_NAME              "${LZF_EXTERNAL_LIB_PREFIX}${LZF_LIB_CORENAME}")

#-----------------------------------------------------------------------------
# Set the target names of all the libraries
#-----------------------------------------------------------------------------
set (LZF_LIB_TARGET             "${LZF_LIB_CORENAME}-static")

if (NOT LZF_INSTALL_BIN_DIR)
  set (LZF_INSTALL_BIN_DIR bin)
endif ()
if (NOT LZF_INSTALL_LIB_DIR)
  set (LZF_INSTALL_LIB_DIR lib)
endif ()
if (NOT LZF_INSTALL_INCLUDE_DIR)
  set (LZF_INSTALL_INCLUDE_DIR include)
endif ()
if (NOT LZF_INSTALL_DATA_DIR)
  if (NOT WIN32)
    if (APPLE)
      option (LZF_BUILD_FRAMEWORKS "Create a Mac OSX Framework" OFF)
      set (LZF_INSTALL_FWRK_DIR ${CMAKE_INSTALL_FRAMEWORK_PREFIX})
    endif ()
    set (LZF_INSTALL_DATA_DIR share)
    set (LZF_INSTALL_CMAKE_DIR share/cmake)
  else ()
    set (LZF_INSTALL_DATA_DIR ".")
    set (LZF_INSTALL_CMAKE_DIR cmake)
  endif ()
endif ()

#-----------------------------------------------------------------------------
# parse the full version number from lzf.h and include in LZF_VERS_INFO
#-----------------------------------------------------------------------------
#file (READ ${LZF_SRC_DIR}/lzf.h _lzf_h_contents)
set (LZF_VERS_MAJOR "3")
set (LZF_VERS_MINOR "6")
set (LZF_VERS_RELEASE "0")
set (LZF_VERS_SUBRELEASE "")
#message (STATUS "VERSION: ${LZF_VERS_MAJOR}.${LZF_VERS_MINOR}.${LZF_VERS_RELEASE}-${LZF_VERS_SUBRELEASE}")
#string (REGEX REPLACE ".*#define[ \t]+LZF_VERSION[ \t]+\"([0-9A-Za-z.]+)\".*"
#    "\\1" LZF_FULL_VERSION ${_lzf_h_contents})
#message (STATUS "VERSION: ${LZF_FULL_VERSION}")

#-----------------------------------------------------------------------------
set (LZF_PACKAGE "lzf")
set (LZF_PACKAGE_NAME "LZF")
set (LZF_PACKAGE_VERSION "${LZF_VERS_MAJOR}.${LZF_VERS_MINOR}")
set (LZF_PACKAGE_VERSION_MAJOR "${LZF_VERS_MAJOR}.${LZF_VERS_MINOR}")
set (LZF_PACKAGE_VERSION_MINOR "${LZF_VERS_RELEASE}")
set (LZF_PACKAGE_STRING "${LZF_PACKAGE_NAME} ${LZF_PACKAGE_VERSION}-${LZF_VERS_SUBRELEASE}")
set (LZF_PACKAGE_TARNAME "lzf")
set (LZF_PACKAGE_URL "http://www.hdfgroup.org")
set (LZF_PACKAGE_BUGREPORT "help@hdfgroup.org")

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
#-------------------------------------------------------------------------------
macro (LZF_SET_LIB_OPTIONS libtarget defaultlibname libtype)
  set (libname "${defaultlibname}")

  # message (STATUS "${libname} libtype: ${libtype}")
  LZF_SET_BASE_OPTIONS (${libtarget} ${libname} ${libtype})

  if (${libtype} MATCHES "SHARED")
    if (WIN32)
      set (LIBLZF_VERSION ${LZF_PACKAGE_VERSION_MAJOR})
    else (WIN32)
      set (LIBLZF_VERSION ${LZF_PACKAGE_VERSION})
    endif (WIN32)

    if (NOT CYGWIN)
      # This property causes shared libraries on Linux to have the full version
      # encoded into their final filename.  We disable this on Cygwin because
      # it causes cygz-${LZF_FULL_VERSION}.dll to be created when cygz.dll
      # seems to be the default.
      #
      # This has no effect with MSVC, on that platform the version info for
      # the DLL comes from the resource file win32/zlib1.rc
      set_target_properties (${libtarget} PROPERTIES VERSION ${LIBLZF_VERSION})
    endif (NOT CYGWIN)
    set_target_properties (${libtarget} PROPERTIES SOVERSION ${LIBLZF_VERSION})
  endif (${libtype} MATCHES "SHARED")

  #-- Apple Specific install_name for libraries
  if (APPLE)
    option (LZF_BUILD_WITH_INSTALL_NAME "Build with library install_name set to the installation path" OFF)
    if (LZF_BUILD_WITH_INSTALL_NAME)
      set_target_properties (${libtarget} PROPERTIES
          LINK_FLAGS "-current_version ${LZF_PACKAGE_VERSION} -compatibility_version ${LZF_PACKAGE_VERSION}"
          INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
          BUILD_WITH_INSTALL_RPATH ${LZF_BUILD_WITH_INSTALL_NAME}
      )
    endif (LZF_BUILD_WITH_INSTALL_NAME)
    if (LZF_BUILD_FRAMEWORKS)
      if (${libtype} MATCHES "SHARED")
        # adapt target to build frameworks instead of dylibs
        set_target_properties(${libtarget} PROPERTIES
            XCODE_ATTRIBUTE_INSTALL_PATH "@rpath"
            FRAMEWORK TRUE
            FRAMEWORK_VERSION ${LZF_PACKAGE_VERSION_MAJOR}
            MACOSX_FRAMEWORK_IDENTIFIER org.hdfgroup.${libtarget}
            MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${LZF_PACKAGE_VERSION_MAJOR}
            MACOSX_FRAMEWORK_BUNDLE_VERSION ${LZF_PACKAGE_VERSION_MAJOR})
      endif (${libtype} MATCHES "SHARED")
    endif (LZF_BUILD_FRAMEWORKS)
  endif (APPLE)

endmacro (LZF_SET_LIB_OPTIONS)

#-------------------------------------------------------------------------------
macro (SET_GLOBAL_VARIABLE name value)
  set (${name} ${value} CACHE INTERNAL "Used to pass variables between directories" FORCE)
endmacro (SET_GLOBAL_VARIABLE)

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
endmacro (IDE_GENERATED_PROPERTIES)

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
endmacro (IDE_SOURCE_PROPERTIES)

#-------------------------------------------------------------------------------
macro (TARGET_NAMING libtarget libtype)
  if (WIN32)
    if (${libtype} MATCHES "SHARED")
      set_target_properties (${libtarget} PROPERTIES OUTPUT_NAME "${libtarget}dll")
    endif (${libtype} MATCHES "SHARED")
  else (WIN32)
    if (${libtype} MATCHES "SHARED")
      set_target_properties (${libtarget} PROPERTIES OUTPUT_NAME "${libtarget}${ARGN}")
    endif (${libtype} MATCHES "SHARED")
  endif (WIN32)
endmacro (TARGET_NAMING)

#-------------------------------------------------------------------------------
macro (INSTALL_TARGET_PDB libtarget targetdestination targetcomponent)
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
endmacro (INSTALL_TARGET_PDB)

#-------------------------------------------------------------------------------
macro (INSTALL_PROGRAM_PDB progtarget targetdestination targetcomponent)
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
endmacro (INSTALL_PROGRAM_PDB)

#-------------------------------------------------------------------------------
macro (LZF_SET_BASE_OPTIONS libtarget libname libtype)
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

endmacro (LZF_SET_BASE_OPTIONS)

#-------------------------------------------------------------------------------
macro (LZF_IMPORT_SET_LIB_OPTIONS libtarget libname libtype libversion)
  LZF_SET_BASE_OPTIONS (${libtarget} ${libname} ${libtype})

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

endmacro (LZF_IMPORT_SET_LIB_OPTIONS)

#-------------------------------------------------------------------------------
macro (TARGET_C_PROPERTIES wintarget libtype addcompileflags addlinkflags)
  if (MSVC)
    TARGET_MSVC_PROPERTIES (${wintarget} ${libtype} "${addcompileflags} ${WIN_COMPILE_FLAGS}" "${addlinkflags} ${WIN_LINK_FLAGS}")
  else (MSVC)
    set_target_properties (${wintarget}
        PROPERTIES
            COMPILE_FLAGS "${addcompileflags}"
            LINK_FLAGS "${addlinkflags}"
    )
  endif (MSVC)
endmacro (TARGET_C_PROPERTIES)

#-------------------------------------------------------------------------------
macro (TARGET_MSVC_PROPERTIES wintarget libtype addcompileflags addlinkflags)
  if (MSVC)
    set_target_properties (${wintarget}
        PROPERTIES
            COMPILE_FLAGS "${addcompileflags}"
            LINK_FLAGS "${addlinkflags}"
    )
  endif (MSVC)
endmacro (TARGET_MSVC_PROPERTIES)

#-----------------------------------------------------------------------------
# Setup output Directories
#-----------------------------------------------------------------------------
if (NOT LZF_EXTERNALLY_CONFIGURED)
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
  )
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries"
  )
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries."
  )
else ()
  # if we are externally configured, but the project uses old cmake scripts
  # this may not be set
  if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
  endif ()
endif ()

#-----------------------------------------------------------------------------
# Targets built within this project are exported at Install time for use
# by other projects using FindLZF.
#-----------------------------------------------------------------------------
if (NOT LZF_EXPORTED_TARGETS)
  set (LZF_EXPORTED_TARGETS "lzf-targets")
endif ()

#-----------------------------------------------------------------------------
# To include a library in the list exported by the project AT BUILD TIME,
# add it to this variable. This is NOT used by Make Install, but for projects
# which include LZF as a sub-project within their build tree
#-----------------------------------------------------------------------------
set_global_variable (LZF_LIBRARIES_TO_EXPORT "")

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${LZF_RESOURCES_DIR}/ConfigureChecks.cmake)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

#-----------------------------------------------------------------------------
# When building utility executables that generate other (source) files :
# we make use of the following variables defined in the root CMakeLists.
# Certain systems may add /Debug or /Release to output paths
# and we need to call the executable from inside the CMake configuration
#-----------------------------------------------------------------------------
set (EXE_EXT "")
if (WIN32)
  set (EXE_EXT ".exe")
  add_definitions (-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
  add_definitions (-D_CRT_SECURE_NO_WARNINGS)
  add_definitions (-D_CRT_NONSTDC_NO_DEPRECATE)
endif ()

if (MSVC)
  set (CMAKE_MFC_FLAG 0)
endif ()

set (MAKE_SYSTEM)
if (CMAKE_BUILD_TOOL MATCHES "make")
  set (MAKE_SYSTEM 1)
endif ()

set (CFG_INIT "/${CMAKE_CFG_INTDIR}")
if (MAKE_SYSTEM)
  set (CFG_INIT "")
endif ()

#-----------------------------------------------------------------------------
# Compiler specific flags : Shouldn't there be compiler tests for these
#-----------------------------------------------------------------------------
if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_C_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_C_FLAGS} -std=c99 -fomit-frame-pointer -finline-functions -fno-common")
endif ()
if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -fno-common")
endif ()

#-----------------------------------------------------------------------------
# This is in here to help some of the GCC based IDES like Eclipse
# and code blocks parse the compiler errors and warnings better.
#-----------------------------------------------------------------------------
if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
endif ()
if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
endif ()

#-----------------------------------------------------------------------------
# All libs/tests/examples need the main include directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${LZF_BINARY_DIR} ${LZF_SOURCE_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#============================================================================
# lzf
#============================================================================

INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR})

#-----------------------------------------------------------------------------
# Define lzf Library
#-----------------------------------------------------------------------------
set (LZF_PUBLIC_HEADERS
    lzf.h
)
set (LZF_HDRS
    crc32.h
    lzfP.h
)
set (LZF_SRCS
    lzf_c.c
    lzf_d.c
)

add_library (${LZF_LIB_TARGET} STATIC ${LZF_SRCS} ${LZF_HDRS} ${LZF_PUBLIC_HEADERS})

if (MSVC AND CMAKE_CL_64)
  set_target_properties (${LZF_LIB_TARGET} PROPERTIES STATIC_LIBRARY_FLAGS "/machine:x64")
endif ()
TARGET_C_PROPERTIES (${LZF_LIB_TARGET} STATIC " " " ")
target_link_libraries (${LZF_LIB_TARGET} ${LINK_LIBS})
LZF_SET_LIB_OPTIONS (${LZF_LIB_TARGET} ${LZF_LIB_NAME} STATIC)
set_target_properties(${LZF_LIB_TARGET} PROPERTIES
    PUBLIC_HEADER "${LZF_PUBLIC_HEADERS}"
    INTERFACE_INCLUDE_DIRECTORIES "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
)

#-----------------------------------------------------------------------------
# Generate the config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${LZF_RESOURCES_DIR}/config.h.in ${LZF_BINARY_DIR}/config.h @ONLY)

#-----------------------------------------------------------------------------
# Set includes needed for build
#-----------------------------------------------------------------------------
set (LZF_INCLUDES_BUILD_TIME
    ${LZF_SRC_DIR} ${LZF_BINARY_DIR}
)
