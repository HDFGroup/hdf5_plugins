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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "variable-filter.h"
#include "adaptive-filter.h"
#include "combined-filter.h"
#include "time-filter.h"
#include "difference-filter.h"
#include "bit-sorter-filter.h"
#include "sign-filter.h"
#include "lorenzo-filter.h"
#include "filter-cascade-filter.h"

const char kFilterClassIdLetters[] = {
    'a',    //kAdaptiveFilterClass
    ' ',    //kCombinedFilterClass. Illegal in a description string, they are inserted as needed.
    't',    //kTimeFilterClass
    'd',    //kDifferenceFilterClass
    'b',    //kBitSorterFilterClass
    's',    //kSignFilterClass
    'l',    //kLorenzoFilterClass
};

VariableFilter* vf_makeWithString(const char* description) {
    unsigned long i;
    VariableFilter* firstFilter = 0, *result = 0;
    if(!*description) return 0;    //Safety first!
    for(i = 0; i < sizeof(kFilterClassIdLetters); i++) if(kFilterClassIdLetters[i] == *description) break;
    if(i >= sizeof(kFilterClassIdLetters)) return 0;    //Trying to process a file with an unknown mafisc filter (produced by a newer version?)
    description++;
    switch((vf_type)i) {
        case kAdaptiveFilterClass: firstFilter = (VariableFilter*)af_makeWithString(&description); break;
        case kCombinedFilterClass: return 0;
        case kTimeFilterClass: firstFilter = (VariableFilter*)tf_makeWithString(&description); break;
        case kDifferenceFilterClass: firstFilter = (VariableFilter*)df_makeWithString(&description); break;
        case kBitSorterFilterClass: firstFilter = (VariableFilter*)bsf_makeWithString(&description); break;
        case kSignFilterClass: firstFilter = (VariableFilter*)sf_makeWithString(&description); break;
        case kLorenzoFilterClass: firstFilter = (VariableFilter*)lf_makeWithString(&description); break;
        case kFilterCascadeFilterClass: return 0;
        default: assert(0);
    }
    if(!firstFilter) return 0;    //Safety!
    if(!*description) return firstFilter;    //Was it the last one?
    if(vf_producesByteStream(firstFilter)) {    //Since all filters expect a value stream as input, the first filter must not produce a byte stream if it's not the only one.
        vf_delete(firstFilter);
        return 0;
    }
    //Combine the first filter with the rest filter otherwise.
    result = (VariableFilter*)cf_make(firstFilter, vf_makeWithString(description));
    if(!result) vf_delete(firstFilter);
    return result;
}

char* vf_toString(VariableFilter* me) {
    if(!me) {
        char* result = (char*)malloc(1);
        *result = 0;
        return result;
    }
    switch(me->type) {
        case kAdaptiveFilterClass: return af_toString((AdaptiveFilter*)me);
        case kCombinedFilterClass: return cf_toString((CombinedFilter*)me);
        case kTimeFilterClass: return tf_toString((TimeFilter*)me);
        case kDifferenceFilterClass: return df_toString((DifferenceFilter*)me);
        case kBitSorterFilterClass: return bsf_toString((BitSorterFilter*)me);
        case kSignFilterClass: return sf_toString((SignFilter*)me);
        case kLorenzoFilterClass: return lf_toString((LorenzoFilter*)me);
        case kFilterCascadeFilterClass: return fcf_toString((FilterCascadeFilter*)me);
        default: assert(0);
    }
    return 0;
}

void vf_setVarProperties(VariableFilter* me, size_t dataTypeSize, int dimCount, size_t* dimSizes) {
//    fprintf(stderr, "vf_setVarProperties(%p, %li, %i, %p)\n", me, dataTypeSize, dimCount, dimSizes);
    int i;
    if(me->size) free(me->size);
    me->dimensions = dimCount;
    me->size = (size_t*)malloc(sizeof(size_t) * (size_t)dimCount);
    for(i = 0; i < dimCount; i++) me->size[i] = dimSizes[i];
    me->valueCount = 1;
    me->valueSize = dataTypeSize;
    for(i = 0; i < me->dimensions; i++) me->valueCount *= me->size[i];
    switch(me->type) {
        case kAdaptiveFilterClass: af_varPropertiesSet((AdaptiveFilter*)me); break;
        case kCombinedFilterClass: cf_varPropertiesSet((CombinedFilter*)me); break;
        case kTimeFilterClass: tf_varPropertiesSet((TimeFilter*)me); break;
        case kDifferenceFilterClass: df_varPropertiesSet((DifferenceFilter*)me); break;
        case kBitSorterFilterClass: bsf_varPropertiesSet((BitSorterFilter*)me); break;
        case kSignFilterClass: sf_varPropertiesSet((SignFilter*)me); break;
        case kLorenzoFilterClass: lf_varPropertiesSet((LorenzoFilter*)me); break;
        case kFilterCascadeFilterClass: fcf_varPropertiesSet((FilterCascadeFilter*)me); break;
        default: assert(0);
    }
}

void vf_init(VariableFilter* me, vf_type aType) {
//    printf("vf_init()\n");
    me->type = aType;
    me->size = 0;
}

