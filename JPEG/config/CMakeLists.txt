cmake_minimum_required (VERSION 3.18)
PROJECT (JPEG C)

#-----------------------------------------------------------------------------
# Basic JPEG stuff here
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
set (JPEG_RESOURCES_DIR          ${JPEG_SOURCE_DIR})
set (JPEG_SRC_DIR                ${JPEG_SOURCE_DIR})

#-----------------------------------------------------------------------------
# Set the core names of all the libraries
#-----------------------------------------------------------------------------
set (JPEG_LIB_CORENAME           "jpeg")

#-----------------------------------------------------------------------------
# Set the true names of all the libraries if customized by external project
#-----------------------------------------------------------------------------
set (JPEG_LIB_NAME              "${JPEG_EXTERNAL_LIB_PREFIX}${JPEG_LIB_CORENAME}")

#-----------------------------------------------------------------------------
# Set the target names of all the libraries
#-----------------------------------------------------------------------------
set (JPEG_LIB_TARGET             "${JPEG_LIB_CORENAME}-static")

#-----------------------------------------------------------------------------
# parse the full version number from jpeglib.h and include in JPEG_FULL_VERSION
#-----------------------------------------------------------------------------
file (READ ${JPEG_SRC_DIR}/jpeglib.h _jpeglib_h_contents)
string (REGEX REPLACE ".*#define[ \t]+JPEG_LIB_VERSION[ \t]+([0-9][0-9]*).*$"
    "\\1" JPEG_FULL_VERSION ${_jpeglib_h_contents})
string (REGEX REPLACE ".*#define[ \t]+JPEG_LIB_VERSION_MAJOR[ \t]+([0-9][0-9]*).*$"
    "\\1" JPEG_VERS_MAJOR ${_jpeglib_h_contents})
string (REGEX REPLACE ".*#define[ \t]+JPEG_LIB_VERSION_MINOR[ \t]+([0-9][0-9]*).*$"
    "\\1" JPEG_VERS_MINOR ${_jpeglib_h_contents})
#message (STATUS "VERSION: ${JPEG_FULL_VERSION}")

#set (JPEG_VERS_MAJOR 8)
#set (JPEG_VERS_MINOR 0)
set (JPEG_VERS_RELEASE 0)

