/************************************************************

  This example shows how to write data and read it from a dataset
  using blosc compression. 
  blosc filter is not available in HDF5. 
  The example uses a new feature available in HDF5 version 1.8.11 
  to discover, load and register filters at run time.  

 ************************************************************/

#include "hdf5.h"
#include <stdio.h>
#include <stdlib.h>

#define FILE            "h5ex_d_blosc.h5"
#define DATASET         "DS1"
#define DIM0            32
#define DIM1            64
#define CHUNK0          4
#define CHUNK1          8
#define H5Z_FILTER_BLOSC        32001

int
main (void)
{
    hid_t           file, space, dset, dcpl;    /* Handles */
    herr_t          status;
    htri_t          avail;
    H5Z_filter_t    filter_id = 0;
    char            filter_name[80];
    hsize_t         dims[2] = {DIM0, DIM1},
                    chunk[2] = {CHUNK0, CHUNK1};
    size_t          nelmts = 6;                /* number of elements in cd_values */
    unsigned int    flags;
    unsigned        filter_config;
    unsigned int cd_values[6];     /* blosc iparameters */
    unsigned int       values_out[6] = {99, 99, 99, 99, 99, 99};          
    int             wdata[DIM0][DIM1],          /* Write buffer */
                    rdata[DIM0][DIM1],          /* Read buffer */
                    max;
    hsize_t         i, j;

    /*
     * Initialize data.
     */
    for (i=0; i<DIM0; i++)
        for (j=0; j<DIM1; j++)
            wdata[i][j] = i * j - j;

    /*
     * Create a new file using the default properties.
     */
    file = H5Fcreate (FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Create dataspace.  Setting maximum size to NULL sets the maximum
     * size to be the current size.
     */
    space = H5Screate_simple (2, dims, NULL);

    /*
     * Create the dataset creation property list, add the gzip
     * compression filter and set the chunk size.
     */
    dcpl = H5Pcreate (H5P_DATASET_CREATE);
    cd_values[4] = 5;  
    cd_values[5] = 1;  
    status = H5Pset_filter (dcpl, H5Z_FILTER_BLOSC, H5Z_FLAG_OPTIONAL, (size_t)6, cd_values);

    /* 
     * Check that filter is registered with the library now.
     * If it is registered, retrieve filter's configuration. 
     */
    avail = H5Zfilter_avail(H5Z_FILTER_BLOSC);
    if (avail) {
        status = H5Zget_filter_info (H5Z_FILTER_BLOSC, &filter_config);
        if ( (filter_config & H5Z_FILTER_CONFIG_ENCODE_ENABLED) && 
                (filter_config & H5Z_FILTER_CONFIG_DECODE_ENABLED) ) 
            printf ("blosc filter is available for encoding and decoding.\n");
    }     
    status = H5Pset_chunk (dcpl, 2, chunk);

    /*
     * Create the dataset.
     */
    printf ("....Writing blosc compressed data ................\n");
    dset = H5Dcreate (file, DATASET, H5T_STD_I32LE, space, H5P_DEFAULT, dcpl,
                H5P_DEFAULT);
    printf("dset id %d\n", (int)dset);

    /*
     * Write the data to the dataset.
     */
    status = H5Dwrite (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                wdata[0]);

    /*
     * Close and release resources.
     */
    status = H5Pclose (dcpl);
    status = H5Dclose (dset);
    status = H5Sclose (space);
    status = H5Fclose (file);
    status = H5close();


    printf ("....Close the file and reopen for reading ........\n");
    /*
     * Now we begin the read section of this example.
     */

    /*
     * Open file and dataset using the default properties.
     */
    file = H5Fopen (FILE, H5F_ACC_RDONLY, H5P_DEFAULT);
    dset = H5Dopen (file, DATASET, H5P_DEFAULT);

    /*
     * Retrieve dataset creation property list.
     */
    dcpl = H5Dget_create_plist (dset);

    /*
     * Retrieve and print the filter id, compression level and filter's name for blosc.
     */
    filter_id = H5Pget_filter2 (dcpl, (unsigned) 0, &flags, &nelmts, values_out, sizeof(filter_name), filter_name, NULL);
    printf ("Filter info is available from the dataset creation property \n ");
    printf ("  Filter identifier is ");
    switch (filter_id) {
        case H5Z_FILTER_BLOSC:
            printf ("%d\n", filter_id);
            printf ("   Number of parameters is %d with the value %u %u\n", nelmts, values_out[4], values_out[5]);
            printf ("   To find more about the filter check %s\n", filter_name);
            break;
        default:
            printf ("Not expected filter\n");
            break;
    }
    /* 
     * Check that filter is not registered with the library yet.
     */
     avail = H5Zfilter_avail(H5Z_FILTER_BLOSC);
    if (!avail) 
        printf ("blosc filter is not yet available after the H5Pget_filter call.\n");
    else
        return 1;
    
    
    /*
     * Read the data using the default properties.
     */
    printf ("....Reading blosc compressed data ................\n");
    status = H5Dread (dset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                rdata[0]);

    /*
     * Find the maximum value in the dataset, to verify that it was
     * read correctly.
     */
    max = rdata[0][0];
    for (i=0; i<DIM0; i++)
        for (j=0; j<DIM1; j++) {
            /*printf("%d \n", rdata[i][j]); */
            if (max < rdata[i][j])
                max = rdata[i][j];
        }
    /*
     * Print the maximum value.
     */
    printf ("Maximum value in %s is %d\n", DATASET, max);
    /* 
     * Check that filter is registered with the library now.
     */
    avail = H5Zfilter_avail(H5Z_FILTER_BLOSC);
    if (avail)  
        printf ("blosc filter is available now since H5Dread triggered loading of the filter.\n");
         

    /*
     * Close and release resources.
     */
    status = H5Pclose (dcpl);
    status = H5Dclose (dset);
    status = H5Fclose (file);

    return 0;
}