void vf_process(VariableFilter* me, const void* inputBuffer, void* outputBuffer) {
//    printf("vf_process()\n");
    if(!me->size) return;    //make sure, its in the operational state
    switch(me->type) {
        case kAdaptiveFilterClass: af_process((AdaptiveFilter*)me, inputBuffer, outputBuffer); break;
        case kCombinedFilterClass: cf_process((CombinedFilter*)me, inputBuffer, outputBuffer); break;
        case kTimeFilterClass: tf_process((TimeFilter*)me, inputBuffer, outputBuffer); break;
        case kDifferenceFilterClass: df_process((DifferenceFilter*)me, inputBuffer, outputBuffer); break;
        case kBitSorterFilterClass: bsf_process((BitSorterFilter*)me, inputBuffer, outputBuffer); break;
        case kSignFilterClass: sf_process((SignFilter*)me, inputBuffer, outputBuffer); break;
        case kLorenzoFilterClass: lf_process((LorenzoFilter*)me, inputBuffer, outputBuffer); break;
        case kFilterCascadeFilterClass: fcf_process((FilterCascadeFilter*)me, inputBuffer, outputBuffer); break;
        default: assert(0);
    }
    me->slapCount++;
}

void vf_processReverse(VariableFilter* me, const void* inputBuffer, void* outputBuffer) {
//    printf("vf_process()\n");
    if(!me->size) return;    //make sure, its in the operational state
    switch(me->type) {
        case kAdaptiveFilterClass: af_processReverse((AdaptiveFilter*)me, inputBuffer, outputBuffer); break;
        case kCombinedFilterClass: cf_processReverse((CombinedFilter*)me, inputBuffer, outputBuffer); break;
        case kTimeFilterClass: tf_processReverse((TimeFilter*)me, inputBuffer, outputBuffer); break;
        case kDifferenceFilterClass: df_processReverse((DifferenceFilter*)me, inputBuffer, outputBuffer); break;
        case kBitSorterFilterClass: bsf_processReverse((BitSorterFilter*)me, inputBuffer, outputBuffer); break;
        case kSignFilterClass: sf_processReverse((SignFilter*)me, inputBuffer, outputBuffer); break;
        case kLorenzoFilterClass: lf_processReverse((LorenzoFilter*)me, inputBuffer, outputBuffer); break;
        case kFilterCascadeFilterClass: fcf_processReverse((FilterCascadeFilter*)me, inputBuffer, outputBuffer); break;
        default: assert(0);    //Can't reverse filter this file.
    }
    me->slapCount++;
}

size_t vf_getOutputSize(VariableFilter* me) {
//    printf("vf_getOutputSize()\n");
    if(!me->size) return 0;    //make sure, its in the operational state
    switch(me->type) {
        case kCombinedFilterClass: return cf_getOutputSize((CombinedFilter*)me);
        case kAdaptiveFilterClass:
        case kTimeFilterClass:
        case kDifferenceFilterClass:
        case kBitSorterFilterClass:
        case kSignFilterClass:
        case kLorenzoFilterClass:
        case kFilterCascadeFilterClass:    //Just added all the cases to make the compiler shut up.
        default: return me->slapCount * me->valueCount * me->valueSize;
    }
}

int vf_producesByteStream(VariableFilter* me) {
//    printf("vf_producesByteStream()\n");
    switch(me->type) {
        case kCombinedFilterClass: return cf_producesByteStream((CombinedFilter*)me);
        case kAdaptiveFilterClass: return af_producesByteStream((AdaptiveFilter*)me);
        case kTimeFilterClass: return tf_producesByteStream((TimeFilter*)me);
        case kDifferenceFilterClass: return df_producesByteStream((DifferenceFilter*)me);
        case kBitSorterFilterClass: return bsf_producesByteStream((BitSorterFilter*)me);
        case kSignFilterClass: return sf_producesByteStream((SignFilter*)me);
        case kLorenzoFilterClass: return lf_producesByteStream((LorenzoFilter*)me);
        case kFilterCascadeFilterClass: return fcf_producesByteStream((FilterCascadeFilter*)me);
        default: assert(0);
    }
}

void vf_destroy(VariableFilter* me) {
//    printf("vf_destroy()\n");
    if(me->size) free(me->size);
}

void vf_delete(VariableFilter* me) {
//    printf("vf_delete()\n");
    switch(me->type) {
        case kAdaptiveFilterClass: af_destroy((AdaptiveFilter*)me); break;
        case kCombinedFilterClass: cf_destroy((CombinedFilter*)me); break;
        case kTimeFilterClass: tf_destroy((TimeFilter*)me); break;
        case kDifferenceFilterClass: df_destroy((DifferenceFilter*)me); break;
        case kBitSorterFilterClass: bsf_destroy((BitSorterFilter*)me); break;
        case kSignFilterClass: sf_destroy((SignFilter*)me); break;
        case kLorenzoFilterClass: lf_destroy((LorenzoFilter*)me); break;
        case kFilterCascadeFilterClass: fcf_destroy((FilterCascadeFilter*)me); break;
        default: assert(0);
    }
    free(me);
}

#if defined(_WIN32) && defined(_MSC_VER)
int vasprintf(char **strp, const char *fmt, va_list ap)
{
  int r = -1, size = _vscprintf(fmt, ap);

  if ((size >= 0) && (size < INT_MAX))
  {
    *strp = (char *)malloc(size+1); //+1 for null
    if (*strp)
    {
      r = vsnprintf(*strp, size+1, fmt, ap);  //+1 for null
      if ((r < 0) || (r > size))
      {
        free(*strp); strp = 0;
        r = -1;
      }
    }
  }
  else { *strp = 0; }

  return(r);
}
int asprintf(char **strp, const char *fmt, ...)
{
  int r;
  va_list ap;
  va_start(ap, fmt);
  r = vasprintf(strp, fmt, ap);
  va_end(ap);
  return(r);
}
#endif
