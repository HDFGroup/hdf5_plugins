#
# Copyright by The HDF Group.
# All rights reserved.
#
# This file is part of HDF5.  The full HDF5 copyright notice, including
# terms governing use, modification, and redistribution, is contained in
# the COPYING file, which can be found at the root of the source code
# distribution tree, or in https://www.hdfgroup.org/licenses.
# If you do not have access to either file, you may request a copy from
# help@hdfgroup.org.
#
#-----------------------------------------------------------------------------
# Include all the necessary files for macros
#-----------------------------------------------------------------------------
include (CheckFunctionExists)
include (CheckIncludeFile)
include (CheckIncludeFiles)
include (CheckLibraryExists)
include (CheckSymbolExists)
include (CheckTypeSize)
include (CheckVariableExists)
include (TestBigEndian)

# Check for Darwin (not just Apple - we also want to catch OpenDarwin)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (HAVE_DARWIN 1)
endif ()

# Check for Solaris
if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
    set (HAVE_SOLARIS 1)
endif ()

#-----------------------------------------------------------------------------
# This MACRO checks IF the symbol exists in the library and IF it
# does, it appends library to the list.
#-----------------------------------------------------------------------------
set (H5PL_LINK_LIBS "")
macro (CHECK_LIBRARY_EXISTS_CONCAT LIBRARY SYMBOL VARIABLE)
  CHECK_LIBRARY_EXISTS ("${LIBRARY};${H5PL_LINK_LIBS}" ${SYMBOL} "" ${VARIABLE})
  if (${VARIABLE})
    set (H5PL_LINK_LIBS ${H5PL_LINK_LIBS} ${LIBRARY})
  endif ()
endmacro ()

# ----------------------------------------------------------------------
# WINDOWS Hard code Values
# ----------------------------------------------------------------------
set (WINDOWS)

if (MINGW)
  set (HAVE_MINGW 1)
  set (WINDOWS 1) # MinGW tries to imitate Windows
  set (CMAKE_REQUIRED_FLAGS "-DWIN32_LEAN_AND_MEAN=1 -DNOGDI=1")
  set (HAVE_WINSOCK2_H 1)
endif ()

if (WIN32 AND NOT MINGW)
  if (NOT UNIX)
    set (WINDOWS 1)
    set (CMAKE_REQUIRED_FLAGS "/DWIN32_LEAN_AND_MEAN=1 /DNOGDI=1")
    if (MSVC)
      set (HAVE_VISUAL_STUDIO 1)
    endif ()
  endif ()
endif ()

if (WINDOWS)
  set (HDF_REQUIRED_LIBRARIES "ws2_32.lib;wsock32.lib")
  set (HAVE_WIN32_API 1)
  set (HAVE_LIBM 1)
  set (HAVE_STRDUP 1)
  set (HAVE_SYSTEM 1)
  set (HAVE_LONGJMP 1)
  if (NOT MINGW)
    set (HAVE_GETHOSTNAME 1)
    set (HAVE_FUNCTION 1)
  endif ()
  if (NOT UNIX AND NOT CYGWIN)
    set (HAVE_LIBWS2_32 1)
    set (HAVE_LIBWSOCK32 1)
  endif ()
endif ()

# ----------------------------------------------------------------------
# END of WINDOWS Hard code Values
# ----------------------------------------------------------------------

if (NOT WINDOWS)
  TEST_BIG_ENDIAN (WORDS_BIGENDIAN)
endif ()

#-----------------------------------------------------------------------------
# Check IF header file exists and add it to the list.
#-----------------------------------------------------------------------------
macro (CHECK_INCLUDE_FILE_CONCAT FILE VARIABLE)
  CHECK_INCLUDE_FILES ("${USE_INCLUDES};${FILE}" ${VARIABLE})
  if (${VARIABLE})
    set (USE_INCLUDES ${USE_INCLUDES} ${FILE})
  endif ()
endmacro ()

