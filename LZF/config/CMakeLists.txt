cmake_minimum_required (VERSION 3.18)
PROJECT (LZF C)

#-----------------------------------------------------------------------------
# Basic LZF stuff here
#-----------------------------------------------------------------------------

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
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set (CMAKE_C_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_C_FLAGS} -std=c99 -fomit-frame-pointer -finline-functions -fno-common")
endif ()
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set (CMAKE_CXX_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -fno-common")
endif ()

#-----------------------------------------------------------------------------
# This is in here to help some of the GCC based IDES like Eclipse
# and code blocks parse the compiler errors and warnings better.
#-----------------------------------------------------------------------------
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
endif ()
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
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

if (MSVC)
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|x86_64)$")
    set_target_properties(${ZLIB_LIB_TARGET} PROPERTIES STATIC_LIBRARY_FLAGS "/machine:x64")
  elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64")
    set_target_properties(${ZLIB_LIB_TARGET} PROPERTIES STATIC_LIBRARY_FLAGS "/machine:ARM64")
  endif()
endif ()
TARGET_C_PROPERTIES (${LZF_LIB_TARGET} STATIC)
target_link_libraries (${LZF_LIB_TARGET} ${H5PL_LINK_LIBS})
HDF_IMPORT_SET_LIB_OPTIONS (${LZF_LIB_TARGET} ${LZF_LIB_NAME} STATIC "")
set_target_properties(${LZF_LIB_TARGET} PROPERTIES
    PUBLIC_HEADER "${LZF_PUBLIC_HEADERS}"
    INTERFACE_INCLUDE_DIRECTORIES "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
)

#-----------------------------------------------------------------------------
# Generate the config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${LZF_SOURCE_DIR}/config.h.in ${LZF_BINARY_DIR}/config.h @ONLY)

#-----------------------------------------------------------------------------
# Set includes needed for build
#-----------------------------------------------------------------------------
set (LZF_INCLUDES_BUILD_TIME
    ${LZF_SOURCE_DIR} ${LZF_BINARY_DIR}
)
