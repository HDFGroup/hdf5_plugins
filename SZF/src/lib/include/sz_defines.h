/**
 *  @file sz_defines.h
 *  @author Sheng Di
 *  @date July, 2019
 *  @brief Header file for the dataCompression.c.
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _SZ_DEFINES_H
#define _SZ_DEFINES_H

#include <stdio.h>

#include "szf_config.h"

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif

#ifdef HAVE_STRING_H
# if !defined STDC_HEADERS && defined HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif

#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>      /* For gettimeofday(), in microseconds */
#endif

#ifdef HAVE_TIME_H
# include <time.h>          /* For time(), in seconds */
#endif

#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#endif

#include <limits.h>

/*
 * Skip inttypes.h if we already have all the required macros. If we
 * have the macros, we assume that we have the matching typedefs too.
 */
#if !defined(UINT32_C) || !defined(UINT64_C) || !defined(UINT32_MAX) || !defined(UINT64_MAX)
 /*
  * MSVC has no C99 support, and thus it cannot be used to
  * compile libsz. The libsz API has to still be usable
  * from MSVC, so we need to define the required standard
  * integer types here.
  */
# if defined(_WIN32) && defined(_MSC_VER)
   typedef unsigned __int8 uint8_t;
   typedef unsigned __int32 uint32_t;
   typedef unsigned __int64 uint64_t;
# else
   /* Use the standard inttypes.h. */
#  ifdef __cplusplus
   /*
    * C99 sections 7.18.2 and 7.18.4 specify
    * that C++ implementations define the limit
    * and constant macros only if specifically
    * requested. Note that if you want the
    * format macros (PRIu64 etc.) too, you need
    * to define __STDC_FORMAT_MACROS before
    * including sz.h, since re-including
    * inttypes.h with __STDC_FORMAT_MACROS
    * defined doesn't necessarily work.
    */
#   ifndef __STDC_LIMIT_MACROS
#     define __STDC_LIMIT_MACROS 1
#   endif
#   ifndef __STDC_CONSTANT_MACROS
#    define __STDC_CONSTANT_MACROS 1
#   endif
#  endif /* __cplusplus */

#  include <inttypes.h>
# endif

 /*
  * Some old systems have only the typedefs in inttypes.h, and
  * lack all the macros. For those systems, we need a few more
  * hacks. We assume that unsigned int is 32-bit and unsigned
  * long is either 32-bit or 64-bit. If these hacks aren't
  * enough, the application has to setup the types manually
  * before including sz.h.
  */
# ifndef UINT32_C
#  if defined(_WIN32) && defined(_MSC_VER)
#   define UINT32_C(n) n ## UI32
#  else
#   define UINT32_C(n) n ## U
#  endif
# endif

# ifndef UINT64_C
#  if defined(_WIN32) && defined(_MSC_VER)
#   define UINT64_C(n) n ## UI64
#  else
    /* Get ULONG_MAX. */
#   include <limits.h>
#   if ULONG_MAX == 4294967295UL
#    define UINT64_C(n) n ## ULL
#   else
#    define UINT64_C(n) n ## UL
#   endif
#  endif
# endif

# ifndef UINT32_MAX
#  define UINT32_MAX (UINT32_C(4294967295))
# endif

# ifndef UINT64_MAX
#  define UINT64_MAX (UINT64_C(18446744073709551615))
# endif
#endif

#ifdef _WIN32
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif


//typedef char int8_t;
//typedef unsigned char uint8_t;
//typedef short int16_t;
//typedef unsigned short uint16_t;
//typedef int int32_t;
//typedef unsigned int uint32_t;
//typedef long int64_t;
//typedef unsigned long uint64_t;

#define SZ_VERNUM 0x0200
#define SZ_VER_MAJOR 2
#define SZ_VER_MINOR 1
#define SZ_VER_BUILD 9
#define SZ_VER_REVISION 0

#define PASTRI 103
#define HZ 102 //deprecated
#define SZ 101
#define SZ_Transpose 104

//prediction mode of temporal dimension based compression
#define SZ_PREVIOUS_VALUE_ESTIMATE 0

#define MIN_NUM_OF_ELEMENTS 20 //if the # elements <= 20, skip the compression

