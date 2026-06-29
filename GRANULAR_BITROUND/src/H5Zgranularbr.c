/* Copyright (C) 2021--present Charlie Zender */

/*
 * This file is an example of an HDF5 filter plugin.
 * The plugin can be used with the HDF5 library vesrion 1.8.11+ to read and write
 * HDF5 datasets quantized with Granular BitRound.
 */

#include "granularbr_config.h"
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

/* Standard header files */
#ifdef HAVE_FEATURES_H
/* Needed to define __USE_BSD that recent GCC compilers use in math.h to define M_LN2... */
#include <features.h> /* __USE_BSD */
#endif
#ifdef HAVE_MATH_H
/* Needed for M_LN10, M_LN2 in ccr_gbr() */
#include <math.h> /* sin cos cos sin 3.14159 */
#endif

/* 3rd party vendors */
#include "H5PLextern.h" /* HDF5 Plugin Library: H5PLget_plugin_type(), H5PLget_plugin_info() */

#define H5Z_FILTER_GRANULARBR 32023 /**< Filter ID registered with the HDF Group */

#define PUSH_ERR(func, minor, str)                                                                           \
    H5Epush(H5E_DEFAULT, __FILE__, func, __LINE__, H5E_ERR_CLS, H5E_PLINE, minor, str)

/* Tokens and typedefs */
#define CCR_FLT_NAME    "Granular BitRound filter" /**< Filter name in vernacular for HDF5 messages */
#define CCR_FLT_NSD_DFL 3 /**< Default number of significant digits for quantization */
#define CCR_FLT_PRM_NBR                                                                                      \
    5 /**< Number of parameters sent to filter (in cd_params array). NB: keep identical with                 \
         ccr.h:GRANULARBR_FLT_PRM_NBR */
#define CCR_FLT_PRM_PSN_NSD 0 /**< Ordinal position of NSD in parameter list (cd_params array) */
#define CCR_FLT_PRM_PSN_DATUM_SIZE                                                                           \
    1 /**< Ordinal position of datum_size in parameter list (cd_params array) */
#define CCR_FLT_PRM_PSN_HAS_MSS_VAL                                                                          \
    2 /**< Ordinal position of missing value flag in parameter list (cd_params array) */
#define CCR_FLT_PRM_PSN_MSS_VAL                                                                              \
    3 /**< Ordinal position of missing value in parameter list (cd_params array)                             \
           NB: HDF5 dataset fill value uses two cd_params slots so it can be single or double-precision.     \
           Single-precision values are read as first 4-bytes starting at cd_params[4]                        \
           (and cd_params[5] is ignored), while double-precision values are read as first 8-bytes starting   \
           at cd_params[4] and ending with cd_params[5]. */

/* Compatibility tokens and typedefs retain source-code compatibility between NCO and filter
 These tokens mimic netCDF/NCO code but do not rely on or interfere with either */
#ifndef NC_FLOAT
#define NC_FLOAT 5
#endif /* !NC_FLOAT */
#ifndef NC_DOUBLE
#define NC_DOUBLE 6
#endif /* !NC_DOUBLE */

/* Minimum number of explicit significant bits to preserve when zeroing/bit-masking floating point values
 Codes will preserve at least two explicit bits, IEEE significant representation contains one implicit bit
 Thus preserve a least three bits which is approximately one significant decimal digit
 Used in nco_ppc_bitmask() and nco_ppc_bitmask_scl() */
#define NCO_PPC_BIT_XPL_NBR_MIN 2

/* Pointer union for floating point and bitmask types */
typedef union { /* ptr_unn */
    float              *fp;
    double             *dp;
    unsigned int       *ui32p;
    unsigned long long *ui64p;
    void               *vp;
} ptr_unn;

static size_t H5Z_filter_granularbr(unsigned int flags, size_t cd_nelmts, const unsigned int cd_values[],
                                    size_t nbytes, size_t *buf_size, void **buf);
herr_t        ccr_set_local_granularbr(hid_t dcpl, hid_t type, hid_t space);
htri_t        ccr_can_apply_granularbr(hid_t dcpl, hid_t type, hid_t space);

/**
 * The filter table maps filter identification numbers to structs that
 * contain a pointers to the granular bitround filter function.
 */
const H5Z_class2_t H5Z_GRANULARBR[1] = {{
    H5Z_CLASS_T_VERS,                    /**< H5Z_class_t version */
    (H5Z_filter_t)H5Z_FILTER_GRANULARBR, /**< Filter ID number */
#ifdef FILTER_DECODE_ONLY
    0, /**< Encoder availability flag */
#else
    1, /**< Encoder availability flag */
#endif
    1,                                                /**< Encoder availability flag */
    CCR_FLT_NAME,                                     /**< Filter name for debugging */
    (H5Z_can_apply_func_t)(ccr_can_apply_granularbr), /**< Callback to determine if current variable meets
                                                         filter criteria */
    (H5Z_set_local_func_t)(ccr_set_local_granularbr), /**< Callback to determine and set per-variable filter
                                                         parameters */
    (H5Z_func_t)(H5Z_filter_granularbr),              /**< Function to implement filter */
}};

