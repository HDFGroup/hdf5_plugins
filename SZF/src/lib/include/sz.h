/**
 *  @file sz.h
 *  @author Sheng Di
 *  @date April, 2015
 *  @brief Header file for the whole compressor.
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "sz_defines.h"

#include "conf.h"
#include "VarSet.h"
#include "pastri.h"
#include "Huffman.h"
#include "ByteToolkit.h"


#ifndef _SZ_H
#define _SZ_H

#define SZ_COMPUTE_1D_NUMBER_OF_BLOCKS( COUNT, NUM_BLOCKS, BLOCK_SIZE ) \
    if (COUNT <= BLOCK_SIZE){                  \
        NUM_BLOCKS = 1;             \
    }                                   \
    else{                               \
        NUM_BLOCKS = COUNT / BLOCK_SIZE;       \
    }                                   \

#define SZ_COMPUTE_2D_NUMBER_OF_BLOCKS( COUNT, NUM_BLOCKS, BLOCK_SIZE ) \
    if (COUNT <= BLOCK_SIZE){                   \
        NUM_BLOCKS = 1;             \
    }                                   \
    else{                               \
        NUM_BLOCKS = COUNT / BLOCK_SIZE;        \
    }                                   \

#define SZ_COMPUTE_3D_NUMBER_OF_BLOCKS( COUNT, NUM_BLOCKS, BLOCK_SIZE ) \
    if (COUNT <= BLOCK_SIZE){                   \
        NUM_BLOCKS = 1;             \
    }                                   \
    else{                               \
        NUM_BLOCKS = COUNT / BLOCK_SIZE;        \
    }                                   \

#define SZ_COMPUTE_BLOCKCOUNT( COUNT, NUM_BLOCKS, SPLIT_INDEX,       \
                                       EARLY_BLOCK_COUNT, LATE_BLOCK_COUNT ) \
    EARLY_BLOCK_COUNT = LATE_BLOCK_COUNT = COUNT / NUM_BLOCKS;               \
    SPLIT_INDEX = COUNT % NUM_BLOCKS;                                        \
    if (0 != SPLIT_INDEX) {                                                  \
        EARLY_BLOCK_COUNT = EARLY_BLOCK_COUNT + 1;                           \
    }                                                                        \

//typedef unsigned long unsigned long;
//typedef unsigned int uint;

typedef union lint16
{
    unsigned short usvalue;
    short svalue;
    unsigned char byte[2];
} lint16;

typedef union lint32
{
    int ivalue;
    unsigned int uivalue;
    unsigned char byte[4];
} lint32;

typedef union lint64
{
    long lvalue;
    unsigned long ulvalue;
    unsigned char byte[8];
} lint64;

typedef union ldouble
{
    double value;
    unsigned long lvalue;
    unsigned char byte[8];
} ldouble;

typedef union lfloat
{
    float value;
    unsigned int ivalue;
    unsigned char byte[4];
} lfloat;

/* array meta data and compression parameters for SZ_Init_Params() */
typedef struct sz_params
{
    int dataType;
    unsigned int max_quant_intervals; //max number of quantization intervals for quantization
    unsigned int quantization_intervals;
    unsigned int maxRangeRadius;
    int sol_ID;// it's SZ or SZ_Transpose, unless the setting is PASTRI compression mode (./configure --enable-pastri)
    int losslessCompressor;
    int sampleDistance; //2 bytes
    float predThreshold;  // 2 bytes
    int szMode; //* 0 (best speed) or 1 (better compression with Zstd/Gzip) or 3 temporal-dimension based compression
    int gzipMode; //* four options: Z_NO_COMPRESSION, or Z_BEST_SPEED, Z_BEST_COMPRESSION, Z_DEFAULT_COMPRESSION
    int  errorBoundMode; //4bits (0.5byte), //ABS, REL, ABS_AND_REL, or ABS_OR_REL, PSNR, or PW_REL, PSNR
    double absErrBound; //absolute error bound
    double relBoundRatio; //value range based relative error bound ratio
    double psnr; //PSNR
    double normErr;
    double pw_relBoundRatio; //point-wise relative error bound
    int segment_size; //only used for 2D/3D data compression with pw_relBoundRatio (deprecated)
    int pwr_type; //only used for 2D/3D data compression with pw_relBoundRatio

    int protectValueRange; //0 or 1
    float fmin, fmax;
    double dmin, dmax;

    int snapshotCmprStep; //perform single-snapshot-based compression if time_step == snapshotCmprStep
    int predictionMode;

    int accelerate_pw_rel_compression;
    int plus_bits;

    int randomAccess;
    int withRegression;

} sz_params;

