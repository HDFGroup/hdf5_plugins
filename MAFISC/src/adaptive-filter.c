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

#include "config.h"

#define _GNU_SOURCE

#include <stdio.h>
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#ifdef HAVE_STRING_H
# if !defined STDC_HEADERS && defined HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <assert.h>
#include <stdarg.h>
#include <math.h>
#ifdef DKRZ_COMPRESSION_USE_GMP
    #include <gmp.h>
#endif
#include "adaptive-filter.h"

/*typedef struct {
    VariableFilter super;
    void* referenceBuffer1, *referenceBuffer2, *referenceBuffer3;    //Initialized with zeros, then set to each processed slap.
    size_t predictorCount;    //Size of predictions and weights array.
    int64_t* zeroPredictor, *constPredictors, *constSlapPredictor, *linearPredictors, *linearSlapPredictor, *quadraticPredictors, *quadraticSlapPredictor;    //pointers into the predictions array for easy access
    int64_t* predictions;
    int64_t* weights;    //The coefficients for the linear combination of the different predictions. 48.16 bits fx representation
    double* statistics;    //For each prediction the number of times it has been selected.
} AdaptiveFilter;*/

const int kWeightPPBs = 32;

AdaptiveFilter* af_makeWithString(const char** description) {
    if(!**description) return 0;    //Safety.
    switch(*((*description)++)) {
        case 's': return af_make(kBestPrediction, kBestRewardLearning, kInformedPredictions, 1);
        case 'l': return af_make(kBestPrediction, kLastBestLearning, kInformedPredictions, 0);
        case 't': return af_make(kBestPrediction, kLastBestLearning, kInformedPredictions, 1);
        default: assert(0);    //Illegal method letter.
    }
    return 0;
}

AdaptiveFilter* af_make(PredictionMode pMode, LearningMode lMode, InformationMode iMode, int useTimePredictions) {
//    printf("af_make()\n");
    AdaptiveFilter* me = (AdaptiveFilter*)malloc(sizeof(AdaptiveFilter));
    af_init(me, pMode, lMode, iMode, useTimePredictions);
    return me;
}

void af_init(AdaptiveFilter* me, PredictionMode pMode, LearningMode lMode, InformationMode iMode, int useTimePredictions) {
    vf_init(super(), kAdaptiveFilterClass);
    me->predictorCount = 0;
    me->referenceBuffer3 = me->referenceBuffer2 = me->referenceBuffer1 = me->predictions = me->weights = 0;
    me->statistics = 0;
    me->pMode = pMode;
    me->lMode = lMode;
    me->iMode = iMode;
    me->useTimePredictions = useTimePredictions;
}

void af_printStatistics(AdaptiveFilter* me);    //Shut up.
void af_printStatistics(AdaptiveFilter* me) {
    size_t i;
    for(i = 0; i < me->predictorCount; i++) {
        if(i) printf(" ");
        if(&me->predictions[i] == me->constPredictors) printf("| ");
        if(&me->predictions[i] == me->linearPredictors) printf("| ");
        if(&me->predictions[i] == me->quadraticPredictors) printf("| ");
        printf("%g", me->statistics[i]);
    }
    printf("\n");
}

char* af_toString(AdaptiveFilter* me) {
    char* result = 0;
    char second = 0;
    if(me->iMode == kInformedPredictions) {
        if(me->pMode == kBestPrediction) {
            if(me->lMode == kBestRewardLearning && me->useTimePredictions) second = 's';
            if(me->lMode == kLastBestLearning && !me->useTimePredictions) second = 'l';
            if(me->lMode == kLastBestLearning && me->useTimePredictions) second = 't';
        }
    }
    if(!second) return 0;
    if(asprintf(&result, "%c%c", kFilterClassIdLetters[super()->type], second) <= 0) return 0;
    return result;
}

