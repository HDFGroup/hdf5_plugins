/*
* Copyright (c) 2011 Nathanael Hübbe
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "H5PLextern.h"

#include <hdf5.h>
#include <lzma.h>

#include "mafisc.h"
#include "simple-hash.h"
#include "filter-cascade-filter.h"

/*
* Some switches controlling the compression.
*/

#define USE_GLOBAL_CASCADE_FILTER
#define USE_FAST_LZMA_OPTIONS
#define USE_RANDOM_TESTING
#define RANDOM_TESTING_ROUNDS 1
#define MAX_DIFF_FILTER_COUNT 3

/*
* The eligible methods.
*/

/*const char* const differenceFilters[] = {
    "",

    "d0", "d1", "d2", "d3", "dz",

    "d0d0", "d0d1", "d0d2", "d0dz",
    "d1d1", "d1d2", "d1dz",
    "d2d2", "d2dz",
    "dzdz",

    "d0d0d0", "d0d0d1", "d0d0dz",
    "d0d1d1", "d0d1dz",
    "d0dzdz",
    "d1d1d1", "d1d1dz",
    "d1dzdz",
    "dzdzdz"
};//*/
const char* const differenceFilters[] = {"", "d0", "dz"};
const char* const lorenzoFilters[] = {""};
const char* const adaptiveFilters[] = {"", "al"};
const char* const signFilters[] = {"", "sn", "sr"};
const char* const bitFilters[] = {"", "bb", "bg"};
// const char* const differenceFilters[] = {""};
// const char* const lorenzoFilters[] = {"l"};
// const char* const adaptiveFilters[] = {""};
// const char* const signFilters[] = {""};
// const char* const bitFilters[] = {""};
#define differenceFilterCount (sizeof(differenceFilters)/sizeof(char*))
#define lorenzoFilterCount (sizeof(lorenzoFilters)/sizeof(char*))
#define adaptiveFilterCount (sizeof(adaptiveFilters)/sizeof(char*))
#define signFilterCount (sizeof(signFilters)/sizeof(char*))
#define bitFilterCount (sizeof(bitFilters)/sizeof(char*))

/*
* Forward declarations for all the static functions in this file.
*/

static htri_t compressorCanCompress(hid_t dcpl_id, hid_t type_id, hid_t space_id);
static herr_t compressorSetLocal(hid_t pList, hid_t type, hid_t space);
static size_t compressorFilter(unsigned int flags, size_t cd_nelmts, const unsigned int cd_values[], size_t nBytes, size_t *buf_size, void **buf);

static FilterCascadeFilter* getGlobalFilter(void);
static unsigned int randBelow(unsigned int limit);
static unsigned long compressionMethodCount(void);
static char* compressionMethodString(unsigned long number);
static char* randomCompressionMethodString(void);
//static VariableFilter* randomCompressionMethod(void);
//static void gRandomCompressionMethod(void);

static SimpleHash* getCompressionMethodHash(void);
static char* getCompressionMethodString(unsigned long datasetId);
//static VariableFilter* getCompressionMethod(unsigned long datasetId);
//static void gGetCompressionMethod(unsigned long datasetId);
static void setCompressionMethodString(unsigned long datasetId, const char* method);
#ifndef USE_GLOBAL_CASCADE_FILTER
    static void setCompressionMethod(unsigned long datasetId, VariableFilter* filter);
#endif
static void gSetCompressionMethod(unsigned long datasetId);

static void doFpXpConversion(size_t valueSize, size_t dimCount, size_t* dimLengths, const void* dataIn, void* dataOut);
static void switchByteOrder(size_t dataTypeSize, size_t valueCount, const void* iBuffer, void* oBuffer);

static long doLzmaCompression(size_t *byteCount, const void* iBuffer, void* oBuffer);
static long doLzmaDecompression(size_t *byteCount, size_t oBufferSize, const void* iBuffer, void* oBuffer);

static size_t decompress(size_t nBytes, size_t* bufferSize, void** buffer);
static void doFiltering(const char* method, unsigned dataTypeSize, unsigned rank, size_t* dims, size_t valueCount, long nativeIsLE, const void* inputBuffer, void* outputBuffer);

//A handy tool to add something to a byte stream.
#define copyUpdate(to, from, count) memcpy(to, from, count), to = (void*)((size_t)to + count)

/*
* Some statistics that can be output by calling printMafiscFilterTimes().
*/

static clock_t compressionTime = 0, lzmaCompressionTime = 0, filterCompressionTime = 0;
static size_t compressedData = 0;
static clock_t decompressionTime = 0, lzmaDecompressionTime = 0, filterDecompressionTime = 0;
static size_t decompressedData = 0;

//format is expected to contain exactly two floating point format specifiers, the first for the time in seconds, the second for the throughput in MB/s.
//void printDkrzFilterStatLine(const char *format, clock_t clocks, size_t data);
#define printDkrzFilterStatLine(format, clocks, data) {\
    double seconds = ((double)clocks)/(double)CLOCKS_PER_SEC;\
    double rate = ((double)data)/(1000000*seconds);\
    printf(format, seconds, rate);\
}

