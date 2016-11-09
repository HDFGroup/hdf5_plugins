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

#ifndef DIFFERENCE_FILTER_INCLUDE_GUARD
#define DIFFERENCE_FILTER_INCLUDE_GUARD

#include "variable-filter.h"

typedef struct {
	VariableFilter super;
	int workingDim;
} DifferenceFilter;

//public:
DifferenceFilter* df_make(int workingDim);
DifferenceFilter* df_makeWithString(const char** description);
char* df_toString(DifferenceFilter* me);

void df_process(DifferenceFilter* me, const void* inputBuffer, void* outputBuffer);
void df_processReverse(DifferenceFilter* me, const void* inputBuffer, void* outputBuffer);

int df_producesByteStream(DifferenceFilter* me);

//protected:
void df_init(DifferenceFilter* me, int workingDim);	//Called by df_make().
void df_varPropertiesSet(DifferenceFilter* me);	//Called by vf_setVarProperties().
void df_destroy(DifferenceFilter* me);	//Called by vf_delete().

#endif //DIFFERENCE_FILTER_INCLUDE_GUARD

