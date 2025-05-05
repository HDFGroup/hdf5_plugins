/*
 * Dynamically loaded filter plugin for HDF5 blosc2 filter.
 * Copyright (C) 2016  The THG Group
 * Author: Francesc Alted
 * Date: 2016-09-27
 *
 */

#include "blosc2_config.h"
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
#include <errno.h>

#include <assert.h>

#include "H5PLextern.h"

#include "blosc2.h"
#include "b2nd.h"

/* Version numbers */
#define BLOSC2_PLUGIN_VERSION_MAJOR    1    /**< for major interface/format changes  */
#define BLOSC2_PLUGIN_VERSION_MINOR    0   /**< for minor interface/format changes  */
#define BLOSC2_PLUGIN_VERSION_RELEASE  0    /**< for tweaks, bug-fixes, or development */

#define BLOSC2_PLUGIN_VERSION_STRING   "1.0.0-dev"  /**< string version.  Sync with above! */
#define BLOSC2_PLUGIN_VERSION_DATE     "2024-05-03 #$"    /* date version */

/* Filter revision number, starting at 1 */
#define FILTER_BLOSC2_VERSION 1

#define FILTER_BLOSC2 32026 /**< Filter ID registered with the HDF Group */


/* An opaque NumPy data type format for B2ND that respects the type size.
 * The actual type is irrelevant since HDF5 already stores it. */
#define B2ND_OPAQUE_NPDTYPE_FORMAT "|V%zd"

/* The maximum size of a formatted NumPy data type string:
 * "|V18446744073709551616\0". */
#define B2ND_OPAQUE_NPDTYPE_MAXLEN (2 + 20 + 1)

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

#define GET_FILTER(a, b, c, d, e, f, g) H5Pget_filter_by_id(a,b,c,d,e,f,g,NULL)

/* Auxiliary filter values are:
 *
 * - 0: filter revision (FILTER_BLOSC2_VERSION)
 * - 1: block size (in bytes)
 * - 2: type size (in bytes)
 * - 3: chunk size (in bytes)
 * - 4: compression level
 * - 5: shuffle method
 * - 6: compressor code
 * - 7: chunk rank (number of dimensions) (present if 1 < rank <= BLOSC2_MAX_DIM, for B2ND)
 * - 8 + i: length of chunk dimension i (0 <= i < rank)
 *
 * If a value is specified, all values before it must be specified too.
 *
 * If the chunk rank is specified, chunk dimensions must follow.
 */
#define MAX_FILTER_VALUES (8 + BLOSC2_MAX_DIM)
/* Compression level default */
#define DEFAULT_CLEVEL 5
/* Shuffle default */
#define DEFAULT_SHUFFLE 1
  /* Codec by default */
#define DEFAULT_COMPCODE BLOSC_BLOSCLZ

static size_t blosc2_filter_function(unsigned int flags, size_t cd_nelmts,
                    const unsigned int cd_values[], size_t nbytes,
                    size_t *buf_size, void **buf);
herr_t blosc2_set_local(hid_t dcpl, hid_t type, hid_t space);

/**
 * The filter table maps filter identification numbers to structs that
 * contain a pointers to the blosc filter function.
 */
const H5Z_class2_t blosc2_H5Filter[1] = {{
    H5Z_CLASS_T_VERS,       /**<  H5Z_class_t version */
    (H5Z_filter_t)(FILTER_BLOSC2),         /**<  Filter id number */
#ifdef FILTER_DECODE_ONLY
    0,                   /**<  encoder_present flag (false is not available) */
#else
    1,                   /**<  encoder_present flag (set to true) */
#endif
    1,                   /**<  decoder_present flag (set to true) */
    "HDF5 blosc2 filter; see https://github.com/HDFGroup/hdf5_plugins/blob/master/docs/RegisteredFilterPlugins.md", /* Filter name for debugging */
    NULL,                           /**<  The "can apply" callback */
    (H5Z_set_local_func_t)(blosc2_set_local), /**<  The "set local" callback */
    (H5Z_func_t)(blosc2_filter_function),    /**<  The actual filter function */
}};