void printMafiscFilterTimes(void) {
    printf("Compression:\n");
    printDkrzFilterStatLine("\tfilter %gs (%g MB/s)\n", filterCompressionTime, compressedData);
    printDkrzFilterStatLine("\tlzma   %gs (%g MB/s)\n", lzmaCompressionTime, compressedData);
    printDkrzFilterStatLine("\ttotal  %gs (%g MB/s)\n", compressionTime, compressedData);
    printf("Decompression:\n");
    printDkrzFilterStatLine("\tfilter %gs (%g MB/s)\n", filterDecompressionTime, decompressedData);
    printDkrzFilterStatLine("\tlzma   %gs (%g MB/s)\n", lzmaDecompressionTime, decompressedData);
    printDkrzFilterStatLine("\ttotal  %gs (%g MB/s)\n", decompressionTime, decompressedData);
}

H5PL_type_t H5PLget_plugin_type(void) {
    return H5PL_TYPE_FILTER;
}

const void *H5PLget_plugin_info(void) {
    static H5Z_class2_t filterClass = {
        .version = H5Z_CLASS_T_VERS,
        .id = MAFISC_ID,
        .encoder_present = 1,
        .decoder_present = 1,
        .name = "MAFISC",
        .can_apply = &compressorCanCompress,
        .set_local = &compressorSetLocal,
        .filter = &compressorFilter
    };
    return &filterClass;
}

//Returns a random number guaranteed to be smaller than the limit.
static unsigned int randBelow(unsigned int limit) {
//    fprintf(stderr, "randBelow()\n");
    unsigned long value = (unsigned long)rand();
    assert(RAND_MAX < 0x100000000l);
    value *= limit;
    value /= ((unsigned long)RAND_MAX) + 1;
    return (unsigned int) value;
}

/*static uint64_t getChecksum(size_t byteCount, const void* data) {
    size_t i;
    uint64_t sum = 0;
    for(i = 0; i < byteCount; i++) {
        sum = (sum << 1) + (sum >> 63) + ((const char*)data)[i];
    }
    return sum;
}*/

//Returns the singleton FilterCascadeFilter.
static FilterCascadeFilter* getGlobalFilter(void) {
    static FilterCascadeFilter* gFilter = NULL;
    if(!gFilter) gFilter = fcf_make();
    return gFilter;
}

//Return the number of generateable compression methods.
static unsigned long compressionMethodCount(void) {
    static unsigned long result = 0;
    int i;
    if(result) return result;
    result = 1;
    for(i = 0; i < MAX_DIFF_FILTER_COUNT; i++) result *= differenceFilterCount;
    result *= lorenzoFilterCount;
    result *= adaptiveFilterCount;
    result *= signFilterCount;
    result *= bitFilterCount;
    return result;
}

//Return a compression method string by its number. The legal range is [0, compressionMethodCount()[.
static char* compressionMethodString(unsigned long number) {
    unsigned long differenceFilterIndizes[MAX_DIFF_FILTER_COUNT];
    long i;
    unsigned long stringSize = 1;    //1 for termination
    for(i = 0; i < MAX_DIFF_FILTER_COUNT; i++) {
        differenceFilterIndizes[i] = number % differenceFilterCount;
        number /= differenceFilterCount;
        stringSize += strlen(differenceFilters[differenceFilterIndizes[i]]);
    }

    unsigned long lorenzoFilterIndex = number % lorenzoFilterCount;
    number /= lorenzoFilterCount;
    stringSize += strlen(lorenzoFilters[lorenzoFilterIndex]);

    unsigned long adaptiveFilterIndex = number % adaptiveFilterCount;
    number /= adaptiveFilterCount;
    stringSize += strlen(adaptiveFilters[adaptiveFilterIndex]);

    unsigned long signFilterIndex = number % signFilterCount;
    number /= signFilterCount;
    stringSize += strlen(signFilters[signFilterIndex]);

    unsigned long bitFilterIndex = number % bitFilterCount;
    stringSize += strlen(bitFilters[bitFilterIndex]);

    //Since modulos are used, the indizes are guaranteed to be in range, even if number isn't.
    char* filterString = calloc(stringSize, 1);
    if(!filterString) return NULL;
    char* curPtr = filterString;
    for(i = 0; i < MAX_DIFF_FILTER_COUNT; i++) {
        strcpy(curPtr, differenceFilters[differenceFilterIndizes[i]]);
        curPtr += strlen(differenceFilters[differenceFilterIndizes[i]]);
    }
    strcpy(curPtr, lorenzoFilters[lorenzoFilterIndex]);
    curPtr += strlen(lorenzoFilters[lorenzoFilterIndex]);
    strcpy(curPtr, adaptiveFilters[adaptiveFilterIndex]);
    curPtr += strlen(adaptiveFilters[adaptiveFilterIndex]);
    strcpy(curPtr, signFilters[signFilterIndex]);
    curPtr += strlen(signFilters[signFilterIndex]);
    strcpy(curPtr, bitFilters[bitFilterIndex]);
    curPtr += strlen(bitFilters[bitFilterIndex]);
    assert(!*curPtr);
    assert(stringSize == (curPtr - filterString + 1));    //strcpy/strlen is safe as long as this is true.
    return filterString;
}

