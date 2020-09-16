/**
 *  @file sz_uint32.h
 *  @author Sheng Di
 *  @date July, 2017
 *  @brief Header file for the sz_uint32.c.
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _SZ_UInt32_H
#define _SZ_UInt32_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

unsigned int optimize_intervals_uint32_1D(uint32_t *oriData, size_t dataLength, double realPrecision);
unsigned int optimize_intervals_uint32_2D(uint32_t *oriData, size_t r1, size_t r2, double realPrecision);
unsigned int optimize_intervals_uint32_3D(uint32_t *oriData, size_t r1, size_t r2, size_t r3, double realPrecision);
unsigned int optimize_intervals_uint32_4D(uint32_t *oriData, size_t r1, size_t r2, size_t r3, size_t r4, double realPrecision);
TightDataPointStorageI* SZ_compress_uint32_1D_MDQ(uint32_t *oriData, size_t dataLength, double realPrecision, int64_t valueRangeSize, int64_t minValue);
void SZ_compress_args_uint32_StoreOriData(uint32_t* oriData, size_t dataLength, TightDataPointStorageI* tdps, unsigned char** newByteData, size_t *outSize);
void SZ_compress_args_uint32_NoCkRngeNoGzip_1D(unsigned char** newByteData, uint32_t *oriData, 
size_t dataLength, double realPrecision, size_t *outSize, int64_t valueRangeSize, uint32_t minValue);
TightDataPointStorageI* SZ_compress_uint32_2D_MDQ(uint32_t *oriData, size_t r1, size_t r2, double realPrecision, int64_t valueRangeSize, int64_t minValue);
TightDataPointStorageI* SZ_compress_uint32_3D_MDQ(uint32_t *oriData, size_t r1, size_t r2, size_t r3, double realPrecision, int64_t valueRangeSize, int64_t minValue);
void SZ_compress_args_uint32_NoCkRngeNoGzip_3D(unsigned char** newByteData, uint32_t *oriData, size_t r1, size_t r2, size_t r3, double realPrecision, size_t *outSize, int64_t valueRangeSize, int64_t minValue);
TightDataPointStorageI* SZ_compress_uint32_4D_MDQ(uint32_t *oriData, size_t r1, size_t r2, size_t r3, size_t r4, double realPrecision, int64_t valueRangeSize, int64_t minValue);
void SZ_compress_args_uint32_NoCkRngeNoGzip_4D(unsigned char** newByteData, uint32_t *oriData, size_t r1, size_t r2, size_t r3, size_t r4, double realPrecision, 
size_t *outSize, int64_t valueRangeSize, int64_t minValue);
void SZ_compress_args_uint32_withinRange(unsigned char** newByteData, uint32_t *oriData, size_t dataLength, size_t *outSize);

int SZ_compress_args_uint32_wRngeNoGzip(unsigned char** newByteData, uint32_t *oriData, 
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t *outSize, 
int errBoundMode, double absErr_Bound, double relBoundRatio);

int SZ_compress_args_uint32(unsigned char** newByteData, uint32_t *oriData, 
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t *outSize, 
int errBoundMode, double absErr_Bound, double relBoundRatio);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _SZ_UInt32_H  ----- */