void ccr_gbr(const int nsd, const int type, const size_t sz, const int has_mss_val, ptr_unn mss_val,
             ptr_unn op1);

H5PL_type_t
H5PLget_plugin_type(void)
{
    return H5PL_TYPE_FILTER;
}
const void *
H5PLget_plugin_info(void)
{
    return H5Z_GRANULARBR;
}

/**
 * \brief HDF5 Granular BitRound Filter
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
 * \details H5Z_filter_granularbr() HDF5 Granular BitRound Filter
 */
size_t
H5Z_filter_granularbr(unsigned int flags, size_t cd_nelmts, const unsigned int cd_values[], size_t nbytes,
                      size_t *buf_size, void **buf)
{
    const char fnc_nm[] = "H5Z_filter_granularbr()"; /* Function name */
    char       errmsg[256];

    if (flags & H5Z_FLAG_REVERSE) {
        /* Currently supported quantization methods (Granular BitRound) store results in IEEE754 format
         These quantized buffers are full of legal IEEE754 numbers that need no "dequantization"
         In other words, the input values in buf are also the output values */
        return nbytes;
    }
    else {
        /* Set parameters needed by quantization library filter */
        int     nsd         = cd_values[CCR_FLT_PRM_PSN_NSD];
        size_t  datum_size  = cd_values[CCR_FLT_PRM_PSN_DATUM_SIZE];
        int     has_mss_val = cd_values[CCR_FLT_PRM_PSN_HAS_MSS_VAL]; /* [flg] Flag for missing values */
        ptr_unn mss_val;                                              /* [val] Value of missing value */
        ptr_unn op1;                                                  /* I/O [frc] Values to quantize */

        /* ISO C, including gcc -pedantic, forbids casting unions (like mss_val) to incompatible data-types
         * (e.g., NULL, which is void *) so, instead, initialize union member to NULL */
        mss_val.vp = NULL;

#ifdef CCR_FLT_DBG_INFO
        (void)fprintf(stderr, "INFO: %s reports datum size = %lu B, has_mss_val = %d\n", fnc_nm, datum_size,
                      has_mss_val);
#endif

        /* Quantization is only for floating-point data (data_class == H5T_FLOAT)
         Following block assumes all buf values are either 4-byte or 8-byte floating point */
        switch (datum_size) {
            /* Cast input buffer pointer to correct numeric type */
            case 4:
                /* Single-precision floating-point data */
                if (has_mss_val) {
                    mss_val.fp = (float *)(cd_values + CCR_FLT_PRM_PSN_MSS_VAL);
#ifdef CCR_FLT_DBG_INFO
                    (void)fprintf(stderr, "INFO: \"%s\" filter function %s reports missing value = %g\n",
                                  CCR_FLT_NAME, fnc_nm, *mss_val.fp);
#endif
                }
                op1.fp = (float *)(*buf);
                ccr_gbr(nsd, NC_FLOAT, nbytes / sizeof(float), has_mss_val, mss_val, op1);
                break;
            case 8:
                /* Double-precision floating-point data */
                if (has_mss_val) {
                    mss_val.dp = (double *)(cd_values + CCR_FLT_PRM_PSN_MSS_VAL);
#ifdef CCR_FLT_DBG_INFO
                    (void)fprintf(stderr, "INFO: \"%s\" filter function %s reports missing value = %g\n",
                                  CCR_FLT_NAME, fnc_nm, *mss_val.dp);
#endif
                }
                op1.dp = (double *)(*buf);
                ccr_gbr(nsd, NC_DOUBLE, nbytes / sizeof(double), has_mss_val, mss_val, op1);
                break;
            default:
                sprintf(errmsg, "\"%s\" filter function %s reports datum size = %lu B is invalid\n",
                        CCR_FLT_NAME, fnc_nm, datum_size);
                PUSH_ERR("H5Z_filter_granularbr", H5E_CALLBACK, errmsg);
                goto error;
                break;
        }

        return nbytes;
    }

    return 1;

error:
    /* Quantization filters generally allocate no memory, so just return with error code */
    return 0;
}

/**
 * \brief Callback to determine if current variable meets filter criteria
 *
 * \param[in] dcpl Dataset creation property list ID
 * \param[in] type Dataset type ID
 * \param[in] space Dataset space ID
 * \return htri_t
 *
 * \details ccr_can_apply_granularbr() Callback to determine if current variable meets filter criteria
 */
htri_t
ccr_can_apply_granularbr(hid_t dcpl, hid_t type, hid_t space)
{
    char errmsg[256];
    /* Data space must be simple, i.e., a multi-dimensional array */
    if (H5Sis_simple(space) <= 0) {
        sprintf(errmsg, "Cannot apply filter \"%s\" filter because data space is not simple.\n",
                CCR_FLT_NAME);
        PUSH_ERR("ccr_can_apply_granularbr", H5E_CALLBACK, errmsg);
        return 0;
    }

    /* Filter can be applied */
    return 1;
} /* !ccr_can_apply_granularbr() */

/**
 * \brief Callback to determine and set per-variable filter parameters
 *
 * \param[in] dcpl Dataset creation property list ID
 * \param[in] type Dataset type ID
 * \param[in] space Dataset space ID
 * \return herr_t
 *
 * \details ccr_set_local_granularbr() Callback to determine and set per-variable filter parameters
 */