//Randomly pick a compression method from a broad selection of sensible methods. The caller is responsible to free the returned string.
static char* randomCompressionMethodString(void) {
//    fprintf(stderr, "randomCompressionMethod()\n");
    return compressionMethodString(randBelow(compressionMethodCount()));
}

/*//Randomly pick a compression method from a broad selection of sensible methods. Less efficient than gRandomCompressionMethod() but reentrant.
static VariableFilter* randomCompressionMethod(void) {
//    fprintf(stderr, "randomCompressionMethod()\n");
    char* filterString = randomCompressionMethodString();
    VariableFilter* result = vf_makeWithString(filterString);
    free(filterString);
    return result;
}//*/

/*//Randomly pick a compression method from a broad selection of sensible methods and set the global filter to it.
static void gRandomCompressionMethod(void) {
//    fprintf(stderr, "randomCompressionMethod()\n");
    char* filterString = randomCompressionMethodString();
    fcf_setMethod(getGlobalFilter(), filterString);
    free(filterString);
}//*/

static SimpleHash* getCompressionMethodHash(void) {
//    fprintf(stderr, "getCompressionMethodHash()\n");
    static SimpleHash* gHash = 0;
    if(!gHash) gHash = sh_create(128);
    return gHash;
}

//Retrieves a filter description from a hash table using the datasetId (which is expected to be generated randomly) or a random method iff no description is set for the datasetId yet. The caller is responsible to free the returned string.
static char* getCompressionMethodString(unsigned long datasetId) {
//    fprintf(stderr, "getCompressionMethod()\n");
    char* methodString;
    if(sh_retrieveValue(getCompressionMethodHash(), datasetId, (long*)&methodString)) {
        return randomCompressionMethodString();
    } else {
        char* result;
        if(asprintf(&result, "%s", methodString) < 0) return NULL;
        return result;
    }
}

/*//Retrieves a filter description from a hash table using the datasetId (which is expected to be generated randomly) and reconstructs the VariableFilter for it.
static VariableFilter* getCompressionMethod(unsigned long datasetId) {
//    fprintf(stderr, "getCompressionMethod()\n");
    char* methodString = getCompressionMethodString(datasetId);
    VariableFilter* result = vf_makeWithString(methodString);
    free(methodString);
    return result;
}//*/

/*//Retrieves a filter description from a hash table using the datasetId (which is expected to be generated randomly) and sets the global filter to it.
static void gGetCompressionMethod(unsigned long datasetId) {
//    fprintf(stderr, "getCompressionMethod()\n");
    char* methodString = getCompressionMethodString(datasetId);
    fcf_setMethod(getGlobalFilter(), methodString);
    free(methodString);
}//*/

//Remember a filter description for the given datasetId (which is expected to be generated randomly).
static void setCompressionMethodString(unsigned long datasetId, const char* method) {
//    fprintf(stderr, "setCompressionMethod()\n");
    char* copiedString;
    if(!method) return;
    if(asprintf(&copiedString, "%s", method) < 0) return;
    if(sh_setValue(getCompressionMethodHash(), datasetId, (long*)&copiedString)) {
        //Executed both in the case of an error and the case of successfull replacement. In the first case the copiedString has not been entered in the table, in the later case it now contains the old string. And in both cases it needs to be disposed off.
        free(copiedString);
    }
}

//Remember a filter description for the given datasetId (which is expected to be generated randomly).
#ifndef USE_GLOBAL_CASCADE_FILTER
    static void setCompressionMethod(unsigned long datasetId, VariableFilter* filter) {
    //    fprintf(stderr, "setCompressionMethod()\n");
        char* methodString = vf_toString(filter);
        setCompressionMethodString(datasetId, methodString);
        free(methodString);
    }
#endif

//Remember the global filter description under the given datasetId (which is expected to be generated randomly).
static void gSetCompressionMethod(unsigned long datasetId) {
//    fprintf(stderr, "setCompressionMethod()\n");
    char* methodString = vf_toString((VariableFilter*)getGlobalFilter());
    setCompressionMethodString(datasetId, methodString);
}

