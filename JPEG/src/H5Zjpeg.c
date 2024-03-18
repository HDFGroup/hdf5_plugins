/*
 * JPEG HDF5 filter
 *
 * Author: Mark Rivers <rivers@cars.uchicago.edu>
 * Created: 2019
 *
 *
 */

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "H5PLextern.h"

#include "jpeglib.h"

/*
 * <setjmp.h> is used for the optional error recovery mechanism shown in
 * the second part of the example.
 */

#include <setjmp.h>

static size_t H5Z_filter_jpeg(unsigned int flags, size_t cd_nelmts,
                     const unsigned int cd_values[], size_t nbytes,
                     size_t *buf_size, void **buf);
static htri_t H5Z_jpeg_can_apply(hid_t dcpl_id, hid_t type_id, hid_t space_id);

#define H5Z_FILTER_JPEG 32019

#define PUSH_ERR(func, minor, str) H5Epush(H5E_DEFAULT, __FILE__, func, __LINE__, H5E_ERR_CLS, H5E_PLINE, minor, str)

const H5Z_class2_t H5Z_JPEG[1] = {{
    H5Z_CLASS_T_VERS,                 /* H5Z_class_t version */
    (H5Z_filter_t)H5Z_FILTER_JPEG,    /* Filter id number             */
#ifdef FILTER_DECODE_ONLY
    0,                                /* encoder_present flag (false is not available) */
#else
    1,                                /* encoder_present flag (set to true) */
#endif
    1,                                /* decoder_present flag (set to true) */
    "HDF5 jpeg filter; see https://portal.hdfgroup.org/documentation/hdf5-docs/registered_filter_plugins.html",
                                      /* Filter name for debugging    */
#if 0
    (H5Z_can_apply_func_t)(H5Z_jpeg_can_apply),      /* The "can apply" callback     */
#else
    NULL,                       /* The "can apply" callback     */
#endif
    NULL,                             /* The "set local" callback     */
    (H5Z_func_t)H5Z_filter_jpeg,      /* The actual filter function   */
}};

H5PL_type_t   H5PLget_plugin_type(void) {return H5PL_TYPE_FILTER;}
const void *H5PLget_plugin_info(void) {return H5Z_JPEG;}

