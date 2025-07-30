/*
 * The filter function  H5Z_filter_bshuf was adopted from
 * https://github.com/kiyo-masui/bitshuffle
 * The plugin can be used with the HDF5 library vesrion 1.8.11+ to read
 * HDF5 datasets filtered with bshuf.
 */

/*
 * Bitshuffle HDF5 filter
 *
 * This file is part of Bitshuffle
 * Author: Kiyoshi Masui <kiyo@physics.ubc.ca>
 * Website: https://www.github.com/kiyo-masui/bitshuffle
 * Created: 2014
 *
 * See LICENSE file for details about copyright and rights to use.
 *
 *
 * Header File
 *
 * Filter Options
 * --------------
 *  block_size (option slot 0) : integer (optional)
 *      What block size to use (in elements not bytes). Default is 0,
 *      for which bitshuffle will pick a block size with a target of 8kb.
 *  Compression (option slot 1) : 0 or BSHUF_H5_COMPRESS_LZ4
 *      Whether to apply LZ4 compression to the data after bitshuffling.
 *      This is much faster than applying compression as a second filter
 *      because it is done when the small block of data is already in the
 *      L1 cache.
 *
 *      For LZ4 compression, the compressed format of the data is the same as
 *      for the normal LZ4 filter described in
 *      https://support.hdfgroup.org/services/filters/HDF5_LZ4.pdf.
 *
 */

#include "bshuf_config.h"
#include <stdio.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef STDC_HEADERS
#include <stdlib.h>
#include <stddef.h>
#else
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#endif
#ifdef HAVE_STRING_H
#if !defined STDC_HEADERS && defined HAVE_MEMORY_H
#include <memory.h>
#endif
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>

#include "H5PLextern.h"

#include "bitshuffle.h"

#define BSHUF_H5_COMPRESS_LZ4  2
#define BSHUF_H5_COMPRESS_ZSTD 3

#define BSHUF_H5FILTER 32008 /**< Filter ID registered with the HDF Group */

#define PUSH_ERR(func, minor, str)                                                                           \
    H5Epush(H5E_DEFAULT, __FILE__, func, __LINE__, H5E_ERR_CLS, H5E_PLINE, minor, str)

// Prototypes from bitshuffle.c
void     bshuf_write_uint64_BE(void *buf, uint64_t num);
uint64_t bshuf_read_uint64_BE(void *buf);
void     bshuf_write_uint32_BE(void *buf, uint32_t num);
uint32_t bshuf_read_uint32_BE(const void *buf);

static size_t H5Z_bshuf_filter(unsigned int flags, size_t cd_nelmts, const unsigned int cd_values[],
                               size_t nbytes, size_t *buf_size, void **buf);
herr_t        H5Z_bshuf_set_local(hid_t dcpl, hid_t type, hid_t space);

/**
 * The filter table maps filter identification numbers to structs that
 * contain a pointers to the bitshuffle filter function.
 */
const H5Z_class2_t H5Z_BSHUF[1] = {{
    H5Z_CLASS_T_VERS,               /**< H5Z_class_t version          */
    (H5Z_filter_t)(BSHUF_H5FILTER), /**< Filter id number             */
#ifdef FILTER_DECODE_ONLY
    0, /**< encoder_present flag (false is not available) */
#else
    1, /**< encoder_present flag (set to true) */
#endif
    1,                                                          /**< decoder_present flag         */
    "bitshuffle; see https://github.com/kiyo-masui/bitshuffle", /**< Filter name for debugging    */
    NULL,                                                       /**< The "can apply" callback     */
    (H5Z_set_local_func_t)(H5Z_bshuf_set_local),                /**< The "set local" callback     */
    (H5Z_func_t)(H5Z_bshuf_filter)                              /**< The actual filter function   */
}};

H5PL_type_t
H5PLget_plugin_type(void)
{
    return H5PL_TYPE_FILTER;
}
const void *
H5PLget_plugin_info(void)
{
    return H5Z_BSHUF;
}

// Only called on compresion, not on reverse.
/**
 * \brief Callback to determine and set per-variable filter parameters.
 *  Only called on compresion, not on reverse.
 *
 * \param[in] dcpl Dataset creation property list ID
 * \param[in] type Dataset type ID
 * \param[in] space Dataset space ID
 * \return herr_t
 *
 * \details H5Z_bshuf_set_local() Callback to determine and set per-variable filter parameters
 */
