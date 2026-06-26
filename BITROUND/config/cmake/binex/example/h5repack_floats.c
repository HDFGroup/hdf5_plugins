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

/*
 * Generates an HDF5 file with a chunked dataset of IEEE 32-bit floats and no
 * filters applied. Used as input for the h5repack BitRound test so the
 * filter's set_local() callback retains the filter in the pipeline (the
 * callback removes itself on non-float data).
 */

#include <stdio.h>
#include "hdf5.h"

#define FILENAME "h5repack_floats.h5"
#define DATASET  "data"
#define DIM0     32
#define DIM1     64
#define CHUNK0   4
#define CHUNK1   8

int
main(void)
{
    hid_t   file_id  = H5I_INVALID_HID;
    hid_t   space_id = H5I_INVALID_HID;
    hid_t   dcpl_id  = H5I_INVALID_HID;
    hid_t   dset_id  = H5I_INVALID_HID;
    hsize_t dims[2]  = {DIM0, DIM1};
    hsize_t chunk[2] = {CHUNK0, CHUNK1};
    float   wdata[DIM0][DIM1];
    int     i, j;
    int     ret_value = 1;

    /* Non-integer floats: the 0.137f multiplier ensures every value carries
     * non-trivial mantissa bits, so quantization by BitRound is visible
     * across the entire dataset rather than only on values that exceed the
     * retained mantissa width. */
    for (i = 0; i < DIM0; i++)
        for (j = 0; j < DIM1; j++)
            wdata[i][j] = ((float)(i * j) - (float)j) * 0.137f;

    file_id = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (file_id < 0)
        goto done;

    space_id = H5Screate_simple(2, dims, NULL);
    if (space_id < 0)
        goto done;

    dcpl_id = H5Pcreate(H5P_DATASET_CREATE);
    if (dcpl_id < 0)
        goto done;
    if (H5Pset_chunk(dcpl_id, 2, chunk) < 0)
        goto done;

    dset_id = H5Dcreate(file_id, DATASET, H5T_IEEE_F32LE, space_id, H5P_DEFAULT, dcpl_id, H5P_DEFAULT);
    if (dset_id < 0)
        goto done;

    if (H5Dwrite(dset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void *)wdata) < 0)
        goto done;

    ret_value = 0;

done:
    if (dset_id >= 0)
        H5Dclose(dset_id);
    if (dcpl_id >= 0)
        H5Pclose(dcpl_id);
    if (space_id >= 0)
        H5Sclose(space_id);
    if (file_id >= 0)
        H5Fclose(file_id);

    if (ret_value != 0)
        fprintf(stderr, "h5repack_floats: failed to create %s\n", FILENAME);

    return ret_value;
}