H5PL_type_t H5PLget_plugin_type(void) { return H5PL_TYPE_FILTER; }
const void* H5PLget_plugin_info(void) { return blosc2_H5Filter; }


/**
 * \brief Callback to determine and set per-variable filter parameters
 *
 * \param[in] dcpl Dataset creation property list ID
 * \param[in] type Dataset type ID
 * \param[in] space Dataset space ID
 * \return herr_t
 *
 * \details blosc_set_local() Callback to determine and set per-variable filter parameters
 *
 *  Filter setup.  Records the following inside the DCPL:
 *
 *    1. Set slot 0 to the filter revision.
 *
 *    2. Compute the type size in bytes and store it in slot 2.
 *
 *    3. Compute the chunk size in bytes and store it in slot 3.
 *
 *    4. If 1 < rank <= BLOSC2_MAX_DIM, store it in slot 7, and chunk dimensions in the following slots.
 */
herr_t blosc2_set_local(hid_t dcpl, hid_t type, hid_t space) {

    int ndim;
    int i;
    herr_t r;

    unsigned int typesize, basetypesize;
    unsigned int bufsize;
    hsize_t chunkshape[H5S_MAX_RANK];
    unsigned int flags;
    size_t nelements = MAX_FILTER_VALUES;
    unsigned int values[MAX_FILTER_VALUES];
    hid_t super_type;
    H5T_class_t classt;
#ifdef BLOSC2_DEBUG
    fprintf(stderr, "Blosc: blosc_set_local start\n");
#endif

    memset(values, 0, sizeof(values));
    r = GET_FILTER(dcpl, FILTER_BLOSC2, &flags, &nelements, values, 0, NULL);
    if (r < 0) return -1;
#ifdef BLOSC2_DEBUG
    fprintf(stderr, "Blosc: H5Pget_filter_by_id\n");
#endif

    if (nelements < 4)
        nelements = 4;  /* First 4 slots reserved. */

    /* Set Blosc2 info in first slot */
    values[0] = FILTER_BLOSC2_VERSION;

    ndim = H5Pget_chunk(dcpl, H5S_MAX_RANK, chunkshape);
    if (ndim < 0)
        return -1;
    if (ndim > H5S_MAX_RANK) {
        PUSH_ERR("blosc2_set_local", H5E_CALLBACK, "Chunk rank exceeds HDF5 limit");
        return -1;
    }

    typesize = (unsigned int) H5Tget_size(type);
    if (typesize == 0) return -1;
#ifdef BLOSC2_DEBUG
    fprintf(stderr, "Blosc: H5Tget_size\n");
#endif
    /* Get the size of the base type, even for ARRAY types */
    classt = H5Tget_class(type);
    if (classt == H5T_ARRAY) {
        /* Get the array base component */
        super_type = H5Tget_super(type);
        basetypesize = (unsigned int) H5Tget_size(super_type);
        /* Release resources */
        H5Tclose(super_type);
    } else {
        basetypesize = typesize;
    }

    values[2] = basetypesize;

    /* Get the size of the chunk */
    bufsize = typesize;
    for (i = 0; i < ndim; i++) {
        bufsize *= (unsigned int) chunkshape[i];
    }
    values[3] = bufsize;

#ifdef BLOSC2_DEBUG
    fprintf(stderr, "Blosc2: Computed buffer size %d\n", bufsize);
#endif

    if (1 < ndim && ndim <= BLOSC2_MAX_DIM) {
        if (nelements < 5) { values[4] = DEFAULT_CLEVEL; }
        if (nelements < 6) { values[5] = DEFAULT_SHUFFLE; }
        if (nelements < 7) { values[6] = DEFAULT_COMPCODE; }

        values[7] = ndim;
        for (int i = 0; i < ndim; i++) {
            values[8 + i] = (unsigned int)(chunkshape[i]);
        };

        nelements = 8 + ndim;
    } else if (ndim > 1) {
        /* The user may be expecting more efficient storage than we can currently provide,
         * so convey some information when tracing. */
        BLOSC_TRACE_WARNING("Chunk rank %d exceeds B2ND build limit %d, "
                "using plain Blosc2 instead", ndim, BLOSC2_MAX_DIM);
    }

    r = H5Pmodify_filter(dcpl, FILTER_BLOSC2, flags, nelements, values);
    if (r < 0)
        return -1;

#ifdef BLOSC2_DEBUG
    fprintf(stderr, "Blosc: blosc_set_local end\n");
#endif
    return 1;
}


