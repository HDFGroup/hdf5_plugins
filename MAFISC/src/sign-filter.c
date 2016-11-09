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
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "sign-filter.h"

/*typedef struct {
	VariableFilter super;
	int rotation;
} SignFilter;*/

SignFilter* sf_makeWithString(const char** description) {
	if(!**description) return sf_make(0);	//No reading past the end of the string!
	return sf_make(*((*description)++) == 'r');
}

SignFilter* sf_make(int rotation) {
	SignFilter* me = (SignFilter*)malloc(sizeof(SignFilter));
	sf_init(me, rotation);
	return me;
}

void sf_init(SignFilter* me, int rotation) {
	vf_init(super(), kSignFilterClass);
	me->rotation = rotation;
}

char* sf_toString(SignFilter* me) {
	char* result = 0;
	if(asprintf(&result, "%c%c", kFilterClassIdLetters[super()->type], ((me->rotation) ? 'r' : 'n')) <= 0) return 0;
	return result;
}

//T must be a fixed size signed integer type
#define template(T)\
void sf_process_##T(SignFilter* me, const void* inputBuffer, void* outputBuffer);	/*Shut up.*/\
void sf_process_##T(SignFilter* me, const void* inputBuffer, void* outputBuffer) {\
	size_t valueCount = super()->valueCount, i;\
	if(me->rotation) {\
		for(i = 0; i < valueCount; i++) {\
			T curValue = ((const T*)inputBuffer)[i];\
			u##T mask = (u##T)(curValue >> (8*sizeof(T)-1));	/*create a mask of ones iff the sign bit is set*/\
			((T*)outputBuffer)[i] = (T)((u##T)(curValue << 1) ^ mask);	/*The shift clears the lsb, so that the xor will set the lsb to the sign bit, effectively rotating the value left by one bit.*/\
		}\
	} else {\
		for(i = 0; i < valueCount; i++) {\
			T curValue = ((const T*)inputBuffer)[i];\
			u##T mask = ((u##T)(curValue >> (8*sizeof(T)-1))) >> 1;	/*create a mask of ones for the non sign bits iff the sign bit is set*/\
			((T*)outputBuffer)[i] = (T)((u##T)curValue ^ mask);\
		}\
	}\
}
template(int8_t)
template(int16_t)
template(int32_t)
template(int64_t)
#undef template

void sf_process(SignFilter* me, const void* inputBuffer, void* outputBuffer) {
	switch(super()->valueSize) {
		case 1: sf_process_int8_t(me, inputBuffer, outputBuffer); break;
		case 2: sf_process_int16_t(me, inputBuffer, outputBuffer); break;
		case 4: sf_process_int32_t(me, inputBuffer, outputBuffer); break;
		case 8: sf_process_int64_t(me, inputBuffer, outputBuffer); break;
		default: assert(0);
	}
}

//T must be a fixed size signed integer type
#define template(T)\
void sf_processReverse_##T(SignFilter* me, const void* inputBuffer, void* outputBuffer);	/*Shut up*/\
void sf_processReverse_##T(SignFilter* me, const void* inputBuffer, void* outputBuffer) {\
	size_t valueCount = super()->valueCount, i;\
	if(me->rotation) {\
		for(i = 0; i < valueCount; i++) {\
			T curValue = ((const T*)inputBuffer)[i];\
			T mask = (T)(curValue << (8*sizeof(T) - 1));\
			mask >>= 8*sizeof(T) - 1;	/*create a mask of ones iff the sign bit is set*/\
			((T*)outputBuffer)[i] = (T)((((u##T)curValue) >> 1) ^ (u##T)mask);	/*The shift clears the msb, so that the xor will set the msb to the sign bit, effectively rotating the value right by one bit.*/\
		}\
	} else {\
		for(i = 0; i < valueCount; i++) {\
			T curValue = ((const T*)inputBuffer)[i];\
			u##T mask = ((u##T)(curValue >> (8*sizeof(T)-1))) >> 1;	/*create a mask of ones for the non sign bits iff the sign bit is set*/\
			((T*)outputBuffer)[i] = (T)(((u##T)curValue) ^ mask);\
		}\
	}\
}
template(int8_t)
template(int16_t)
template(int32_t)
template(int64_t)
#undef template

void sf_processReverse(SignFilter* me, const void* inputBuffer, void* outputBuffer) {
	switch(super()->valueSize) {
		case 1: sf_processReverse_int8_t(me, inputBuffer, outputBuffer); break;
		case 2: sf_processReverse_int16_t(me, inputBuffer, outputBuffer); break;
		case 4: sf_processReverse_int32_t(me, inputBuffer, outputBuffer); break;
		case 8: sf_processReverse_int64_t(me, inputBuffer, outputBuffer); break;
		default: assert(0);
	}
}

int sf_producesByteStream(SignFilter* me) { return 0; }

void sf_varPropertiesSet(SignFilter* me) {
	me = NULL;	//Shut up.
}

void sf_destroy(SignFilter* me) {
	vf_destroy(super());
}

