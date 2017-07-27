/*
 * Dynamically loaded filter plugin for HDF5 blosc filter.
 * Copyright (C) 2016  The THG Group
 * Author: Francesc Alted
 * Date: 2016-09-27
 *
 */


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "H5PLextern.h"

#include "blosc.h"

/* Version numbers */
#define BLOSC_PLUGIN_VERSION_MAJOR    1    /* for major interface/format changes  */
#define BLOSC_PLUGIN_VERSION_MINOR    0   /* for minor interface/format changes  */
#define BLOSC_PLUGIN_VERSION_RELEASE  0    /* for tweaks, bug-fixes, or development */

#define BLOSC_PLUGIN_VERSION_STRING   "1.0.0-dev"  /* string version.  Sync with above! */
#define BLOSC_PLUGIN_VERSION_DATE     "2016-09-03 #$"    /* date version */

/* Filter revision number, starting at 1 */
/* #define FILTER_BLOSC_VERSION 1 */
#define FILTER_BLOSC_VERSION 2  /* multiple compressors since Blosc 1.3 */

/* Filter ID registered with the HDF Group */
#define FILTER_BLOSC 32001

#if 0
#if defined(__GNUC__)
#define PUSH_ERR(func, minor, str, ...) H5Epush(H5E_DEFAULT, __FILE__, func, __LINE__, H5E_ERR_CLS, H5E_PLINE, minor, str, ##__VA_ARGS__)
#elif defined(_MSC_VER)
#define PUSH_ERR(func, minor, str, ...) H5Epush(H5E_DEFAULT, __FILE__, func, __LINE__, H5E_ERR_CLS, H5E_PLINE, minor, str, __VA_ARGS__)
#else
/* This version is portable but it's better to use compiler-supported
   approaches for handling the trailing comma issue when possible. */
#define PUSH_ERR(func, minor, ...) H5Epush(H5E_DEFAULT, __FILE__, func, __LINE__, H5E_ERR_CLS, H5E_PLINE, minor, __VA_ARGS__)
#endif    /* defined(__GNUC__) */
#else
#define PUSH_ERR(func, minor, str) H5Epush(H5E_DEFAULT, __FILE__, func, __LINE__, H5E_ERR_CLS, H5E_PLINE, minor, str)
#endif

static size_t blosc_filter(unsigned int flags, size_t cd_nelmts,
                    const unsigned int cd_values[], size_t nbytes,
                    size_t *buf_size, void **buf);
herr_t blosc_set_local(hid_t dcpl, hid_t type, hid_t space);

const H5Z_class2_t blosc_H5Filter[1] = {{
    H5Z_CLASS_T_VERS,       /* H5Z_class_t version */
    (H5Z_filter_t)(FILTER_BLOSC),         /* Filter id number */
#ifdef FILTER_DECODE_ONLY
    0,                   /* encoder_present flag (false is not available) */
#else
    1,                   /* encoder_present flag (set to true) */
#endif
    1,                   /* decoder_present flag (set to true) */
    "HDF5 blosc filter; see http://www.hdfgroup.org/services/contributions.html", /* Filter name for debugging */
    NULL,                           /* The "can apply" callback */
    (H5Z_set_local_func_t)(blosc_set_local), /* The "set local" callback */
    (H5Z_func_t)(blosc_filter),    /* The actual filter function */
}};


H5PL_type_t H5PLget_plugin_type(void) { return H5PL_TYPE_FILTER; }
const void* H5PLget_plugin_info(void) { return blosc_H5Filter; }


/*  Filter setup.  Records the following inside the DCPL:

    1. If version information is not present, set slots 0 and 1 to the filter
       revision and Blosc version, respectively.

    2. Compute the type size in bytes and store it in slot 2.

    3. Compute the chunk size in bytes and store it in slot 3.
*/
herr_t blosc_set_local(hid_t dcpl, hid_t type, hid_t space) {

    int ndims;
    int i;
    herr_t r;

    unsigned int typesize, basetypesize;
    unsigned int bufsize;
    hsize_t chunkdims[32];
    unsigned int flags;
    size_t nelements = 8;
    unsigned int values[] = {0,0,0,0,0,0,0,0};
    hid_t super_type;
    H5T_class_t classt;
#ifdef BLOSC_DEBUG
    fprintf(stderr, "Blosc: blosc_set_local start\n");
#endif

    r = H5Pget_filter_by_id2(dcpl, FILTER_BLOSC, &flags, &nelements, values, 0, NULL, NULL);
    if (r < 0) return -1;
#ifdef BLOSC_DEBUG
    fprintf(stderr, "Blosc: H5Pget_filter_by_id\n");
#endif

    if (nelements < 4) nelements = 4;  /* First 4 slots reserved. */

    /* Set Blosc info in first two slots */
    values[0] = FILTER_BLOSC_VERSION;
    values[1] = BLOSC_VERSION_FORMAT;

    ndims = H5Pget_chunk(dcpl, 32, chunkdims);
    if (ndims < 0) return -1;
    if (ndims > 32) {
        PUSH_ERR("blosc_set_local", H5E_CALLBACK, "Chunk rank exceeds limit");
        return -1;
    }

    typesize = H5Tget_size(type);
    if (typesize==0) return -1;
#ifdef BLOSC_DEBUG
    fprintf(stderr, "Blosc: H5Tget_size\n");
#endif
    /* Get the size of the base type, even for ARRAY types */
    classt = H5Tget_class(type);
    if (classt == H5T_ARRAY) {
      /* Get the array base component */
      super_type = H5Tget_super(type);
      basetypesize = H5Tget_size(super_type);
      /* Release resources */
      H5Tclose(super_type);
    }
    else {
      basetypesize = typesize;
    }

    /* Limit large typesizes (they are pretty expensive to shuffle
       and, in addition, Blosc does not handle typesizes larger than
       256 bytes). */
    if (basetypesize > BLOSC_MAX_TYPESIZE) basetypesize = 1;
    values[2] = basetypesize;

    /* Get the size of the chunk */
    bufsize = typesize;
    for (i = 0; i < ndims; i++) {
        bufsize *= chunkdims[i];
    }
    values[3] = bufsize;

#ifdef BLOSC_DEBUG
    fprintf(stderr, "Blosc: Computed buffer size %d\n", bufsize);
#endif

    r = H5Pmodify_filter(dcpl, FILTER_BLOSC, flags, nelements, values);
    if (r < 0) return -1;

#ifdef BLOSC_DEBUG
    fprintf(stderr, "Blosc: blosc_set_local end\n");
#endif
    return 1;
}

