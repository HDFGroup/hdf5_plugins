/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of the HDF5 Granular BitRound filter plugin source.     *
 * The full copyright notice, including terms governing use, modification,   *
 * and redistribution, is contained in the file COPYING, which can be found  *
 * at the root of the source code distribution tree. If you do not have      *
 * access to this file, you may request a copy from help@hdfgroup.org.       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Round-trip test for special floating-point values through the Granular
 * BitRound filter. Verifies that NaN, +Inf, -Inf, -0.0, and +0.0 are passed
 * through the filter unchanged (bit-identical) on both the float and double
 * code paths, and that at least one normal value is quantized (proving the
 * filter actually ran). The filter is loaded dynamically via HDF5's plugin
 * mechanism (HDF5_PLUGIN_PATH); no h5dump comparison is involved so cross-
 * platform NaN/Inf printf formatting cannot affect the result.
 */

#include "hdf5.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define FILENAME              "h5filter_specials_bitround.h5"
#define DSET_F32              "f32"
#define DSET_F64              "f64"
#define N                     16
#define N_SPECIALS            5
#define H5Z_FILTER_GRANULARBR 32023

/* Build exact bit patterns via memcpy so the compiler cannot constant-fold
 * away -0.0f or normalize a quiet NaN to a different payload. */
static void
init_input_f32(float *buf, uint32_t *bits_out)
{
    const uint32_t specials[N_SPECIALS] = {
        0x7FC00000U, /* quiet NaN  */
        0x7F800000U, /* +Inf       */
        0xFF800000U, /* -Inf       */
        0x80000000U, /* -0.0       */
        0x00000000U, /* +0.0       */
    };
    for (int i = 0; i < N_SPECIALS; i++)
        memcpy(&buf[i], &specials[i], sizeof(float));
    for (int i = N_SPECIALS; i < N; i++)
        buf[i] = ((float)i - 3.7f) * 0.137f;
    memcpy(bits_out, buf, N * sizeof(uint32_t));
}

static void
init_input_f64(double *buf, uint64_t *bits_out)
{
    const uint64_t specials[N_SPECIALS] = {
        0x7FF8000000000000ULL, /* quiet NaN  */
        0x7FF0000000000000ULL, /* +Inf       */
        0xFFF0000000000000ULL, /* -Inf       */
        0x8000000000000000ULL, /* -0.0       */
        0x0000000000000000ULL, /* +0.0       */
    };
    for (int i = 0; i < N_SPECIALS; i++)
        memcpy(&buf[i], &specials[i], sizeof(double));
    for (int i = N_SPECIALS; i < N; i++)
        buf[i] = ((double)i - 3.7) * 0.137;
    memcpy(bits_out, buf, N * sizeof(uint64_t));
}

static int
roundtrip_f32(hid_t file_id)
{
    hid_t   space_id = H5I_INVALID_HID, dcpl_id = H5I_INVALID_HID, dset_id = H5I_INVALID_HID;
    int     ret = 1;
    hsize_t dims[1] = {N}, chunk[1] = {N};
    /* NSD=3, sizeof(data)=4, has_mss_val=0, mss_val_byt_1to4=0, mss_val_byt_5to8=0 */
    const unsigned int cd_values[5] = {3, 4, 0, 0, 0};
    float              input[N], output[N];
    uint32_t           input_bits[N], output_bits[N];
    int                any_quantized = 0;

    init_input_f32(input, input_bits);

    if ((space_id = H5Screate_simple(1, dims, NULL)) < 0)
        goto done;
    if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        goto done;
    if (H5Pset_chunk(dcpl_id, 1, chunk) < 0)
        goto done;
    if (H5Pset_filter(dcpl_id, H5Z_FILTER_GRANULARBR, H5Z_FLAG_MANDATORY, 5, cd_values) < 0)
        goto done;
    if ((dset_id = H5Dcreate(file_id, DSET_F32, H5T_IEEE_F32LE, space_id, H5P_DEFAULT, dcpl_id,
                             H5P_DEFAULT)) < 0)
        goto done;
    if (H5Dwrite(dset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, input) < 0)
        goto done;
    H5Dclose(dset_id);
    dset_id = H5I_INVALID_HID;

    if ((dset_id = H5Dopen(file_id, DSET_F32, H5P_DEFAULT)) < 0)
        goto done;
    if (H5Dread(dset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, output) < 0)
        goto done;
    memcpy(output_bits, output, sizeof(output_bits));

    for (int i = 0; i < N_SPECIALS; i++) {
        if (input_bits[i] != output_bits[i]) {
            fprintf(stderr, "FAIL [f32, idx %d]: special value mutated: in=0x%08x out=0x%08x\n", i,
                    input_bits[i], output_bits[i]);
            goto done;
        }
    }
    for (int i = N_SPECIALS; i < N; i++) {
        if (input_bits[i] != output_bits[i]) {
            any_quantized = 1;
            break;
        }
    }
    if (!any_quantized) {
        fprintf(stderr, "FAIL [f32]: no normal value was quantized; filter did not run\n");
        goto done;
    }

    ret = 0;

done:
    if (dset_id >= 0)
        H5Dclose(dset_id);
    if (dcpl_id >= 0)
        H5Pclose(dcpl_id);
    if (space_id >= 0)
        H5Sclose(space_id);
    return ret;
}

