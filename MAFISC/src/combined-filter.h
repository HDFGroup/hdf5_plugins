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

#ifndef COMBINED_FILTER_INCLUDE_GUARD
#define COMBINED_FILTER_INCLUDE_GUARD

#include "variable-filter.h"

typedef struct {
	VariableFilter super, *first, *second;
	void* tempBuffer;	//Used to pass the output of the first filter to the second filter
} CombinedFilter;

//public:
CombinedFilter* cf_make(VariableFilter* first, VariableFilter* second);	//Iff it succeeds, this takes ownership of the arguments so that they will be deleted when the combined filter is destroyed.
char* cf_toString(CombinedFilter* me);

void cf_process(CombinedFilter* me, const void* inputBuffer, void* outputBuffer);
void cf_processReverse(CombinedFilter* me, const void* inputBuffer, void* outputBuffer);

size_t cf_getOutputSize(CombinedFilter* me);	//This gives the amount of data actually produced by the second filter. This is for informational purposes only, the data is still written into the fixed sized output buffers supplied to vf_process. But this returns the size that would have been used if a variable size output format would have been used.
int cf_producesByteStream(CombinedFilter* me);

//protected:
void cf_init(CombinedFilter* me, VariableFilter* first, VariableFilter* second);
void cf_varPropertiesSet(CombinedFilter* me);
void cf_destroy(CombinedFilter* me);

#endif	//COMBINED_FILTER_INCLUDE_GUARD