#-----------------------------------------------------------------------------
set (JPEG_PACKAGE "jpeg")
set (JPEG_PACKAGE_NAME "JPEG")
set (JPEG_PACKAGE_VERSION "${JPEG_VERS_MAJOR}.${JPEG_VERS_MINOR}")
set (JPEG_PACKAGE_VERSION_MAJOR "${JPEG_VERS_MAJOR}.${JPEG_VERS_MINOR}")
set (JPEG_PACKAGE_VERSION_MINOR "${JPEG_VERS_RELEASE}")
set (JPEG_PACKAGE_STRING "${JPEG_PACKAGE_NAME} ${JPEG_PACKAGE_VERSION}")
set (JPEG_PACKAGE_TARNAME "${JPEG_PACKAGE_NAME}${HDF_PACKAGE_EXT}")
set (JPEG_PACKAGE_URL "http://www.hdfgroup.org")
set (JPEG_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (JPEG_PACKAGE_SOVERSION "${JPEG_VERS_MAJOR}.${JPEG_VERS_MINOR}.${JPEG_VERS_RELEASE}")
set (JPEG_PACKAGE_SOVERSION_MAJOR "${JPEG_VERS_MAJOR}")

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
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
    message (VERBOSE "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
      "MinSizeRel" "RelWithDebInfo")
  endif()
endmacro ()

#-------------------------------------------------------------------------------
macro (JPEG_SET_LIB_OPTIONS libtarget libname libtype)
  set (LIB_OUT_NAME "${libname}")

  if (${libtype} MATCHES "SHARED")
    set (PACKAGE_SOVERSION ${JPEG_PACKAGE_SOVERSION})
    set (PACKAGE_COMPATIBILITY ${JPEG_VERS_MAJOR}.0.0)
    set (PACKAGE_CURRENT ${JPEG_VERS_MAJOR}.${JPEG_VERS_MINOR}.0)
    if (WIN32)
      set (LIB_VERSION ${JPEG_PACKAGE_VERSION_MAJOR})
    else ()
      set (LIB_VERSION ${JPEG_PACKAGE_SOVERSION_MAJOR})
    endif ()
    set_target_properties (${libtarget} PROPERTIES VERSION ${PACKAGE_SOVERSION})
    if (WIN32)
        set (${LIB_OUT_NAME} "${LIB_OUT_NAME}-${LIB_VERSION}")
    else ()
        set_target_properties (${libtarget} PROPERTIES SOVERSION ${LIB_VERSION})
    endif ()
    if (CMAKE_C_OSX_CURRENT_VERSION_FLAG)
      set_property(TARGET ${libtarget} APPEND PROPERTY
          LINK_FLAGS "${CMAKE_C_OSX_CURRENT_VERSION_FLAG}${PACKAGE_CURRENT} ${CMAKE_C_OSX_COMPATIBILITY_VERSION_FLAG}${PACKAGE_COMPATIBILITY}"
      )
    endif ()
  endif ()
  JPEG_SET_BASE_OPTIONS (${libtarget} ${LIB_OUT_NAME} ${libtype})

  #-- Apple Specific install_name for libraries
  if (APPLE)
    option (JPEG_BUILD_WITH_INSTALL_NAME "Build with library install_name set to the installation path" OFF)
    if (JPEG_BUILD_WITH_INSTALL_NAME)
      set_target_properties (${libtarget} PROPERTIES
          LINK_FLAGS "-current_version ${JPEG_PACKAGE_VERSION} -compatibility_version ${JPEG_PACKAGE_VERSION}"
          INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
          BUILD_WITH_INSTALL_RPATH ${JPEG_BUILD_WITH_INSTALL_NAME}
      )
    endif ()
    if (JPEG_BUILD_FRAMEWORKS)
      if (${libtype} MATCHES "SHARED")
        # adapt target to build frameworks instead of dylibs
        set_target_properties(${libtarget} PROPERTIES
            XCODE_ATTRIBUTE_INSTALL_PATH "@rpath"
            FRAMEWORK TRUE
            FRAMEWORK_VERSION ${JPEG_PACKAGE_VERSION_MAJOR}
            MACOSX_FRAMEWORK_IDENTIFIER org.hdfgroup.${libtarget}
            MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${JPEG_PACKAGE_VERSION_MAJOR}
            MACOSX_FRAMEWORK_BUNDLE_VERSION ${JPEG_PACKAGE_VERSION_MAJOR})
      endif ()
    endif ()
  endif ()
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
  if (WIN32 AND MSVC AND NOT DISABLE_PDB_FILES)
    get_target_property (target_type ${libtarget} TYPE)
    if (${libtype} MATCHES "SHARED")
      set (targetfilename $<TARGET_PDB_FILE:${libtarget}>)
    else ()
      get_property (target_name TARGET ${libtarget} PROPERTY $<IF:$<CONFIG:Debug>,OUTPUT_NAME_DEBUG,OUTPUT_NAME_RELWITHDEBINFO>)
      set (targetfilename $<TARGET_FILE_DIR:${libtarget}>/${target_name}.pdb)
    endif ()
    install (
      FILES ${targetfilename}
      DESTINATION ${targetdestination}
      CONFIGURATIONS Debug RelWithDebInfo
      COMPONENT ${targetcomponent}
      OPTIONAL
    )
  endif ()
endmacro ()

#-------------------------------------------------------------------------------
macro (INSTALL_PROGRAM_PDB progtarget targetdestination targetcomponent)
  if (WIN32 AND MSVC)
    install (
      FILES $<TARGET_PDB_FILE:${progtarget}>
      DESTINATION ${targetdestination}
      CONFIGURATIONS Debug RelWithDebInfo
      COMPONENT ${targetcomponent}
      OPTIONAL
    )
  endif ()
endmacro ()

#-------------------------------------------------------------------------------
macro (JPEG_SET_BASE_OPTIONS libtarget libname libtype)
  # message (STATUS "${libname} libtype: ${libtype}")
  if (${libtype} MATCHES "SHARED")
    set (LIB_RELEASE_NAME "${libname}")
    set (LIB_DEBUG_NAME "${libname}${CMAKE_DEBUG_POSTFIX}")
  else ()
    if (WIN32 AND NOT MINGW)
      set (LIB_RELEASE_NAME "lib${libname}")
      set (LIB_DEBUG_NAME "lib${libname}${CMAKE_DEBUG_POSTFIX}")
    else ()
      set (LIB_RELEASE_NAME "${libname}")
      set (LIB_DEBUG_NAME "${libname}${CMAKE_DEBUG_POSTFIX}")
    endif ()
  endif ()

  set_target_properties (${libtarget} PROPERTIES
      OUTPUT_NAME                ${LIB_RELEASE_NAME}
#      OUTPUT_NAME_DEBUG          ${LIB_DEBUG_NAME}
      OUTPUT_NAME_RELEASE        ${LIB_RELEASE_NAME}
      OUTPUT_NAME_MINSIZEREL     ${LIB_RELEASE_NAME}
      OUTPUT_NAME_RELWITHDEBINFO ${LIB_RELEASE_NAME}
  )

  if (${libtype} MATCHES "STATIC")
    if (WIN32)
      set_target_properties (${libtarget} PROPERTIES
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
macro (JPEG_IMPORT_SET_LIB_OPTIONS libtarget libname libtype libversion)
  JPEG_SET_BASE_OPTIONS (${libtarget} ${libname} ${libtype})

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
      if (MINGW)
        set_target_properties (${libtarget} PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${IMPORT_LIB_NAME}.lib"
            IMPORTED_LOCATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${IMPORT_LIB_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
        )
      elseif (CYGWIN)
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
macro (TARGET_C_PROPERTIES wintarget libtype)
  target_compile_options(${wintarget} PRIVATE
      $<$<C_COMPILER_ID:MSVC>:${WIN_COMPILE_FLAGS}>
      $<$<CXX_COMPILER_ID:MSVC>:${WIN_COMPILE_FLAGS}>
  )
  if(MSVC)
    set_property(TARGET ${wintarget} APPEND PROPERTY LINK_FLAGS "${WIN_LINK_FLAGS}")
  endif()
endmacro ()

macro (HDF_DIR_PATHS package_prefix)
  option (HDF_USE_GNU_DIRS "TRUE to use GNU Coding Standard install directory variables, FALSE to use historical settings" FALSE)
  if (HDF_USE_GNU_DIRS)
    include(GNUInstallDirs)
    if (NOT ${package_prefix}_INSTALL_BIN_DIR)
      set (${package_prefix}_INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR})
    endif ()
    if (NOT ${package_prefix}_INSTALL_LIB_DIR)
      set (${package_prefix}_INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
    endif ()
    if (NOT ${package_prefix}_INSTALL_JAR_DIR)
      set (${package_prefix}_INSTALL_JAR_DIR ${CMAKE_INSTALL_LIBDIR})
    endif ()
    if (NOT ${package_prefix}_INSTALL_INCLUDE_DIR)
      set (${package_prefix}_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR})
    endif ()
    if (NOT ${package_prefix}_INSTALL_MODULE_DIR)
      set (${package_prefix}_INSTALL_MODULE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/mod)
    endif ()
    if (NOT ${package_prefix}_INSTALL_DATA_DIR)
      set (${package_prefix}_INSTALL_DATA_DIR ${CMAKE_INSTALL_DATADIR})
    endif ()
    if (NOT ${package_prefix}_INSTALL_CMAKE_DIR)
      set (${package_prefix}_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake)
    endif ()
    if (NOT ${package_prefix}_INSTALL_DOC_DIR)
      set (${package_prefix}_INSTALL_DOC_DIR ${CMAKE_INSTALL_DOCDIR})
    endif ()
    message(STATUS "GNU: ${${package_prefix}_INSTALL_DOC_DIR}")
  endif ()

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
  if (NOT ${package_prefix}_INSTALL_MODULE_DIR)
    set (${package_prefix}_INSTALL_MODULE_DIR mod)
  endif ()
  if (NOT ${package_prefix}_INSTALL_DATA_DIR)
    if (NOT MSVC)
      if (APPLE)
        if (${package_prefix}_BUILD_FRAMEWORKS)
          set (${package_prefix}_INSTALL_EXTRA_DIR ../SharedSupport)
        else ()
          set (${package_prefix}_INSTALL_EXTRA_DIR share)
        endif ()
        set (${package_prefix}_INSTALL_FWRK_DIR ${CMAKE_INSTALL_FRAMEWORK_PREFIX})
      endif ()
      set (${package_prefix}_INSTALL_DATA_DIR share)
    else ()
      set (${package_prefix}_INSTALL_DATA_DIR ".")
    endif ()
  endif ()
  if (NOT ${package_prefix}_INSTALL_CMAKE_DIR)
    set (${package_prefix}_INSTALL_CMAKE_DIR cmake)
  endif ()
  if (NOT ${package_prefix}_INSTALL_DOC_DIR)
    set (${package_prefix}_INSTALL_DOC_DIR ${${package_prefix}_INSTALL_DATA_DIR})
  endif ()
  message(STATUS "Final: ${${package_prefix}_INSTALL_DOC_DIR}")

  # Always use full RPATH, i.e. don't skip the full RPATH for the build tree
  set (CMAKE_SKIP_BUILD_RPATH  FALSE)
  # when building, don't use the install RPATH already
  # (but later on when installing)
  set (CMAKE_INSTALL_RPATH_USE_LINK_PATH  FALSE)
  # add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
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

  #set the default debug suffix for all library targets
  if(NOT CMAKE_DEBUG_POSTFIX)
    if (WIN32)
      set (CMAKE_DEBUG_POSTFIX "_D")
    else ()
      set (CMAKE_DEBUG_POSTFIX "_debug")
    endif ()
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
        ${PROJECT_BINARY_DIR}/mod CACHE PATH "Single Directory for all fortran modules."
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

  if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if (CMAKE_HOST_UNIX)
      set (CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/HDF_Group/${HDF5_PACKAGE_NAME}/${HDF5_PACKAGE_VERSION}"
        CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
    else ()
      GetDefaultWindowsPrefixBase(CMAKE_GENERIC_PROGRAM_FILES)
      set (CMAKE_INSTALL_PREFIX
        "${CMAKE_GENERIC_PROGRAM_FILES}/HDF_Group/${HDF5_PACKAGE_NAME}/${HDF5_PACKAGE_VERSION}"
        CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
      set (CMAKE_GENERIC_PROGRAM_FILES)
    endif ()
  endif ()
endmacro ()

HDF_DIR_PATHS(${JPEG_PACKAGE_NAME})

#-----------------------------------------------------------------------------
# Targets built within this project are exported at Install time for use
# by other projects
#-----------------------------------------------------------------------------
if (NOT JPEG_EXPORTED_TARGETS)
  set (JPEG_EXPORTED_TARGETS "jpeg-targets")
endif ()

#-----------------------------------------------------------------------------
# To include a library in the list exported by the project AT BUILD TIME,
# add it to this variable. This is NOT used by Make Install, but for projects
# which include JPEG as a sub-project within their build tree
#-----------------------------------------------------------------------------
set_global_variable (JPEG_LIBRARIES_TO_EXPORT "")

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${JPEG_RESOURCES_DIR}/ConfigureChecks.cmake)
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
  add_definitions (-D_CONSOLE)
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
if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_LOADED)
  set (CMAKE_CXX_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -fno-common")
endif ()

#-----------------------------------------------------------------------------
# This is in here to help some of the GCC based IDES like Eclipse
# and code blocks parse the compiler errors and warnings better.
#-----------------------------------------------------------------------------
if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
endif ()
if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_LOADED)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
endif ()

#-----------------------------------------------------------------------------
set (MEMORYMGR jmemnobs)
option (ENABLE_MEMORYMGR "Select memory manager" OFF)
if (ENABLE_MEMORYMGR)
  set (MEMORYMGR jmemname)
endif ()

#-----------------------------------------------------------------------------
# All libs/tests/examples need the main include directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${JPEG_BINARY_DIR} ${JPEG_SOURCE_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#-----------------------------------------------------------------------------
# Generate the jconfig.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${JPEG_RESOURCES_DIR}/jconfig.h.in ${JPEG_BINARY_DIR}/jconfig.h @ONLY)

#-----------------------------------------------------------------------------
# Set includes needed for build
#-----------------------------------------------------------------------------
set (JPEG_INCLUDES_BUILD_TIME
    ${JPEG_SRC_DIR} ${JPEG_BINARY_DIR}
)

#-----------------------------------------------------------------------------
# Define jpeg Library
#-----------------------------------------------------------------------------
set (JPEG_SRCS
    ${JPEG_SOURCE_DIR}/jaricom.c
    ${JPEG_SOURCE_DIR}/jcapimin.c
    ${JPEG_SOURCE_DIR}/jcapistd.c
    ${JPEG_SOURCE_DIR}/jcarith.c
    ${JPEG_SOURCE_DIR}/jccoefct.c
    ${JPEG_SOURCE_DIR}/jccolor.c
    ${JPEG_SOURCE_DIR}/jcdctmgr.c
    ${JPEG_SOURCE_DIR}/jchuff.c
    ${JPEG_SOURCE_DIR}/jcinit.c
    ${JPEG_SOURCE_DIR}/jcmainct.c
    ${JPEG_SOURCE_DIR}/jcmarker.c
    ${JPEG_SOURCE_DIR}/jcmaster.c
    ${JPEG_SOURCE_DIR}/jcomapi.c
    ${JPEG_SOURCE_DIR}/jcparam.c
    ${JPEG_SOURCE_DIR}/jcprepct.c
    ${JPEG_SOURCE_DIR}/jcsample.c
    ${JPEG_SOURCE_DIR}/jctrans.c
    ${JPEG_SOURCE_DIR}/jdapimin.c
    ${JPEG_SOURCE_DIR}/jdapistd.c
    ${JPEG_SOURCE_DIR}/jdarith.c
    ${JPEG_SOURCE_DIR}/jdatadst.c
    ${JPEG_SOURCE_DIR}/jdatasrc.c
    ${JPEG_SOURCE_DIR}/jdcoefct.c
    ${JPEG_SOURCE_DIR}/jdcolor.c
    ${JPEG_SOURCE_DIR}/jddctmgr.c
    ${JPEG_SOURCE_DIR}/jdhuff.c
    ${JPEG_SOURCE_DIR}/jdinput.c
    ${JPEG_SOURCE_DIR}/jdmainct.c
    ${JPEG_SOURCE_DIR}/jdmarker.c
    ${JPEG_SOURCE_DIR}/jdmaster.c
    ${JPEG_SOURCE_DIR}/jdmerge.c
    ${JPEG_SOURCE_DIR}/jdpostct.c
    ${JPEG_SOURCE_DIR}/jdsample.c
    ${JPEG_SOURCE_DIR}/jdtrans.c
    ${JPEG_SOURCE_DIR}/jerror.c
    ${JPEG_SOURCE_DIR}/jfdctflt.c
    ${JPEG_SOURCE_DIR}/jfdctfst.c
    ${JPEG_SOURCE_DIR}/jfdctint.c
    ${JPEG_SOURCE_DIR}/jidctflt.c
    ${JPEG_SOURCE_DIR}/jidctfst.c
    ${JPEG_SOURCE_DIR}/jidctint.c
    ${JPEG_SOURCE_DIR}/jquant1.c
    ${JPEG_SOURCE_DIR}/jquant2.c
    ${JPEG_SOURCE_DIR}/jutils.c
    ${JPEG_SOURCE_DIR}/jmemmgr.c
    ${JPEG_SOURCE_DIR}/${MEMORYMGR}.c
)

set (JPEG_HDRS
    ${JPEG_SOURCE_DIR}/jdct.h
    ${JPEG_SOURCE_DIR}/jinclude.h
    ${JPEG_SOURCE_DIR}/jmemsys.h
    ${JPEG_SOURCE_DIR}/jpegint.h
    ${JPEG_SOURCE_DIR}/jversion.h
)

set (JPEG_PUBLIC_HEADERS
    ${JPEG_SOURCE_DIR}/jerror.h
    ${JPEG_SOURCE_DIR}/jmorecfg.h
    ${JPEG_SOURCE_DIR}/jpeglib.h
)

add_library (${JPEG_LIB_TARGET} STATIC ${JPEG_SRCS} ${JPEG_HDRS} ${JPEG_PUBLIC_HEADERS})
target_include_directories(${JPEG_LIB_TARGET} PRIVATE "${JPEG_SOURCE_DIR};${CMAKE_BINARY_DIR}")
TARGET_C_PROPERTIES (${JPEG_LIB_TARGET} STATIC)
target_link_libraries (${JPEG_LIB_TARGET} PRIVATE ${LINK_LIBS})
JPEG_SET_LIB_OPTIONS (${JPEG_LIB_TARGET} ${JPEG_LIB_NAME} STATIC)
set_target_properties(${JPEG_LIB_TARGET} PROPERTIES
    PUBLIC_HEADER "${JPEG_PUBLIC_HEADERS}"
    LINKER_LANGUAGE C
    INTERFACE_INCLUDE_DIRECTORIES "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
)
#