//Negates all bits of floating point numbers iff the number is negative to get to a fake one complement representation.
//In this fake one complement representation differences can be calculated using fixed point arithmetic.
//Using fixed point arithmetic guarantees revertability while the fake one complement representation guarantees
//that no very large differences result when the zero point is crossed.
//
//dataIn and dataOut may be the same buffer
static void doFpXpConversion(size_t valueSize, size_t dimCount, size_t* dimLengths, const void* dataIn, void* dataOut) {
//    fprintf(stderr, "doFpXpConversion()\n");
    size_t dataSize = 1, i;
    for(i = 0; i < dimCount; i++) dataSize *= dimLengths[i];
    switch(valueSize) {
        case 4: {
            const uint32_t* fIn = (const uint32_t*)dataIn;
            uint32_t* fOut = (uint32_t*)dataOut;
            for(i = 0; i < dataSize; i++) {
                uint32_t curValue = fIn[i];
                if(curValue & 0x80000000) curValue ^= 0x7fffffff;
                fOut[i] = curValue;
            }
        } break; case 8: {
            const uint64_t* fIn = (const uint64_t*)dataIn;
            uint64_t* fOut = (uint64_t*)dataOut;
            for(i = 0; i < dataSize; i++) {
                uint64_t curValue = fIn[i];
                if(curValue & 0x8000000000000000) curValue ^= 0x7fffffffffffffff;
                fOut[i] = curValue;
            }
        } break; default: {
            memmove(dataOut, dataIn, dataSize*valueSize);
        } break;
    }
}

//Invert the order of the bytes in all values, switching big endian values to little endian ones and vice versa.
//The buffers can be the same, overlap is not handled correctly, though.
static void switchByteOrder(size_t dataTypeSize, size_t valueCount, const void* iBuffer, void* oBuffer) {
//    fprintf(stderr, "switchByteOrder(%li, %li, xxx, xxx)\n", dataTypeSize, bufferSize);
    const uint8_t* input = (const uint8_t*)iBuffer;
    uint8_t* output = (uint8_t*)oBuffer;
    size_t i;
    switch(dataTypeSize) {
        case 1:
            if(iBuffer != oBuffer) memcpy(oBuffer, iBuffer, valueCount*1);
            break;
        case 2:
            for(i = 0; i < valueCount; i++) {
                uint8_t temp = input[2*i];
                output[2*i] = input[2*i + 1];
                output[2*i + 1] = temp;
            }
            break;
        case 4:
            for(i = 0; i < valueCount; i++) {
                uint8_t temp1 = input[4*i], temp2 = input[4*i + 1];
                output[4*i] = input[4*i + 3];
                output[4*i + 1] = input[4*i + 2];
                output[4*i + 2]    = temp2;
                output[4*i + 3] = temp1;
            }
            break;
        case 8:
            for(i = 0; i < valueCount; i++) {
                uint8_t temp1 = input[8*i], temp2 = input[8*i + 1], temp3 = input[8*i + 2], temp4 = input[8*i + 3];
                output[8*i] = input[8*i + 7];
                output[8*i + 1] = input[8*i + 6];
                output[8*i + 2] = input[8*i + 5];
                output[8*i + 3] = input[8*i + 4];
                output[8*i + 4] = temp4;
                output[8*i + 5] = temp3;
                output[8*i + 6] = temp2;
                output[8*i + 7] = temp1;
            }
            break;
        default:
            assert(0);    //Can only convert endianness of 2, 4 and 8 byte values.
    }
}

static htri_t compressorCanCompress(hid_t dcpl_id, hid_t type_id, hid_t space_id) {
//    fprintf(stderr, "compressorCanCompress()\n");
    htri_t result = H5Sis_simple(space_id);
    H5T_class_t dataTypeClass;
    if(result <= 0) return result;    //The dataspace must be simple.
    dataTypeClass = H5Tget_class(type_id);
    switch(dataTypeClass) {
        case H5T_INTEGER:
        case H5T_FLOAT:
            switch(H5Tget_size(type_id)) {
                case 1:
                case 2:
                case 4:
                case 8: return 1;
                default: return 0;
            }
        case H5T_NO_CLASS:
        case H5T_TIME:
        case H5T_STRING:
        case H5T_BITFIELD:
        case H5T_OPAQUE:
        case H5T_COMPOUND:
        case H5T_REFERENCE:
        case H5T_ENUM:
        case H5T_VLEN:
        case H5T_ARRAY:
        case H5T_NCLASSES:    //default would have been sufficient...
        default: return 0;
    }
    dcpl_id = 0;    //Shut up. Never executed.
}

