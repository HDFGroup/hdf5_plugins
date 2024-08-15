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

#include "difference-filter.h"

/*typedef struct {
    VariableFilter super;
    int workingDim;
} DifferenceFilter;*/

char makeBase36Digit(int value) {
    if(value < 0) return '0';
    if(value < 10) return '0' + value;
    if(value < 36) return 'a' + value - 10;
    return 'z';
}

int readBase36Digit(char digit) {
    if('0' <= digit && digit <= '9') return digit - '0';
    if('a' <= digit && digit <= 'z') return digit - 'a' + 10;
    if('A' <= digit && digit <= 'Z') return digit - 'A' + 10;
    return 0;
}

DifferenceFilter* df_makeWithString(const char** description) {
    if(!**description) return df_make(35);    //No reading past the end of the string!
    return df_make(readBase36Digit(*((*description)++)));
}

DifferenceFilter* df_make(int workingDim) {
    DifferenceFilter* me = (DifferenceFilter*)malloc(sizeof(DifferenceFilter));
    df_init(me, workingDim);
    return me;
}

void df_init(DifferenceFilter* me, int workingDim) {
    vf_init(super(), kDifferenceFilterClass);
    me->workingDim = workingDim;
    if(me->workingDim < 0) me->workingDim = 0;
    if(me->workingDim >= 36) me->workingDim = 35;    //Make sure that we can store the filter in a description string.
    //No check against the dimension count, since the dimension count of the dataset is not known here. Even worse, it can be changed later, so we need to remember the requested dimension, even if when it is unusable.
}

char* df_toString(DifferenceFilter* me) {
    char* result = 0;
    if(asprintf(&result, "%c%c", kFilterClassIdLetters[super()->type], makeBase36Digit(me->workingDim)) <= 0) return 0;
    return result;
}

#define template(T) \
void df_process_##T(DifferenceFilter* me, const void* inputBuffer, void* outputBuffer);    /*Shut up.*/\
void df_process_##T(DifferenceFilter* me, const void* inputBuffer, void* outputBuffer) {\
    const T* in = (const T*)inputBuffer;\
    T* out = (T*)outputBuffer;\
    uint64_t i, count = super()->valueCount;\
    int curDimension = super()->dimensions - 1;\
    uint64_t curOffset = 1, nextOffset = super()->size[curDimension];\
    if(curDimension <= me->workingDim) nextOffset = 0xffffffffffffffffl;    /*Do not increment the offset further.*/\
    out[0] = in[0];\
    for(i = 1; i < count; i++) {\
        if(i >= nextOffset) {\
            curOffset = nextOffset;\
            curDimension--;\
            nextOffset *= super()->size[curDimension];\
            if(curDimension <= me->workingDim) nextOffset = 0xffffffffffffffffl;    /*Do not increment the offset further.*/\
        }\
        out[i] = (T)(in[i] - in[i - curOffset]);\
    }\
}
template(int8_t)
template(int16_t)
template(int32_t)
template(int64_t)
#undef template

void df_process(DifferenceFilter* me, const void* inputBuffer, void* outputBuffer) {
    switch(super()->valueSize) {
        case 1: df_process_int8_t(me, inputBuffer, outputBuffer); break;
        case 2: df_process_int16_t(me, inputBuffer, outputBuffer); break;
        case 4: df_process_int32_t(me, inputBuffer, outputBuffer); break;
        case 8: df_process_int64_t(me, inputBuffer, outputBuffer); break;
        default: assert(0);
    }
}

#define template(T) \
void df_processReverse_##T(DifferenceFilter* me, const void* inputBuffer, void* outputBuffer);    /*Shut up.*/\
void df_processReverse_##T(DifferenceFilter* me, const void* inputBuffer, void* outputBuffer) {\
    const T* in = (const T*)inputBuffer;\
    T* out = (T*)outputBuffer;\
    uint64_t i, count = super()->valueCount;\
    int curDimension = super()->dimensions - 1;\
    uint64_t curOffset = 1, nextOffset = super()->size[curDimension];\
    if(curDimension <= me->workingDim) nextOffset = 0xffffffffffffffffl;    /*Do not increment the offset further.*/\
    out[0] = in[0];\
    for(i = 1; i < count; i++) {\
        if(i >= nextOffset) {\
            curOffset = nextOffset;\
            curDimension--;\
            nextOffset *= super()->size[curDimension];\
            if(curDimension <= me->workingDim) nextOffset = 0xffffffffffffffffl;    /*Do not increment the offset further.*/\
        }\
        out[i] = (T)(in[i] + out[i - curOffset]);\
    }\
}
template(int8_t)
template(int16_t)
template(int32_t)
template(int64_t)
#undef template

void df_processReverse(DifferenceFilter* me, const void* inputBuffer, void* outputBuffer) {
    switch(super()->valueSize) {
        case 1: df_processReverse_int8_t(me, inputBuffer, outputBuffer); break;
        case 2: df_processReverse_int16_t(me, inputBuffer, outputBuffer); break;
        case 4: df_processReverse_int32_t(me, inputBuffer, outputBuffer); break;
        case 8: df_processReverse_int64_t(me, inputBuffer, outputBuffer); break;
        default: assert(0);
    }
}

int df_producesByteStream(DifferenceFilter* me) { return 0; }

void df_varPropertiesSet(DifferenceFilter* me) {
    me = NULL;    //Shut up.
}

void df_destroy(DifferenceFilter* me) {
    vf_destroy(super());
}

