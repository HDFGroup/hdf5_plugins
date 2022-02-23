/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of the HDF5 SZ filter plugin source.  The full         *
 * copyright notice, including terms governing use, modification, and        *
 * terms governing use, modification, and redistribution, is contained in    *
 * the file COPYING, which can be found at the root of the SZ source code   *
 * distribution tree.  If you do not have access to this file, you may       *
 * request a copy from help@hdfgroup.org.                                    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/************************************************************

  This example shows how to write data and read it from a dataset
  using SZ compression.
  SZ filter is not available in HDF5.
  The example uses a new feature available in HDF5 version 1.8.11
  to discover, load and register filters at run time.

 ************************************************************/

#include "hdf5.h"
#include <stdio.h>
#include <stdlib.h>

#define FILE            "h5ex_d_sz.h5"
#define DATASET         "DS1"
#define DIM_SIZE        3
#define DIM0            8
#define DIM1            8
#define DIM2            128
#define CHUNK0          8
#define CHUNK1          8
#define CHUNK2          128
#define H5Z_FILTER_SZ        32017

int
main (void)
{
    hid_t           file_id = -1;    /* Handles */
    hid_t           space_id = -1;    /* Handles */
    hid_t           dset_id = -1;    /* Handles */
    hid_t           dcpl_id = -1;    /* Handles */
    herr_t          status;
    htri_t          avail;
    H5Z_filter_t    filter_id = 0;
    char            filter_name[80];
    hsize_t         dims[DIM_SIZE] = {DIM0, DIM1, DIM2},
                    chunk[DIM_SIZE] = {CHUNK0, CHUNK1, CHUNK2};
    size_t          nelmts = 5;                /* number of elements in cd_values */
    size_t          nbEle = DIM0*DIM1*DIM2;
    int             dataType = 0; /* FLOAT */
    unsigned int    flags;
    unsigned        filter_config;
    const unsigned int    cd_values[10] = {DIM_SIZE, dataType, DIM2, DIM1, DIM0, 0, 0, 0, 0, 0};
    unsigned int    values_out[10] = {99, 99, 99, 99, 99, 99, 99, 99, 99, 99};
    float           wdata[DIM0][DIM1][DIM2],          /* Write buffer */
                    rdata[DIM0][DIM1][DIM2],          /* Read buffer */
                    max;
    hsize_t         i, j, k;
    int             ret_value = 1;

    /*
     * Initialize data.
     */
    for (i=0; i<DIM0; i++)
        for (j=0; j<DIM1; j++)
            for (k=0; k<DIM2; k++)
                wdata[i][j][k]= (float)(j * k) - (float)(i);

    /*
     * Create a new file using the default properties.
     */
    file_id = H5Fcreate (FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (file_id < 0) goto done;

    /*
     * Create dataspace.  Setting maximum size to NULL sets the maximum
     * size to be the current size.
     */
    space_id = H5Screate_simple (DIM_SIZE, dims, NULL);
    if (space_id < 0) goto done;

    /*
     * Create the dataset creation property list, add the gzip
     * compression filter and set the chunk size.
     */
    dcpl_id = H5Pcreate (H5P_DATASET_CREATE);
    if (dcpl_id < 0) goto done;

    status = H5Pset_filter (dcpl_id, H5Z_FILTER_SZ, H5Z_FLAG_MANDATORY, nelmts, cd_values);
    if (status < 0) goto done;

    /*
     * Check that filter is registered with the library now.
     * If it is registered, retrieve filter's configuration.
     */
    avail = H5Zfilter_avail(H5Z_FILTER_SZ);
    if (avail) {
        status = H5Zget_filter_info (H5Z_FILTER_SZ, &filter_config);
        if ( (filter_config & H5Z_FILTER_CONFIG_ENCODE_ENABLED) &&
                (filter_config & H5Z_FILTER_CONFIG_DECODE_ENABLED) )
            printf ("SZ filter is available for encoding and decoding.\n");
    }
    else {
        printf ("H5Zfilter_avail - not found.\n");
        goto done;
    }
    status = H5Pset_chunk (dcpl_id, DIM_SIZE, chunk);
    if (status < 0) printf ("failed to set chunk.\n");

    /*
     * Create the dataset.
     */
    printf ("....Create dataset ................\n");
    dset_id = H5Dcreate (file_id, DATASET, H5T_IEEE_F32LE, space_id, H5P_DEFAULT, dcpl_id, H5P_DEFAULT);
    if (dset_id < 0) {
        printf ("failed to create dataset.\n");
        goto done;
    }

    /*
     * Write the data to the dataset.
     */
    printf ("....Writing SZ compressed data ................\n");
    status = H5Dwrite (dset_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata[0]);
    if (status < 0) printf ("failed to write data.\n");

    /*
     * Close and release resources.
     */
    H5Dclose (dset_id);
    dset_id = -1;
    H5Pclose (dcpl_id);
    dcpl_id = -1;
    H5Sclose (space_id);
    space_id = -1;
    H5Fclose (file_id);
    file_id = -1;
    status = H5close();
    if (status < 0) {
        printf ("/nFAILED to close library/n");
        goto done;
    }


    printf ("....Close the file and reopen for reading ........\n");
    /*
     * Now we begin the read section of this example.
     */

    /*
     * Open file and dataset using the default properties.
     */
    file_id = H5Fopen (FILE, H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file_id < 0) goto done;

    dset_id = H5Dopen (file_id, DATASET, H5P_DEFAULT);
    if (dset_id < 0) goto done;

    /*
     * Retrieve dataset creation property list.
     */
    dcpl_id = H5Dget_create_plist (dset_id);
    if (dcpl_id < 0) goto done;

    /*
     * Retrieve and print the filter id, compression level and filter's name for SZ.
     */
    filter_id = H5Pget_filter2 (dcpl_id, (unsigned) 0, &flags, &nelmts, values_out, sizeof(filter_name), filter_name, NULL);
    printf ("Filter info is available from the dataset creation property\n");
    printf ("   Filter identifier is ");
    switch (filter_id) {
        case H5Z_FILTER_SZ:
            printf ("%d\n", filter_id);
            printf ("   Number of parameters is %d with the value %u\n", nelmts, values_out[0]);
            printf ("   To find more about the filter check %s\n", filter_name);
            break;
        default:
            printf ("Not expected filter\n");
            break;
    }

    /*
     * Read the data using the default properties.
     */
    printf ("....Reading SZ compressed data ................\n");
    status = H5Dread (dset_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata[0]);
    if (status < 0) printf ("failed to read data.\n");

    /*
     * Find the maximum value in the dataset, to verify that it was
     * read correctly.
     */
    max = rdata[0][0][0];
    for (i=0; i<DIM0; i++)
        for (j=0; j<DIM1; j++)
            for (k=0; k<DIM2; k++) {
                /*printf("%f \n", rdata[i][j][k]); */
                if (max < rdata[i][j][k])
                    max = rdata[i][j][k];
            }
    /*
     * Print the maximum value.
     */
    printf ("Maximum value in %s is %6.4f\n", DATASET, max);
    /*
     * Check that filter is registered with the library now.
     */
    avail = H5Zfilter_avail(H5Z_FILTER_SZ);
    if (avail)
        printf ("SZ filter is available now since H5Dread triggered loading of the filter.\n");

    ret_value = 0;

done:
    /*
     * Close and release resources.
     */
    if (dcpl_id >= 0) H5Pclose (dcpl_id);
    if (dset_id >= 0) H5Dclose (dset_id);
    if (space_id >= 0) H5Sclose (space_id);
    if (file_id >= 0) H5Fclose (file_id);

    return ret_value;
}