/*
 * ERROR HANDLING:
 *
 * The JPEG library's standard error handler (jerror.c) is divided into
 * several "methods" which you can override individually.  This lets you
 * adjust the behavior without duplicating a lot of code, which you might
 * have to update with each future release.
 *
 * We override the "error_exit" method so that
 * control is returned to the library's caller when a fatal error occurs,
 * rather than calling exit() as the standard error_exit method does.
 *
 * We use C's setjmp/longjmp facility to return control.  This means that the
 * routine which calls the JPEG library must first execute a setjmp() call to
 * establish the return point.  We want the replacement error_exit to do a
 * longjmp().  But we need to make the setjmp buffer accessible to the
 * error_exit routine.  To do this, we make a private extension of the
 * standard JPEG error handler object.
 *
 * Here's the extended error handler struct:
 */

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;    /* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

static size_t H5Z_filter_jpeg(unsigned int flags, size_t cd_nelmts,
                     const unsigned int cd_values[], size_t nbytes,
                     size_t *buf_size, void **buf)
{
    /* We use our private extension JPEG error handler.
    * Note that this struct must live as long as the main JPEG parameter
    * struct, to avoid dangling-pointer problems.
    */
    struct my_error_mgr jpegErr;
    size_t buf_size_out = 0;
    void *out_buf = NULL;
    unsigned char *outData = NULL;
    char msg[80];

    if (flags & H5Z_FLAG_REVERSE) {
        /* We're decompressing */
        /* This struct contains the JPEG decompression parameters and pointers to
         * working space (which is allocated as needed by the JPEG library).
         */
        struct jpeg_decompress_struct jpegInfo;
        unsigned long compressedSize;
        size_t elem_size = 1;
        int err = 0;
        unsigned char *output = NULL;

#ifdef JPEG_DEBUG
        fprintf(stderr, "   decompressing nbytes: %ld\n", nbytes);
#endif
        /* allocate and initialize JPEG decompression object */

        /* We set up the normal JPEG error routines, then override error_exit. */
        jpegInfo.err = jpeg_std_error(&jpegErr.pub);
        jpegErr.pub.error_exit = my_error_exit;
        /* Establish the setjmp return context for my_error_exit to use. */
        if (setjmp(jpegErr.setjmp_buffer)) {
          /* If we get here, the JPEG code has signaled an error.
           * We need to clean up the JPEG object and return.
           */
          jpeg_destroy_decompress(&jpegInfo);
          PUSH_ERR("jpeg_h5_filter", H5E_CALLBACK, "Could not initialize JPEG decompression object.");
          goto failed;
        }
        /* Now we can initialize the JPEG decompression object. */
        jpeg_create_decompress(&jpegInfo);

        compressedSize = (unsigned long) nbytes;

        /* specify data source (eg, a file) */
        jpeg_mem_src(&jpegInfo, *buf,  compressedSize);

        /* read file parameters with jpeg_read_header() */
        jpeg_read_header(&jpegInfo, TRUE);

        /* Start decompressor */
        jpeg_start_decompress(&jpegInfo);

        buf_size_out = elem_size * jpegInfo.output_height * jpegInfo.output_width * jpegInfo.output_components;
        out_buf = malloc(buf_size_out);
        if (out_buf == NULL) {
            PUSH_ERR("jpeg_h5_filter", H5E_CALLBACK, "Could not allocate output buffer.");
            goto failed;
        }
        output = out_buf;

        while (jpegInfo.output_scanline < jpegInfo.output_height) {
            unsigned char *row_pointer[1] = { output };

            err = jpeg_read_scanlines(&jpegInfo, row_pointer, 1);
            if (err != 1) {
                sprintf(msg, "Error decoding JPE with error code %d.", err);
                PUSH_ERR("jpeg_h5_filter", H5E_CALLBACK, msg);
                goto failed;
            }
            output += jpegInfo.output_width*jpegInfo.output_components;
        }

        /* Finish decompression */
        jpeg_finish_decompress(&jpegInfo);

        /* Release JPEG decompression object */

        /* This is an important step since it will release a good deal of memory. */
        jpeg_destroy_decompress(&jpegInfo);
    }
    else {
        /* We're compressing */
        /*
         * cd_values[0] = quality factor
         * cd_values[1] = nx
         * cd_values[2] = ny
         * cd_values[3] = 0=Mono, 1=RGB
         */
        struct jpeg_compress_struct jpegInfo;
        int qualityFactor;
        int colorMode;
        unsigned char *pData = NULL;
        unsigned long outSize = 0;
        JSAMPROW row_pointer[1];
        int sizeX;
        int sizeY;
        int nwrite = 0;
        size_t expectedSize;

#ifdef JPEG_DEBUG
        fprintf(stderr, "    compressing nbytes: %ld\n", nbytes);
#endif
        if (cd_nelmts != 4) {
            PUSH_ERR("jpeg_h5_filter", H5E_CALLBACK, "cd_nelmts must be 4");
            goto failed;
        }
        qualityFactor = cd_values[0];
        if (qualityFactor < 1)
          qualityFactor = 1;
        if (qualityFactor > 100)
          qualityFactor = 100;
        sizeX         = cd_values[1];
        sizeY         = cd_values[2];
        colorMode     = cd_values[3];

        /* Sanity check to make sure we have been passed a complete image */
        expectedSize = sizeX * sizeY;
        if (colorMode == 1) expectedSize *= 3;
        if (expectedSize != nbytes) {
            sprintf(msg, "nbytes=%ld does not match image size=%ld.", nbytes, expectedSize);
            PUSH_ERR("jpeg_h5_filter", H5E_CALLBACK, msg);
            goto failed;
        }

        /* allocate and initialize JPEG compression object */

        /* We have to set up the error handler first, in case the initialization
         * step fails.  (Unlikely, but it could happen if you are out of memory.)
         * This routine fills in the contents of struct jerr, and returns jerr's
         * address which we place into the link field in cinfo.
         */
        /* Initialize the JPEG compression object. */

        /* We set up the normal JPEG error routines, then override error_exit. */
        jpegInfo.err = jpeg_std_error(&jpegErr.pub);
        jpegErr.pub.error_exit = my_error_exit;
        /* Establish the setjmp return context for my_error_exit to use. */
        if (setjmp(jpegErr.setjmp_buffer)) {
          /* If we get here, the JPEG code has signaled an error.
           * We need to clean up the JPEG object and return.
           */
          jpeg_destroy_compress(&jpegInfo);
          PUSH_ERR("jpeg_h5_filter", H5E_CALLBACK, "Could not initialize JPEG compression object.");
          goto failed;
        }
        jpegInfo.err = jpeg_std_error(&jpegErr);
        /* Now we can initialize the JPEG compression object. */
        jpeg_create_compress(&jpegInfo);

        /* set parameters for compression */

        /* First we supply a description of the input image.
         * Four fields of the cinfo struct must be filled in:
         */
        jpegInfo.image_width  = (JDIMENSION)sizeX;
        jpegInfo.image_height = (JDIMENSION)sizeY;

        if (colorMode == 0) {
            jpegInfo.input_components = 1;
            jpegInfo.in_color_space = JCS_GRAYSCALE;
        }
        else {
            jpegInfo.input_components = 3;
            jpegInfo.in_color_space = JCS_RGB;
        }

        /* Now use the library's routine to set default compression parameters.
         * (You must set at least cinfo.in_color_space before calling this,
         * since the defaults depend on the source color space.)
         */
        jpeg_set_defaults(&jpegInfo);
        /* Now you can set any non-default parameters you wish to.
         * Here we just illustrate the use of quality (quantization table) scaling:
         */
        jpeg_set_quality(&jpegInfo, qualityFactor, TRUE);
        jpeg_mem_dest(&jpegInfo,
                      (unsigned char **)&outData,
                      (unsigned long*) &outSize);

        /* Start compressor */

        /* TRUE ensures that we will write a complete interchange-JPEG file.
         * Pass TRUE unless you are very sure of what you're doing.
         */
        jpeg_start_compress(&jpegInfo, TRUE);

        pData = (unsigned char *)*buf;

        while ((int)jpegInfo.next_scanline < sizeY) {
            row_pointer[0] = pData;
            nwrite = jpeg_write_scanlines(&jpegInfo, row_pointer, 1);
            pData += sizeX * jpegInfo.input_components;
            if (nwrite != 1) {
                PUSH_ERR("jpeg_h5_filter", H5E_CALLBACK, "Error writing JPEG data");
                goto failed;
            }
        }

        /* Finish compression */
        jpeg_finish_compress(&jpegInfo);
        buf_size_out = outSize;

        out_buf = malloc(buf_size_out);
        if (!out_buf) {
            PUSH_ERR("jpeg_h5_filter", H5E_CALLBACK, "Failed to allocate JPEG array");
            goto failed;
        }
        memcpy(out_buf, outData, buf_size_out);

        /* release JPEG compression object */

        /* This is an important step since it will release a good deal of memory. */
        jpeg_destroy_compress(&jpegInfo);

        if (outData)
            free(outData);
    }
    free(*buf);
    *buf = out_buf;
    *buf_size = buf_size_out;
    return buf_size_out;