void af_varPropertiesSet(AdaptiveFilter* me) {
//    printf("af_varPropertiesSet(t)\n");
    //Cleanup the last run (if there was one).
    if(me->referenceBuffer1) free(me->referenceBuffer1);
    if(me->referenceBuffer2) free(me->referenceBuffer2);
    if(me->referenceBuffer3) free(me->referenceBuffer3);
    if(me->predictions) free(me->predictions);
    if(me->weights) free(me->weights);
    if(me->statistics) {
        af_printStatistics(me);
        free(me->statistics);
    }
    //Some space to store the last slaps.
    me->referenceBuffer1 = calloc(super()->valueCount, super()->valueSize);
    me->referenceBuffer2 = calloc(super()->valueCount, super()->valueSize);
    me->referenceBuffer3 = calloc(super()->valueCount, super()->valueSize);
    //Setup the predictors.
    me->predictorCount = 3*(size_t)(super()->dimensions)+4;
    me->predictions = (int64_t*)calloc(me->predictorCount, sizeof(int64_t));
    {
        int64_t* nextPrediction = me->predictions;
        me->zeroPredictor = nextPrediction; nextPrediction += 1;
        me->constPredictors = nextPrediction; nextPrediction += super()->dimensions;
        me->constSlapPredictor = nextPrediction; nextPrediction += 1;
        me->linearPredictors = nextPrediction; nextPrediction += super()->dimensions;
        me->linearSlapPredictor = nextPrediction; nextPrediction += 1;
        me->quadraticPredictors = nextPrediction; nextPrediction += super()->dimensions;
        me->quadraticSlapPredictor = nextPrediction; nextPrediction += 1;
        assert(nextPrediction == me->predictions + me->predictorCount);
    }
    me->weights = (int64_t*)calloc(me->predictorCount, sizeof(int64_t));
    me->statistics = 0;    //(double*)calloc(me->predictorCount, sizeof(double));
    switch(me->lMode) {
#ifdef DKRZ_COMPRESSION_USE_GMP
        case kBackstepLearning: {    //The handling must allow for a zero vector of weights, so we can start with it as well.
        } break;
#endif
        case kBestRewardLearning:
        case kLastBestLearning: {
            me->weights[0] = 1ull << kWeightPPBs;    //so we don't get a division by zero
        } break;
        default: assert(0);
    }
}

#define template(T)\
void af_setPredictions_##T(AdaptiveFilter* me, const void* inputBuffer, int64_t curIndex);    /*Shut up.*/\
void af_setPredictions_##T(AdaptiveFilter* me, const void* inputBuffer, int64_t curIndex) {\
    int64_t i;\
    int64_t curOffset = 1;\
    switch(me->iMode) {\
        case kInformedPredictions: {\
            *(me->zeroPredictor) = 0;\
            for(i = super()->dimensions-1; i >= 0; i--) {\
                if(curIndex - 3*curOffset >= 0) goto doQuadratic;\
                if(curIndex - 2*curOffset >= 0) goto doLinear;\
                if(curIndex - curOffset >= 0) goto doConst;\
                goto doNothing;\
            doQuadratic:\
                me->quadraticPredictors[i] = 3*(((const T*)inputBuffer)[curIndex - curOffset] - ((const T*)inputBuffer)[curIndex - 2*curOffset]) + ((const T*)inputBuffer)[curIndex - 3*curOffset];\
            doLinear:\
                me->linearPredictors[i] = 2*((const T*)inputBuffer)[curIndex - curOffset] - ((const T*)inputBuffer)[curIndex - 2*curOffset];\
            doConst:\
                me->constPredictors[i] = ((const T*)inputBuffer)[curIndex - curOffset];\
            doNothing:\
                curOffset *= (int64_t)super()->size[i];\
            }\
            if(me->useTimePredictions) {\
                *(me->linearSlapPredictor) = 2*((T*)me->referenceBuffer1)[curIndex] - ((T*)me->referenceBuffer2)[curIndex];\
                *(me->quadraticSlapPredictor) = 3*(((T*)me->referenceBuffer1)[curIndex] - ((T*)me->referenceBuffer2)[curIndex]) + ((T*)me->referenceBuffer3)[curIndex];\
            } else {\
                *(me->constSlapPredictor) = *(me->linearSlapPredictor) = *(me->quadraticSlapPredictor) = 0;\
            }\
        } break; case kSimplePredictions: {\
            for(i = super()->dimensions-1; i >= 0; i--) {\
                if(curIndex - 3*curOffset >= 0) goto doThird;\
                if(curIndex - 2*curOffset >= 0) goto doSecond;\
                if(curIndex - curOffset >= 0) goto doFirst;\
                goto doNone;\
            doThird:\
                me->quadraticPredictors[i] = ((const T*)inputBuffer)[curIndex - 3*curOffset];\
            doSecond:\
                me->linearPredictors[i] = ((const T*)inputBuffer)[curIndex - 2*curOffset];\
            doFirst:\
                me->constPredictors[i] = ((const T*)inputBuffer)[curIndex - curOffset];\
            doNone:\
                curOffset *= (int64_t)super()->size[i];\
            }\
            if(me->useTimePredictions) {\
                *(me->constSlapPredictor) = ((T*)me->referenceBuffer1)[curIndex];\
                *(me->linearSlapPredictor) = ((T*)me->referenceBuffer2)[curIndex];\
                *(me->quadraticSlapPredictor) = ((T*)me->referenceBuffer3)[curIndex];\
            } else {\
                *(me->constSlapPredictor) = *(me->linearSlapPredictor) = *(me->quadraticSlapPredictor) = 0;\
            }\
        } break; default: assert(0);\
    }\
}
template(int8_t)
template(int16_t)
template(int32_t)
template(int64_t)
#undef template

