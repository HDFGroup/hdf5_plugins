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

#include "filter-cascade-filter.h"

const char* const filterDescriptionStrings[] = { "as", "al", "bb", "bg", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da", "db", "dc", "dd", "de", "df", "dg", "dh", "di", "dj", "dk", "dl", "dm", "dn", "do", "dp", "dq", "dr", "ds", "dt", "du", "dv", "dw", "dx", "dy", "dz", "l", "sr", "sn", "t" };

struct FilterCascadeFilter {
    VariableFilter super;
    VariableFilter** filters;    //This array owns the filters. It is synchronous to filterDescriptionStrings, for each filterDescriptionString there is an entry in here at the same position. Modifications of the filters themselves are made through it. No automatic array since C can't compute the number of filterDescriptionStrings at compile time.
    size_t filterCount;    //Could have been a global const.

    VariableFilter** cascade;    //References the filters in the filters array and describes the current filtering method. May reference an individual filter any number of times.
    size_t cascadeLength;    //Count of the currently active entries.
    size_t cascadeSize;    //Size of the cascade array in entries.

    void* buffer;    //Used to pass the output from filter to filter.
    size_t bufferSize;    //Size of the buffer in bytes.
};

#define allocate(type, name, count) type* name = (type*)malloc(sizeof(type)*count)

FilterCascadeFilter* fcf_make(void) {
    allocate(FilterCascadeFilter, me, 1);
    if(!me) return 0;
    return fcf_init(me);
}

FilterCascadeFilter* fcf_init(FilterCascadeFilter* me) {
    size_t i;
    me->filters = me->cascade = (VariableFilter**)(me->buffer = NULL);
    vf_init(super(), kFilterCascadeFilterClass);
    me->filterCount = sizeof(filterDescriptionStrings)/sizeof(char*);
    me->filters = (VariableFilter**)calloc(me->filterCount, sizeof(VariableFilter*));
    for(i = 0; i < me->filterCount; i++) if(!(me->filters[i] = vf_makeWithString(filterDescriptionStrings[i]))) goto error;
    me->cascadeLength = 0;
    me->cascadeSize = 16;
    if(!(me->cascade = (VariableFilter**)malloc(me->cascadeSize * sizeof(VariableFilter*)))) goto error;
    me->bufferSize = 0;
    if(!(me->buffer = malloc(me->bufferSize))) goto error;
    return me;

error:
    vf_delete(super());
    return 0;
}

int fcf_setMethod(FilterCascadeFilter* me, const char* method) {
    size_t chars = strlen(method);
    if(chars > me->cascadeSize) {
        allocate(VariableFilter*, newCascade, chars);
        size_t newLength = 0;
        while(*method) {
            size_t i;
            for(i = 0; i < me->filterCount; i++) {
                if(!memcmp(filterDescriptionStrings[i], method, strlen(filterDescriptionStrings[i]))) break;
            }
            if(i >= me->filterCount) return -1;
            newCascade[newLength++] = me->filters[i];
            method += strlen(filterDescriptionStrings[i]);
        }
        free(me->cascade);
        me->cascade = newCascade;
        me->cascadeLength = newLength;
        me->cascadeSize = chars;
    } else {
        me->cascadeLength = 0;
        while(*method) {
            size_t i;
            for(i = 0; i < me->filterCount; i++) {
                if(!memcmp(filterDescriptionStrings[i], method, strlen(filterDescriptionStrings[i]))) break;
            }
            if(i >= me->filterCount) return -1;
            me->cascade[me->cascadeLength++] = me->filters[i];
            method += strlen(filterDescriptionStrings[i]);
        }
    }
    return 0;
}

char* fcf_toString(FilterCascadeFilter* me) {
    allocate(char, result, 1);
    size_t i;
    if(!result) return 0;
    *result = 0;    //Start with an empty string.
    for(i = 0; i < me->cascadeLength; i++) {
        char* temp1 = vf_toString(me->cascade[i]), *temp2;
        if(asprintf(&temp2, "%s%s", result, temp1) < 0) return 0;
        free(temp1);
        free(result);
        result = temp2;
    }
    return result;
}

void fcf_varPropertiesSet(FilterCascadeFilter* me) {
    size_t i;
    for(i = 0; i < me->filterCount; i++) {
        vf_setVarProperties(me->filters[i], super()->valueSize, super()->dimensions, super()->size);
    }
    if(super()->valueSize * super()->valueCount > me->bufferSize) {
        free(me->buffer);
        me->bufferSize = super()->valueSize * super()->valueCount;
        me->buffer = malloc(me->bufferSize);
    }
}

void fcf_process(FilterCascadeFilter* me, const void* inputBuffer, void* outputBuffer) {
    size_t i, j;
    if(me->cascadeLength & 1) {
        memcpy(me->buffer, inputBuffer, super()->valueSize * super()->valueCount);
    } else {
        memcpy(outputBuffer, inputBuffer, super()->valueSize * super()->valueCount);
    }
    for(i = me->cascadeLength, j = 0; i; i--, j++) {
        if(i & 1) {
            vf_process(me->cascade[j], me->buffer, outputBuffer);
        } else {
            vf_process(me->cascade[j], outputBuffer, me->buffer);
        }
    }
}

void fcf_processReverse(FilterCascadeFilter* me, const void* inputBuffer, void* outputBuffer) {
    size_t i;
    if(me->cascadeLength & 1) {
        memcpy(me->buffer, inputBuffer, super()->valueSize * super()->valueCount);
    } else {
        memcpy(outputBuffer, inputBuffer, super()->valueSize * super()->valueCount);
    }
    for(i = me->cascadeLength; i; i--) {
        if(i & 1) {
            vf_processReverse(me->cascade[i-1], me->buffer, outputBuffer);
        } else {
            vf_processReverse(me->cascade[i-1], outputBuffer, me->buffer);
        }
    }
}

int fcf_producesByteStream(FilterCascadeFilter* me) {
    if(me->cascadeLength) return vf_producesByteStream(me->cascade[me->cascadeLength-1]);
    return 0;
}

void fcf_destroy(FilterCascadeFilter* me) {
    if(me->filters) {
        size_t i;
        for(i = 0; i < me->filterCount; i++) if(me->filters[i]) vf_delete(me->filters[i]);
        free(me->filters);
    }
    if(me->cascade) free(me->cascade);
    if(me->buffer) free(me->buffer);
}