failed:
    if (outData)
        free(outData);
    if (out_buf)
        free(out_buf);
    return 0;
}

static htri_t
H5Z_jpeg_can_apply(hid_t dcpl_id, hid_t type_id, hid_t chunk_space_id)
{
    size_t dsize;
    htri_t retval = 0;
    H5T_class_t dclass;

    fprintf(stderr, "    H5Z_jpeg_can_apply\n");
    /* get datatype class, size and space dimensions */
    if (H5T_NO_CLASS == (dclass = H5Tget_class(type_id))) {
        PUSH_ERR("jpeg_h5_filter", H5E_BADTYPE, "bad datatype class");
        goto done;
    }

    if (0 == (dsize = H5Tget_size(type_id))) {
        PUSH_ERR("jpeg_h5_filter", H5E_BADTYPE, "bad datatype size");
        goto done;
    }

    /* confirm JPEG library can handle this data */
    if (!(dclass == H5T_INTEGER)) {
        PUSH_ERR("jpeg_h5_filter", H5E_BADTYPE,"requires datatype class of H5T_INTEGER");
        goto done;
    }

    if (!(dsize == 1)) {
        PUSH_ERR("jpeg_h5_filter", H5E_BADTYPE, "requires datatype size of 1");
        goto done;
    }

    retval = 1;

done:

    return retval;
}
