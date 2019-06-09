/*
 * This file is an example of an HDF5 filter plugin.
 * The filter function  H5Z_filter_lzf was adopted from
 * PyTables http://www.pytables.org.
 * The plugin can be used with the HDF5 library vesrion 1.8.11+ to read
 * HDF5 datasets compressed with lzf.
 */

/*
 *
Copyright Notice and Statement for PyTables Software Library and Utilities:

Copyright (c) 2002-2004 by Francesc Alted
Copyright (c) 2005-2007 by Carabos Coop. V.
Copyright (c) 2008-2010 by Francesc Alted
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

a. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

b. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

c. Neither the name of Francesc Alted nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
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

#include "H5PLextern.h"

#include "lzf.h"

/*
 * LZF compression is an external filter registered by PyTables developers
 * with The HDF Group
 * See http://www.hdfgroup.org/services/contributions.html for more information.
 */

/* Filter revision number, starting at 1 */
#define FILTER_LZF_VERSION 4

/* Filter ID registered with the HDF Group */
#define H5Z_FILTER_LZF 32000

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

static size_t H5Z_lzf_filter(unsigned int flags, size_t cd_nelmts,
                    const unsigned int cd_values[], size_t nbytes,
                    size_t *buf_size, void **buf);
herr_t H5Z_lzf_set_local(hid_t dcpl, hid_t type, hid_t space);

const H5Z_class2_t H5Z_LZF[1] = {{
        H5Z_CLASS_T_VERS,       /* H5Z_class_t version */
        (H5Z_filter_t)(H5Z_FILTER_LZF),         /* Filter id number             */
#ifdef FILTER_DECODE_ONLY
    0,                   /* encoder_present flag (false is not available) */
#else
    1,                   /* encoder_present flag (set to true) */
#endif
        1,              /* decoder_present flag (set to true) */
        "HDF5 lzf filter; see http://www.hdfgroup.org/services/contributions.html",
        /* Filter name for debugging    */
        NULL,                       /* The "can apply" callback     */
        (H5Z_set_local_func_t)(H5Z_lzf_set_local), /* The "set local" callback */
        (H5Z_func_t)(H5Z_lzf_filter),         /* The actual filter function   */
}};

H5PL_type_t H5PLget_plugin_type(void) { return H5PL_TYPE_FILTER; }
const void* H5PLget_plugin_info(void) { return H5Z_LZF; }


/*  Filter setup.  Records the following inside the DCPL:

    1.  If version information is not present, set slots 0 and 1 to the filter
        revision and LZF API version, respectively.

    2. Compute the chunk size in bytes and store it in slot 2.
*/
herr_t H5Z_lzf_set_local(hid_t dcpl, hid_t type, hid_t space) {

    int ndims;
    int i;
    herr_t r;

    unsigned int bufsize;
    hsize_t chunkdims[32];
    unsigned int flags;
    size_t nelements = 8;
    unsigned int values[] = {0,0,0,0,0,0,0,0};
#ifdef H5PY_LZF_DEBUG
    fprintf(stderr, "LZF: lzf_set_local start\n");
#endif

    r = H5Pget_filter_by_id2(dcpl, H5Z_FILTER_LZF, &flags, &nelements, values, 0, NULL, NULL);
    if (r < 0) return -1;
#ifdef H5PY_LZF_DEBUG
    fprintf(stderr, "LZF: H5Pget_filter_by_id\n");
#endif

    if (nelements < 3) nelements = 3;  /* First 3 slots reserved.  If any higher
                                      slots are used, preserve the contents. */

    /* It seems the H5Z_FLAG_REVERSE flag doesn't work here, so we have to be
       careful not to clobber any existing version info */
    if (values[0] == 0) values[0] = FILTER_LZF_VERSION;
    if (values[1] == 0) values[1] = LZF_VERSION;

    ndims = H5Pget_chunk(dcpl, 32, chunkdims);
    if (ndims < 0) return -1;
    if (ndims > 32) {
        PUSH_ERR("lzf_set_local", H5E_CALLBACK, "Chunk rank exceeds limit");
        return -1;
    }

    bufsize = H5Tget_size(type);
    if (bufsize == 0) return -1;
#ifdef H5PY_LZF_DEBUG
    fprintf(stderr, "LZF: H5Tget_size\n");
#endif

    for (i = 0;i < ndims;i++) {
        bufsize *= chunkdims[i];
    }

    values[2] = bufsize;

#ifdef H5PY_LZF_DEBUG
    fprintf(stderr, "LZF: Computed buffer size %d\n", bufsize);
#endif

    r = H5Pmodify_filter(dcpl, H5Z_FILTER_LZF, flags, nelements, values);
    if (r < 0) return -1;

#ifdef H5PY_LZF_DEBUG
    fprintf(stderr, "LZF: lzf_set_local end\n");
#endif
    return 1;
}

/* The filter function */
static size_t H5Z_lzf_filter(unsigned int flags, size_t cd_nelmts,
        const unsigned int cd_values[], size_t nbytes,
        size_t *buf_size, void **buf)
{

    void* outbuf = NULL;
    size_t outbuf_size = 0;

    unsigned int status = 0;        /* Return code from lzf routines */

    /* We're compressing */
    if(!(flags & H5Z_FLAG_REVERSE)){

        /* Allocate an output buffer exactly as long as the input data; if
           the result is larger, we simply return 0.  The filter is flagged
           as optional, so HDF5 marks the chunk as uncompressed and
           proceeds.
        */

        outbuf_size = (*buf_size);

#ifdef H5PY_LZF_DEBUG
        fprintf(stderr, "LZF: Compress %zd chunk w/buffer %zd\n", nbytes, outbuf_size);
#endif

        outbuf = malloc(outbuf_size);

        if (outbuf == NULL) {
            PUSH_ERR("lzf_filter", H5E_CALLBACK, "Can't allocate compression buffer");
            goto failed;
        }

        status = lzf_compress(*buf, nbytes, outbuf, outbuf_size);
        if (status < 0) {
            PUSH_ERR("lzf_filter", H5E_CALLBACK, "LZF compression error");
            goto failed;
        }

    /* We're decompressing */
    }
    else {
        if ((cd_nelmts >= 3) && (cd_values[2] != 0)) {
            outbuf_size = cd_values[2];   /* Precomputed buffer guess */
        }
        else {
            outbuf_size = (*buf_size);
        }

        free(outbuf);

#ifdef H5PY_LZF_DEBUG
        fprintf(stderr, "Decompress %d chunk w/buffer %d\n", nbytes, outbuf_size);
#endif

        outbuf = malloc(outbuf_size);

        if (outbuf == NULL) {
            PUSH_ERR("lzf_filter", H5E_CALLBACK, "Can't allocate decompression buffer");
            goto failed;
        }

        status = lzf_decompress(*buf, nbytes, outbuf, outbuf_size);

        if (!status) {    /* compression failed */

            if (errno == E2BIG) {
                outbuf_size += (*buf_size);
#ifdef H5PY_LZF_DEBUG
                fprintf(stderr, "    Too small: %d\n", outbuf_size);
#endif
            }
            else if (errno == EINVAL) {
                PUSH_ERR("lzf_filter", H5E_CALLBACK, "Invalid data for LZF decompression");
                goto failed;
            }
            else {
                PUSH_ERR("lzf_filter", H5E_CALLBACK, "Unknown LZF decompression error");
                goto failed;
            }
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
