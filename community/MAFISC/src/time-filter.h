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

#ifndef TIME_FILTER_INCLUDE_GUARD
#define TIME_FILTER_INCLUDE_GUARD

#include "variable-filter.h"

typedef struct {
	VariableFilter super;
	void* referenceBuffer;	//Initialized with zeros, then set to each processed slap.
} TimeFilter;

//public:
TimeFilter* tf_make(void);
TimeFilter* tf_makeWithString(const char** description);
char* tf_toString(TimeFilter* me);

void tf_process(TimeFilter* me, const void* inputBuffer, void* outputBuffer);
void tf_processReverse(TimeFilter* me, const void* inputBuffer, void* outputBuffer);

int tf_producesByteStream(TimeFilter* me);

//protected:
void tf_init(TimeFilter* me);
void tf_varPropertiesSet(TimeFilter* me);
void tf_destroy(TimeFilter* me);

#endif	//TIME_FILTER_INCLUDE_GUARD

