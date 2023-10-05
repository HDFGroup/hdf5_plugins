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
if (CMAKE_CXX_COMPILER_LOADED)
  include (CheckIncludeFileCXX)
  include (TestForSTDNamespace)
endif ()

# Check for Darwin (not just Apple - we also want to catch OpenDarwin)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (JPEG_HAVE_DARWIN 1)
endif ()

# Check for Solaris
if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
    set (JPEG_HAVE_SOLARIS 1)
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
  set (JPEG_HAVE_MINGW 1)
  set (WINDOWS 1) # MinGW tries to imitate Windows
  set (CMAKE_REQUIRED_FLAGS "-DWIN32_LEAN_AND_MEAN=1 -DNOGDI=1")
  set (JPEG_HAVE_WINSOCK2_H 1)
  set (HAVE_PROTOTYPES 1) # jpeg
endif ()

if (WIN32 AND NOT MINGW)
  if (NOT UNIX)
    set (WINDOWS 1)
    set (CMAKE_REQUIRED_FLAGS "/DWIN32_LEAN_AND_MEAN=1 /DNOGDI=1")
    if (MSVC)
      set (JPEG_HAVE_VISUAL_STUDIO 1)
    endif ()
  endif ()
endif ()

if (WINDOWS)
  set (JPEG_REQUIRED_LIBRARIES "ws2_32.lib;wsock32.lib")
  set (JPEG_HAVE_WIN32_API 1)
  set (HAVE_STDDEF_H 1)
  set (HAVE_SYS_STAT_H 1)
  set (HAVE_SYS_TYPES_H 1)
  set (HAVE_WINSOCK_H 1)
  set (HAVE_LIBM 1)
  set (HAVE_LIBWS2_32 1)
  set (HAVE_LIBWSOCK32 1)
  set (HAVE_PROTOTYPES 1) # jpeg
endif ()

# ----------------------------------------------------------------------
# END of WINDOWS Hard code Values
# ----------------------------------------------------------------------

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
CHECK_INCLUDE_FILE_CONCAT ("unistd.h"        HAVE_UNISTD_H)
CHECK_INCLUDE_FILE_CONCAT ("sys/stat.h"      HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE_CONCAT ("sys/types.h"     HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE_CONCAT ("stddef.h"        HAVE_STDDEF_H)
CHECK_INCLUDE_FILE_CONCAT ("stdint.h"        HAVE_STDINT_H)
CHECK_INCLUDE_FILE_CONCAT ("stdarg.h"        HAVE_STDARG_H)

# IF the c compiler found stdint, check the C++ as well. On some systems this
# file will be found by C but not C++, only do this test IF the C++ compiler
# has been initialized (e.g. the project also includes some c++)
if (HAVE_STDINT_H AND CMAKE_CXX_COMPILER_LOADED)
  CHECK_INCLUDE_FILE_CXX ("stdint.h" HAVE_STDINT_H_CXX)
  if (NOT HAVE_STDINT_H_CXX)
    set (HAVE_STDINT_H "" CACHE INTERNAL "Have includes HAVE_STDINT_H")
    set (USE_INCLUDES ${USE_INCLUDES} "stdint.h")
  endif ()
endif ()

# Windows
if (NOT CYGWIN)
  CHECK_INCLUDE_FILE_CONCAT ("winsock2.h"      HAVE_WINSOCK_H)
endif ()

#----------------------------------------------------------------
# JPEG special check
#----------------------------------------------------------------
CHECK_TYPE_SIZE ("unsigned char"           UNSIGNED_CHAR)
CHECK_TYPE_SIZE ("unsigned short"          UNSIGNED_SHORT)

CHECK_INCLUDE_FILE_CONCAT ("locale.h"        HAVE_LOCALE_H)

CHECK_INCLUDE_FILE_CONCAT ("string.h"        HAVE_STRING_H)
CHECK_INCLUDE_FILE_CONCAT ("strings.h"       HAVE_STRINGS_H)
CHECK_INCLUDE_FILE_CONCAT ("stdlib.h"        HAVE_STDLIB_H)
CHECK_INCLUDE_FILE_CONCAT ("memory.h"        HAVE_MEMORY_H)
CHECK_INCLUDE_FILE_CONCAT ("dlfcn.h"         HAVE_DLFCN_H)
CHECK_INCLUDE_FILE_CONCAT ("inttypes.h"      HAVE_INTTYPES_H)

#-----------------------------------------------------------------------------
#  Check for the math library "m"
#-----------------------------------------------------------------------------
if (MINGW OR NOT WINDOWS)
  CHECK_LIBRARY_EXISTS_CONCAT ("m" ceil     HAVE_LIBM)
  CHECK_LIBRARY_EXISTS_CONCAT ("ws2_32" WSAStartup  HAVE_LIBWS2_32)
  CHECK_LIBRARY_EXISTS_CONCAT ("wsock32" gethostbyname HAVE_LIBWSOCK32)
endif ()

# _Bool type support
CHECK_INCLUDE_FILE_CONCAT (stdbool.h    HAVE_STDBOOL_H)

# For other tests to use the same libraries
set (JPEG_REQUIRED_LIBRARIES ${JPEG_REQUIRED_LIBRARIES} ${LINK_LIBS})

set (USE_INCLUDES "")
if (WINDOWS)
  set (USE_INCLUDES ${USE_INCLUDES} "windows.h")
endif ()

#-----------------------------------------------------------------------------
#  Check for large file support
#-----------------------------------------------------------------------------

# The linux-lfs option is deprecated.
set (LINUX_LFS 0)

set (JPEG_EXTRA_FLAGS)
if (MINGW OR NOT WINDOWS)
  # Might want to check explicitly for Linux and possibly Cygwin
  # instead of checking for not Solaris or Darwin.
  if (NOT JPEG_HAVE_SOLARIS AND NOT JPEG_HAVE_DARWIN)
  # Linux Specific flags
#  set (JPEG_EXTRA_FLAGS -D_POSIX_SOURCE -D_DEFAULT_SOURCE)
  option (JPEG_ENABLE_LARGE_FILE "Enable support for large (64-bit) files on Linux." ON)
  if (JPEG_ENABLE_LARGE_FILE AND NOT DEFINED TEST_LFS_WORKS_RUN)
    set (msg "Performing TEST_LFS_WORKS")
    set (TEST_LFS_WORKS 1 CACHE INTERNAL ${msg})
    set (LARGEFILE 1)
    set (JPEG_EXTRA_FLAGS ${JPEG_EXTRA_FLAGS} -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE)
    message (STATUS "${msg} with presets... yes")
  endif ()
  endif ()
  set (CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} ${JPEG_EXTRA_FLAGS})
endif ()

add_definitions (${JPEG_EXTRA_FLAGS})

#-----------------------------------------------------------------------------
# Check for some functions that are used
#
CHECK_FUNCTION_EXISTS (void              HAVE_VOID)
CHECK_FUNCTION_EXISTS (const             HAVE_CONST)

