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
* AdaptiveFilter
*
* A filter that uses the last values seen in each direction (only backwards, not forward) to make a prediction for the next value. The value stored in the output buffer is the input value minus the prediction. There are several methods implemented to adapt the predictions to the data, the method to use is chosen by some constants in adaptive-filter.c.
*/

#ifndef ADAPTIVE_FILTER_INCLUDE_GUARD
#define ADAPTIVE_FILTER_INCLUDE_GUARD

#include "variable-filter.h"

typedef enum {
#ifdef DKRZ_COMPRESSION_USE_GMP
	kWeightedPrediction,	//Do a weighted average of the predictions. Needs GMP to be able to calculate the magnitude of the weights vector correctly. But since weighted predictions proved to be useless and since it adds another library dependency, I axed it.
#endif
	kBestPrediction	//Choose the prediction with the highest weight.
} PredictionMode;

typedef enum {
#ifdef DKRZ_COMPRESSION_USE_GMP
	kBackstepLearning,	//Do a gradient backstep to adapt to the given data. Went with kWeightedPrediction and has the same GMP dependency, so again, it's axed.
#endif
	kBestRewardLearning,	//Increase the weight of the best prediction and reduce all others by a factor to ensure that the weights are limited.
	kLastBestLearning	//Always use the prediction that was best on the last value seen. Simple and powerful.
} LearningMode;

typedef enum {
	kInformedPredictions,	//Use linear of quadratic extrapolations to predict the next value.
	kSimplePredictions	//Replace the linear and quadratic extrapolations with the values they depend upon. This was meant to be a basis for machine learning, the extrapolation was to be deduced by trial and error. Though it would have allowed for different kinds of extrapolations, including multidimensional ones, it's performance is inferior to the informed predictions.
} InformationMode;

//The AdaptiveFilter class definition.
typedef struct {
	VariableFilter super;
	void* referenceBuffer1, *referenceBuffer2, *referenceBuffer3;	//Initialized with zeros, then set to each processed slap.
	size_t predictorCount;	//Size of predictions and weights array.
	int64_t* zeroPredictor, *constPredictors, *constSlapPredictor, *linearPredictors, *linearSlapPredictor, *quadraticPredictors, *quadraticSlapPredictor;	//pointers into the predictions array for easy access
	int64_t* predictions;
	int64_t* weights;	//The coefficients for the linear combination of the different predictions.
	double* statistics;	//For each prediction the number of times it has been selected.
	PredictionMode pMode;
	LearningMode lMode;
	InformationMode iMode;
	int useTimePredictions;
} AdaptiveFilter;

//public:
AdaptiveFilter* af_make(PredictionMode pMode, LearningMode lMode, InformationMode iMode, int useTimePredictions);
AdaptiveFilter* af_makeWithString(const char** description);
char* af_toString(AdaptiveFilter* me);

void af_process(AdaptiveFilter* me, const void* inputBuffer, void* outputBuffer);
void af_processReverse(AdaptiveFilter* me, const void* inputBuffer, void* outputBuffer);

int af_producesByteStream(AdaptiveFilter* me);

//protected:
void af_init(AdaptiveFilter* me, PredictionMode pMode, LearningMode lMode, InformationMode iMode, int useTimePredictions);
void af_varPropertiesSet(AdaptiveFilter* me);
void af_destroy(AdaptiveFilter* me);

#endif	//ADAPTIVE_FILTER_INCLUDE_GUARD