typedef struct sz_metadata
{
    int versionNumber[3]; //only used for checking the version by calling SZ_GetMetaData()
    int isConstant; //only used for checking if the data are constant values by calling SZ_GetMetaData()
    int isLossless; //only used for checking if the data compression was lossless, used only by calling SZ_GetMetaData()
    int sizeType; //only used for checking whether the size type is "int" or "long" in the compression, used only by calling SZ_GetMetaData()
    size_t dataSeriesLength; //# number of data points in the dataset
    int defactoNBBins; //real number of quantization bins
    struct sz_params* conf_params; //configuration parameters
} sz_metadata;

typedef struct sz_exedata
{
    char optQuantMode;    //opt Quantization (0: fixed ; 1: optimized)
    int intvCapacity; // the number of intervals for the linear-scaling quantization
    int intvRadius;  // the number of intervals for the radius of the quantization range (intvRadius=intvCapacity/2)
    unsigned int SZ_SIZE_TYPE; //the length (# bytes) of the size_t in the system at runtime //4 or 8: sizeof(size_t)
} sz_exedata;

/*We use a linked list to maintain time-step meta info for time-step based compression*/
typedef struct sz_tsc_metainfo
{
    int totalNumOfSteps;
    int currentStep;
    char metadata_filename[256];
    FILE *metadata_file;
    unsigned char* bit_array; //sihuan added
    size_t intersect_size; //sihuan added
    int64_t* hist_index; //sihuan added: prestep index

} sz_tsc_metadata;

