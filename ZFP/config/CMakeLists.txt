cmake_minimum_required(VERSION 3.18)

# Enable MACOSX_RPATH by default
cmake_policy(SET CMP0042 NEW)

# Fail immediately if not using an out-of-source build
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
  message(FATAL_ERROR
    "In-source builds are not supported.  Please create a build directory "
    "separate from the source directory")
endif()

#------------------------------------------------------------------------------#
# Parse version number from zfp.h
#------------------------------------------------------------------------------#
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/include/zfp/version.h _zfp_h_contents)
string(REGEX REPLACE ".*#define[ \t]+ZFP_VERSION_MAJOR[ \t]+([0-9]+).*"
     "\\1" ZFP_VERSION_MAJOR ${_zfp_h_contents})
string(REGEX REPLACE ".*#define[ \t]+ZFP_VERSION_MINOR[ \t]+([0-9]+).*"
    "\\1" ZFP_VERSION_MINOR ${_zfp_h_contents})
string(REGEX REPLACE ".*#define[ \t]+ZFP_VERSION_PATCH[ \t]+([0-9]+).*"
    "\\1" ZFP_VERSION_PATCH ${_zfp_h_contents})
string(REGEX REPLACE ".*#define[ \t]+ZFP_VERSION_TWEAK[ \t]+([0-9]+).*"
    "\\1" ZFP_VERSION_TWEAK ${_zfp_h_contents})

if(${ZFP_VERSION_TWEAK} EQUAL 0)
  set(ZFP_VERSION
    "${ZFP_VERSION_MAJOR}.${ZFP_VERSION_MINOR}.${ZFP_VERSION_PATCH}")
else()
  set(ZFP_VERSION
    "${ZFP_VERSION_MAJOR}.${ZFP_VERSION_MINOR}.${ZFP_VERSION_PATCH}.${ZFP_VERSION_TWEAK}")
endif()

project(ZFP VERSION ${ZFP_VERSION})

#------------------------------------------------------------------------------#
# Some boilerplate to setup nice output directories
#------------------------------------------------------------------------------#
include(GNUInstallDirs)
set(CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/zfp
  CACHE STRING "Installation CMake subdirectory")

list(INSERT CMAKE_MODULE_PATH 0 "${ZFP_SOURCE_DIR}/cmake")
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ZFP_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ZFP_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ZFP_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
endif()

#------------------------------------------------------------------------------#
# Top level options
#------------------------------------------------------------------------------#

# Windows (Visual Studio) specific options
if(MSVC)
  # Use this to get a usable export library when building a DLL on Windows
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

  # Silence extraneous Visual Studio specific warnings
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
  add_compile_options(/wd4146)
  add_compile_options(/wd4305)
endif()

# Suggest C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

if(MSVC OR MINGW)
  set(CMAKE_C_STANDARD 90)
endif()

message(STATUS "Compiling with C standard: ${CMAKE_C_STANDARD}")

# Suggest C++98
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 98)
endif()
message(STATUS "Compiling with C++ standard: ${CMAKE_CXX_STANDARD}")

include(CMakeDependentOption)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Compile-time options.

set(ZFP_BIT_STREAM_WORD_SIZE 64 CACHE STRING
  "Use smaller bit stream word type for finer rate granularity")
set_property(CACHE ZFP_BIT_STREAM_WORD_SIZE PROPERTY STRINGS "8;16;32;64")

if(CMAKE_C_COMPILER_ID MATCHES "PGI|NVHPC")
  # Use default alignment to address PGI compiler bug.
  set(ZFP_CACHE_LINE_SIZE 0 CACHE STRING "Cache line alignment in bytes")
  mark_as_advanced(ZFP_CACHE_LINE_SIZE)
endif()

set(PPM_CHROMA 2 CACHE STRING "Chroma block dimensionality for ppm example")
set_property(CACHE PPM_CHROMA PROPERTY STRINGS "1;2")

set(ZFP_ROUNDING_MODE ZFP_ROUND_NEVER CACHE STRING
  "Rounding mode for reducing bias")
set_property(CACHE ZFP_ROUNDING_MODE PROPERTY STRINGS "ZFP_ROUND_NEVER;ZFP_ROUND_FIRST;ZFP_ROUND_LAST")

option(ZFP_WITH_DAZ "Treat subnormals as zero to avoid overflow" OFF)

option(ZFP_WITH_CUDA "Enable CUDA parallel compression" OFF)

option(ZFP_WITH_BIT_STREAM_STRIDED "Enable strided access for progressive zfp streams" OFF)
mark_as_advanced(ZFP_WITH_BIT_STREAM_STRIDED)

option(ZFP_WITH_TIGHT_ERROR "Reduce slack in absolute errors" OFF)

option(ZFP_WITH_ALIGNED_ALLOC "Enable aligned memory allocation" OFF)
mark_as_advanced(ZFP_WITH_ALIGNED_ALLOC)

option(ZFP_WITH_CACHE_TWOWAY "Use two-way skew-associative cache" OFF)
mark_as_advanced(ZFP_WITH_CACHE_TWOWAY)

option(ZFP_WITH_CACHE_FAST_HASH
  "Use a faster but more collision prone hash function" OFF)
mark_as_advanced(ZFP_WITH_CACHE_FAST_HASH)

option(ZFP_WITH_CACHE_PROFILE "Count cache misses" OFF)
mark_as_advanced(ZFP_WITH_CACHE_PROFILE)

# Handle compile-time macros

if((DEFINED ZFP_INT64) AND (DEFINED ZFP_INT64_SUFFIX))
  list(APPEND zfp_public_defs ZFP_INT64=${ZFP_INT64})
  list(APPEND zfp_public_defs ZFP_INT64_SUFFIX=${ZFP_INT64_SUFFIX})