void af_setPredictions(AdaptiveFilter* me, const void* inputBuffer, int64_t curIndex);    //Shut up.
void af_setPredictions(AdaptiveFilter* me, const void* inputBuffer, int64_t curIndex) {
    switch(super()->valueSize) {
        case 1: af_setPredictions_int8_t(me, inputBuffer, curIndex); break;
        case 2: af_setPredictions_int16_t(me, inputBuffer, curIndex); break;
        case 4: af_setPredictions_int32_t(me, inputBuffer, curIndex); break;
        case 8: af_setPredictions_int64_t(me, inputBuffer, curIndex); break;
        default: assert(0);
    }
}

int64_t af_predict(AdaptiveFilter* me, int64_t curValue);    //Shut up.
int64_t af_predict(AdaptiveFilter* me, int64_t curValue) {
    switch(me->pMode) {
#ifdef DKRZ_COMPRESSION_USE_GMP
        case kWeightedPrediction: {
            int64_t i;
            static mpz_t temp1, result;    //Leaf routine. Init only once.
            static int initialized = 0;
            if(!initialized) {
                mpz_init(temp1);
                mpz_init(result);
                initialized = 1;
            }
            for(i = me->predictorCount-1; i >= 0; i--) {
                mpz_set_si(temp1, me->weights[i]);
                mpz_mul_si(temp1, temp1, me->predictions[i]);
                mpz_add(result, result, temp1);
            }
            mpz_tdiv_q_2exp(result, result, kWeightPPBs);    //Compensate for the ppbs in the weights.
            int64_t returnValue = mpz_get_si(result);
            return returnValue;/*                mpz_set_si(curPrediction, ((T*)me->referenceBuffer1)[curIndex]);\
                mpz_set_si(temp, ((T*)me->referenceBuffer2)[curIndex]);\
                mpz_sub(temp, curPrediction, temp);\
                mpz_set_si(curPrediction, ((T*)me->referenceBuffer3)[curIndex]);\
                mpz_addmul_ui(curPrediction, temp, 3);\
                *(me->quadraticSlapPredictor) = mpz_get_si(curPrediction);*/\

        } break;
#endif //DKRZ_COMPRESSION_USE_GMP
        case kBestPrediction: {
            int64_t i, bestWeight = 0, bestIndex = 0;
            for(i = (int64_t)(me->predictorCount-1); i >= 0; i--) {
                if(me->weights[i] >= bestWeight) {
                    bestWeight = me->weights[i];
                    bestIndex = i;
                }
            }
            if(me->statistics) me->statistics[bestIndex]++;
            return me->predictions[bestIndex];
        } break; default: assert(0);
    }
    return curValue;    //Shut up. Never executed, but the compiler doesn't know...
}