herr_t
ccr_set_local_granularbr(hid_t dcpl, hid_t type, hid_t space)
{
    const char fnc_nm[] = "ccr_set_local_granularbr()"; /* [sng] Function name */

    herr_t rcd; /* [flg] Return code */

    /* Initialize filter parameters with default values */
    unsigned int ccr_flt_prm[CCR_FLT_PRM_NBR] = {CCR_FLT_NSD_DFL, 0, 0, 0, 0};

    /* Initialize output variables for call to H5Pget_filter_by_id() */
    unsigned int  flags     = 0;
    size_t        cd_nelmts = CCR_FLT_PRM_NBR;
    unsigned int *cd_values = ccr_flt_prm;
    char          errmsg[256];

    /* Retrieve parameters specified by user
     https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#FunctionIndex
     Ignore name and filter_config by setting last three arguments to 0/NULL */
    rcd = H5Pget_filter_by_id(dcpl, H5Z_FILTER_GRANULARBR, &flags, &cd_nelmts, cd_values, 0, NULL, NULL);
    if (rcd < 0) {
        sprintf(errmsg,
                "%s filter callback function %s reports H5Pget_filter_by_id() failed to get filter flags and "
                "parameters for current variable\n",
                CCR_FLT_NAME, fnc_nm);
        PUSH_ERR("ccr_set_local_granularbr", H5E_CALLBACK, errmsg);
        return 0;
    }

    /* Data class for this variable */
    H5T_class_t data_class; /* [enm] Data type class identifier (H5T_FLOAT, H5T_INT, H5T_STRING, ...) */
    data_class = H5Tget_class(type);
    if (data_class < 0) {
        sprintf(errmsg,
                "%s filter callback function %s reports H5Tget_class() returned invalid data type class "
                "identifier = %d for current variable\n",
                CCR_FLT_NAME, fnc_nm, (int)data_class);
        PUSH_ERR("ccr_set_local_granularbr", H5E_CALLBACK, errmsg);
        return 0;
    }
    else if (data_class != H5T_FLOAT) {
#ifdef CCR_FLT_DBG_INFO
        (void)fprintf(stdout,
                      "INFO: \"%s\" filter callback function %s reports H5Tget_class() returned data type "
                      "class identifier = %d != H5T_FLOAT = %d. Attempting to remove quantization filter "
                      "using H5Premove_filter()...",
                      CCR_FLT_NAME, fnc_nm, (int)data_class, H5T_FLOAT);
#endif
        rcd = H5Premove_filter(dcpl, H5Z_FILTER_GRANULARBR);
        if (rcd < 0) {
#ifdef CCR_FLT_DBG_INFO
            (void)fprintf(stdout, "failure :(\n");
#endif
            return 0;
        } /* !rcd */
#ifdef CCR_FLT_DBG_INFO
        (void)fprintf(stdout, "success!\n");
#endif
        return 1;
    }

    /* Set data class in filter parameter list
     20200921: Remove data_class from filter parameter list as it is not needed */
    // ccr_flt_prm[CCR_FLT_PRM_PSN_DATA_CLASS]=(unsigned int)data_class;
    /* Datum size for this variable */
    size_t datum_size; /* [B] Bytes per data value */
    datum_size = H5Tget_size(type);
    if (datum_size <= 0) {
        sprintf(errmsg,
                "%s filter callback function %s reports H5Tget_size() returned invalid datum size = %lu B\n",
                CCR_FLT_NAME, fnc_nm, datum_size);
        PUSH_ERR("ccr_set_local_granularbr", H5E_CALLBACK, errmsg);
        return 0;
    }
    /* Set datum size in filter parameter list */
    ccr_flt_prm[CCR_FLT_PRM_PSN_DATUM_SIZE] = (unsigned int)datum_size;

    /* Which variable is this? fxm find and add variable name to debugging info */

    /* Find, set, and pass per-variable has_mss_val and mss_val arguments
     https://support.hdfgroup.org/HDF5/doc_resource/H5Fill_Values.html */
    int has_mss_val = 0; /* [flg] Flag for missing values */

    H5D_fill_value_t status;
    rcd = H5Pfill_value_defined(dcpl, &status);
    if (rcd < 0) {
        sprintf(
            errmsg,
            "\"%s\" filter callback function %s reports H5Pfill_value_defined() returns error code = %d\n",
            CCR_FLT_NAME, fnc_nm, rcd);
        PUSH_ERR("ccr_set_local_granularbr", H5E_CALLBACK, errmsg);
        return 0;
    }

#ifdef CCR_FLT_DBG_INFO
    {
        (void)fprintf(stdout,
                      "INFO: \"%s\" filter callback function %s reports H5Pfill_value_defined() status = %d "
                      "meaning ... ",
                      CCR_FLT_NAME, fnc_nm, status);
        if (status == H5D_FILL_VALUE_UNDEFINED)
            (void)fprintf(stdout, "Fill-value is undefined\n");
        else if (status == H5D_FILL_VALUE_DEFAULT)
            (void)fprintf(stdout, "Fill-value is the library default\n");
        else if (status == H5D_FILL_VALUE_USER_DEFINED)
            (void)fprintf(stdout, "Fill-value is defined by user application\n");
    }
#endif

    ptr_unn mss_val; /* [val] Value of missing value */
    mss_val.vp = NULL;
    if (status == H5D_FILL_VALUE_USER_DEFINED && data_class == H5T_FLOAT) {
        unsigned int *ui32p; /* [ptr] Pointer to missing value */

        has_mss_val = 1;
        mss_val.vp  = (void *)malloc(datum_size);
        rcd         = H5Pget_fill_value(dcpl, type, mss_val.vp);
        if (rcd < 0) {
            sprintf(
                errmsg,
                "\"%s\" filter callback function %s reports H5Pget_fill_value() returns error code = %d\n",
                CCR_FLT_NAME, fnc_nm, rcd);
            PUSH_ERR("ccr_set_local_granularbr", H5E_CALLBACK, errmsg);
            return 0;
        }

        /* Set missing value in filter parameter list */
        if (datum_size == 4) {
#ifdef CCR_FLT_DBG_INFO
            (void)fprintf(stderr, "INFO: \"%s\" filter callback function %s reports missing value = %g\n",
                          CCR_FLT_NAME, fnc_nm, *mss_val.fp);
#endif
            ui32p                              = (unsigned int *)mss_val.fp;
            cd_values[CCR_FLT_PRM_PSN_MSS_VAL] = *ui32p;
        }
        else if (datum_size == 8) {
#ifdef CCR_FLT_DBG_INFO
            (void)fprintf(stderr, "INFO: \"%s\" filter callback function %s reports missing value = %g\n",
                          CCR_FLT_NAME, fnc_nm, *mss_val.dp);
#endif
            ui32p = (unsigned int *)mss_val.dp;
            /* Copy first four-bytes of missing value into unsigned int parameter */
            cd_values[CCR_FLT_PRM_PSN_MSS_VAL] = *ui32p;
            /* Copy second four-bytes of missing value into next unsigned int parameter */
            cd_values[CCR_FLT_PRM_PSN_MSS_VAL + 1L] = *(ui32p + 1);
        }

        /* Free fill value memory */
        if (mss_val.vp)
            free(mss_val.vp);
    }

    /* Set missing value flag in filter parameter list */
    ccr_flt_prm[CCR_FLT_PRM_PSN_HAS_MSS_VAL] = has_mss_val;

    /* Update invoked filter with generic parameters as invoked with variable-specific values */
    rcd = H5Pmodify_filter(dcpl, H5Z_FILTER_GRANULARBR, flags, CCR_FLT_PRM_NBR, cd_values);
    if (rcd < 0) {
        sprintf(errmsg,
                "\"%s\" filter callback function %s reports H5Pmodify_filter() unable to modify filter "
                "parameters\n",
                CCR_FLT_NAME, fnc_nm);
        PUSH_ERR("ccr_set_local_granularbr", H5E_CALLBACK, errmsg);
        return 0;
    }

    return 1;
}