//Compresses a given buffer into another given buffer using the lzma library.
//Returns 0 on success.
//The oBuffer is assumed to be just as big as the iBuffer,
//inflation is considered to be a failure.
//On success *byteCount is reduced to the compressed size.
//Does not change *byteCount on failure, *oBuffer may change though.
//
//The following code is based on the xz_pipe_comp/xz_pipe_decomp examples
//by Daniel Mealha Cabrita, who thankfully placed it in the public domain.
static long doLzmaCompression(size_t *byteCount, const void* iBuffer, void* oBuffer) {
//    fprintf(stderr, "doLzmaCompression()\n");
    //Initialize xz encoder.
    lzma_check check = LZMA_CHECK_CRC64;
    lzma_stream strm = LZMA_STREAM_INIT; /* alloc and init lzma_stream struct */
    lzma_ret ret_xz;
#ifdef USE_FAST_LZMA_OPTIONS
    //The fast options speed up compression by a factor of 3.75 on typical climate data but the output is 3% larger than the compressed size with the easy encoder.
    lzma_options_lzma options = {
        .dict_size = (*byteCount > (1 << 23) ? (1 << 23) : (unsigned int)*byteCount),
        .preset_dict = NULL,
        .preset_dict_size = 0,
        .lc = 1,
        .lp = 0,
        .pb = 2,
        .mode = LZMA_MODE_FAST,
        .nice_len = 273,
        .mf = LZMA_MF_HC4,
        .depth = 1,
    };
    lzma_filter filters[] = {
        {
            .id = LZMA_FILTER_LZMA2,
            .options = (void*)&options,
        },
        { .id = LZMA_VLI_UNKNOWN, },
    };
    ret_xz = lzma_stream_encoder(&strm, filters, check);
#else
    uint32_t preset = 6 | LZMA_PRESET_EXTREME;
    ret_xz = lzma_easy_encoder(&strm, preset, check);
#endif
    if (ret_xz != LZMA_OK) return -1;
    //Compress the data.
    strm.next_in = (const uint8_t*)iBuffer;
    strm.avail_in = *byteCount;
    strm.next_out = (uint8_t*)oBuffer;
    strm.avail_out = *byteCount;
    ret_xz = lzma_code (&strm, LZMA_RUN);
    if ((ret_xz != LZMA_OK) && (ret_xz != LZMA_STREAM_END)) goto error;
    ret_xz = lzma_code (&strm, LZMA_FINISH);
    if ((ret_xz != LZMA_OK) && (ret_xz != LZMA_STREAM_END)) goto error;
    //Inform the caller of what has been done.
    if(!strm.avail_out) goto error;
    *byteCount -= strm.avail_out;
    lzma_end(&strm);
    return 0;
error:
    lzma_end(&strm);
    return -1;
}

//Decompresses a given buffer into another given buffer using the lzma library.
//Returns 0 on success.
//On success *byteCount is increased to the decompressed size.
//Does not change *byteCount on failure, *oBuffer may change though.
//
//The following code is based on the xz_pipe_comp/xz_pipe_decomp examples
//by Daniel Mealha Cabrita, who thankfully placed it in the public domain.
static long doLzmaDecompression(size_t *byteCount, size_t oBufferSize, const void* iBuffer, void* oBuffer) {
//    fprintf(stderr, "doLzmaDecompression()\n");
    //Initialize xz decoder.
    lzma_stream strm = LZMA_STREAM_INIT; /* alloc and init lzma_stream struct */
    const uint32_t flags = LZMA_TELL_UNSUPPORTED_CHECK | LZMA_CONCATENATED;
    const uint64_t memory_limit = UINT64_MAX; /* no memory limit */
    lzma_ret ret_xz;
    ret_xz = lzma_stream_decoder (&strm, memory_limit, flags);
    if (ret_xz != LZMA_OK) {
        fprintf(stderr, "Stream init error\n");
        return -1;
    }
    //Decompress the data.
    strm.next_in = (const uint8_t*)iBuffer;
    strm.avail_in = *byteCount;
    strm.next_out = (uint8_t*)oBuffer;
    strm.avail_out = oBufferSize;
    ret_xz = lzma_code (&strm, LZMA_RUN);
    if ((ret_xz != LZMA_OK) && (ret_xz != LZMA_STREAM_END)) {
        fprintf(stderr, "Stream run error\n");
        goto error;
    }
    ret_xz = lzma_code (&strm, LZMA_FINISH);
    if ((ret_xz != LZMA_OK) && (ret_xz != LZMA_STREAM_END)) {
        fprintf(stderr, "Stream finish error\n");
        goto error;
    }
    //Inform the caller of what has been done.
    if(!strm.avail_out) {
        //Check if there's any data we have not extracted.
        long x;
        strm.next_out = (uint8_t*)&x;
        strm.avail_out = sizeof(x);
        ret_xz = lzma_code(&strm, LZMA_FINISH);
        if ((ret_xz != LZMA_OK) && (ret_xz != LZMA_STREAM_END) && (ret_xz != LZMA_BUF_ERROR)) {    //A buffer error is ok, since it gets thrown when calling lzma_code with neither input nor output.
            fprintf(stderr, "Stream end check error\n");
            goto error;
        }
        if(strm.avail_out != sizeof(x)) {
            fprintf(stderr, "Too much decompressed data.\n");
            goto error;
        }
        strm.avail_out = 0;    //Reset for the return value interpretation.
    }
    *byteCount = oBufferSize - strm.avail_out;
    lzma_end (&strm);
    return 0;
error:
    lzma_end(&strm);
    return -1;
}

//A struct containing only unsigned ints, so that it may safely be casted to an unsigned int array.
typedef struct {
    unsigned int version, datasetId, dataTypeSize, isFloat, byteOrderIsLE, rank, dimSizes[];
} CompressorPrivate;
//Format of the cd_values array:
//cd_values[0] = version = 0;
//cd_values[1] = datasetId (created randomly);
//cd_values[2] = dataTypeSize (in bytes);
//cd_values[3] = isFloat (true, if the datatype is a float type);
//cd_values[4] = byteOrder (same as H5T_order_t);
//cd_values[5] = rank (# of dimension of a chunk);
//cd_values[6] = Size of first dimension (the size of the chunk!);
//...
//cd_values[6+rank-1] = Size of last dimension;
//Additional values is an error.