herr_t
H5Z_bshuf_set_local(hid_t dcpl, hid_t type, hid_t space)
{

    herr_t r;
    size_t ii;

    unsigned int elem_size;

    unsigned int flags;
    size_t       nelements    = 8;
    size_t       nelem_max    = 11;
    unsigned int values[]     = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int tmp_values[] = {0, 0, 0, 0, 0, 0, 0, 0};
    char         msg[80];

    r = H5Pget_filter_by_id2(dcpl, BSHUF_H5FILTER, &flags, &nelements, tmp_values, 0, NULL, NULL);
    if (r < 0)
        return -1;

    // First 3 slots reserved. Move any passed options to higher addresses.
    for (ii = 0; ii < nelements && ii + 3 < nelem_max; ii++) {
        values[ii + 3] = tmp_values[ii];
    }

    nelements = 3 + nelements;

    values[0] = BSHUF_VERSION_MAJOR;
    values[1] = BSHUF_VERSION_MINOR;

    elem_size = H5Tget_size(type);
#ifdef BSHUF_DEBUG
    fprintf(stderr, "BSHUF: Computed elem_size %d\n", elem_size);
#endif
    if (elem_size <= 0) {
        PUSH_ERR("bshuf_h5_set_local", H5E_CALLBACK, "Invalid element size.");
        return -1;
    }

    values[2] = elem_size;

    // Validate user supplied arguments.
    if (nelements > 3) {
        if (values[3] % 8 || values[3] < 0) {
            sprintf(msg, "Error in bitshuffle. Invalid block size: %d.", values[3]);
            fprintf(stderr, "BSHUF: Error in bitshuffle. Invalid block size: %d\n", values[3]);
            PUSH_ERR("bshuf_h5_set_local", H5E_CALLBACK, msg);
            return -1;
        }
    }
    if (nelements > 4) {
        switch (values[4]) {
            case 0:
                break;
            case BSHUF_H5_COMPRESS_LZ4:
                break;
#ifdef ZSTD_SUPPORT
            case BSHUF_H5_COMPRESS_ZSTD:
                break;
#endif
            default:
                PUSH_ERR("bshuf_h5_set_local", H5E_CALLBACK, "Invalid bitshuffle compression.");
        }
    }

    r = H5Pmodify_filter(dcpl, BSHUF_H5FILTER, flags, nelements, values);
    if (r < 0)
        return -1;

    return 1;
}

/**
 * \brief HDF5 BitShuffle Filter
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
 * \details H5Z_bshuf_filter() HDF5 BitShuffle Filter
 */
