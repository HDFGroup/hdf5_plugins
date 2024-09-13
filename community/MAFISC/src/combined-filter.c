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

#include "combined-filter.h"

/*typedef struct {
    VariableFilter super, *first, *second;
    void* tempBuffer;    //Used to pass the output of the first filter to the second filter
    size_t valueSize, bufferSize;
} CombinedFilter;*/

CombinedFilter* cf_make(VariableFilter* first, VariableFilter* second) {
//    printf("cf_make()\n");
    CombinedFilter* me;
    if(!(first && second)) return 0;
    me = (CombinedFilter*)malloc(sizeof(CombinedFilter));
    cf_init(me, first, second);
    return me;
}

void cf_init(CombinedFilter* me, VariableFilter* first, VariableFilter* second) {
    vf_init(super(), kCombinedFilterClass);
    me->first = first;
    me->second = second;
    me->tempBuffer = 0;
}

char* cf_toString(CombinedFilter* me) {
    char* firstString = 0, *secondString = 0, *result = 0;
    if(!me->first || !me->second) return 0;
    if(!(firstString = vf_toString(me->first))) return 0;
    if(!(secondString = vf_toString(me->second))) goto errorSecond;
    if(asprintf(&result, "%s%s", firstString, secondString) <= 0) result = 0;    //Yes, I know I'm being parnoid setting it back to zero after a failed call...

    free(secondString);
errorSecond:
    free(firstString);
    return result;
}

void cf_varPropertiesSet(CombinedFilter* me) {
//    printf("cf_varPropertiesSet(t)\n");
    if(me->tempBuffer) free(me->tempBuffer);
    me->tempBuffer = malloc(super()->valueCount*super()->valueSize);
    vf_setVarProperties(me->first, super()->valueSize, super()->dimensions, super()->size);
    vf_setVarProperties(me->second, super()->valueSize, super()->dimensions, super()->size);
}

void cf_process(CombinedFilter* me, const void* inputBuffer, void* outputBuffer) {
    vf_process(me->first, inputBuffer, me->tempBuffer);
    vf_process(me->second, me->tempBuffer, outputBuffer);
}

void cf_processReverse(CombinedFilter* me, const void* inputBuffer, void* outputBuffer) {
    vf_processReverse(me->second, inputBuffer, me->tempBuffer);
    vf_processReverse(me->first, me->tempBuffer, outputBuffer);
}

int cf_producesByteStream(CombinedFilter* me) {
    return vf_producesByteStream(me->second);
}

size_t cf_getOutputSize(CombinedFilter* me) {
    return vf_getOutputSize(me->second);
}

void cf_destroy(CombinedFilter* me) {
//    printf("cf_destroy()\n");
    if(me->tempBuffer) free(me->tempBuffer);
    if(me->first) vf_delete(me->first);
    if(me->second) vf_delete(me->second);
    vf_destroy(super());
}

