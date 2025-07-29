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
           NB: Missing value (_FillValue) uses two cd_params slots so it can be single or double-precision.  \
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
#ifndef NC_FILL_FLOAT
#define NC_FILL_FLOAT (9.9692099683868690e+36f) /* near 15 * 2^119 */
#endif                                          /* !NC_FILL_FLOAT */
#ifndef NC_FILL_DOUBLE
#define NC_FILL_DOUBLE (9.9692099683868690e+36)
#endif /* !NC_FILL_DOUBLE */

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
    const double dgt_per_bit =
        M_LN2 / M_LN10; /* 0.301 [frc] Decimal digits per bit of precision = log10(2) */

    const int bit_xpl_nbr_sgn_flt =
        23; /* [nbr] Bits 0-22 of SP significands are explicit. Bit 23 is implicitly 1. */
    const int bit_xpl_nbr_sgn_dbl =
        52; /* [nbr] Bits 0-51 of DP significands are explicit. Bit 52 is implicitly 1. */

    double mnt;            /* [frc] Mantissa, 0.5 <= mnt < 1.0 */
    double mnt_fabs;       /* [frc] fabs(mantissa) */
    double mnt_log10_fabs; /* [frc] log10(fabs(mantissa))) */
    double val;            /* [frc] Copy of input value to avoid indirection */

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

    if (type == NC_FLOAT && prc_bnr_xpl_rqr >= bit_xpl_nbr_sgn_flt)
        return;
    if (type == NC_DOUBLE && prc_bnr_xpl_rqr >= bit_xpl_nbr_sgn_dbl)
        return;

    switch (type) {
        case NC_FLOAT:
            /* Missing value for comparison is _FillValue (if any) otherwise default NC_FILL_FLOAT/DOUBLE */
            if (has_mss_val)
                mss_val_cmp_flt = *mss_val.fp;
            else
                mss_val_cmp_flt = NC_FILL_FLOAT;
            bit_xpl_nbr_sgn = bit_xpl_nbr_sgn_flt;
            u32_ptr         = op1.ui32p;
            for (idx = 0L; idx < sz; idx++) {
                if ((val = op1.fp[idx]) != mss_val_cmp_flt && u32_ptr[idx] != 0U) {
                    mnt            = frexp(val, &xpn_bs2); /* DGG19 p. 4102 (8) */
                    mnt_fabs       = fabs(mnt);
                    mnt_log10_fabs = log10(mnt_fabs);
                    /* 20211003 Continuous determination of dgt_nbr improves CR by ~10% */
                    dgt_nbr =
                        (int)floor(xpn_bs2 * dgt_per_bit + mnt_log10_fabs) + 1;  /* DGG19 p. 4102 (8.67) */
                    qnt_pwr         = (int)floor(bit_per_dgt * (dgt_nbr - nsd)); /* DGG19 p. 4101 (7) */
                    prc_bnr_xpl_rqr = mnt_fabs == 0.0
                                          ? 0
                                          : abs((int)floor(xpn_bs2 - bit_per_dgt * mnt_log10_fabs) -
                                                qnt_pwr); /* Protect against mnt = -0.0 */
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
            /* Missing value for comparison is _FillValue (if any) otherwise default NC_FILL_FLOAT/DOUBLE */
            if (has_mss_val)
                mss_val_cmp_dbl = *mss_val.dp;
            else
                mss_val_cmp_dbl = NC_FILL_DOUBLE;
            bit_xpl_nbr_sgn = bit_xpl_nbr_sgn_dbl;
            u64_ptr         = op1.ui64p;

            for (idx = 0L; idx < sz; idx++) {
                if ((val = op1.dp[idx]) != mss_val_cmp_dbl && u64_ptr[idx] != 0U) {
                    mnt            = frexp(val, &xpn_bs2); /* DGG19 p. 4102 (8) */
                    mnt_fabs       = fabs(mnt);
                    mnt_log10_fabs = log10(mnt_fabs);
                    /* 20211003 Continuous determination of dgt_nbr improves CR by ~10% */
                    dgt_nbr =
                        (int)floor(xpn_bs2 * dgt_per_bit + mnt_log10_fabs) + 1;  /* DGG19 p. 4102 (8.67) */
                    qnt_pwr         = (int)floor(bit_per_dgt * (dgt_nbr - nsd)); /* DGG19 p. 4101 (7) */
                    prc_bnr_xpl_rqr = mnt_fabs == 0.0
                                          ? 0
                                          : abs((int)floor(xpn_bs2 - bit_per_dgt * mnt_log10_fabs) -
                                                qnt_pwr); /* Protect against mnt = -0.0 */
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
                } /* !mss_val_cmp_dbl */
            } /* !idx */
            break;
        default:
            sprintf(errmsg, "%s reports datum size = %d B is invalid for %s filter\n", fnc_nm, type,
                    CCR_FLT_NAME);
            PUSH_ERR("ccr_bgr", H5E_CALLBACK, errmsg);
            break;
    }
}