static herr_t compressorSetLocal(hid_t pList, hid_t type, hid_t space) {
//    fprintf(stderr, "compressorSetLocal()\n");
    int rank = H5Sget_simple_extent_ndims(space), i;
    if(rank <= 0) return -4;
    {
        int chunkRank;
        hsize_t *chunkSize;
        chunkSize = (hsize_t*)malloc(rank * sizeof(hsize_t));
        chunkRank = H5Pget_chunk(pList, rank, chunkSize);
        if(chunkRank <= 0) return -1;
        if(chunkRank > rank) return -2;
        {
            unsigned int *cd_values;
            CompressorPrivate* privates;
            cd_values = (unsigned int *)malloc((sizeof(CompressorPrivate)/sizeof(unsigned int) + (size_t)chunkRank) * sizeof(unsigned int));
            privates = (CompressorPrivate*)cd_values;
            privates->version = 0;
            privates->datasetId = (unsigned int)((rand() << 16) ^ rand());    //Two calls to make sure all bits are random, otherwise the msb would be zero...
            if(!(privates->dataTypeSize = (unsigned int)H5Tget_size(type))) return -5;
            privates->isFloat = (H5Tget_class(type) == H5T_FLOAT);
            switch(H5Tget_order(type)) {
                case H5T_ORDER_LE:
                    privates->byteOrderIsLE = 1;
                    break;
                case H5T_ORDER_BE:
                    privates->byteOrderIsLE = 0;
                    break;
                case H5T_ORDER_ERROR:
                case H5T_ORDER_VAX:
//                case H5T_ORDER_MIXED:
                case H5T_ORDER_NONE:
                default:
                    return -3;    //Don't know about VAX-Order...
            }
            privates->rank = (unsigned int)chunkRank;
            for(i = 0; i < chunkRank; i++) {
                if(chunkSize[i] > 0xffffffffl) return -6;    //Safety.
                privates->dimSizes[i] = (unsigned int)(chunkSize[i]);
            }
            return H5Pmodify_filter(
                pList,
                MAFISC_ID,
                H5Z_FLAG_OPTIONAL,
                sizeof(CompressorPrivate)/sizeof(unsigned int) + (unsigned int)chunkRank,
                cd_values
            );    //This filter is optional, even if the user thinks differently. Also remember our parameters.
        }
    }
}