endif()

if((DEFINED ZFP_UINT64) AND (DEFINED ZFP_UINT64_SUFFIX))
  list(APPEND zfp_public_defs ZFP_UINT64=${ZFP_UINT64})
  list(APPEND zfp_public_defs ZFP_UINT64_SUFFIX=${ZFP_UINT64_SUFFIX})
endif()

# This odd cmake pattern here lets the OpenMP feature be either auto-detected,
# explicitly enabled, or explicitly disabled, instead of just on or off.
if(DEFINED ZFP_WITH_OPENMP)
  option(ZFP_WITH_OPENMP "Enable OpenMP parallel compression"
    ${ZFP_WITH_OPENMP})
  if(ZFP_WITH_OPENMP)
    find_package(OpenMP COMPONENTS C REQUIRED)
  endif()
else()
  find_package(OpenMP COMPONENTS C)
  option(ZFP_WITH_OPENMP "Enable OpenMP parallel compression" ${OPENMP_FOUND})
endif()

# Suppress CMake warning about unused variable in this file
set(TOUCH_UNUSED_VARIABLE ${ZFP_OMP_TESTS_ONLY})

# Some compilers don't use explicit libraries on the link line for OpenMP but
# instead need to treat the OpenMP C flags as both compile and link flags
# i.e. -fopenmp for compiling and -lgomp for linking, use -fomp for both
# compiling and linking
if(ZFP_WITH_OPENMP AND NOT OpenMP_C_LIBRARIES)
  set(OpenMP_C_LIBRARIES ${OpenMP_C_FLAGS})
endif()

if(ZFP_WITH_CUDA)
  # use CUDA_BIN_DIR hint
  set(ENV{CUDA_BIN_PATH} ${CUDA_BIN_DIR})
  find_package(CUDA)
  if(NOT CUDA_FOUND)
    message(FATAL_ERROR "ZFP_WITH_CUDA is enabled, but a CUDA installation was not found.")
  endif()
  if(${CUDA_VERSION_MAJOR} LESS 7)
    message(FATAL_ERROR "zfp requires at least CUDA 7.0.")
  endif()
endif()

if(NOT (ZFP_BIT_STREAM_WORD_SIZE EQUAL 64))
  list(APPEND zfp_private_defs BIT_STREAM_WORD_TYPE=uint${ZFP_BIT_STREAM_WORD_SIZE})
endif()

if(DEFINED ZFP_CACHE_LINE_SIZE)
  # Add to zfp_public_defs since many tests currently include files from src.
#  list(APPEND zfp_public_defs ZFP_CACHE_LINE_SIZE=${ZFP_CACHE_LINE_SIZE})
  list(APPEND zfp_private_defs ZFP_CACHE_LINE_SIZE=${ZFP_CACHE_LINE_SIZE})
endif()

if(ZFP_WITH_BIT_STREAM_STRIDED)
  list(APPEND zfp_public_defs BIT_STREAM_STRIDED)
endif()

if(NOT (ZFP_ROUNDING_MODE EQUAL ZFP_ROUND_NEVER))
  list(APPEND zfp_private_defs ZFP_ROUNDING_MODE=${ZFP_ROUNDING_MODE})
endif()

if(ZFP_WITH_TIGHT_ERROR)
  if((ZFP_ROUNDING_MODE EQUAL 0) OR (ZFP_ROUNDING_MODE STREQUAL ZFP_ROUND_NEVER))
    message(FATAL_ERROR "ZFP_WITH_TIGHT_ERROR requires ZFP_ROUND_FIRST or ZFP_ROUND_LAST rounding mode")
  endif()
  list(APPEND zfp_private_defs ZFP_WITH_TIGHT_ERROR)
endif()

if(ZFP_WITH_DAZ)
  list(APPEND zfp_private_defs ZFP_WITH_DAZ)
endif()

if(ZFP_WITH_ALIGNED_ALLOC)
  list(APPEND zfp_compressed_array_defs ZFP_WITH_ALIGNED_ALLOC)
endif()

if(ZFP_WITH_CACHE_TWOWAY)
  list(APPEND zfp_compressed_array_defs ZFP_WITH_CACHE_TWOWAY)
endif()

if(ZFP_WITH_CACHE_FAST_HASH)
  list(APPEND zfp_compressed_array_defs ZFP_WITH_CACHE_FAST_HASH)
endif()

if(ZFP_WITH_CACHE_PROFILE)
  list(APPEND zfp_compressed_array_defs ZFP_WITH_CACHE_PROFILE)
endif()

list(APPEND ppm_private_defs PPM_CHROMA=${PPM_CHROMA})

# Link libm only if necessary
include(CheckCSourceCompiles)
check_c_source_compiles("#include<math.h>\nfloat f; int main(){sqrt(f);return 0;}" HAVE_MATH)
if(NOT HAVE_MATH)
  set(CMAKE_REQUIRED_LIBRARIES m)
  check_c_source_compiles("#include<math.h>\nfloat f; int main(){sqrt(f);return 0;}" HAVE_LIBM_MATH)
  unset(CMAKE_REQUIRED_LIBRARIES)
  if(NOT HAVE_LIBM_MATH)
    message(FATAL_ERROR "Unable to use C math library functions (with or without -lm)")
  endif()
endif()

#------------------------------------------------------------------------------#
# Add source code
#------------------------------------------------------------------------------#

set(ZFP_LIBRARY_PREFIX "" CACHE STRING
  "Prefix to prepend to the output library name")
mark_as_advanced(ZFP_LIBRARY_PREFIX)

add_subdirectory(src)

#------------------------------------------------------------------------------#
# Build type: one of None, Debug, Release, RelWithDebInfo, MinSizeRel
#------------------------------------------------------------------------------#
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE Release)
endif()
