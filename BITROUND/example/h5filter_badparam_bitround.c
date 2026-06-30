/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of the HDF5 BitRound filter plugin source.              *
 * The full copyright notice, including terms governing use, modification,   *
 * and redistribution, is contained in the file COPYING, which can be found  *
 * at the root of the source code distribution tree. If you do not have      *
 * access to this file, you may request a copy from help@hdfgroup.org.       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Negative test for the BitRound filter's parameter validation.
 *
 * BitRound's set_local() callback rejects a missing or out-of-range NSB by
 * returning a negative value, which makes H5Dcreate fail at dataset-creation
 * time. This test verifies that end-to-end:
 *   1. a valid NSB (7) creates the dataset successfully -- this also proves the
 *      plugin is actually loaded, so the rejection steps below cannot pass for
 *      the wrong reason (e.g. a missing plugin);
 *   2. NSB = 0 (out of range) is rejected, so H5Dcreate fails;
 *   3. no NSB at all (a filter added with zero parameters) is rejected too.
 *
 * The filter is loaded dynamically via HDF5_PLUGIN_PATH. The expected errors in
 * steps 2-3 are part of the test, so automatic error-stack printing is disabled.
 * The verdict is the exit code (0 = PASS); no golden-file comparison.
 */

#include <stdio.h>
#include "hdf5.h"

#define FILENAME            "h5filter_badparam_bitround.h5"
#define N                   64
#define CHUNK               16
#define H5Z_FILTER_BITROUND 32032

/* Attempt to create a chunked float32 dataset with the BitRound filter
 * configured from `nelmts` cd_values (cd may be NULL when nelmts == 0).
 * Returns 1 if H5Dcreate succeeded, 0 if it failed. Cleans up either way. */
static int
try_create_f32(hid_t file, const char *name, size_t nelmts, const unsigned int *cd)
{
    hid_t   space = H5I_INVALID_HID, dcpl = H5I_INVALID_HID, dset = H5I_INVALID_HID;
    hsize_t dims[1] = {N}, chunk[1] = {CHUNK};
    int     created = 0;

    if ((space = H5Screate_simple(1, dims, NULL)) < 0)
        goto done;
    if ((dcpl = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        goto done;
    if (H5Pset_chunk(dcpl, 1, chunk) < 0)
        goto done;
    /* H5Pset_filter only stores the parameters; set_local() validates them
     * (presence and range of NSB) later, during H5Dcreate. */
    if (H5Pset_filter(dcpl, H5Z_FILTER_BITROUND, H5Z_FLAG_MANDATORY, nelmts, cd) < 0)
        goto done;

    dset    = H5Dcreate(file, name, H5T_IEEE_F32LE, space, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    created = (dset >= 0);

done:
    if (dset >= 0)
        H5Dclose(dset);
    if (dcpl != H5I_INVALID_HID)
        H5Pclose(dcpl);
    if (space != H5I_INVALID_HID)
        H5Sclose(space);
    return created;
}

int
main(void)
{
    hid_t        file;
    unsigned int nsb_ok = 7, nsb_zero = 0;
    int          valid_ok, zero_ok, missing_ok;

    /* The rejection cases below are expected to fail; keep the error stack quiet. */
    H5Eset_auto2(H5E_DEFAULT, NULL, NULL);

    file = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (file < 0) {
        fprintf(stderr, "FAIL: H5Fcreate(%s) failed\n", FILENAME);
        return 1;
    }

    /* 1. valid NSB must succeed (also proves the plugin is loaded). */
    valid_ok = try_create_f32(file, "valid", (size_t)1, &nsb_ok);
    /* 2. NSB = 0 is out of range and must be rejected. */
    zero_ok = try_create_f32(file, "zero", (size_t)1, &nsb_zero);
    /* 3. No NSB at all (nelmts = 0) must be rejected -- NSB is mandatory. */
    missing_ok = try_create_f32(file, "missing", (size_t)0, NULL);

    H5Fclose(file);

    if (!valid_ok) {
        fprintf(stderr, "FAIL: valid NSB=7 dataset was not created (plugin missing or broken?)\n");
        return 1;
    }
    if (zero_ok) {
        fprintf(stderr, "FAIL: NSB=0 dataset was created; set_local() should have rejected it\n");
        return 1;
    }
    if (missing_ok) {
        fprintf(stderr, "FAIL: dataset with no NSB was created; set_local() should have rejected it\n");
        return 1;
    }

    printf("PASS: BitRound requires NSB -- NSB=7 succeeds; NSB=0 and missing-NSB rejected\n");
    return 0;
}