void af_learn(AdaptiveFilter* me, int64_t curValue);    //Shut up.
void af_learn(AdaptiveFilter* me, int64_t curValue) {
    int64_t i;
    switch(me->lMode) {
#ifdef DKRZ_COMPRESSION_USE_GMP
        case kBackstepLearning: {
            //Since the prediction is just the scalar product of the weight and the prediction vectors, the gradient is simply the prediction vector multiplied by 2 times the error. Iff the backstep is -error*predictionVector/predictionVector^2, then the same prediction would give a perfect result.
            static mpz_t psquare, error, temp1;    //Leaf routine. Init only once.
            static int initialized = 0;
            if(!initialized) {
                mpz_init(psquare);
                mpz_init(error);
                mpz_init(temp1);
                initialized = 1;
            }
            //Calculate the magnitude of the prediction vector and the error made.
            mpz_set_ui(psquare, 0);
            mpz_set_ui(error, 0);
            for(i = me->predictorCount-1; i >= 0; i--) {
                mpz_set_si(temp1, me->predictions[i]);
                mpz_addmul(psquare, temp1, temp1);
                mpz_set_si(temp1, me->weights[i]);
                mpz_mul_si(temp1, temp1, me->predictions[i]);
                mpz_add(error, error, temp1);
            }
            if(!mpz_sgn(psquare)) return;    //Abort if all predictions were zero.
            mpz_tdiv_q_2exp(error, error, kWeightPPBs);    //Compensate for the ppbs in the weights.
            mpz_set_si(temp1, curValue);
            mpz_sub(error, error, temp1);
            //update the weights
            for(i = me->predictorCount-1; i >= 0; i--) {
                mpz_mul_si(temp1, error, -me->predictions[i]);
                mpz_mul_2exp(temp1, temp1, kWeightPPBs);    //add some ppbs
                mpz_tdiv_q(temp1, temp1, psquare);
                assert(mpz_fits_slong_p(temp1));
                me->weights[i] += mpz_get_si(temp1);
            }
        } break;
#endif //DKRZ_COMPRESSION_USE_GMP
        case kBestRewardLearning: {
            //calculate the errors
            int64_t bestError, bestIndex;
            int64_t *errors;
            errors = (int64_t *)malloc(me->predictorCount * sizeof(int64_t));
            for(i = (int64_t)(me->predictorCount-1); i >= 0; i--) {
                errors[i] = me->predictions[i] - curValue;
                if(errors[i] < 0) errors[i] *= -1;
            }
            //find the best predictor and reduce the weights (multiplication by 0x0.ffff to bound the weights to 16.kWeightPPBs bits)
            bestError = 0x7fffffffffffffff, bestIndex = 0;
            for(i = (int64_t)(me->predictorCount-1); i >= 0; i--) {
                if(errors[i] <= bestError) bestError = errors[bestIndex = i];    //The <= ensures that the fastest changing dimension or the value itself wins in a draw.
                me->weights[i] = (me->weights[i] * 0xffff) >> 16;
//                if(me->statistics) me->statistics[i] += errors[i];
            }
            //reward the best predictor
            me->weights[bestIndex] += ((int64_t)1) << kWeightPPBs;
        } break; case kLastBestLearning: {
            //calculate the errors
            int64_t bestError, bestIndex;
            int64_t *errors;
            errors = (int64_t *)malloc(me->predictorCount * sizeof(int64_t));
            for(i = (int64_t)(me->predictorCount-1); i >= 0; i--) {
                errors[i] = me->predictions[i] - curValue;
                if(errors[i] < 0) errors[i] *= -1;
            }
            //find the best predictor and reset the weights
            bestError = 0x7fffffffffffffff, bestIndex = 0;
            for(i = (int64_t)(me->predictorCount-1); i >= 0; i--) {
                if(errors[i] <= bestError) bestError = errors[bestIndex = i];    //The <= ensures that the fastest changing dimension or the value itself wins in a draw.
                me->weights[i] = 0;
//                if(me->statistics) me->statistics[i] += errors[i];
            }
            //reward the best predictor
            me->weights[bestIndex] = ((int64_t)1) << kWeightPPBs;
        } break;
        default: assert(0);
    }
}

