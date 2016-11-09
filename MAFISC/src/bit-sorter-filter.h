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

/**
* BitSorterFilter
*
* This filter sorts the bits by their position in the values into the record, i. e. all sign bits form a single consecutive block that is zero iff no negative values are present in the data. Since there are frequently bits that are always set in a value or not set, this leads to large uniform block that can be compressed well, at the expense of making other parts look quite random.
*/

#ifndef BIT_SORTER_FILTER_INCLUDE_GUARD
#define BIT_SORTER_FILTER_INCLUDE_GUARD

#include "variable-filter.h"

//The type of the sorting that is done.
//kGlobalSort sorts all the first bits of the values into one big block.
//kByteSort distributes the n most significant bits of a value to the most significant bits of the n bytes of the value and so on. This is done to distribute the entropy evenly across the bytes.
//kBitSort sorts the bits inside the values by their frequency inside the record.
typedef enum {kGlobalSort, kByteSort} SortingMode;

typedef struct {
	VariableFilter super;
	SortingMode mode;
} BitSorterFilter;

//public:
BitSorterFilter* bsf_make(SortingMode mode);
BitSorterFilter* bsf_makeWithString(const char** description);	//Reads one more character that must be 'g' for global mode or 'b' for byte mode.
char* bsf_toString(BitSorterFilter* me);

void bsf_process(BitSorterFilter* me, const void* inputBuffer, void* outputBuffer);
void bsf_processReverse(BitSorterFilter* me, const void* inputBuffer, void* outputBuffer);

int bsf_producesByteStream(BitSorterFilter* me);

//protected:
void bsf_init(BitSorterFilter* me, SortingMode mode);	//Called by lsf_make().
void bsf_varPropertiesSet(BitSorterFilter* me);	//Called by vf_setVarProperties().
void bsf_destroy(BitSorterFilter* me);	//Called by vf_delete().

#endif //BIT_SORTER_FILTER_INCLUDE_GUARD