#ifdef __cplusplus
extern "C" {
#endif

extern int versionNumber[4];

//-------------------key global variables--------------
extern int dataEndianType; //*endian type of the data read from disk
extern int sysEndianType; //*sysEndianType is actually set automatically.

extern sz_params *confparams_cpr;
extern sz_params *confparams_dec;
extern sz_exedata *exe_params;

//------------------------------------------------
extern SZ_VarSet* sz_varset;
extern sz_multisteps *multisteps; //compression based on multiple time steps (time-dimension based compression)
extern sz_tsc_metadata *sz_tsc;

//for pastri
#ifdef PASTRI
extern pastri_params pastri_par;
#endif

//sz.h
SZFLIB_API HuffmanTree* SZ_Reset();

SZFLIB_API int SZ_Init(const char *configFilePath);

SZFLIB_API int SZ_Init_Params(sz_params *params);

SZFLIB_API size_t computeDataLength(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

SZFLIB_API int computeDimension(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

SZFLIB_API int SZ_compress_args_float_subblock(unsigned char* compressedBytes, float *oriData,
            size_t r5, size_t r4, size_t r3, size_t r2, size_t r1,
            size_t s5, size_t s4, size_t s3, size_t s2, size_t s1,
            size_t e5, size_t e4, size_t e3, size_t e2, size_t e1,
            size_t *outSize, int errBoundMode, double absErr_Bound, double relBoundRatio);

SZFLIB_API int SZ_compress_args_double_subblock(unsigned char* compressedBytes, double *oriData,
            size_t r5, size_t r4, size_t r3, size_t r2, size_t r1,
            size_t s5, size_t s4, size_t s3, size_t s2, size_t s1,
            size_t e5, size_t e4, size_t e3, size_t e2, size_t e1,
            size_t *outSize, int errBoundMode, double absErr_Bound, double relBoundRatio);

SZFLIB_API unsigned char *SZ_compress(int dataType, void *data, size_t *outSize, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

SZFLIB_API unsigned char* SZ_compress_args(int dataType, void *data, size_t *outSize, int errBoundMode, double absErrBound,
            double relBoundRatio, double pwrBoundRatio, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

SZFLIB_API int SZ_compress_args2(int dataType, void *data, unsigned char* compressed_bytes, size_t *outSize,
            int errBoundMode, double absErrBound, double relBoundRatio, double pwrBoundRatio,
            size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

SZFLIB_API int SZ_compress_args3(int dataType, void *data, unsigned char* compressed_bytes, size_t *outSize, int errBoundMode, double absErrBound, double relBoundRatio,
            size_t r5, size_t r4, size_t r3, size_t r2, size_t r1,
            size_t s5, size_t s4, size_t s3, size_t s2, size_t s1,
            size_t e5, size_t e4, size_t e3, size_t e2, size_t e1);

SZFLIB_API unsigned char *SZ_compress_rev_args(int dataType, void *data, void *reservedValue, size_t *outSize, int errBoundMode, double absErrBound, double relBoundRatio,
            size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

SZFLIB_API int SZ_compress_rev_args2(int dataType, void *data, void *reservedValue, unsigned char* compressed_bytes, size_t *outSize, int errBoundMode, double absErrBound, double relBoundRatio,
            size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
SZFLIB_API unsigned char *SZ_compress_rev(int dataType, void *data, void *reservedValue, size_t *outSize, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

SZFLIB_API void SZ_Create_ParamsExe(sz_params** conf_params, sz_exedata** exe_params);

SZFLIB_API void *SZ_decompress(int dataType, unsigned char *bytes, size_t byteLength, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
SZFLIB_API size_t SZ_decompress_args(int dataType, unsigned char *bytes, size_t byteLength, void* decompressed_array, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

SZFLIB_API sz_metadata* SZ_getMetadata(unsigned char* bytes);
SZFLIB_API void SZ_printMetadata(sz_metadata* metadata);


SZFLIB_API void filloutDimArray(size_t* dim, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

SZFLIB_API size_t compute_total_batch_size();

SZFLIB_API void SZ_registerVar(int var_id, char* varName, int dataType, void* data,
            int errBoundMode, double absErrBound, double relBoundRatio, double pwRelBoundRatio,
            size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

SZFLIB_API int SZ_deregisterVar_ID(int var_id);
SZFLIB_API int SZ_deregisterVar(char* varName);
SZFLIB_API int SZ_deregisterAllVars();

SZFLIB_API int SZ_compress_ts_select_var(int cmprType, unsigned char* var_ids, unsigned char var_count, unsigned char** newByteData, size_t *outSize);
SZFLIB_API int SZ_compress_ts(int cmprType, unsigned char** newByteData, size_t *outSize);
SZFLIB_API void SZ_decompress_ts_select_var(unsigned char* var_ids, unsigned char var_count, unsigned char *bytes, size_t bytesLength);
SZFLIB_API void SZ_decompress_ts(unsigned char *bytes, size_t byteLength);

SZFLIB_API void SZ_Finalize();

SZFLIB_API void convertSZParamsToBytes(sz_params* params, unsigned char* result);
SZFLIB_API void convertBytesToSZParams(unsigned char* bytes, sz_params* params);

SZFLIB_API unsigned char* SZ_compress_customize(const char* appName, void* userPara, int dataType, void* data, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t *outSize, int *status);

SZFLIB_API void* SZ_decompress_customize(const char* appName, void* userPara, int dataType, unsigned char* bytes, size_t byteLength, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, int* status);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _SZ_H  ----- */
