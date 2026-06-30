/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of the HDF5 BitRound filter plugin source. The full     *
 * copyright notice, including terms governing use, modification, and        *
 * redistribution, is contained in the file COPYING, which can be found at   *
 * the root of the source code distribution tree. If you do not have access  *
 * to this file, you may request a copy from help@hdfgroup.org.              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/************************************************************
 *
 * Self-checking example for the BitRound quantization filter.
 *
 * Writes a chunked float32 (then float64) dataset through the BitRound filter
 * paired with deflate, reads it back, and verifies:
 *   - every non-special value is within the BitRound relative-error bound
 *     2^-(NSB+1);
 *   - the user fill value, +/-0.0, and NaN are preserved bit-exact (the filter
 *     skips them);
 *   - at least one normal value was actually quantized (the filter ran).
 *
 * The filter is loaded at run time via HDF5's plugin mechanism, so
 * HDF5_PLUGIN_PATH must point at the built plugin. The verdict is the program
 * exit code (0 = PASS), so no golden-file comparison is needed.
 *
 ************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "hdf5.h"

#define FILENAME            "h5ex_d_bitround.h5"
#define DSET_F32            "DS_f32"
#define DSET_F64            "DS_f64"
#define N                   1024
#define CHUNK               256
#define H5Z_FILTER_BITROUND 32032

#define FILL_F32 (-999.0f)
#define FILL_F64 (-999.0)

/* Indices used for the special values sprinkled into the data. */
#define IDX_FILL  3
#define IDX_ZERO  7
#define IDX_NZERO 11
#define IDX_NAN   17

static herr_t
add_filters(hid_t dcpl, unsigned int nsb)
{
    /* BitRound takes a single user parameter: NSB. */
    if (H5Pset_filter(dcpl, H5Z_FILTER_BITROUND, H5Z_FLAG_MANDATORY, (size_t)1, &nsb) < 0)
        return -1;
    /* Pair with a lossless compressor, if available. BitRound alone does not
     * change the byte count; the zeroed low bits compress downstream. */
    if (H5Zfilter_avail(H5Z_FILTER_DEFLATE) > 0) {
        if (H5Pset_deflate(dcpl, 6) < 0)
            return -1;
    }
    return 0;
}