static int
roundtrip_f64(hid_t file_id)
{
    hid_t   space_id = H5I_INVALID_HID, dcpl_id = H5I_INVALID_HID, dset_id = H5I_INVALID_HID;
    int     ret = 1;
    hsize_t dims[1] = {N}, chunk[1] = {N};
    /* NSD=3, sizeof(data)=8, has_mss_val=0, mss_val_byt_1to4=0, mss_val_byt_5to8=0 */
    const unsigned int cd_values[5] = {3, 8, 0, 0, 0};
    double             input[N], output[N];
    uint64_t           input_bits[N], output_bits[N];
    int                any_quantized = 0;

    init_input_f64(input, input_bits);

    if ((space_id = H5Screate_simple(1, dims, NULL)) < 0)
        goto done;
    if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        goto done;
    if (H5Pset_chunk(dcpl_id, 1, chunk) < 0)
        goto done;
    if (H5Pset_filter(dcpl_id, H5Z_FILTER_GRANULARBR, H5Z_FLAG_MANDATORY, 5, cd_values) < 0)
        goto done;
    if ((dset_id = H5Dcreate(file_id, DSET_F64, H5T_IEEE_F64LE, space_id, H5P_DEFAULT, dcpl_id,
                             H5P_DEFAULT)) < 0)
        goto done;
    if (H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, input) < 0)
        goto done;
    H5Dclose(dset_id);
    dset_id = H5I_INVALID_HID;

    if ((dset_id = H5Dopen(file_id, DSET_F64, H5P_DEFAULT)) < 0)
        goto done;
    if (H5Dread(dset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, output) < 0)
        goto done;
    memcpy(output_bits, output, sizeof(output_bits));

    for (int i = 0; i < N_SPECIALS; i++) {
        if (input_bits[i] != output_bits[i]) {
            fprintf(stderr, "FAIL [f64, idx %d]: special value mutated: in=0x%016llx out=0x%016llx\n", i,
                    (unsigned long long)input_bits[i], (unsigned long long)output_bits[i]);
            goto done;
        }
    }
    for (int i = N_SPECIALS; i < N; i++) {
        if (input_bits[i] != output_bits[i]) {
            any_quantized = 1;
            break;
        }
    }
    if (!any_quantized) {
        fprintf(stderr, "FAIL [f64]: no normal value was quantized; filter did not run\n");
        goto done;
    }

    ret = 0;

done:
    if (dset_id >= 0)
        H5Dclose(dset_id);
    if (dcpl_id >= 0)
        H5Pclose(dcpl_id);
    if (space_id >= 0)
        H5Sclose(space_id);
    return ret;
}

int
main(void)
{
    hid_t file_id;
    int   r_f32 = 1, r_f64 = 1;

    file_id = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (file_id < 0) {
        fprintf(stderr, "FAIL: H5Fcreate(%s) failed\n", FILENAME);
        return 1;
    }

    r_f32 = roundtrip_f32(file_id);
    r_f64 = roundtrip_f64(file_id);

    H5Fclose(file_id);

    if (r_f32 == 0 && r_f64 == 0) {
        printf("PASS: BitRound special-value round-trip (f32 + f64)\n");
        return 0;
    }
    return 1;
}
