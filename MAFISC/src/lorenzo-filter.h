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

#ifndef LORENZO_FILTER_INCLUDE_GUARD
#define LORENZO_FILTER_INCLUDE_GUARD

#include "variable-filter.h"

typedef struct {
	VariableFilter super;
} LorenzoFilter;

//public:
LorenzoFilter* lf_make();
LorenzoFilter* lf_makeWithString(const char** description);
char* lf_toString(LorenzoFilter* me);

void lf_process(LorenzoFilter* me, const void* inputBuffer, void* outputBuffer);
void lf_processReverse(LorenzoFilter* me, const void* inputBuffer, void* outputBuffer);

int lf_producesByteStream(LorenzoFilter* me);

//protected:
void lf_init(LorenzoFilter* me);	//Called by df_make().
void lf_varPropertiesSet(LorenzoFilter* me);	//Called by vf_setVarProperties().
void lf_destroy(LorenzoFilter* me);	//Called by vf_delete().

#endif //DIFFERENCE_FILTER_INCLUDE_GUARD

