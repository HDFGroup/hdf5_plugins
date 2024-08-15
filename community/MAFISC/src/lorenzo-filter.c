/*
* Copyright (c) 2012 Nathanael Hübbe
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

#include "mafisc_config.h"

#define _GNU_SOURCE

#include <stdio.h>
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
#include <assert.h>

#include "lorenzo-filter.h"

/*typedef struct {
    VariableFilter super;
} LorenzoFilter;*/

LorenzoFilter* lf_makeWithString(const char** description) {
    return lf_make();
}

LorenzoFilter* lf_make() {
    LorenzoFilter* me = (LorenzoFilter*)malloc(sizeof(LorenzoFilter));
    lf_init(me);
    return me;
}

void lf_init(LorenzoFilter* me) {
    vf_init(super(), kLorenzoFilterClass);
}

char* lf_toString(LorenzoFilter* me) {
    char* result = 0;
    if(asprintf(&result, "%c", kFilterClassIdLetters[super()->type]) <= 0) return 0;
    return result;
}

//TODO: Optimize for speed using the observation, that two neighbouring hypercubes share an (n-1)-dimensional hypercube.
#define template(T) \
void lf_process_##T(LorenzoFilter* me, const void* inputBuffer, void* outputBuffer);    /*Shut up.*/\
void lf_process_##T(LorenzoFilter* me, const void* inputBuffer, void* outputBuffer) {\
    const T* in = (const T*)inputBuffer;\
    T* out = (T*)outputBuffer;\
    uint64_t i, j, count = super()->valueCount, curDimensionOffset;\
    int dimensionCount = super()->dimensions, curDimension;\
    /*Calculate the offsets of the corners of a hypercube and the corresponding signs.*/\
    uint64_t offsetCount = 1 << dimensionCount;\
    uint64_t* offsets = (uint64_t*)malloc(sizeof(uint64_t)*offsetCount);\
    T* signs = (T*)malloc(sizeof(T)*offsetCount);\
    offsets[0] = 0;\
    signs[0] = (T)1;\
    curDimensionOffset = 1;\
    for(curDimension = dimensionCount-1; curDimension >= 0; curDimension--) {\
        uint64_t subcubeSize = 1 << (dimensionCount - curDimension - 1);\
        for(i = 0; i < subcubeSize; i++) {\
            offsets[i+subcubeSize] = offsets[i] + curDimensionOffset;\
            signs[i+subcubeSize] = signs[i]*(T)-1l;\
        }\
        curDimensionOffset *= super()->size[curDimension];\
    }\
    /*Filter the data*/\
    for(i = 0; i < count; i++) {\
        T filteredValue = 0;\
        for(j = 0; j < offsetCount && offsets[j] <= i; j++) {\
            filteredValue += signs[j]*in[i - offsets[j]];\
        }\
        out[i] = filteredValue;\
    }\
}
template(int8_t)
template(int16_t)
template(int32_t)
template(int64_t)
#undef template

void lf_process(LorenzoFilter* me, const void* inputBuffer, void* outputBuffer) {
    switch(super()->valueSize) {
        case 1: lf_process_int8_t(me, inputBuffer, outputBuffer); break;
        case 2: lf_process_int16_t(me, inputBuffer, outputBuffer); break;
        case 4: lf_process_int32_t(me, inputBuffer, outputBuffer); break;
        case 8: lf_process_int64_t(me, inputBuffer, outputBuffer); break;
        default: assert(0);
    }
}

#define template(T) \
void lf_processReverse_##T(LorenzoFilter* me, const void* inputBuffer, void* outputBuffer);    /*Shut up.*/\
void lf_processReverse_##T(LorenzoFilter* me, const void* inputBuffer, void* outputBuffer) {\
    const T* in = (const T*)inputBuffer;\
    T* out = (T*)outputBuffer;\
    uint64_t i, j, count = super()->valueCount, curDimensionOffset;\
    int dimensionCount = super()->dimensions, curDimension;\
    /*Calculate the offsets of the corners of a hypercube and the corresponding signs.*/\
    uint64_t offsetCount = 1 << dimensionCount;\
    uint64_t* offsets = (uint64_t*)malloc(sizeof(uint64_t)*offsetCount);\
    T* signs = (T*)malloc(sizeof(T)*offsetCount);\
    offsets[0] = 0;\
    signs[0] = (T)-1l;    /*Initialize all the signs negative of the encoding one, this effectively inverts the prediction part.*/\
    curDimensionOffset = 1;\
    for(curDimension = dimensionCount-1; curDimension >= 0; curDimension--) {\
        uint64_t subcubeSize = 1 << (dimensionCount - curDimension - 1);\
        for(i = 0; i < subcubeSize; i++) {\
            offsets[i+subcubeSize] = offsets[i] + curDimensionOffset;\
            signs[i+subcubeSize] = signs[i]*(T)-1l;\
        }\
        curDimensionOffset *= super()->size[curDimension];\
    }\
    signs[0] = (T)1;    /*The stored difference, however, should not be inverted.*/\
    /*Filter the data*/\
    for(i = 0; i < count; i++) {\
        T filteredValue = in[i];\
        for(j = 1; j < offsetCount && offsets[j] <= i; j++) {\
            filteredValue += signs[j]*out[i - offsets[j]];\
        }\
        out[i] = filteredValue;\
    }\
}
template(int8_t)
template(int16_t)
template(int32_t)
template(int64_t)
#undef template

void lf_processReverse(LorenzoFilter* me, const void* inputBuffer, void* outputBuffer) {
    switch(super()->valueSize) {
        case 1: lf_processReverse_int8_t(me, inputBuffer, outputBuffer); break;
        case 2: lf_processReverse_int16_t(me, inputBuffer, outputBuffer); break;
        case 4: lf_processReverse_int32_t(me, inputBuffer, outputBuffer); break;
        case 8: lf_processReverse_int64_t(me, inputBuffer, outputBuffer); break;
        default: assert(0);
    }
}

int lf_producesByteStream(LorenzoFilter* me) { return 0; }

void lf_varPropertiesSet(LorenzoFilter* me) {
    me = NULL;    //Shut up.
}

void lf_destroy(LorenzoFilter* me) {
    vf_destroy(super());
}

