cmake_minimum_required (VERSION 3.14)
PROJECT (BZ2 C)

#-----------------------------------------------------------------------------
# Basic BZ2 stuff here
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Set the core names of all the libraries
#-----------------------------------------------------------------------------
set (BZ2_LIB_CORENAME              "bz2")

#-----------------------------------------------------------------------------
# Set the true names of all the libraries if customized by external project
#-----------------------------------------------------------------------------
set (BZ2_LIB_NAME              "${BZ2_EXTERNAL_LIB_PREFIX}${BZ2_LIB_CORENAME}")

#-----------------------------------------------------------------------------
# Set the target names of all the libraries
#-----------------------------------------------------------------------------
set (BZ2_LIB_TARGET              "${BZ2_LIB_CORENAME}-static")

if (NOT BZ2_INSTALL_BIN_DIR)
  set (BZ2_INSTALL_BIN_DIR bin)
endif ()
if (NOT BZ2_INSTALL_LIB_DIR)
  set (BZ2_INSTALL_LIB_DIR lib)
endif ()
if (NOT BZ2_INSTALL_INCLUDE_DIR)
  set (BZ2_INSTALL_INCLUDE_DIR include)
endif ()
if (NOT BZ2_INSTALL_DATA_DIR)
  if (NOT WIN32)
    if (APPLE)
      if (BZ2_BUILD_FRAMEWORKS)
        set (BZ2_INSTALL_EXTRA_DIR ../SharedSupport)
      else ()
        set (BZ2_INSTALL_EXTRA_DIR share)
      endif ()
      set (BZ2_INSTALL_FWRK_DIR ${CMAKE_INSTALL_FRAMEWORK_PREFIX})
    endif ()
    set (BZ2_INSTALL_DATA_DIR share)
    set (BZ2_INSTALL_CMAKE_DIR share/cmake)
  else ()
    set (BZ2_INSTALL_DATA_DIR ".")
    set (BZ2_INSTALL_CMAKE_DIR cmake)
  endif ()
endif ()

set (BZ2_FULL_VERSION "1.0.8")
set (BZ2_VERS_MAJOR "1")
set (BZ2_VERS_MINOR "0")
set (BZ2_VERS_RELEASE "8")
message (STATUS "VERSION: ${BZ2_FULL_VERSION}")
set (BZ2_SOVERS_MAJOR 1)
set (BZ2_SOVERS_MINOR 0)
set (BZ2_SOVERS_RELEASE 8)

#-----------------------------------------------------------------------------
set (BZ2_PACKAGE "bz2")
set (BZ2_PACKAGE_NAME "bz2")
set (BZ2_PACKAGE_VERSION "${BZ2_VERS_MAJOR}.${BZ2_VERS_MINOR}.${BZ2_VERS_RELEASE}")
set (BZ2_PACKAGE_VERSION_STRING "${BZ2_PACKAGE_VERSION}")
set (BZ2_PACKAGE_VERSION_MAJOR "${BZ2_VERS_MAJOR}")
set (BZ2_PACKAGE_VERSION_MINOR "${BZ2_VERS_MINOR}")
set (BZ2_PACKAGE_STRING "${BZ2_PACKAGE_NAME} ${BZ2_PACKAGE_VERSION}")
set (BZ2_PACKAGE_TARNAME "${BZ2_PACKAGE_NAME}{BZ2_PACKAGE_EXT}")
set (BZ2_PACKAGE_URL "http://www.hdfgroup.org")
set (BZ2_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (BZ2_PACKAGE_SOVERSION "${BZ2_SOVERS_MAJOR}.${BZ2_SOVERS_MINOR}.${BZ2_SOVERS_RELEASE}")
set (BZ2_PACKAGE_SOVERSION_MAJOR "${BZ2_SOVERS_MAJOR}")

#-----------------------------------------------------------------------------
# Option to Build Shared and Static libs, default is shared
#-----------------------------------------------------------------------------
option (BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

#-----------------------------------------------------------------------------
# bzip2 Library Settings
#-----------------------------------------------------------------------------
if (WIN32)
  set (BUILD_BZ2 1)
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
# All libs/tests/utils need the main include directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${BZ2_BINARY_DIR} ${BZ2_SOURCE_DIR} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#-----------------------------------------------------------------------------
# Set includes needed for build
#-----------------------------------------------------------------------------
set (BZ2_INCLUDES_BUILD_TIME
    ${BZ2_SOURCE_DIR} ${BZ2_BINARY_DIR}
)

#-----------------------------------------------------------------------------
# Define BZ2 Library
#-----------------------------------------------------------------------------
set (BZ2_SRCS
      ${BZ2_SOURCE_DIR}/blocksort.c
      ${BZ2_SOURCE_DIR}/huffman.c
      ${BZ2_SOURCE_DIR}/crctable.c
      ${BZ2_SOURCE_DIR}/randtable.c
      ${BZ2_SOURCE_DIR}/compress.c
      ${BZ2_SOURCE_DIR}/decompress.c
      ${BZ2_SOURCE_DIR}/bzlib.c
)
set (BZ2_PUBLIC_HEADERS
      ${BZ2_SOURCE_DIR}/bzlib.h
)

add_library (${BZ2_LIB_TARGET} STATIC ${BZ2_SRCS} ${BZ2_PUBLIC_HEADERS})
target_include_directories(${BZ2_LIB_TARGET} PRIVATE "${BZ2_SOURCE_DIR};${CMAKE_BINARY_DIR}")
TARGET_C_PROPERTIES (${BZ2_LIB_TARGET} STATIC)
target_link_libraries (${BZ2_LIB_TARGET} PRIVATE ${LINK_LIBS})
HDF_IMPORT_SET_LIB_OPTIONS (${BZ2_LIB_TARGET} ${BZ2_LIB_NAME} STATIC "")
set_target_properties(${BZ2_LIB_TARGET} PROPERTIES
    PUBLIC_HEADER "${BZ2_PUBLIC_HEADERS}"
    LINKER_LANGUAGE C
    INTERFACE_INCLUDE_DIRECTORIES "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
)
