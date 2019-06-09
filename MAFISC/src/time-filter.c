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

#include "config.h"

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

#include "time-filter.h"

TimeFilter* tf_makeWithString(const char** description) {
    description = NULL;    //Shut up the compiler.
    return tf_make();
}

TimeFilter* tf_make(void) {
//    printf("tf_make()\n");
    TimeFilter* me = (TimeFilter*)malloc(sizeof(TimeFilter));
    tf_init(me);
    return me;
}

void tf_init(TimeFilter* me) {
//    printf("tf_init()\n");
    vf_init(&(me->super), kTimeFilterClass);
    me->referenceBuffer = 0;
}

char* tf_toString(TimeFilter* me) {
    char* result = 0;
    if(asprintf(&result, "%c", kFilterClassIdLetters[super()->type]) <= 0) return 0;
    return result;
}

void tf_varPropertiesSet(TimeFilter* me) {
//    printf("tf_varPropertiesSet()\n");
    if(me->referenceBuffer) free(me->referenceBuffer);
    me->referenceBuffer = calloc(super()->valueCount, super()->valueSize);    //get a zero initialized buffer
}

#define template(T)\
void tf_process_##T(TimeFilter* me, const void* inputBuffer, void* outputBuffer);    /*Shut up.*/\
void tf_process_##T(TimeFilter* me, const void* inputBuffer, void* outputBuffer) {\
    int64_t i;\
    const T *iBuf = (const T*)inputBuffer;\
    T temp, *oBuf = (T*)outputBuffer, *rBuf = (T*)(me->referenceBuffer);\
    for(i = (int64_t)(super()->valueCount-1); i >= 0; i--) {\
        temp = iBuf[i];\
        oBuf[i] = (T)(temp - rBuf[i]);\
        rBuf[i] = temp;\
    }\
}
template(int8_t)
template(int16_t)
template(int32_t)
template(int64_t)
#undef template

void tf_process(TimeFilter* me, const void* inputBuffer, void* outputBuffer) {
//    printf("tf_process()\n");
    switch(super()->valueSize) {
        case 1: tf_process_int8_t(me, inputBuffer, outputBuffer); break;
        case 2: tf_process_int16_t(me, inputBuffer, outputBuffer); break;
        case 4: tf_process_int32_t(me, inputBuffer, outputBuffer); break;
        case 8: tf_process_int64_t(me, inputBuffer, outputBuffer); break;
        default: assert(0);
    }
}

#define template(T)\
void tf_processReverse_##T(TimeFilter* me, const void* inputBuffer, void* outputBuffer);    /*Shut up.*/\
void tf_processReverse_##T(TimeFilter* me, const void* inputBuffer, void* outputBuffer) {\
    int64_t i;\
    const T *iBuf = (const T*)inputBuffer;\
    T *oBuf = (T*)outputBuffer, *rBuf = (T*)(me->referenceBuffer);\
    for(i = (int64_t)(super()->valueCount-1); i >= 0; i--) {\
        rBuf[i] = oBuf[i] = (T)(iBuf[i] + rBuf[i]);\
    }\
}
template(int8_t)
template(int16_t)
template(int32_t)
template(int64_t)
#undef template

void tf_processReverse(TimeFilter* me, const void* inputBuffer, void* outputBuffer) {
//    printf("tf_process()\n");
    switch(super()->valueSize) {
        case 1: tf_processReverse_int8_t(me, inputBuffer, outputBuffer); break;
        case 2: tf_processReverse_int16_t(me, inputBuffer, outputBuffer); break;
        case 4: tf_processReverse_int32_t(me, inputBuffer, outputBuffer); break;
        case 8: tf_processReverse_int64_t(me, inputBuffer, outputBuffer); break;
        default: assert(0);
    }
}

int tf_producesByteStream(TimeFilter* me) { return 0; }

void tf_destroy(TimeFilter* me) {
//    printf("tf_destroy()\n");
    if(me->referenceBuffer) free(me->referenceBuffer);
    vf_destroy(&(me->super));
}