static int
check_f32(unsigned int nsb)
{
    hid_t    file = H5I_INVALID_HID, space = H5I_INVALID_HID, dcpl = H5I_INVALID_HID, dset = H5I_INVALID_HID;
    hsize_t  dims[1] = {N}, chunk[1] = {CHUNK};
    float    wdata[N], rdata[N];
    float    fill = FILL_F32;
    float    nan_val;
    uint32_t nan_bits      = 0x7FC00000u; /* quiet NaN */
    int      ret           = 1;
    double   bound         = ldexp(1.0, -(int)(nsb + 1)); /* 2^-(NSB+1) */
    int      any_quantized = 0;
    hsize_t  i;

    memcpy(&nan_val, &nan_bits, sizeof(nan_val));
    for (i = 0; i < N; i++)
        wdata[i] = (float)i * 0.001f - 0.5f;
    wdata[IDX_FILL]  = fill;
    wdata[IDX_ZERO]  = 0.0f;
    wdata[IDX_NZERO] = -0.0f;
    wdata[IDX_NAN]   = nan_val;

    if ((space = H5Screate_simple(1, dims, NULL)) < 0)
        goto done;
    if ((dcpl = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        goto done;
    if (H5Pset_chunk(dcpl, 1, chunk) < 0)
        goto done;
    if (H5Pset_fill_value(dcpl, H5T_NATIVE_FLOAT, &fill) < 0)
        goto done;
    if (add_filters(dcpl, nsb) < 0)
        goto done;
    if ((dset = H5Dcreate(file = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT), DSET_F32,
                          H5T_IEEE_F32LE, space, H5P_DEFAULT, dcpl, H5P_DEFAULT)) < 0)
        goto done;
    if (H5Dwrite(dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata) < 0)
        goto done;
    H5Dclose(dset);
    dset = H5I_INVALID_HID;

    if ((dset = H5Dopen(file, DSET_F32, H5P_DEFAULT)) < 0)
        goto done;
    if (H5Dread(dset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata) < 0)
        goto done;

    printf("[f32 NSB=%u] error bound 2^-(NSB+1) = %g; first 8 values (in -> out):\n", nsb, bound);
    for (i = 0; i < 8; i++)
        printf("    % .6f -> % .6f\n", wdata[i], rdata[i]);

    for (i = 0; i < N; i++) {
        if (i == IDX_FILL || i == IDX_ZERO || i == IDX_NZERO || i == IDX_NAN) {
            /* special values must be bit-exact */
            uint32_t wbits, rbits;
            memcpy(&wbits, &wdata[i], sizeof(wbits));
            memcpy(&rbits, &rdata[i], sizeof(rbits));
            if (wbits != rbits) {
                fprintf(stderr, "FAIL [f32 idx %lu]: special value changed 0x%08x -> 0x%08x\n",
                        (unsigned long)i, wbits, rbits);
                goto done;
            }
            continue;
        }
        if (wdata[i] != rdata[i])
            any_quantized = 1;
        if (wdata[i] != 0.0f) {
            double rel = fabs((double)rdata[i] - (double)wdata[i]) / fabs((double)wdata[i]);
            if (rel > bound) {
                fprintf(stderr, "FAIL [f32 idx %lu]: rel err %g > bound %g (in=%g out=%g)\n",
                        (unsigned long)i, rel, bound, wdata[i], rdata[i]);
                goto done;
            }
        }
    }
    if (!any_quantized) {
        fprintf(stderr, "FAIL [f32]: no normal value was quantized; filter did not run\n");
        goto done;
    }

    printf("[f32 NSB=%u] PASS: within error bound; fill/+-0/NaN preserved.\n\n", nsb);
    ret = 0;

done:
    if (dset != H5I_INVALID_HID)
        H5Dclose(dset);
    if (dcpl != H5I_INVALID_HID)
        H5Pclose(dcpl);
    if (space != H5I_INVALID_HID)
        H5Sclose(space);
    if (file != H5I_INVALID_HID)
        H5Fclose(file);
    return ret;
}

static int
check_f64(unsigned int nsb)
{
    hid_t    file = H5I_INVALID_HID, space = H5I_INVALID_HID, dcpl = H5I_INVALID_HID, dset = H5I_INVALID_HID;
    hsize_t  dims[1] = {N}, chunk[1] = {CHUNK};
    double   wdata[N], rdata[N];
    double   fill = FILL_F64;
    double   nan_val;
    uint64_t nan_bits      = 0x7FF8000000000000ULL; /* quiet NaN */
    int      ret           = 1;
    double   bound         = ldexp(1.0, -(int)(nsb + 1));
    int      any_quantized = 0;
    hsize_t  i;

    memcpy(&nan_val, &nan_bits, sizeof(nan_val));
    for (i = 0; i < N; i++)
        wdata[i] = (double)i * 0.001 - 0.5;
    wdata[IDX_FILL]  = fill;
    wdata[IDX_ZERO]  = 0.0;
    wdata[IDX_NZERO] = -0.0;
    wdata[IDX_NAN]   = nan_val;

    if ((space = H5Screate_simple(1, dims, NULL)) < 0)
        goto done;
    if ((dcpl = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        goto done;
    if (H5Pset_chunk(dcpl, 1, chunk) < 0)
        goto done;
    if (H5Pset_fill_value(dcpl, H5T_NATIVE_DOUBLE, &fill) < 0)
        goto done;
    if (add_filters(dcpl, nsb) < 0)
        goto done;
    /* reuse the file created by check_f32 if present, else create */
    if ((file = H5Fopen(FILENAME, H5F_ACC_RDWR, H5P_DEFAULT)) < 0)
        file = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (file < 0)
        goto done;
    if ((dset = H5Dcreate(file, DSET_F64, H5T_IEEE_F64LE, space, H5P_DEFAULT, dcpl, H5P_DEFAULT)) < 0)
        goto done;
    if (H5Dwrite(dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata) < 0)
        goto done;
    H5Dclose(dset);
    dset = H5I_INVALID_HID;

    if ((dset = H5Dopen(file, DSET_F64, H5P_DEFAULT)) < 0)
        goto done;
    if (H5Dread(dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata) < 0)
        goto done;

    printf("[f64 NSB=%u] error bound 2^-(NSB+1) = %g; first 8 values (in -> out):\n", nsb, bound);
    for (i = 0; i < 8; i++)
        printf("    % .9f -> % .9f\n", wdata[i], rdata[i]);

    for (i = 0; i < N; i++) {
        if (i == IDX_FILL || i == IDX_ZERO || i == IDX_NZERO || i == IDX_NAN) {
            uint64_t wbits, rbits;
            memcpy(&wbits, &wdata[i], sizeof(wbits));
            memcpy(&rbits, &rdata[i], sizeof(rbits));
            if (wbits != rbits) {
                fprintf(stderr, "FAIL [f64 idx %lu]: special value changed 0x%016llx -> 0x%016llx\n",
                        (unsigned long)i, (unsigned long long)wbits, (unsigned long long)rbits);
                goto done;
            }
            continue;
        }
        if (wdata[i] != rdata[i])
            any_quantized = 1;
        if (wdata[i] != 0.0) {
            double rel = fabs(rdata[i] - wdata[i]) / fabs(wdata[i]);
            if (rel > bound) {
                fprintf(stderr, "FAIL [f64 idx %lu]: rel err %g > bound %g (in=%g out=%g)\n",
                        (unsigned long)i, rel, bound, wdata[i], rdata[i]);
                goto done;
            }
        }
    }
    if (!any_quantized) {
        fprintf(stderr, "FAIL [f64]: no normal value was quantized; filter did not run\n");
        goto done;
    }

    printf("[f64 NSB=%u] PASS: within error bound; fill/+-0/NaN preserved.\n\n", nsb);
    ret = 0;

done:
    if (dset != H5I_INVALID_HID)
        H5Dclose(dset);
    if (dcpl != H5I_INVALID_HID)
        H5Pclose(dcpl);
    if (space != H5I_INVALID_HID)
        H5Sclose(space);
    if (file != H5I_INVALID_HID)
        H5Fclose(file);
    return ret;
}

int
main(void)
{
    int r32, r64;

    if (H5Zfilter_avail(H5Z_FILTER_BITROUND) <= 0)
        printf("Note: BitRound filter not yet registered; it will load on first use.\n");

    r32 = check_f32(7);
    r64 = check_f64(10);

    if (r32 == 0 && r64 == 0) {
        printf("PASS: BitRound round-trip (f32 NSB=7, f64 NSB=10)\n");
        return 0;
    }
    return 1;
}
