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
include (CheckStructHasMember)

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
set (LINK_LIBS "")
macro (CHECK_LIBRARY_EXISTS_CONCAT LIBRARY SYMBOL VARIABLE)
  CHECK_LIBRARY_EXISTS ("${LIBRARY};${LINK_LIBS}" ${SYMBOL} "" ${VARIABLE})
  if (${VARIABLE})
    set (LINK_LIBS ${LINK_LIBS} ${LIBRARY})
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
CHECK_INCLUDE_FILE_CONCAT ("sys/file.h"      HAVE_SYS_FILE_H)
CHECK_INCLUDE_FILE_CONCAT ("sys/ioctl.h"     HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILE_CONCAT ("sys/resource.h"  HAVE_SYS_RESOURCE_H)
CHECK_INCLUDE_FILE_CONCAT ("sys/socket.h"    HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILE_CONCAT ("sys/stat.h"      HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE_CONCAT ("sys/time.h"      HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE_CONCAT ("sys/types.h"     HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE_CONCAT ("features.h"      HAVE_FEATURES_H)
CHECK_INCLUDE_FILE_CONCAT ("dirent.h"        HAVE_DIRENT_H)
CHECK_INCLUDE_FILE_CONCAT ("stddef.h"        HAVE_STDDEF_H)
CHECK_INCLUDE_FILE_CONCAT ("stdint.h"        HAVE_STDINT_H)
CHECK_INCLUDE_FILE_CONCAT ("unistd.h"        HAVE_UNISTD_H)

# Darwin
CHECK_INCLUDE_FILE_CONCAT ("mach/mach_time.h" ${HDF_PREFIX}_HAVE_MACH_MACH_TIME_H)

# Windows
CHECK_INCLUDE_FILE_CONCAT ("io.h"            HAVE_IO_H)
if (NOT CYGWIN)
  CHECK_INCLUDE_FILE_CONCAT ("winsock2.h"      HAVE_WINSOCK_H)
endif ()
CHECK_INCLUDE_FILE_CONCAT ("sys/timeb.h"     ${HDF_PREFIX}_HAVE_SYS_TIMEB_H)

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
set (HDF_REQUIRED_LIBRARIES ${HDF_REQUIRED_LIBRARIES} ${LINK_LIBS})

set (USE_INCLUDES "")
if (WINDOWS)
  set (USE_INCLUDES ${USE_INCLUDES} "windows.h")
endif ()

# For other specific tests, use this MACRO.
macro (HDF_FUNCTION_TEST OTHER_TEST)
  if (NOT DEFINED ${OTHER_TEST})
    set (MACRO_CHECK_FUNCTION_DEFINITIONS "-D${OTHER_TEST} ${CMAKE_REQUIRED_FLAGS}")

    foreach (def
        HAVE_SYS_TIME_H
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
        ${H5SZ_RESOURCES_DIR}/H5PLTests.c
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
        ${H5SZ_RESOURCES_DIR}/H5PLTests.c
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

add_definitions (${HDF_EXTRA_FLAGS})

#-----------------------------------------------------------------------------
# Check for some functions that are used
#
CHECK_FUNCTION_EXISTS (vprintf               HAVE_VPRINTF)
CHECK_FUNCTION_EXISTS (_doprnt               HAVE_DOPRNT)
CHECK_FUNCTION_EXISTS (memset                HAVE_MEMSET)

if (MINGW OR NOT WINDOWS)

  # ----------------------------------------------------------------------
  # Check for MONOTONIC_TIMER support (used in clock_gettime).  This has
  # to be done after any POSIX/BSD defines to ensure that the test gets
  # the correct POSIX level on linux.
  CHECK_VARIABLE_EXISTS (CLOCK_MONOTONIC HAVE_CLOCK_MONOTONIC)

  #-----------------------------------------------------------------------------
  # Check a bunch of time functions
  #-----------------------------------------------------------------------------
  CHECK_STRUCT_HAS_MEMBER("struct timespec" tv_sec "time.h" HAVE_TIMESPEC)
  CHECK_STRUCT_HAS_MEMBER("struct tm" tm_gmtoff "time.h" HAVE_TM_GMTOFF)
  CHECK_STRUCT_HAS_MEMBER("struct tm" __tm_gmtoff "time.h" HAVE___TM_GMTOFF)
  CHECK_STRUCT_HAS_MEMBER("struct tm" tm_sec "sys/types.h;sys/time.h;time.h" TIME_WITH_SYS_TIME)
  if (HAVE_SYS_TIME_H)
    CHECK_STRUCT_HAS_MEMBER("struct tm" tz_minuteswest "sys/types.h;sys/time.h;time.h" HAVE_STRUCT_TIMEZONE)
  else ()
    CHECK_STRUCT_HAS_MEMBER("struct tm" tz_minuteswest "sys/types.h;time.h" HAVE_STRUCT_TIMEZONE)
  endif ()
  CHECK_FUNCTION_EXISTS (clock_gettime      HAVE_CLOCK_GETTIME)

  foreach (test
      HAVE_ATTRIBUTE
      SYSTEM_SCOPE_THREADS
  )
    HDF_FUNCTION_TEST (${test})
  endforeach ()
endif ()

#-----------------------------------------------------------------------------
# Configure Checks which require CXX compilation must go here
#-----------------------------------------------------------------------------
if (CMAKE_CXX_COMPILER_LOADED)

  include (CheckIncludeFileCXX)
  include (TestForSTDNamespace)

  # IF the c compiler found stdint, check the C++ as well. On some systems this
  # file will be found by C but not C++, only do this test IF the C++ compiler
  # has been initialized (e.g. the project also includes some c++)
  if (HAVE_STDINT_H)
    CHECK_INCLUDE_FILE_CXX ("stdint.h" HAVE_STDINT_H_CXX)
    if (NOT HAVE_STDINT_H_CXX)
      set (HAVE_STDINT_H "" CACHE INTERNAL "Have includes HAVE_STDINT_H")
      set (USE_INCLUDES ${USE_INCLUDES} "stdint.h")
    endif ()
  endif ()

endif ()