/* Cross-platform-deterministic table for floor(log10(|val|)) + 1.
 *
 * The original encoding called log10()/floor() per value to compute
 *   dgt_nbr = floor(log10(|val|)) + 1
 * which is "the number of decimal digits in the integer part of |val|".
 * The C standard does not require log10() to be correctly rounded (only
 * approximately, typically within ~1 ULP), and feeding its result into
 * floor() can flip the result by +/-1 when the true value is near an
 * integer boundary. Different libm implementations (e.g. macOS BSD libm
 * vs Linux glibc) therefore produce different stored bytes for inputs
 * that fall near power-of-10 boundaries -- a serious reproducibility
 * issue for scientific data.
 *
 * Replacing the libm chain with a direct binary search of a power-of-10
 * table makes the computation bit-deterministic on any IEEE 754 platform
 * (the only floating-point operation involved is comparison) and removes
 * the libm log10() dependency from the encoding inner loop.
 *
 * Range: covers all double-precision normal-range powers of 10. Inputs
 * smaller than 1e-308 fall back to the table's lower bound, which is
 * well outside the range of values where Granular BitRound is sensibly
 * applied.
 *
 */
#define CCR_GBR_POW10_KMIN   (-308)
#define CCR_GBR_POW10_KMAX   (308)
#define CCR_GBR_POW10_OFFSET (-CCR_GBR_POW10_KMIN)
#define CCR_GBR_POW10_SIZE   (CCR_GBR_POW10_KMAX - CCR_GBR_POW10_KMIN + 1)