/* The filter function */
size_t blosc_filter(unsigned int flags, size_t cd_nelmts,
                    const unsigned int cd_values[], size_t nbytes,
                    size_t *buf_size, void **buf) {

    void* outbuf = NULL;
    int status = 0;                /* Return code from Blosc routines */
    size_t typesize;
    size_t outbuf_size;
    int clevel = 5;                /* Compression level default */
    int doshuffle = 1;             /* Shuffle default */
    int compcode;                  /* Blosc compressor */
    int code;
    char *compname = "zstd";       /* The compressor by default */
    char *complist;
    char errmsg[256];

    /* Filter params that are always set */
    typesize = cd_values[2];      /* The datatype size */
    outbuf_size = cd_values[3];   /* Precomputed buffer guess */
    /* Optional params */
    if (cd_nelmts >= 5) {
        clevel = cd_values[4];        /* The compression level */
    }
    if (cd_nelmts >= 6) {
        doshuffle = cd_values[5];     /* BLOSC_SHUFFLE, BLOSC_BITSHUFFLE */
    }
    if (cd_nelmts >= 7) {
        compcode = cd_values[6];     /* The Blosc compressor used */
        /* Check that we actually have support for the compressor code */
        complist = blosc_list_compressors();
        code = blosc_compcode_to_compname(compcode, &compname);
        if (code == -1) {
            sprintf(errmsg, "this Blosc library does not have support for "
                    "the '%s' compressor, but only for: %s",
                    compname, complist);
            PUSH_ERR("blosc_filter", H5E_CALLBACK, errmsg);
            goto failed;
        }
    }

    /* We're compressing */
    if(!(flags & H5Z_FLAG_REVERSE)){

        /* Allocate an output buffer exactly as long as the input data; if
           the result is larger, we simply return 0.  The filter is flagged
           as optional, so HDF5 marks the chunk as uncompressed and
           proceeds.
        */

        outbuf_size = (*buf_size);

#ifdef BLOSC_DEBUG
        fprintf(stderr, "Blosc: Compress %zd chunk w/buffer %zd\n", nbytes, outbuf_size);
#endif

        outbuf = malloc(outbuf_size);

        if (outbuf == NULL) {
            PUSH_ERR("blosc_filter", H5E_CALLBACK,
                     "Can't allocate compression buffer");
            goto failed;
        }

        blosc_set_compressor(compname);
        status = blosc_compress(clevel, doshuffle, typesize, nbytes, *buf, outbuf, nbytes);
        if (status < 0) {
            PUSH_ERR("blosc_filter", H5E_CALLBACK, "Blosc compression error");
            goto failed;
        }

    /* We're decompressing */
    }
    else {
        /* declare dummy variables */
        size_t cbytes, blocksize;

        free(outbuf);

        /* Extract the exact outbuf_size from the buffer header.
         *
         * NOTE: the guess value got from "cd_values" corresponds to the
         * uncompressed chunk size but it should not be used in a general
         * cases since other filters in the pipeline can modify the buffere
         *  size.
         */
        blosc_cbuffer_sizes(*buf, &outbuf_size, &cbytes, &blocksize);

#ifdef BLOSC_DEBUG
        fprintf(stderr, "Blosc: Decompress %zd chunk w/buffer %zd\n", nbytes, outbuf_size);
#endif

        outbuf = malloc(outbuf_size);

        if (outbuf == NULL) {
            PUSH_ERR("blosc_filter", H5E_CALLBACK, "Can't allocate decompression buffer");
            goto failed;
        }

        status = blosc_decompress(*buf, outbuf, outbuf_size);
        if (status <= 0) {    /* decompression failed */
            PUSH_ERR("blosc_filter", H5E_CALLBACK, "Blosc decompression error");
            goto failed;
        } /* if !status */
    } /* compressing vs decompressing */

    if (status != 0) {
        free(*buf);
        *buf = outbuf;
        *buf_size = outbuf_size;
        return status;  /* Size of compressed/decompressed data */
    }

failed:
    free(outbuf);
    return 0;
} /* End filter function */
