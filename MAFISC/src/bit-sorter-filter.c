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
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include <stdio.h>
#include <assert.h>

#include "bit-sorter-filter.h"

/*typedef struct {
    VariableFilter super;
    kSortingMode mode;
} BitSorterFilter;*/

BitSorterFilter* bsf_makeWithString(const char** description) {
    char modeByte;
    if(!**description) return 0;    //Safety.
    modeByte = *((*description)++);
    if(modeByte == 'g') return bsf_make(kGlobalSort);
    if(modeByte == 'b') return bsf_make(kByteSort);
    return 0;
}

BitSorterFilter* bsf_make(SortingMode mode) {
    BitSorterFilter* me = (BitSorterFilter*)malloc(sizeof(BitSorterFilter));
    bsf_init(me, mode);
    return me;
}

char* bsf_toString(BitSorterFilter* me) {
    char* result = 0;
    switch(me->mode) {
        case kGlobalSort: {
            if(asprintf(&result, "%cg", kFilterClassIdLetters[super()->type]) <= 0) return 0;
        } break; case kByteSort: {
            if(asprintf(&result, "%cb", kFilterClassIdLetters[super()->type]) <= 0) return 0;
        } break;
        default: assert(0);
    }
    return result;
}

#define template(T)\
/*T must be unsigned!*/\
void bsf_byte_sort_##T(BitSorterFilter* me, const void* inputBuffer, void* outputBuffer);    /*Shut up.*/\
void bsf_byte_sort_##T(BitSorterFilter* me, const void* inputBuffer, void* outputBuffer) {\
    assert(super()->valueSize == sizeof(T));\
    {\
        size_t valueCount = super()->valueCount, i;\
        const T* in = (const T*)inputBuffer;\
        T* out = (T*)outputBuffer;\
        for(i = 0; i < valueCount; i++) {\
            T curValue = in[i];\
            T outputValue = 0;\
            uint8_t* outputBytes = (uint8_t*)&outputValue;\
            unsigned int j, k;\
            for(j = 0; j < 8; j++) {\
                for(k = 0; k < sizeof(T); k++) {\
                    outputBytes[k] = (uint8_t)((T)(outputBytes[k] << 1) | (curValue >> (8*sizeof(T) - 1)));\
                    curValue = (T)(curValue << 1);\
                }\
            }\
            out[i] = outputValue;\
        }\
    }\
}
template(uint8_t)
template(uint16_t)
template(uint32_t)
template(uint64_t)
#undef template

void bsf_process(BitSorterFilter* me, const void* inputBuffer, void* outputBuffer) {
    size_t valueSize = super()->valueSize;
    switch(me->mode) {
        case kGlobalSort: {
            size_t valueCount = super()->valueCount;
            size_t bit = 0, value = 0, outputBit = 0;
            uint8_t outputByte = 0, curBit;
            for(; bit < 8*valueSize; bit++) {
                for(value = 0; value < valueCount; value++, outputBit++) {
                    outputByte = (uint8_t)(outputByte << 1);
                    curBit = (uint8_t)(((const uint8_t*)inputBuffer)[value*valueSize + (bit >> 3)] << (bit & 7));
                    outputByte |= curBit >> 7;
                    if((outputBit & 7) == 7) {
                        ((uint8_t*)outputBuffer)[outputBit >> 3] = outputByte;
                        outputByte = 0;
                    }
                }
            }
        } break; case kByteSort: {
            switch(valueSize) {
                case 1: bsf_byte_sort_uint8_t(me, inputBuffer, outputBuffer); break;
                case 2: bsf_byte_sort_uint16_t(me, inputBuffer, outputBuffer); break;
                case 4: bsf_byte_sort_uint32_t(me, inputBuffer, outputBuffer); break;
                case 8: bsf_byte_sort_uint64_t(me, inputBuffer, outputBuffer); break;
                default: assert(0);
            }
        } break; default: assert(0);
    }
}

#define template(T)\
/*T must be unsigned!*/\
void bsf_byte_unsort_##T(BitSorterFilter* me, const void* inputBuffer, void* outputBuffer);    /*Shut up.*/\
void bsf_byte_unsort_##T(BitSorterFilter* me, const void* inputBuffer, void* outputBuffer) {\
    assert(super()->valueSize == sizeof(T));\
    {\
        size_t valueCount = super()->valueCount, i;\
        const T* in = (const T*)inputBuffer;\
        T* out = (T*)outputBuffer;\
        for(i = 0; i < valueCount; i++) {\
            T curValue = in[i];\
            T outputValue = 0;\
            uint8_t* inputBytes = (uint8_t*)&curValue;\
            unsigned int j, k;\
            for(j = 0; j < 8; j++) {\
                for(k = 0; k < sizeof(T); k++) {\
                    uint8_t curBit = (uint8_t)(inputBytes[k] << j);\
                    outputValue = (T)((outputValue << 1) | (curBit >> 7));\
                }\
            }\
            out[i] = outputValue;\
        }\
    }\
}
template(uint8_t)
template(uint16_t)
template(uint32_t)
template(uint64_t)
#undef template

void bsf_processReverse(BitSorterFilter* me, const void* inputBuffer, void* outputBuffer) {
    size_t valueSize = super()->valueSize;
    switch(me->mode) {
        case kGlobalSort: {
            size_t valueCount = super()->valueCount;
            size_t bit = 0, value = 0, outputBit = 0;
            uint8_t outputByte = 0;
            for(value = 0; value < valueCount; value++) {
                for(bit = 0; bit < 8*valueSize; bit++, outputBit++) {
                    size_t inputBit = value + valueCount*bit;
                    uint8_t curBit = (uint8_t)(((const uint8_t*)inputBuffer)[inputBit >> 3] << (inputBit & 7));
                    outputByte = (uint8_t)(outputByte << 1);
                    outputByte |= curBit >> 7;
                    if((outputBit & 7) == 7) {
                        ((uint8_t*)outputBuffer)[outputBit >> 3] = outputByte;
                        outputByte = 0;
                    }
                }
            }
        } break; case kByteSort: {
            switch(valueSize) {
                case 1: bsf_byte_unsort_uint8_t(me, inputBuffer, outputBuffer); break;
                case 2: bsf_byte_unsort_uint16_t(me, inputBuffer, outputBuffer); break;
                case 4: bsf_byte_unsort_uint32_t(me, inputBuffer, outputBuffer); break;
                case 8: bsf_byte_unsort_uint64_t(me, inputBuffer, outputBuffer); break;
                default: assert(0);
            }
        } break; default: assert(0);
    }
}

void bsf_init(BitSorterFilter* me, SortingMode mode) {
    vf_init(super(), kBitSorterFilterClass);
    me->mode = mode;
}

int bsf_producesByteStream(BitSorterFilter* me) { return 1; }

void bsf_varPropertiesSet(BitSorterFilter* me) {
    me = 0;    //Shut up.
}

void bsf_destroy(BitSorterFilter* me) {
    vf_destroy(super());
}