size_t
H5Z_bshuf_filter(unsigned int flags, size_t cd_nelmts, const unsigned int cd_values[], size_t nbytes,
                 size_t *buf_size, void **buf)
{

    size_t size, elem_size;
    int    err = -1;
    char   msg[80];
    size_t block_size = 0;
    size_t buf_size_out, nbytes_uncomp, nbytes_out;
    char  *in_buf  = *buf;
    void  *out_buf = NULL;

    if (cd_nelmts < 3) {
        PUSH_ERR("H5Z_bshuf_filter", H5E_CALLBACK, "Not enough parameters.");
        return 0;
    }
    elem_size = cd_values[2];
#ifdef ZSTD_SUPPORT
    const int comp_lvl = cd_values[5];
#endif

    // User specified block size.
    if (cd_nelmts > 3)
        block_size = cd_values[3];

    if (block_size == 0)
        block_size = bshuf_default_block_size(elem_size);

#ifndef ZSTD_SUPPORT
    if (cd_nelmts > 4 && (cd_values[4] == BSHUF_H5_COMPRESS_ZSTD)) {
        PUSH_ERR("bshuf_h5_filter", H5E_CALLBACK,
                 "ZSTD compression filter chosen but ZSTD support not installed.");
        return 0;
    }
#endif

    // Compression in addition to bitshuffle.
    if (cd_nelmts > 4 && (cd_values[4] == BSHUF_H5_COMPRESS_LZ4 || cd_values[4] == BSHUF_H5_COMPRESS_ZSTD)) {
        if (flags & H5Z_FLAG_REVERSE) {
            // First eight bytes is the number of bytes in the output buffer,
            // little endian.
            nbytes_uncomp = bshuf_read_uint64_BE(in_buf);
            // Override the block size with the one read from the header.
            block_size = bshuf_read_uint32_BE((const char *)in_buf + 8) / elem_size;
            // Skip over the header.
            in_buf += 12;
            buf_size_out = nbytes_uncomp;
        }
        else {
            nbytes_uncomp = nbytes;
            // Pick which compressions library to use
            if (cd_values[4] == BSHUF_H5_COMPRESS_LZ4) {
                buf_size_out =
                    bshuf_compress_lz4_bound(nbytes_uncomp / elem_size, elem_size, block_size) + 12;
            }
#ifdef ZSTD_SUPPORT
            else if (cd_values[4] == BSHUF_H5_COMPRESS_ZSTD) {
                buf_size_out =
                    bshuf_compress_zstd_bound(nbytes_uncomp / elem_size, elem_size, block_size) + 12;
            }
#endif
        }
    }
    else {
        nbytes_uncomp = nbytes;
        buf_size_out  = nbytes;
    }

    // TODO, remove this restriction by memcopying the extra.
    if (nbytes_uncomp % elem_size) {
        PUSH_ERR("H5Z_bshuf_filter", H5E_CALLBACK, "Non integer number of elements.");
        return 0;
    }
    size = nbytes_uncomp / elem_size;

    out_buf = malloc(buf_size_out);
    if (out_buf == NULL) {
        PUSH_ERR("H5Z_bshuf_filter", H5E_CALLBACK, "Could not allocate output buffer.");
        return 0;
    }

    if (cd_nelmts > 4 && (cd_values[4] == BSHUF_H5_COMPRESS_LZ4 || cd_values[4] == BSHUF_H5_COMPRESS_ZSTD)) {
        if (flags & H5Z_FLAG_REVERSE) {
            // Bit unshuffle/decompress.
            // Pick which compressions library to use
            if (cd_values[4] == BSHUF_H5_COMPRESS_LZ4) {
                err = bshuf_decompress_lz4(in_buf, out_buf, size, elem_size, block_size);
            }
#ifdef ZSTD_SUPPORT
            else if (cd_values[4] == BSHUF_H5_COMPRESS_ZSTD) {
                err = bshuf_decompress_zstd(in_buf, out_buf, size, elem_size, block_size);
            }
#endif
            nbytes_out = nbytes_uncomp;
        }
        else {
            // Bit shuffle/compress.
            // Write the header, described in
            // http://www.hdfgroup.org/services/filters/HDF5_LZ4.pdf.
            // Techincally we should be using signed integers instead of
            // unsigned ones, however for valid inputs (positive numbers) these
            // have the same representation.
            bshuf_write_uint64_BE(out_buf, nbytes_uncomp);
            bshuf_write_uint32_BE((char *)out_buf + 8, block_size * elem_size);
            if (cd_values[4] == BSHUF_H5_COMPRESS_LZ4) {
                err = bshuf_compress_lz4(in_buf, (char *)out_buf + 12, size, elem_size, block_size);
            }
#ifdef ZSTD_SUPPORT
            else if (cd_values[4] == BSHUF_H5_COMPRESS_ZSTD) {
                err =
                    bshuf_compress_zstd(in_buf, (char *)out_buf + 12, size, elem_size, block_size, comp_lvl);
            }
#endif
            nbytes_out = err + 12;
        }
    }
    else {
        if (flags & H5Z_FLAG_REVERSE) {
            // Bit unshuffle.
            err = bshuf_bitunshuffle(in_buf, out_buf, size, elem_size, block_size);
        }
        else {
            // Bit shuffle.
            err = bshuf_bitshuffle(in_buf, out_buf, size, elem_size, block_size);
        }
        nbytes_out = nbytes;
    }
    // printf("nb_in %d, nb_uncomp %d, nb_out %d, buf_out %d, block %d\n",
    // nbytes, nbytes_uncomp, nbytes_out, buf_size_out, block_size);

    if (err < 0) {
        sprintf(msg, "Error in bitshuffle with error code %d.", err);
        PUSH_ERR("H5Z_bshuf_filter", H5E_CALLBACK, msg);
        free(out_buf);
        return 0;
    }
    else {
        free(*buf);
        *buf      = out_buf;
        *buf_size = buf_size_out;

        return nbytes_out;
    }
}