/* From Blosc2's "examples/get_blocksize.c". */
int32_t compute_blosc2_blocksize(int32_t chunksize, int32_t typesize,
        int clevel, int compcode) {
    static uint8_t data_dest[BLOSC2_MAX_OVERHEAD];
    blosc2_cparams cparams = BLOSC2_CPARAMS_DEFAULTS;
    cparams.compcode = (compcode < 0) ? DEFAULT_COMPCODE : compcode;
    cparams.clevel = clevel;
    cparams.typesize = typesize;

    if (blosc2_chunk_zeros(cparams, chunksize, data_dest, BLOSC2_MAX_OVERHEAD) < 0) {
        BLOSC_TRACE_ERROR("Failed to create zeroed chunk for blocksize computation");
        return -1;
    }

    int32_t blocksize = -1;
    if (blosc2_cbuffer_sizes(data_dest, NULL, NULL, &blocksize) < 0) {
        BLOSC_TRACE_ERROR("Failed to get chunk sizes for blocksize computation");
        return -1;
    }
    return blocksize;
}


/* Get the maximum block size which is not greater than the given block_size
 * and fits within the given chunk dimensions dims_chunk. Sizes must always be
 * greater than 0.
 *
 * Block dimensions start with 2 (unless the respective chunk dimension is 1),
 * and are doubled starting from the innermost (rightmost) ones, to leverage
 * the locality of C array arrangement.  The resulting block dimensions are
 * placed in the last (output) argument.
 *
 * Based on Python-Blosc2's blosc2.core.compute_chunks_blocks and
 * compute_partition.
 */
int32_t compute_b2nd_block_shape(size_t block_size,
        size_t type_size,
        const int rank,
        const int32_t *dims_chunk,
        int32_t *dims_block) {
    assert(block_size >= 0);
    assert(type_size >= 0);
    size_t nitems = block_size / type_size;

    // Start with the smallest possible block dimensions (1 or 2).
    size_t nitems_new = 1;
    for (int i = 0; i < rank; i++) {
        assert(dims_chunk[i] != 0);
        dims_block[i] = dims_chunk[i] == 1 ? 1 : 2;
        nitems_new *= dims_block[i];
    }

    if (nitems_new > nitems) {
        BLOSC_TRACE_INFO("Target block size is too small (%lu items), raising to %lu items",
                nitems, nitems_new);
    }
    if (nitems_new >= nitems) {
        return nitems_new * type_size;
    }

    // Double block dimensions (bound by chunk dimensions) from right to left
    // while block is under nitems.
    while (nitems_new < nitems) {
        size_t nitems_prev = nitems_new;
        for (int i = rank - 1; i >= 0; i--) {
            if (dims_block[i] * 2 <= dims_chunk[i]) {
                if (nitems_new * 2 <= nitems) {
                    nitems_new *= 2;
                    dims_block[i] *= 2;
                }
            } else if (dims_block[i] < dims_chunk[i]) {
                size_t newitems_ext = (nitems_new / dims_block[i]) * dims_chunk[i];
                if (newitems_ext <= nitems) {
                    nitems_new = newitems_ext;
                    dims_block[i] = dims_chunk[i];
                }
            } else {
                assert(dims_block[i] == dims_chunk[i]);  // nothing to change
            }
        }
        if (nitems_new == nitems_prev) {
            break;  // not progressing anymore
        }
    }
    return nitems_new * type_size;
}

