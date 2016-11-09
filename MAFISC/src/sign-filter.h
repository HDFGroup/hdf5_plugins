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

#ifndef SIGN_FILTER_INCLUDE_GUARD
#define SIGN_FILTER_INCLUDE_GUARD

#include "variable-filter.h"

typedef struct {
	VariableFilter super;
	int rotation;
} SignFilter;

//public:
SignFilter* sf_make(int rotation);	//Also rotate the sign bit to the lsb iff rotation != 0, otherwise do only the controlled inversion of all the other bits.
SignFilter* sf_makeWithString(const char** description);	//Reads one character. Iff it is an 'r', then the filter is rotating.
char* sf_toString(SignFilter* me);

void sf_process(SignFilter* me, const void* inputBuffer, void* outputBuffer);
void sf_processReverse(SignFilter* me, const void* inputBuffer, void* outputBuffer);

int sf_producesByteStream(SignFilter* me);

//protected:
void sf_init(SignFilter* me, int rotation);
void sf_varPropertiesSet(SignFilter* me);	//Called by vf_setVarProperties().
void sf_destroy(SignFilter* me);	//Called by vf_delete().

#endif //SIGN_FILTER_INCLUDE_GUARD