static size_t decompress(size_t nBytes, size_t* bufferSize, void** buffer) {
//    fprintf(stderr, "decompress()\n");
    clock_t startTime = clock(), opStart;
    long i;
    CompressorPrivate* privates = (CompressorPrivate*)*buffer;
    //Get the native byte order.
    int64_t test64 = 0x00ffffffffffff01;
    int32_t test32 = 0x00ffff01;
    int16_t test16 = 0x0001;
    long nativeIsLE = *(char*)&test16;
    if(*(char*)&test64 != *(char*)&test32 || *(char*)&test32 != *(char*)&test16) return 0;    //Fail, if we got a mixed native byte order.
    //Read the compression parameters, doing some thorough error checking.
    if(nBytes < sizeof(CompressorPrivate)) return 0;    //Fail, if metadata is incomplete.
    if(privates->version != 0) return 0;    //Fail, if the metadata format is not known.
    if(nBytes < sizeof(CompressorPrivate) + privates->rank*sizeof(unsigned int)) return 0;    //Fail, if metadata is incomplete.
    {
        unsigned long chunkSize = 1;
        size_t *sizeTDims;
        size_t dataSize;
        const char* methodString;
        size_t methodStringLength;
        VariableFilter* filter;
        void* lzmaData, *buffer1, *buffer2;
        sizeTDims = (size_t *)malloc(privates->rank * sizeof(size_t));
        for(i = 0; i < privates->rank; i++) {
            chunkSize *= privates->dimSizes[i];
            sizeTDims[i] = privates->dimSizes[i];
        }
        methodString = (const char*)&privates->dimSizes[privates->rank];    //Point after the parameter data.
        methodStringLength = strnlen(methodString, nBytes - ((size_t)methodString - (size_t)*buffer));
        if(methodStringLength + 1 + ((size_t)methodString - (size_t)*buffer) >= nBytes) return 0;    //Fail, if there is no compressed data.
#ifdef USE_GLOBAL_CASCADE_FILTER
        fcf_setMethod((FilterCascadeFilter*)(filter = (VariableFilter*)getGlobalFilter()), methodString);
#else
        filter = vf_makeWithString(methodString);
#endif
        assert(filter);    //If this assert fails, then the most likely explanation is, that you tried to process a file produced with an incompatible newer version of mafisc. Technically speaking, this means that a mafisc filter was used for compression that this version does not know about. However, being out of memory could also explain this error.
        vf_setVarProperties(filter, privates->dataTypeSize, (int)(privates->rank), sizeTDims);
        //Decompress the data.
        lzmaData = (void*)((size_t)methodString + methodStringLength + 1);
        buffer1 = malloc(chunkSize*privates->dataTypeSize);
        dataSize = nBytes - ((size_t)lzmaData - (size_t)*buffer);
        opStart = clock();
        if(doLzmaDecompression(&dataSize, chunkSize*privates->dataTypeSize, lzmaData, buffer1)) goto error;
        lzmaDecompressionTime += clock()-opStart;
        if(dataSize != chunkSize*privates->dataTypeSize) goto error;    //Fail, if we couldn't decode a complete chunk.
        buffer2 = malloc(dataSize);
        //Inverse filter the data.
        if(!nativeIsLE && !vf_producesByteStream(filter)) switchByteOrder(privates->dataTypeSize, chunkSize, buffer1, buffer1);
        opStart = clock();
        if(filter) {
            vf_processReverse(filter, buffer1, buffer2);
        } else {
            memmove(buffer2, buffer1, dataSize);
        }
        filterDecompressionTime += clock()-opStart;
        //Convert back to (floating point) values with the required byte order.
        if((privates->byteOrderIsLE && !nativeIsLE) || (!privates->byteOrderIsLE && nativeIsLE)) {
            if(privates->isFloat) {
                doFpXpConversion(privates->dataTypeSize, privates->rank, sizeTDims, buffer2, buffer1);
                switchByteOrder(privates->dataTypeSize, chunkSize, buffer1, buffer2);
            } else {
                switchByteOrder(privates->dataTypeSize, chunkSize, buffer2, buffer2);
            }
        } else {
            if(privates->isFloat) {
                doFpXpConversion(privates->dataTypeSize, privates->rank, sizeTDims, buffer2, buffer2);
            } else {
            }
        }
        decompressedData += dataSize;
        //Remember the filter method.
#ifdef USE_GLOBAL_CASCADE_FILTER
        gSetCompressionMethod(privates->datasetId);
#else
        setCompressionMethod(privates->datasetId, filter);
#endif
        //Pass out the correct buffer and tidy up.
        free(*buffer);
        *buffer = buffer2;
        *bufferSize = dataSize;
        free(buffer1);
#ifndef USE_GLOBAL_CASCADE_FILTER
        if(filter) vf_delete(filter);
#endif
        decompressionTime += clock()-startTime;
        return dataSize;

error:
        fprintf(stderr, "DKRZ-Compression filter: Decompression Error!\n");
        free(buffer1);
        vf_delete(filter);
        return 0;
    }
}

static void doFiltering(const char* method, unsigned dataTypeSize, unsigned rank, size_t* dims, size_t valueCount, long nativeIsLE, const void* inputBuffer, void* outputBuffer) {
    VariableFilter* filter;
#ifdef USE_GLOBAL_CASCADE_FILTER
    filter = (VariableFilter*)getGlobalFilter();
    fcf_setMethod((FilterCascadeFilter*)filter, method);
#else
    filter = vf_makeWithString(method);
#endif
    if(filter) {
        vf_setVarProperties(filter, dataTypeSize, (int)rank, dims);
        vf_process(filter, inputBuffer, outputBuffer);
        if(!nativeIsLE && !vf_producesByteStream(filter)) {
            switchByteOrder(dataTypeSize, valueCount, outputBuffer, outputBuffer);
        }
#ifndef USE_GLOBAL_CASCADE_FILTER
        vf_delete(filter);
#endif
    } else {
        if(nativeIsLE) {
            memmove(outputBuffer, inputBuffer, dataTypeSize*valueCount);
        } else {
            switchByteOrder(dataTypeSize, valueCount, inputBuffer, outputBuffer);
        }
    }
}