static const double ccr_gbr_pow10[CCR_GBR_POW10_SIZE] = {
    1e-308, 1e-307, 1e-306, 1e-305, 1e-304, 1e-303, 1e-302, 1e-301, 1e-300, 1e-299, 1e-298, 1e-297, 1e-296,
    1e-295, 1e-294, 1e-293, 1e-292, 1e-291, 1e-290, 1e-289, 1e-288, 1e-287, 1e-286, 1e-285, 1e-284, 1e-283,
    1e-282, 1e-281, 1e-280, 1e-279, 1e-278, 1e-277, 1e-276, 1e-275, 1e-274, 1e-273, 1e-272, 1e-271, 1e-270,
    1e-269, 1e-268, 1e-267, 1e-266, 1e-265, 1e-264, 1e-263, 1e-262, 1e-261, 1e-260, 1e-259, 1e-258, 1e-257,
    1e-256, 1e-255, 1e-254, 1e-253, 1e-252, 1e-251, 1e-250, 1e-249, 1e-248, 1e-247, 1e-246, 1e-245, 1e-244,
    1e-243, 1e-242, 1e-241, 1e-240, 1e-239, 1e-238, 1e-237, 1e-236, 1e-235, 1e-234, 1e-233, 1e-232, 1e-231,
    1e-230, 1e-229, 1e-228, 1e-227, 1e-226, 1e-225, 1e-224, 1e-223, 1e-222, 1e-221, 1e-220, 1e-219, 1e-218,
    1e-217, 1e-216, 1e-215, 1e-214, 1e-213, 1e-212, 1e-211, 1e-210, 1e-209, 1e-208, 1e-207, 1e-206, 1e-205,
    1e-204, 1e-203, 1e-202, 1e-201, 1e-200, 1e-199, 1e-198, 1e-197, 1e-196, 1e-195, 1e-194, 1e-193, 1e-192,
    1e-191, 1e-190, 1e-189, 1e-188, 1e-187, 1e-186, 1e-185, 1e-184, 1e-183, 1e-182, 1e-181, 1e-180, 1e-179,
    1e-178, 1e-177, 1e-176, 1e-175, 1e-174, 1e-173, 1e-172, 1e-171, 1e-170, 1e-169, 1e-168, 1e-167, 1e-166,
    1e-165, 1e-164, 1e-163, 1e-162, 1e-161, 1e-160, 1e-159, 1e-158, 1e-157, 1e-156, 1e-155, 1e-154, 1e-153,
    1e-152, 1e-151, 1e-150, 1e-149, 1e-148, 1e-147, 1e-146, 1e-145, 1e-144, 1e-143, 1e-142, 1e-141, 1e-140,
    1e-139, 1e-138, 1e-137, 1e-136, 1e-135, 1e-134, 1e-133, 1e-132, 1e-131, 1e-130, 1e-129, 1e-128, 1e-127,
    1e-126, 1e-125, 1e-124, 1e-123, 1e-122, 1e-121, 1e-120, 1e-119, 1e-118, 1e-117, 1e-116, 1e-115, 1e-114,
    1e-113, 1e-112, 1e-111, 1e-110, 1e-109, 1e-108, 1e-107, 1e-106, 1e-105, 1e-104, 1e-103, 1e-102, 1e-101,
    1e-100, 1e-99,  1e-98,  1e-97,  1e-96,  1e-95,  1e-94,  1e-93,  1e-92,  1e-91,  1e-90,  1e-89,  1e-88,
    1e-87,  1e-86,  1e-85,  1e-84,  1e-83,  1e-82,  1e-81,  1e-80,  1e-79,  1e-78,  1e-77,  1e-76,  1e-75,
    1e-74,  1e-73,  1e-72,  1e-71,  1e-70,  1e-69,  1e-68,  1e-67,  1e-66,  1e-65,  1e-64,  1e-63,  1e-62,
    1e-61,  1e-60,  1e-59,  1e-58,  1e-57,  1e-56,  1e-55,  1e-54,  1e-53,  1e-52,  1e-51,  1e-50,  1e-49,
    1e-48,  1e-47,  1e-46,  1e-45,  1e-44,  1e-43,  1e-42,  1e-41,  1e-40,  1e-39,  1e-38,  1e-37,  1e-36,
    1e-35,  1e-34,  1e-33,  1e-32,  1e-31,  1e-30,  1e-29,  1e-28,  1e-27,  1e-26,  1e-25,  1e-24,  1e-23,
    1e-22,  1e-21,  1e-20,  1e-19,  1e-18,  1e-17,  1e-16,  1e-15,  1e-14,  1e-13,  1e-12,  1e-11,  1e-10,
    1e-9,   1e-8,   1e-7,   1e-6,   1e-5,   1e-4,   1e-3,   1e-2,   1e-1,   1e0,    1e1,    1e2,    1e3,
    1e4,    1e5,    1e6,    1e7,    1e8,    1e9,    1e10,   1e11,   1e12,   1e13,   1e14,   1e15,   1e16,
    1e17,   1e18,   1e19,   1e20,   1e21,   1e22,   1e23,   1e24,   1e25,   1e26,   1e27,   1e28,   1e29,
    1e30,   1e31,   1e32,   1e33,   1e34,   1e35,   1e36,   1e37,   1e38,   1e39,   1e40,   1e41,   1e42,
    1e43,   1e44,   1e45,   1e46,   1e47,   1e48,   1e49,   1e50,   1e51,   1e52,   1e53,   1e54,   1e55,
    1e56,   1e57,   1e58,   1e59,   1e60,   1e61,   1e62,   1e63,   1e64,   1e65,   1e66,   1e67,   1e68,
    1e69,   1e70,   1e71,   1e72,   1e73,   1e74,   1e75,   1e76,   1e77,   1e78,   1e79,   1e80,   1e81,
    1e82,   1e83,   1e84,   1e85,   1e86,   1e87,   1e88,   1e89,   1e90,   1e91,   1e92,   1e93,   1e94,
    1e95,   1e96,   1e97,   1e98,   1e99,   1e100,  1e101,  1e102,  1e103,  1e104,  1e105,  1e106,  1e107,
    1e108,  1e109,  1e110,  1e111,  1e112,  1e113,  1e114,  1e115,  1e116,  1e117,  1e118,  1e119,  1e120,
    1e121,  1e122,  1e123,  1e124,  1e125,  1e126,  1e127,  1e128,  1e129,  1e130,  1e131,  1e132,  1e133,
    1e134,  1e135,  1e136,  1e137,  1e138,  1e139,  1e140,  1e141,  1e142,  1e143,  1e144,  1e145,  1e146,
    1e147,  1e148,  1e149,  1e150,  1e151,  1e152,  1e153,  1e154,  1e155,  1e156,  1e157,  1e158,  1e159,
    1e160,  1e161,  1e162,  1e163,  1e164,  1e165,  1e166,  1e167,  1e168,  1e169,  1e170,  1e171,  1e172,
    1e173,  1e174,  1e175,  1e176,  1e177,  1e178,  1e179,  1e180,  1e181,  1e182,  1e183,  1e184,  1e185,
    1e186,  1e187,  1e188,  1e189,  1e190,  1e191,  1e192,  1e193,  1e194,  1e195,  1e196,  1e197,  1e198,
    1e199,  1e200,  1e201,  1e202,  1e203,  1e204,  1e205,  1e206,  1e207,  1e208,  1e209,  1e210,  1e211,
    1e212,  1e213,  1e214,  1e215,  1e216,  1e217,  1e218,  1e219,  1e220,  1e221,  1e222,  1e223,  1e224,
    1e225,  1e226,  1e227,  1e228,  1e229,  1e230,  1e231,  1e232,  1e233,  1e234,  1e235,  1e236,  1e237,
    1e238,  1e239,  1e240,  1e241,  1e242,  1e243,  1e244,  1e245,  1e246,  1e247,  1e248,  1e249,  1e250,
    1e251,  1e252,  1e253,  1e254,  1e255,  1e256,  1e257,  1e258,  1e259,  1e260,  1e261,  1e262,  1e263,
    1e264,  1e265,  1e266,  1e267,  1e268,  1e269,  1e270,  1e271,  1e272,  1e273,  1e274,  1e275,  1e276,
    1e277,  1e278,  1e279,  1e280,  1e281,  1e282,  1e283,  1e284,  1e285,  1e286,  1e287,  1e288,  1e289,
    1e290,  1e291,  1e292,  1e293,  1e294,  1e295,  1e296,  1e297,  1e298,  1e299,  1e300,  1e301,  1e302,
    1e303,  1e304,  1e305,  1e306,  1e307,  1e308,
};

