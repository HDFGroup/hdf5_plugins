/**
 *  @file szd_float_ts.h
 *  @author Sheng Di
 *  @date July, 2017
 *  @brief Header file for the szd_float_ts.c.
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _SZD_Float_TS_H
#define _SZD_Float_TS_H

#include "dataCompression.h"

#ifdef __cplusplus
extern "C" {
#endif

void decompressDataSeries_float_1D_ts(float** data, size_t dataSeriesLength, float* hist_data, TightDataPointStorageF* tdps);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _SZD_Float_TS_H  ----- */
