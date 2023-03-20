/**
 *  @file sz.h
 *  @author Sheng Di
 *  @date July, 2017
 *  @brief Header file for the whole compressor.
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _H5ZSZ_H
#define _H5ZSZ_H


#ifdef _WIN32
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "defines.h"


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
	char optQuantMode;	//opt Quantization (0: fixed ; 1: optimized)
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
HuffmanTree* SZ_Reset();

int SZ_Init(const char *configFilePath);

int SZ_Init_Params(sz_params *params);

size_t computeDataLength(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

int computeDimension(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
int filterDimension(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t* correctedDimension);

int SZ_compress_args_float_subblock(unsigned char* compressedBytes, float *oriData,
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1,
size_t s5, size_t s4, size_t s3, size_t s2, size_t s1,
size_t e5, size_t e4, size_t e3, size_t e2, size_t e1,
size_t *outSize, int errBoundMode, double absErr_Bound, double relBoundRatio);

int SZ_compress_args_double_subblock(unsigned char* compressedBytes, double *oriData,
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1,
size_t s5, size_t s4, size_t s3, size_t s2, size_t s1,
size_t e5, size_t e4, size_t e3, size_t e2, size_t e1,
size_t *outSize, int errBoundMode, double absErr_Bound, double relBoundRatio);

unsigned char *SZ_compress(int dataType, void *data, size_t *outSize, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

unsigned char* SZ_compress_args(int dataType, void *data, size_t *outSize, int errBoundMode, double absErrBound,
double relBoundRatio, double pwrBoundRatio, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

int SZ_compress_args2(int dataType, void *data, unsigned char* compressed_bytes, size_t *outSize,
int errBoundMode, double absErrBound, double relBoundRatio, double pwrBoundRatio,
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

int SZ_compress_args3(int dataType, void *data, unsigned char* compressed_bytes, size_t *outSize, int errBoundMode, double absErrBound, double relBoundRatio,
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1,
size_t s5, size_t s4, size_t s3, size_t s2, size_t s1,
size_t e5, size_t e4, size_t e3, size_t e2, size_t e1);

unsigned char *SZ_compress_rev_args(int dataType, void *data, void *reservedValue, size_t *outSize, int errBoundMode, double absErrBound, double relBoundRatio,
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

int SZ_compress_rev_args2(int dataType, void *data, void *reservedValue, unsigned char* compressed_bytes, size_t *outSize, int errBoundMode, double absErrBound, double relBoundRatio,
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
unsigned char *SZ_compress_rev(int dataType, void *data, void *reservedValue, size_t *outSize, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

void SZ_Create_ParamsExe(sz_params** conf_params, sz_exedata** exe_params);

void *SZ_decompress(int dataType, unsigned char *bytes, size_t byteLength, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
size_t SZ_decompress_args(int dataType, unsigned char *bytes, size_t byteLength, void* decompressed_array, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

sz_metadata* SZ_getMetadata(unsigned char* bytes);
void SZ_printMetadata(sz_metadata* metadata);


void filloutDimArray(size_t* dim, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

size_t compute_total_batch_size();

void SZ_registerVar(int var_id, char* varName, int dataType, void* data,
			int errBoundMode, double absErrBound, double relBoundRatio, double pwRelBoundRatio,
			size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

int SZ_deregisterVar_ID(int var_id);
int SZ_deregisterVar(char* varName);
int SZ_deregisterAllVars();

int SZ_compress_ts_select_var(int cmprType, unsigned char* var_ids, unsigned char var_count, unsigned char** newByteData, size_t *outSize);
int SZ_compress_ts(int cmprType, unsigned char** newByteData, size_t *outSize);
void SZ_decompress_ts_select_var(unsigned char* var_ids, unsigned char var_count, unsigned char *bytes, size_t bytesLength);
void SZ_decompress_ts(unsigned char *bytes, size_t byteLength);

void SZ_Finalize();

void convertSZParamsToBytes(sz_params* params, unsigned char* result);
void convertBytesToSZParams(unsigned char* bytes, sz_params* params);

unsigned char* SZ_compress_customize(const char* appName, void* userPara, int dataType, void* data, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t *outSize, int *status);

unsigned char* SZ_compress_customize_threadsafe(const char* cmprName, void* userPara, int dataType, void* data, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t *outSize, int *status);

void* SZ_decompress_customize(const char* appName, void* userPara, int dataType, unsigned char* bytes, size_t byteLength, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, int* status);

void* SZ_decompress_customize_threadsafe(const char* cmprName, void* userPara, int dataType, unsigned char* bytes, size_t byteLength, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, int *status);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _H5ZSZ_H  ----- */