#define ABS 0
#define REL 1
#define VR_REL 1  //alternative name to REL
#define ABS_AND_REL 2
#define ABS_OR_REL 3
#define PSNR 4
#define NORM 5

#define PW_REL 10
#define ABS_AND_PW_REL 11
#define ABS_OR_PW_REL 12
#define REL_AND_PW_REL 13
#define REL_OR_PW_REL 14

#define SZ_FLOAT 0
#define SZ_DOUBLE 1
#define SZ_UINT8 2
#define SZ_INT8 3
#define SZ_UINT16 4
#define SZ_INT16 5
#define SZ_UINT32 6
#define SZ_INT32 7
#define SZ_UINT64 8
#define SZ_INT64 9

#define LITTLE_ENDIAN_DATA 0 //refers to the endian type of the data read from the disk
#define BIG_ENDIAN_DATA 1 //big_endian (ppc, max, etc.) ; little_endian (x86, x64, etc.)

#define LITTLE_ENDIAN_SYSTEM 0 //refers to the endian type of the system
#define BIG_ENDIAN_SYSTEM 1

#define DynArrayInitLen 1024

#define MIN_ZLIB_DEC_ALLOMEM_BYTES 1000000

//#define maxRangeRadius 32768
//#define maxRangeRadius 1048576//131072

#define SZ_BEST_SPEED 0
#define SZ_BEST_COMPRESSION 1
#define SZ_DEFAULT_COMPRESSION 2
#define SZ_TEMPORAL_COMPRESSION 3

#define SZ_NO_REGRESSION 0
#define SZ_WITH_LINEAR_REGRESSION 1

#define SZ_PWR_MIN_TYPE 0
#define SZ_PWR_AVG_TYPE 1
#define SZ_PWR_MAX_TYPE 2

#define SZ_FORCE_SNAPSHOT_COMPRESSION 0
#define SZ_FORCE_TEMPORAL_COMPRESSION 1
#define SZ_PERIO_TEMPORAL_COMPRESSION 2

//SUCCESS returning status
#define SZ_SCES 0  //successful
#define SZ_NSCS -1 //Not successful
#define SZ_FERR -2 //Failed to open input file
#define SZ_TERR -3 //wrong data type (should be only float or double)
#define SZ_DERR -4 //dimension error
#define SZ_MERR -5 //sz_mode error
#define SZ_BERR -6 //bound-mode error (should be only ABS, REL, ABS_AND_REL, ABS_OR_REL, or PW_REL)

#define SZ_MAINTAIN_VAR_DATA 0
#define SZ_DESTROY_WHOLE_VARSET 1

#define GROUP_COUNT 16 //2^{16}=65536

#define MetaDataByteLength 28
#define MetaDataByteLength_double 36 //meta data length for double type

#define numOfBufferedSteps 1 //the number of time steps in the buffer


#define GZIP_COMPRESSOR 0 //i.e., ZLIB_COMPRSSOR
#define ZSTD_COMPRESSOR 1

//Note: the following setting should be consistent with stateNum in Huffman.h
//#define intvCapacity 65536
//#define intvRadius 32768
//#define intvCapacity 131072
//#define intvRadius 65536

/****************************************************************
*  Export parameters
*****************************************************************/
/*
*  SZF_DLL_EXPORT :
*  Enable exporting of functions when building a Windows DLL
*  SZFLIB_VISIBILITY :
*  Control library symbols visibility.
*/
#ifndef SZFLIB_VISIBILITY
#  if defined(__GNUC__) && (__GNUC__ >= 4)
#    define SZFLIB_VISIBILITY __attribute__ ((visibility ("default")))
#  else
#    define SZFLIB_VISIBILITY
#  endif
#endif
#if defined(SZF_DLL_EXPORT) && (SZF_DLL_EXPORT==1)
#  define SZFLIB_API __declspec(dllexport) SZFLIB_VISIBILITY
#elif defined(SZF_DLL_IMPORT) && (SZF_DLL_IMPORT==1)
#  define SZFLIB_API __declspec(dllimport) SZFLIB_VISIBILITY /* It isn't required but allows to generate better code, saving a function pointer load from the IAT and an indirect jump.*/
#else
#  define SZFLIB_API SZFLIB_VISIBILITY
#endif

#endif /* _SZ_DEFINES_H */
