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
* VariableFilter
*
* Abstract superclass to all filter classes. This defines the interface for filterNcFile() to use.
* When a VariableFilter is instantiated, it is not fully initialized: It still lacks information about the data to process. This is due to the fact, that filterNcFile() needs to accept a single filter but has to apply it to different variables. So, the variable information has to be supplied via vf_setVarProperties() prior to calling vf_process().
*
* Polymorphism is coded by hand, i. e. overriding subclass functions are explicitely called by the generic one. So before a subclass becomes functional, the corresponding calls have to be added to variable-filter.c.
*
* All subclasses must implement:
* xxx_varPropertiesSet(SubClass* me)    //Called at the end of vf_setVarProperties() to inform the subclass.
* xxx_process(SubClass* me, const void* inputBuffer, void* outputBuffer)
* xxx_destroy(SubClass* me)    //The polymorphic destructor part.
*/

#ifndef VARIABLE_FILTER_INCLUDE_GUARD
#define VARIABLE_FILTER_INCLUDE_GUARD

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include <stddef.h>
//#include <netcdf.h>

// A handy macro for subclasses to access the members of their superclass. So, super() is always the upcast me pointer, while super itself refers to the structure member (that should be the first).
#define super() (&(me->super))

// An enum of all the different filter classes.
typedef enum {
    kAdaptiveFilterClass = 0,
    kCombinedFilterClass = 1,
    kTimeFilterClass = 2,
    kDifferenceFilterClass = 3,
    kBitSorterFilterClass = 4,
    kSignFilterClass = 5,
    kLorenzoFilterClass = 6,
    kFilterCascadeFilterClass = 7
} vf_type;

extern const char kFilterClassIdLetters[];    //There is a letter for each filter class. Classes may define their own parameter letters, but should use a fixed amount of these.

//The VariableFilter class definition.
typedef struct {
    vf_type type;
    int dimensions;
    size_t slapCount;
    size_t* size;
    size_t valueSize, valueCount;
} VariableFilter;

//public:
VariableFilter* vf_makeWithString(const char* description);    //Parses the description letter by letter constructing a chain of variableFilters to match the classes indicated by the letters.
char* vf_toString(VariableFilter* me);    ///Just returns a new empty string if this is NULL, so it's safe to pass NULL to it.

void vf_setVarProperties(VariableFilter* me, size_t dataTypeSize, int dimCount, size_t* dimSizes);    //dimCount & dimSizes define the block size that is subsequently passed to vf_process(). Resets the internal state to process a new variable.
void vf_process(VariableFilter* me, const void* inputBuffer, void* outputBuffer);    //Several subsequent calls to vf_process() are assumed to correspond to subsequent slices of a space of higher dimension. setVarProperties() must have been called.
void vf_processReverse(VariableFilter* me, const void* inputBuffer, void* outputBuffer);    //Several subsequent calls to vf_process() are assumed to correspond to subsequent slices of a space of higher dimension. setVarProperties() must have been called.
size_t vf_getOutputSize(VariableFilter* me);    //For filters that compress internally, this gives the amount of data actually produced. This is for informational purposes only, the data is still written into the fixed sized output buffers supplied to vf_process. But this returns the size that would have been used if a variable size output format would have been used.
int vf_producesByteStream(VariableFilter* me);    //Returns true, if the output format is an ordered stream of bytes instead of a value stream with native byte order. This is necessary to determine whether a byte order conversion needs to be applied. Can also be used for method integrity checks.
void vf_delete(VariableFilter* me);

//protected:
void vf_init(VariableFilter* me, vf_type aType);
void vf_destroy(VariableFilter* me);

#if defined(_WIN32) && defined(_MSC_VER)
int asprintf(char **strp, const char *fmt, ...);
#endif

#endif    //VARIABLE_FILTER_INCLUDE_GUARD