#define template(T)\
void af_process_##T(AdaptiveFilter* me, const void* inputBuffer, void* outputBuffer);    /*Shut up.*/\
void af_process_##T(AdaptiveFilter* me, const void* inputBuffer, void* outputBuffer) {\
    size_t i;\
    for(i = 0; i < super()->valueCount; i++) {\
        af_setPredictions(me, inputBuffer, (int64_t)i);\
        ((T*)outputBuffer)[i] = (T)(((const T*)inputBuffer)[i] - (T)af_predict(me, (int64_t)(((const T*)inputBuffer)[i])));\
        af_learn(me, ((const T*)inputBuffer)[i]);\
    }\
}
template(int8_t)
template(int16_t)
template(int32_t)
template(int64_t)
#undef template

void af_process(AdaptiveFilter* me, const void* inputBuffer, void* outputBuffer) {
    void* temp;
    switch(super()->valueSize) {
        case 1: af_process_int8_t(me, inputBuffer, outputBuffer); break;
        case 2: af_process_int16_t(me, inputBuffer, outputBuffer); break;
        case 4: af_process_int32_t(me, inputBuffer, outputBuffer); break;
        case 8: af_process_int64_t(me, inputBuffer, outputBuffer); break;
        default: assert(0);
    }
    //Save the current slap for the next three iterations.
    memcpy(me->referenceBuffer3, inputBuffer, super()->valueCount*super()->valueSize);
    temp = me->referenceBuffer3;
    me->referenceBuffer3 = me->referenceBuffer2;
    me->referenceBuffer2 = me->referenceBuffer1;
    me->referenceBuffer1 = temp;
}

#define template(T)\
void af_processReverse_##T(AdaptiveFilter* me, const void* inputBuffer, void* outputBuffer);    /*Shut up.*/\
void af_processReverse_##T(AdaptiveFilter* me, const void* inputBuffer, void* outputBuffer) {\
    size_t i;\
    for(i = 0; i < super()->valueCount; i++) {\
        af_setPredictions(me, outputBuffer, (int64_t)i);\
        ((T*)outputBuffer)[i] = (T)(((const T*)inputBuffer)[i] + (T)af_predict(me, (int64_t)(((const T*)inputBuffer)[i])));\
        af_learn(me, ((const T*)outputBuffer)[i]);\
    }\
}
template(int8_t)
template(int16_t)
template(int32_t)
template(int64_t)
#undef template

void af_processReverse(AdaptiveFilter* me, const void* inputBuffer, void* outputBuffer) {
    void* temp;
    switch(super()->valueSize) {
        case 1: af_processReverse_int8_t(me, inputBuffer, outputBuffer); break;
        case 2: af_processReverse_int16_t(me, inputBuffer, outputBuffer); break;
        case 4: af_processReverse_int32_t(me, inputBuffer, outputBuffer); break;
        case 8: af_processReverse_int64_t(me, inputBuffer, outputBuffer); break;
        default: assert(0);
    }
    //Save the current slap for the next three iterations.
    memcpy(me->referenceBuffer3, outputBuffer, super()->valueCount*super()->valueSize);
    temp = me->referenceBuffer3;
    me->referenceBuffer3 = me->referenceBuffer2;
    me->referenceBuffer2 = me->referenceBuffer1;
    me->referenceBuffer1 = temp;
}

int af_producesByteStream(AdaptiveFilter* me) { return 0; }

void af_destroy(AdaptiveFilter* me) {
//    printf("af_destroy()\n");
    if(me->referenceBuffer1) free(me->referenceBuffer1);
    if(me->referenceBuffer2) free(me->referenceBuffer2);
    if(me->referenceBuffer3) free(me->referenceBuffer3);
    if(me->predictions) free(me->predictions);
    if(me->weights) free(me->weights);
    if(me->statistics) {
        af_printStatistics(me);
        free(me->statistics);
    }
    vf_destroy(super());
}