/* Returns floor(log10(val_fabs)) + 1, the number of decimal digits in the
 * integer part of val_fabs. Caller must ensure val_fabs > 0. Implemented as
 * a binary search of ccr_gbr_pow10[] -- only floating-point comparison is
 * used, which is bit-deterministic across IEEE 754 platforms. */
static int
ccr_gbr_dgt_nbr(double val_fabs)
{
    int lo, hi;

    /* Below table range: extremely small (subnormal-scale) inputs.
     * Return KMIN as a deterministic floor; not exact but well outside
     * the range where Granular BitRound is sensibly applied. */
    if (val_fabs < ccr_gbr_pow10[0])
        return CCR_GBR_POW10_KMIN;

    /* Binary search for largest index i such that ccr_gbr_pow10[i] <= val_fabs.
     * dgt_nbr = (i - OFFSET) + 1. */
    lo = 0;
    hi = CCR_GBR_POW10_SIZE - 1;
    while (lo < hi) {
        int mid = (lo + hi + 1) / 2;
        if (ccr_gbr_pow10[mid] <= val_fabs)
            lo = mid;
        else
            hi = mid - 1;
    }
    return lo - CCR_GBR_POW10_OFFSET + 1;
}

/**
 * \brief Granular BitRound buffer of float values
 *
 * \param[in] nsd Number of decimal significant digits to quantize to
 * \param[in] type netCDF type of operand
 * \param[in] sz Size (in elements) of buffer to quantize
 * \param[in] has_mss_val Flag for missing values
 * \param[in] mss_val Value of missing value
 * \param[in,out] op1 Values to quantize
 * \return void
 *
 * \details ccr_gbr() Granular BitRound buffer of float values
 */