/**
 * \brief HDF5 Blosc2 Filter
 *
 * \param[in] flags Bitfield that encodes filter direction
 * \param[in] cd_nelmts Number of elements in filter parameter (cd_values[]) array
 * \param[in] cd_values[] Filter parameters
 * \param[in] nbytes Number of bytes in input buffer (before forward/reverse filter)
 * \param[out] buf_size Number of bytes in output buffer (after forward/reverse filter)
 * \param[in.out] buf Values to quantize
 *
 * \return size_t
 *
 * \details blosc2_filter() HDF5 Blosc2 Filter
 */
size_t blosc2_filter_function(unsigned int flags, size_t cd_nelmts,
                              const unsigned int cd_values[], size_t nbytes,
                              size_t *buf_size, void **buf) {

    void* outbuf = NULL;
    int64_t status = 0;                /* Return code from Blosc2 routines */
    size_t blocksize;
    size_t typesize;
    size_t outbuf_size;
    int clevel    = DEFAULT_CLEVEL;
    int doshuffle = DEFAULT_SHUFFLE;
    int compcode  = DEFAULT_COMPCODE;
    char errmsg[256];

    if (cd_nelmts < 4) {
        sprintf(errmsg, "Too few filter parameters for B2ND: %d", cd_nelmts);
        PUSH_ERR("blosc2_filter", H5E_CALLBACK, errmsg);
        goto failed;
    }

    /* Filter params that are always set */
    blocksize = cd_values[1];      /* The block size */
    typesize = cd_values[2];      /* The datatype size */
    outbuf_size = cd_values[3];   /* Precomputed buffer guess */

    /* Filter params that are only set for B2ND */
    int ndim = -1;
    int32_t chunkshape[BLOSC2_MAX_DIM];
    if (cd_nelmts >= 8) {
        /* Get chunk shape for B2ND */
        ndim = cd_values[7];
        if (ndim < 2) {
            sprintf(errmsg, "Chunk rank %d (filter value) is too small for B2ND", ndim);
            PUSH_ERR("blosc2_filter", H5E_CALLBACK, errmsg);
            goto failed;
        }
        if (ndim > BLOSC2_MAX_DIM) {
            sprintf(errmsg, "Chunk rank %d (filter value) exceeds B2ND build limit %d",
                    ndim, BLOSC2_MAX_DIM);
            PUSH_ERR("blosc2_filter", H5E_CALLBACK, errmsg);
            goto failed;
        }
        if (cd_nelmts < (size_t)(8 + ndim)) {
            sprintf(errmsg, "Too few dimensions for B2ND in filter values (%z/%d)",
                    cd_nelmts - 8, ndim);
            PUSH_ERR("blosc2_filter", H5E_CALLBACK, errmsg);
            goto failed;
        }
        for (int i = 0; i < ndim; i++) {
            chunkshape[i] = cd_values[8 + i];
        }
    }

    blosc2_init();

    if (!(flags & H5Z_FLAG_REVERSE)) {
        /* We're compressing */

        if (cd_nelmts < 6) {
            sprintf(errmsg, "Too few filter parameters for Blosc2 compression: %d", cd_nelmts);
            PUSH_ERR("blosc2_filter", H5E_CALLBACK, errmsg);
            goto failed;
        }

        /* Compression params */
        clevel = cd_values[4];        /* The compression level */
        doshuffle = cd_values[5];     /* SHUFFLE, BITSHUFFLE, others */
        if (cd_nelmts >= 7) {
            compcode = cd_values[6];    /* The Blosc2 compressor used */
            /* Check that we actually have support for the compressor code */
            const char* complist = blosc2_list_compressors();
            const char* compname;
            int code = blosc2_compcode_to_compname(compcode, &compname);
            if (code == -1) {
                sprintf(errmsg, "this Blosc2 library does not have support for the '%s' compressor, but only for: %s",
                        compname, complist);
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, errmsg);
                goto failed;
            }
        }

#ifdef BLOSC2_DEBUG
        fprintf(stderr, "Blosc2: Compress %zd chunk w/buffer %zd\n",
                nbytes, *buf_size);
#endif

        blosc2_cparams cparams = BLOSC2_CPARAMS_DEFAULTS;
        cparams.compcode = compcode;
        cparams.typesize = (int32_t) typesize;
        cparams.filters[BLOSC_LAST_FILTER] = doshuffle;
        cparams.clevel = clevel;
        // cparams.blocksize depends on dimensionality

        blosc2_storage storage = {.cparams=&cparams, .contiguous=false};

        if (ndim > 1) {

            b2nd_context_t *ctx = NULL;
            b2nd_array_t *array = NULL;

            if (blocksize == 0) {
                int32_t sugg_blocksize = compute_blosc2_blocksize(outbuf_size, typesize,
                        clevel, compcode);
                if (sugg_blocksize < 0) {
                    PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Failed to compute suggested blocksize");
                    goto failed;
                }
                blocksize = sugg_blocksize;
            }
            int32_t blockdims[BLOSC2_MAX_DIM];
            cparams.blocksize = compute_b2nd_block_shape(blocksize, typesize,
                    ndim, chunkshape,
                    blockdims);

            int64_t chunkshape_l[BLOSC2_MAX_DIM];
            for (int i = 0; i < ndim; i++) {
                chunkshape_l[i] = chunkshape[i];
            }

            char dtype[B2ND_OPAQUE_NPDTYPE_MAXLEN];
            snprintf(dtype, sizeof(dtype), B2ND_OPAQUE_NPDTYPE_FORMAT, typesize);
            if (!(ctx = b2nd_create_ctx(&storage,
                    ndim, chunkshape_l, chunkshape, blockdims,
                    dtype, DTYPE_NUMPY_FORMAT, NULL, 0))) {
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot create B2ND context");
                goto b2nd_comp_out;
            }

            if (b2nd_from_cbuffer(ctx, &array, *buf, nbytes) < 0) {
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot compress buffer into B2ND array");
                goto b2nd_comp_out;
            }

            bool needs_free;
            if (b2nd_to_cframe(array, (uint8_t **)&outbuf, &status, &needs_free) < 0) {
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot convert B2ND array to buffer");
                goto b2nd_comp_out;
            }

            b2nd_comp_out:
            if (array) b2nd_free(array);
            if (ctx) b2nd_free_ctx(ctx);

        } else {
            cparams.blocksize = blocksize;

            blosc2_context *cctx = blosc2_create_cctx(cparams);
            blosc2_schunk* schunk = blosc2_schunk_new(&storage);
            if (schunk == NULL) {
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot create a super-chunk");
                goto b2_comp_out;
            }

            status = blosc2_schunk_append_buffer(schunk, *buf, (int32_t) nbytes);
            if (status < 0) {
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot append buffer to super-chunk");
                goto b2_comp_out;
            }

            bool needs_free;
            status = blosc2_schunk_to_buffer(schunk, (uint8_t **)&outbuf, &needs_free);
            if (status < 0 || !needs_free) {
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot convert super-chunk to buffer");
                goto b2_comp_out;
            }

            b2_comp_out:
            if (schunk) blosc2_schunk_free(schunk);
            if (cctx) blosc2_free_ctx(cctx);

        }

#ifdef BLOSC2_DEBUG
        fprintf(stderr, "Blosc2: Compressed into %zd bytes\n", status);
#endif

    } else {
        /* We're decompressing */
        /* declare dummy variables */
        int32_t cbytes;

        blosc2_schunk* schunk = blosc2_schunk_from_buffer(*buf, (int64_t)nbytes, false);
        if (schunk == NULL) {
            PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot get super-chunk from buffer");
            goto failed;
        }

        /* Although blosc2_decompress_ctx ("else" branch) can decompress b2nd-formatted data,
         * there may be padding bytes when the chunkshape is not a multiple of the blockshape,
         * and only b2nd machinery knows how to handle these correctly.
         */
        if (blosc2_meta_exists(schunk, "b2nd") >= 0
                || blosc2_meta_exists(schunk, "caterva") >= 0) {

            if (ndim < 0) {
                BLOSC_TRACE_WARNING("Auxiliary Blosc2 filter values contain no chunk rank/shape, "
                        "some checks on B2ND arrays will be disabled");
            }

            b2nd_array_t *array = NULL;

            if (b2nd_from_schunk(schunk, &array) < 0) {
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot create B2ND array from super-chunk");
                goto b2nd_decomp_out;
            }
            schunk = NULL;  // owned by the array now, do not free on its own

            /* Check array metadata against filter values */
            if (ndim >= 0 && array->ndim != ndim) {
                sprintf(errmsg, "B2ND array rank (%hhd) != filter rank (%d)", array->ndim, ndim);
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, errmsg);
                goto b2nd_decomp_out;
            }
            int64_t start[BLOSC2_MAX_DIM], stop[BLOSC2_MAX_DIM], size = typesize;
            for (int i = 0; i < array->ndim; i++) {
                start[i] = 0;
                stop[i] = array->shape[i];
                size *= array->shape[i];
                if (ndim >= 0 && array->shape[i] != chunkshape[i]) {
                    /* The HDF5 filter pipeline needs the filter to always return full chunks,
                     * even for margin chunks where data does not fill the whole chunk
                     * (in which case padding should be used
                     * (at least until the last byte with actual data).
                     * Of course, we need to know the chunkshape to check this.
                     */
                    sprintf(errmsg, "B2ND array shape[%d] (%ld) != filter chunkshape[%d] (%d)",
                            i, array->shape[i], i, chunkshape[i]);
                    PUSH_ERR("blosc2_filter", H5E_CALLBACK, errmsg);
                    goto b2nd_decomp_out;
                }
            }
            assert(outbuf_size >= size);

            outbuf = malloc(outbuf_size);
            if (outbuf == NULL) {
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot allocate decompression buffer");
                goto b2nd_decomp_out;
            }

            if (b2nd_get_slice_cbuffer(array, start, stop,
                    outbuf, stop, outbuf_size) < 0) {
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot decompress B2ND array into buffer");
                goto b2nd_decomp_out;
            }
            status = size;

            b2nd_decomp_out:
            if (array) b2nd_free(array);

        } else {

            uint8_t *chunk;
            blosc2_context *dctx = NULL;

            bool needs_free;
            cbytes = blosc2_schunk_get_lazychunk(schunk, 0, &chunk, &needs_free);
            if (cbytes < 0) {
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot get chunk from super-chunk");
                goto b2_decomp_out;
            }

            /* Get the exact outbuf_size from the buffer header */
            int32_t nbytes;
            blosc2_cbuffer_sizes(chunk, &nbytes, NULL, NULL);
            outbuf_size = nbytes;

#ifdef BLOSC2_DEBUG
            fprintf(stderr, "Blosc2: Decompress %zd chunk w/buffer %zd\n", nbytes, outbuf_size);
#endif

            outbuf = malloc(outbuf_size);
            if (outbuf == NULL) {
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot allocate decompression buffer");
                goto b2_decomp_out;
            }

            blosc2_dparams dparams = BLOSC2_DPARAMS_DEFAULTS;
            dparams.schunk = schunk;
            dctx = blosc2_create_dctx(dparams);
            status = blosc2_decompress_ctx(dctx, chunk, cbytes, outbuf, (int32_t) outbuf_size);
            if (status <= 0) {
                PUSH_ERR("blosc2_filter", H5E_CALLBACK, "Cannot decompress chunk into buffer");
                goto b2_decomp_out;
            }

            b2_decomp_out:
            if (dctx) blosc2_free_ctx(dctx);
            if (chunk && needs_free) free(chunk);

        }

        if (schunk) blosc2_schunk_free(schunk);

    } /* compressing vs decompressing */

    if (status > 0) {
        free(*buf);
        *buf = outbuf;
        *buf_size = outbuf_size;
        return status;  /* Size of compressed/decompressed data */
    }

    failed:
    if (outbuf) free(outbuf);
    blosc2_destroy();

    return 0;
} /* End filter function */