#-----------------------------------------------------------------------------
#  Check for the existence of certain header files
#-----------------------------------------------------------------------------
CHECK_INCLUDE_FILE_CONCAT ("sys/stat.h"      HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE_CONCAT ("sys/types.h"     HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE_CONCAT ("stddef.h"        HAVE_STDDEF_H)
CHECK_INCLUDE_FILE_CONCAT ("stdint.h"        HAVE_STDINT_H)
CHECK_INCLUDE_FILE_CONCAT ("unistd.h"        HAVE_UNISTD_H)

# Windows
CHECK_INCLUDE_FILE_CONCAT ("io.h"            HAVE_IO_H)
if (NOT CYGWIN)
  CHECK_INCLUDE_FILE_CONCAT ("winsock2.h"      HAVE_WINSOCK_H)
endif ()

CHECK_INCLUDE_FILE_CONCAT ("pthread.h"       HAVE_PTHREAD_H)
CHECK_INCLUDE_FILE_CONCAT ("string.h"        HAVE_STRING_H)
CHECK_INCLUDE_FILE_CONCAT ("strings.h"       HAVE_STRINGS_H)
CHECK_INCLUDE_FILE_CONCAT ("stdlib.h"        HAVE_STDLIB_H)
CHECK_INCLUDE_FILE_CONCAT ("memory.h"        HAVE_MEMORY_H)
CHECK_INCLUDE_FILE_CONCAT ("dlfcn.h"         HAVE_DLFCN_H)
CHECK_INCLUDE_FILE_CONCAT ("fcntl.h"         HAVE_FCNTL_H)
CHECK_INCLUDE_FILE_CONCAT ("inttypes.h"      HAVE_INTTYPES_H)

# _Bool type support
CHECK_INCLUDE_FILE_CONCAT (stdbool.h    HAVE_STDBOOL_H)

#-----------------------------------------------------------------------------
#  Check for the math library "m"
#-----------------------------------------------------------------------------
if (MINGW OR NOT WINDOWS)
  CHECK_LIBRARY_EXISTS_CONCAT ("m" ceil                HAVE_LIBM)
  CHECK_LIBRARY_EXISTS_CONCAT ("dl" dlopen             HAVE_LIBDL)
  CHECK_LIBRARY_EXISTS_CONCAT ("ws2_32" WSAStartup     HAVE_LIBWS2_32)
  CHECK_LIBRARY_EXISTS_CONCAT ("wsock32" gethostbyname HAVE_LIBWSOCK32)
endif ()

# UCB (BSD) compatibility library
CHECK_LIBRARY_EXISTS_CONCAT ("ucb"    gethostname  HAVE_LIBUCB)

# For other tests to use the same libraries
set (HDF_REQUIRED_LIBRARIES ${HDF_REQUIRED_LIBRARIES} ${H5PL_LINK_LIBS})

set (USE_INCLUDES "")
if (WINDOWS)
  set (USE_INCLUDES ${USE_INCLUDES} "windows.h")
endif ()

# For other specific tests, use this MACRO.
macro (HDF_FUNCTION_TEST OTHER_TEST)
  if (NOT DEFINED ${OTHER_TEST})
    set (MACRO_CHECK_FUNCTION_DEFINITIONS "-D${OTHER_TEST} ${CMAKE_REQUIRED_FLAGS}")

    foreach (def
        HAVE_UNISTD_H
        HAVE_SYS_TYPES_H
    )
      if ("${${def}}")
        set (MACRO_CHECK_FUNCTION_DEFINITIONS "${MACRO_CHECK_FUNCTION_DEFINITIONS} -D${def}")
      endif ()
    endforeach ()

    if (LARGEFILE)
      set (MACRO_CHECK_FUNCTION_DEFINITIONS
          "${MACRO_CHECK_FUNCTION_DEFINITIONS} -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE"
      )
    endif ()

    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15.0")
      message (TRACE "Performing ${OTHER_TEST}")
    endif ()
    try_compile (${OTHER_TEST}
        ${CMAKE_BINARY_DIR}
        ${H5LZF_RESOURCES_DIR}/H5PLTests.c
        COMPILE_DEFINITIONS "${MACRO_CHECK_FUNCTION_DEFINITIONS}"
        LINK_LIBRARIES "${HDF_REQUIRED_LIBRARIES}"
        OUTPUT_VARIABLE OUTPUT
    )
    if (${OTHER_TEST})
      set (${OTHER_TEST} 1 CACHE INTERNAL "Other test ${FUNCTION}")
      if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15.0")
        message (VERBOSE "Performing Other Test ${OTHER_TEST} - Success")
      endif ()
    else ()
      if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15.0")
        message (VERBOSE "Performing Other Test ${OTHER_TEST} - Failed")
      endif ()
      set (${OTHER_TEST} "" CACHE INTERNAL "Other test ${FUNCTION}")
      file (APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
          "Performing Other Test ${OTHER_TEST} failed with the following output:\n"
          "${OUTPUT}\n"
      )
    endif ()
  endif ()
endmacro ()

#-----------------------------------------------------------------------------
# Check for these functions before the time headers are checked
#-----------------------------------------------------------------------------
HDF_FUNCTION_TEST (STDC_HEADERS)

#-----------------------------------------------------------------------------
#  Check for large file support
#-----------------------------------------------------------------------------

# The linux-lfs option is deprecated.
set (LINUX_LFS 0)

set (HDF_EXTRA_C_FLAGS)
set (HDF_EXTRA_FLAGS)
if (MINGW OR NOT WINDOWS)
  # Might want to check explicitly for Linux and possibly Cygwin
  # instead of checking for not Solaris or Darwin.
  if (NOT HAVE_SOLARIS AND NOT HAVE_DARWIN)
  # Linux Specific flags
  # This was originally defined as _POSIX_SOURCE which was updated to
  # _POSIX_C_SOURCE=199506L to expose a greater amount of POSIX
  # functionality so clock_gettime and CLOCK_MONOTONIC are defined
  # correctly. This was later updated to 200112L so that
  # posix_memalign() is visible for the direct VFD code on Linux
  # systems.
  # POSIX feature information can be found in the gcc manual at:
  # http://www.gnu.org/s/libc/manual/html_node/Feature-Test-Macros.html
  set (HDF_EXTRA_C_FLAGS -D_POSIX_C_SOURCE=200809L)

  option (HDF_ENABLE_LARGE_FILE "Enable support for large (64-bit) files on Linux." ON)
  if (HDF_ENABLE_LARGE_FILE AND NOT DEFINED TEST_LFS_WORKS_RUN)
    set (msg "Performing TEST_LFS_WORKS")
    try_run (TEST_LFS_WORKS_RUN   TEST_LFS_WORKS_COMPILE
        ${CMAKE_BINARY_DIR}
        ${H5LZF_RESOURCES_DIR}/H5PLTests.c
        COMPILE_DEFINITIONS "-DTEST_LFS_WORKS"
    )

    # The LARGEFILE definitions were from the transition period
    # and are probably no longer needed. The FILE_OFFSET_BITS
    # check should be generalized for all POSIX systems as it
    # is in the Autotools.
    if (TEST_LFS_WORKS_COMPILE)
      if (TEST_LFS_WORKS_RUN MATCHES 0)
        set (TEST_LFS_WORKS 1 CACHE INTERNAL ${msg})
        set (LARGEFILE 1)
        set (HDF_EXTRA_FLAGS ${HDF_EXTRA_FLAGS} -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE)
        if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15.0")
          message (VERBOSE "${msg}... yes")
        endif ()
      else ()
        set (TEST_LFS_WORKS "" CACHE INTERNAL ${msg})
        if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15.0")
          message (VERBOSE "${msg}... no")
        endif ()
        file (APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
              "Test TEST_LFS_WORKS Run failed with the following exit code:\n ${TEST_LFS_WORKS_RUN}\n"
        )
      endif ()
    else ()
      set (TEST_LFS_WORKS "" CACHE INTERNAL ${msg})
      if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15.0")
          message (VERBOSE "${msg}... no")
      endif ()
      file (APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
          "Test TEST_LFS_WORKS Compile failed\n"
      )
    endif ()
  endif ()
  set (CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} ${HDF_EXTRA_FLAGS})
  endif ()
endif ()

#-----------------------------------------------------------------------------
#  Check the size in bytes of all the int and float types
#-----------------------------------------------------------------------------
macro (HDF_CHECK_TYPE_SIZE type var)
  set (aType ${type})
  set (aVar  ${var})
  if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15.0")
    message (TRACE "Checking size of ${aType} and storing into ${aVar}")
  endif ()
  CHECK_TYPE_SIZE (${aType}   ${aVar})
  if (NOT ${aVar})
    set (${aVar} 0 CACHE INTERNAL "SizeOf for ${aType}")
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15.0")
      message (TRACE "Size of ${aType} was NOT Found")
    endif ()
  endif ()
endmacro ()

HDF_CHECK_TYPE_SIZE (char           SIZEOF_CHAR)
HDF_CHECK_TYPE_SIZE (short          SIZEOF_SHORT)
HDF_CHECK_TYPE_SIZE (int            SIZEOF_INT)
HDF_CHECK_TYPE_SIZE (unsigned       SIZEOF_UNSIGNED)
if (NOT APPLE)
  HDF_CHECK_TYPE_SIZE (long         SIZEOF_LONG)
endif ()
HDF_CHECK_TYPE_SIZE ("long long"    SIZEOF_LONG_LONG)
HDF_CHECK_TYPE_SIZE (__int64        SIZEOF___INT64)
if (NOT SIZEOF___INT64)
  set (SIZEOF___INT64 0)
endif ()

HDF_CHECK_TYPE_SIZE (float          SIZEOF_FLOAT)
HDF_CHECK_TYPE_SIZE (double         SIZEOF_DOUBLE)
HDF_CHECK_TYPE_SIZE ("long double"  SIZEOF_LONG_DOUBLE)

HDF_CHECK_TYPE_SIZE (int8_t         SIZEOF_INT8_T)
HDF_CHECK_TYPE_SIZE (uint8_t        SIZEOF_UINT8_T)
HDF_CHECK_TYPE_SIZE (int_least8_t   SIZEOF_INT_LEAST8_T)
HDF_CHECK_TYPE_SIZE (uint_least8_t  SIZEOF_UINT_LEAST8_T)
HDF_CHECK_TYPE_SIZE (int_fast8_t    SIZEOF_INT_FAST8_T)
HDF_CHECK_TYPE_SIZE (uint_fast8_t   SIZEOF_UINT_FAST8_T)

HDF_CHECK_TYPE_SIZE (int16_t        SIZEOF_INT16_T)
HDF_CHECK_TYPE_SIZE (uint16_t       SIZEOF_UINT16_T)
HDF_CHECK_TYPE_SIZE (int_least16_t  SIZEOF_INT_LEAST16_T)
HDF_CHECK_TYPE_SIZE (uint_least16_t SIZEOF_UINT_LEAST16_T)
HDF_CHECK_TYPE_SIZE (int_fast16_t   SIZEOF_INT_FAST16_T)
HDF_CHECK_TYPE_SIZE (uint_fast16_t  SIZEOF_UINT_FAST16_T)

HDF_CHECK_TYPE_SIZE (int32_t        SIZEOF_INT32_T)
HDF_CHECK_TYPE_SIZE (uint32_t       SIZEOF_UINT32_T)
HDF_CHECK_TYPE_SIZE (int_least32_t  SIZEOF_INT_LEAST32_T)
HDF_CHECK_TYPE_SIZE (uint_least32_t SIZEOF_UINT_LEAST32_T)
HDF_CHECK_TYPE_SIZE (int_fast32_t   SIZEOF_INT_FAST32_T)
HDF_CHECK_TYPE_SIZE (uint_fast32_t  SIZEOF_UINT_FAST32_T)

HDF_CHECK_TYPE_SIZE (int64_t        SIZEOF_INT64_T)
HDF_CHECK_TYPE_SIZE (uint64_t       SIZEOF_UINT64_T)
HDF_CHECK_TYPE_SIZE (int_least64_t  SIZEOF_INT_LEAST64_T)
HDF_CHECK_TYPE_SIZE (uint_least64_t SIZEOF_UINT_LEAST64_T)
HDF_CHECK_TYPE_SIZE (int_fast64_t   SIZEOF_INT_FAST64_T)
HDF_CHECK_TYPE_SIZE (uint_fast64_t  SIZEOF_UINT_FAST64_T)

if (NOT APPLE)
  HDF_CHECK_TYPE_SIZE (size_t       SIZEOF_SIZE_T)
  HDF_CHECK_TYPE_SIZE (ssize_t      SIZEOF_SSIZE_T)
  if (NOT SIZEOF_SSIZE_T)
    set (SIZEOF_SSIZE_T 0)
  endif ()
  if (MINGW OR NOT WINDOWS)
    HDF_CHECK_TYPE_SIZE (ptrdiff_t    SIZEOF_PTRDIFF_T)
  endif ()
endif ()

HDF_CHECK_TYPE_SIZE (off_t          SIZEOF_OFF_T)
HDF_CHECK_TYPE_SIZE (off64_t        SIZEOF_OFF64_T)
if (NOT SIZEOF_OFF64_T)
  set (SIZEOF_OFF64_T 0)
endif ()
HDF_CHECK_TYPE_SIZE (time_t          SIZEOF_TIME_T)

#-----------------------------------------------------------------------------
# Extra C99 types
#-----------------------------------------------------------------------------

# _Bool type support
if (HAVE_STDBOOL_H)
  set (CMAKE_EXTRA_INCLUDE_FILES stdbool.h)
  HDF_CHECK_TYPE_SIZE (bool         SIZEOF_BOOL)
else ()
  HDF_CHECK_TYPE_SIZE (_Bool        SIZEOF_BOOL)
endif ()

#-----------------------------------------------------------------------------
# Check for some functions that are used
#
CHECK_FUNCTION_EXISTS (vprintf               HAVE_VPRINTF)
CHECK_FUNCTION_EXISTS (_doprnt               HAVE_DOPRNT)
CHECK_FUNCTION_EXISTS (memset                HAVE_MEMSET)

if (MINGW OR NOT WINDOWS)
  foreach (test
      HAVE_ATTRIBUTE
      SYSTEM_SCOPE_THREADS
  )
    HDF_FUNCTION_TEST (${test})
  endforeach ()
endif ()
