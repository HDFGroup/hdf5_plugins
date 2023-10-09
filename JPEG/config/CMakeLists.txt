cmake_minimum_required (VERSION 3.18)
PROJECT (JPEG C)

#-----------------------------------------------------------------------------
# Basic JPEG stuff here
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
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
file (READ ${JPEG_SOURCE_DIR}/jpeglib.h _jpeglib_h_contents)
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
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
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
configure_file (${JPEG_SOURCE_DIR}/jconfig.h.in ${JPEG_BINARY_DIR}/jconfig.h @ONLY)

#-----------------------------------------------------------------------------
# Set includes needed for build
#-----------------------------------------------------------------------------
set (JPEG_INCLUDES_BUILD_TIME
    ${JPEG_SOURCE_DIR} ${JPEG_BINARY_DIR}
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
HDF_IMPORT_SET_LIB_OPTIONS (${JPEG_LIB_TARGET} ${JPEG_LIB_NAME} STATIC "")
set_target_properties(${JPEG_LIB_TARGET} PROPERTIES
    PUBLIC_HEADER "${JPEG_PUBLIC_HEADERS}"
    LINKER_LANGUAGE C
    INTERFACE_INCLUDE_DIRECTORIES "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
)
#