static size_t compressorFilter(unsigned int flags, size_t cd_nelmts, const unsigned int cd_values[], size_t nBytes, size_t *buf_size, void **buf) {
    clock_t startTime = clock(), opStart;
    long i;
    const CompressorPrivate* privates = (const CompressorPrivate*)cd_values;
    //Get the native byte order.
    int64_t test64 = 0x00ffffffffffff01;
    int32_t test32 = 0x00ffff01;
    int16_t test16 = 0x0001;
    long nativeIsLE = *(char*)&test16;
    if(*(char*)&test64 != *(char*)&test32 || *(char*)&test32 != *(char*)&test16) return 0;    //Fail, if we got a mixed native byte order.
    //Check for decompression mode.
    if(flags & H5Z_FLAG_REVERSE) return decompress(nBytes, buf_size, buf);
    //Read the cd_values, doing some thorough error checking.
    if(cd_nelmts < 1) return 0;    //Fail, if no metadata is handed in.
    if(cd_values[0] != 0) return 0;    //Fail, if the metadata format is not known.
    if(cd_nelmts < sizeof(CompressorPrivate)/sizeof(unsigned int)) return 0;    //Fail, if there is not enough metadata.
    if(cd_nelmts != sizeof(CompressorPrivate)/sizeof(unsigned int) + privates->rank) return 0;    //Fail, if the dimension count is wrong.
    {
        unsigned long chunkSize = 1;
        size_t *sizeTDims;
        size_t curDataSize, bestDataSize, cd_valuesSize = cd_nelmts*sizeof(unsigned int), result, bestMethodStringSize;
        void* buffer1, *buffer2, *curBuffer, *bestBuffer;
        char* bestMethodString;
        sizeTDims = (size_t *)malloc(privates->rank * sizeof(size_t));
        for(i = 0; i < privates->rank; i++) {
            chunkSize *= privates->dimSizes[i];
            sizeTDims[i] = privates->dimSizes[i];
        }
        if(nBytes != chunkSize*privates->dataTypeSize) return 0;    //Refuse to work on anything but a complete chunk!
        //Set up the filters and the buffers.
        bestMethodString = getCompressionMethodString(privates->datasetId);
        buffer1 = malloc(nBytes);
        buffer2 = malloc(nBytes);
        curBuffer = malloc(nBytes);
        bestBuffer = malloc(nBytes);
        //Convert to 2-complement integers in native byte order.
        if((privates->byteOrderIsLE && !nativeIsLE) || (!privates->byteOrderIsLE && nativeIsLE)) {
            if(privates->isFloat) {
                switchByteOrder(privates->dataTypeSize, chunkSize, *buf, buffer2);
                doFpXpConversion(privates->dataTypeSize, privates->rank, sizeTDims, buffer2, buffer1);
            } else {
                switchByteOrder(privates->dataTypeSize, chunkSize, *buf, buffer1);
            }
        } else {
            if(privates->isFloat) {
                doFpXpConversion(privates->dataTypeSize, privates->rank, sizeTDims, *buf, buffer1);
            } else {
                memmove(buffer1, *buf, nBytes);
            }
        }
        //Filter the data and convert to little endian byte order. Even though I like BE better, LE is the de facto standart and it would be suboptimal to use a non native encoding on the majority of machines.
#ifdef USE_RANDOM_TESTING
        opStart = clock();
        doFiltering(bestMethodString, privates->dataTypeSize, privates->rank, sizeTDims, chunkSize, nativeIsLE, buffer1, buffer2);
        filterCompressionTime += clock()-opStart;
        bestDataSize = nBytes;
        opStart = clock();
        doLzmaCompression(&bestDataSize, buffer2, bestBuffer);    //No need to check result, the unchanged dataXSize is enough.
        lzmaCompressionTime += clock()-opStart;
        for(i = 0; i < RANDOM_TESTING_ROUNDS; i++) {
            char* curMethodString = randomCompressionMethodString();
#else
        bestDataSize = nBytes;    //Won't qualify as compression, so we don't need actual compressed data. We'll test all methods anyway.
        for(i = compressionMethodCount()-1; i >= 0; i--) {
            char* curMethodString = compressionMethodString((unsigned long)i);
#endif
            opStart = clock();
            doFiltering(curMethodString, privates->dataTypeSize, privates->rank, sizeTDims, chunkSize, nativeIsLE, buffer1, buffer2);
            filterCompressionTime += clock()-opStart;
            //Compress the data.
            curDataSize = nBytes;
            opStart = clock();
            doLzmaCompression(&curDataSize, buffer2, curBuffer);    //No need to check result, the unchanged dataXSize is enough.
            lzmaCompressionTime += clock()-opStart;
            //Which compression is best?
            if(strlen(bestMethodString) + bestDataSize > strlen(curMethodString) + curDataSize) {
                free(bestMethodString);
                bestMethodString = curMethodString;
                bestDataSize = curDataSize;
                void* temp2 = bestBuffer;
                bestBuffer = curBuffer;
                curBuffer = temp2;
            } else {
                free(curMethodString);
            }
        }
        //Ok, bestDataSize/bestBuffer/bestMethodString is best. But is it better than raw data?!?
        bestMethodStringSize = strlen(bestMethodString) + 1;
        if((result = cd_valuesSize + bestMethodStringSize + bestDataSize) < nBytes) {
            void* curWritePtr = *buf;
            copyUpdate(curWritePtr, cd_values, cd_valuesSize);
            copyUpdate(curWritePtr, bestMethodString, bestMethodStringSize);
            copyUpdate(curWritePtr, bestBuffer, bestDataSize);
            assert(result == curWritePtr-*buf);

            compressedData += nBytes;
            setCompressionMethodString(privates->datasetId, bestMethodString);
//            fprintf(stderr, "%s\n", bestMethodString);
        } else {
            result = 0;    //No success if not even a single byte could be saved! This also ensures that no buffer enlargement has to be performed.
        }
        //Tidy up.
        free(bestMethodString);
        free(buffer1);
        free(buffer2);
        free(curBuffer);
        free(bestBuffer);
        compressionTime += clock()-startTime;
        return result;
    }
}