void
ccr_gbr(const int nsd, const int type, const size_t sz, const int has_mss_val, ptr_unn mss_val, ptr_unn op1)
{
    const char fnc_nm[] = "ccr_gbr()"; /* [sng] Function name */

    /* Prefer constants defined in math.h, however, ...
     20201002 GCC environments can have hard time defining M_LN10/M_LN2 despite finding math.h */
#ifndef M_LN10
#define M_LN10 2.30258509299404568402 /* log_e 10 */
#endif                                /* M_LN10 */
#ifndef M_LN2
#define M_LN2 0.69314718055994530942           /* log_e 2 */
#endif                                         /* M_LN2 */
    const double bit_per_dgt = M_LN10 / M_LN2; /* 3.32 [frc] Bits per decimal digit of precision = log2(10) */
    /* dgt_per_bit (M_LN2/M_LN10) was used by the old libm-based dgt_nbr formula;
     * the table-based ccr_gbr_dgt_nbr() no longer needs it. */

    const int bit_xpl_nbr_sgn_flt =
        23; /* [nbr] Bits 0-22 of SP significands are explicit. Bit 23 is implicitly 1. */
    const int bit_xpl_nbr_sgn_dbl =
        52; /* [nbr] Bits 0-51 of DP significands are explicit. Bit 52 is implicitly 1. */

    double mnt;      /* [frc] Mantissa, 0.5 <= mnt < 1.0 */
    double mnt_fabs; /* [frc] fabs(mantissa) */
    double val;      /* [frc] Copy of input value to avoid indirection */

    double prc_bnr_xct;     /* [nbr] Binary digits of precision, exact */
    double mss_val_cmp_dbl; /* Missing value for comparison to double precision values */

    float mss_val_cmp_flt; /* Missing value for comparison to single precision values */

    int bit_xpl_nbr_sgn = -1; /* [nbr] Number of explicit bits in significand */
    int bit_xpl_nbr_zro;      /* [nbr] Number of explicit bits to zero */

    int dgt_nbr; /* [nbr] Number of digits before decimal point */
    int qnt_pwr; /* [nbr] Power of two in quantization mask: qnt_msk = 2^qnt_pwr */
    int xpn_bs2; /* [nbr] Binary exponent xpn_bs2 in val = sign(val) * 2^xpn_bs2 * mnt, 0.5 < mnt <= 1.0 */

    size_t idx;

    unsigned int           *u32_ptr;
    unsigned int            msk_f32_u32_zro;
    unsigned int            msk_f32_u32_one;
    unsigned int            msk_f32_u32_hshv;
    unsigned long long int *u64_ptr;
    unsigned long long int  msk_f64_u64_zro;
    unsigned long long int  msk_f64_u64_one;
    unsigned long long int  msk_f64_u64_hshv;
    unsigned short          prc_bnr_ceil; /* [nbr] Exact binary digits of precision rounded-up */
    unsigned short prc_bnr_xpl_rqr;       /* [nbr] Explicitly represented binary digits required to retain */
    char           errmsg[256];

    /* Disallow unreasonable quantization */
    if (nsd <= 0)
        PUSH_ERR("ccr_bgr", H5E_CALLBACK, "assert(nsd > 0) failed\n");
    if (nsd > 16)
        PUSH_ERR("ccr_bgr", H5E_CALLBACK, "assert(nsd <= 16) failed\n");

    /* Note: an early-return check that read prc_bnr_xpl_rqr was previously
     * here. It was likely a copy-paste from H5Zbitgroom.c's ccr_bgr(), where
     * prc_bnr_xpl_rqr is computed once per call from NSD before the loop. In
     * Granular BitRound, prc_bnr_xpl_rqr is computed per value inside the inner
     * loop, so at this point in execution it is uninitialized -- the check was
     * reading garbage stack memory. Removed because no meaningful "early skip"
     * condition can be derived from a per-value variable that has not been
     * computed yet. */

    switch (type) {
        case NC_FLOAT:
            /* Comparison value is the dataset's HDF5 fill value (when user-defined),
             * otherwise NaN -- which compares unequal to every finite value, so the
             * fill-value test is an inert no-op when none was set on the dataset. */
            if (has_mss_val)
                mss_val_cmp_flt = *mss_val.fp;
            else
                mss_val_cmp_flt = NAN;
            bit_xpl_nbr_sgn = bit_xpl_nbr_sgn_flt;
            u32_ptr         = op1.ui32p;
            /* Do not quantize the dataset fill value, +/- zero, NaN, or +/- Inf.
             * isfinite(val) covers !isnan(val) and additionally blocks +/- Inf,
             * which would otherwise reach the bit-shift below with an out-of-range
             * shift count (undefined behavior). netcdf-c upstream does not gate Inf;
             * we diverge here because the standalone filter's loop body computes
             * per-value bit masks that Inf inputs corrupt. */
            for (idx = 0L; idx < sz; idx++) {
                if ((val = op1.fp[idx]) != mss_val_cmp_flt && val != 0.0 && isfinite(val)) {
                    mnt      = frexp(val, &xpn_bs2); /* DGG19 p. 4102 (8) */
                    mnt_fabs = fabs(mnt);
                    /* dgt_nbr = floor(log10(|val|)) + 1, computed via deterministic
                     * table lookup. See ccr_gbr_dgt_nbr() comment block above for
                     * why the libm log10()/floor() chain was replaced. */
                    dgt_nbr = ccr_gbr_dgt_nbr(fabs(val));
                    qnt_pwr = (int)floor(bit_per_dgt * (dgt_nbr - nsd)); /* DGG19 p. 4101 (7) */
                    /* prc_bnr_xpl_rqr: floor(xpn_bs2 - log2(mnt_fabs)) is xpn_bs2
                     * for mnt_fabs in (0.5, 1.0), and xpn_bs2 + 1 when mnt_fabs is
                     * exactly 0.5 (val is an exact power of 2). This integer-only
                     * formulation is bit-deterministic across platforms; the
                     * original libm-based form is not. */
                    prc_bnr_xpl_rqr =
                        (unsigned short)abs(((mnt_fabs == 0.5) ? xpn_bs2 + 1 : xpn_bs2) - qnt_pwr);
                    prc_bnr_xpl_rqr--; /* 20211003 Reduce formula result by 1 bit: Passes all tests, improves
                                          CR by ~10% */

                    bit_xpl_nbr_zro = bit_xpl_nbr_sgn - prc_bnr_xpl_rqr;
                    msk_f32_u32_zro = 0u;               /* Zero all bits */
                    msk_f32_u32_zro = ~msk_f32_u32_zro; /* Turn all bits to ones */
                    /* Bit Shave mask for AND: Left shift zeros into bits to be rounded, leave ones in
                     * untouched bits */
                    msk_f32_u32_zro <<= bit_xpl_nbr_zro;
                    /* Bit Set   mask for OR:  Put ones into bits to be set, zeros in untouched bits */
                    msk_f32_u32_one = ~msk_f32_u32_zro;
                    msk_f32_u32_hshv =
                        msk_f32_u32_one & (msk_f32_u32_zro >> 1); /* Set one bit: the MSB of LSBs */
                    u32_ptr[idx] +=
                        msk_f32_u32_hshv; /* Add 1 to the MSB of LSBs, carry 1 to mantissa or even exponent */
                    u32_ptr[idx] &= msk_f32_u32_zro; /* Shave it */
                }
            }
            break;
        case NC_DOUBLE:
            /* Comparison value is the dataset's HDF5 fill value (when user-defined),
             * otherwise NaN -- which compares unequal to every finite value, so the
             * fill-value test is an inert no-op when none was set on the dataset. */
            if (has_mss_val)
                mss_val_cmp_dbl = *mss_val.dp;
            else
                mss_val_cmp_dbl = NAN;
            bit_xpl_nbr_sgn = bit_xpl_nbr_sgn_dbl;
            u64_ptr         = op1.ui64p;

            /* See float branch above for why isfinite(val) is used instead of !isnan(val). */
            for (idx = 0L; idx < sz; idx++) {
                if ((val = op1.dp[idx]) != mss_val_cmp_dbl && val != 0.0 && isfinite(val)) {
                    mnt      = frexp(val, &xpn_bs2); /* DGG19 p. 4102 (8) */
                    mnt_fabs = fabs(mnt);
                    /* dgt_nbr = floor(log10(|val|)) + 1, computed via deterministic
                     * table lookup. See ccr_gbr_dgt_nbr() comment block above for
                     * why the libm log10()/floor() chain was replaced. */
                    dgt_nbr = ccr_gbr_dgt_nbr(fabs(val));
                    qnt_pwr = (int)floor(bit_per_dgt * (dgt_nbr - nsd)); /* DGG19 p. 4101 (7) */
                    /* prc_bnr_xpl_rqr: floor(xpn_bs2 - log2(mnt_fabs)) is xpn_bs2
                     * for mnt_fabs in (0.5, 1.0), and xpn_bs2 + 1 when mnt_fabs is
                     * exactly 0.5 (val is an exact power of 2). This integer-only
                     * formulation is bit-deterministic across platforms; the
                     * original libm-based form is not. */
                    prc_bnr_xpl_rqr =
                        (unsigned short)abs(((mnt_fabs == 0.5) ? xpn_bs2 + 1 : xpn_bs2) - qnt_pwr);
                    prc_bnr_xpl_rqr--; /* 20211003 Reduce formula result by 1 bit: Passes all tests, improves
                                          CR by ~10% */

                    bit_xpl_nbr_zro = bit_xpl_nbr_sgn - prc_bnr_xpl_rqr;
                    msk_f64_u64_zro = 0u;               /* Zero all bits */
                    msk_f64_u64_zro = ~msk_f64_u64_zro; /* Turn all bits to ones */
                    /* Bit Shave mask for AND: Left shift zeros into bits to be rounded, leave ones in
                     * untouched bits */
                    msk_f64_u64_zro <<= bit_xpl_nbr_zro;
                    /* Bit Set   mask for OR:  Put ones into bits to be set, zeros in untouched bits */
                    msk_f64_u64_one = ~msk_f64_u64_zro;
                    msk_f64_u64_hshv =
                        msk_f64_u64_one & (msk_f64_u64_zro >> 1); /* Set one bit: the MSB of LSBs */
                    u64_ptr[idx] +=
                        msk_f64_u64_hshv; /* Add 1 to the MSB of LSBs, carry 1 to mantissa or even exponent */
                    u64_ptr[idx] &= msk_f64_u64_zro; /* Shave it */
                }                                    /* !mss_val_cmp_dbl */
            }                                        /* !idx */
            break;
        default:
            sprintf(errmsg, "%s reports datum size = %d B is invalid for %s filter\n", fnc_nm, type,
                    CCR_FLT_NAME);
            PUSH_ERR("ccr_bgr", H5E_CALLBACK, errmsg);
            break;
    }
}
