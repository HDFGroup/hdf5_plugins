/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * H5Zbitround.c -- HDF5 BitRound quantization filter plugin.
 *
 * BitRound is a lossy pre-compression filter for IEEE-754 floating-point
 * data. It keeps the leading NSB ("number of significant bits") explicit
 * mantissa bits of every value, rounds at that bit boundary, and zeroes the
 * trailing mantissa bits. The zeroed low bits and the resulting repeated bit
 * patterns are highly compressible by a downstream lossless codec (deflate,
 * zstd, ...). BitRound does not change the byte count on its own; it is a
 * preconditioner and must be paired with a real compressor.
 *
 * Algorithm:
 *   Kloewer M, Razinger M, Dominguez JJ, Dueben PD, Palmer TN.
 *   "Compressing atmospheric data into its real information content."
 *   Nat. Comput. Sci. 1(11), 713-724 (2021).
 *   doi:10.1038/s43588-021-00156-2
 *
 * Code provenance: the quantization core is the integer round-to-nearest
 * "add half-ulp then shave" operation as implemented inline in netcdf-c
 * (libsrc4/nc4var.c, NC_QUANTIZE_BITROUND branch, PR #2232), and the HDF5
 * filter scaffolding (cd_values layout, set_local / can_apply pattern) is
 * adapted from H5Zbitgroom.c in the ccr project. Both were authored by
 * Charlie Zender. The netcdf-c repository is copyright UCAR; the ccr
 * BitGroom plugin is copyright Charlie Zender. Both are BSD-style licensed.
 */

#include "bitround_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "H5PLextern.h"

#define H5Z_FILTER_BITROUND 32032

#define BR_FLT_NAME                                                                                          \
    "BitRound filter (doi:10.1038/s43588-021-00156-2) "                                                      \
    "https://github.com/HDFGroup/hdf5_plugins/tree/master/BITROUND"

/* cd_values[] layout. Only slot [0] is user-supplied; set_local() derives the
 * rest from the dataset. Kept the same shape as the BitGroom/GranularBR
 * sibling plugins so h5repack / h5dump show a consistent parameter set. */
#define BR_FLT_PRM_NBR             5 /**< Total number of cd_values slots */
#define BR_FLT_PRM_PSN_NSB         0 /**< IN: user-supplied number of significant bits */
#define BR_FLT_PRM_PSN_DATUM_SIZE  1 /**< set_local: datum size in bytes (4 or 8) */
#define BR_FLT_PRM_PSN_HAS_MSS_VAL 2 /**< set_local: 1 if a user fill value is defined */
#define BR_FLT_PRM_PSN_MSS_VAL     3 /**< set_local: raw fill-value bytes, uses [3] (low) and [4] (high) */

#define BR_MAX_NSB_FLT 23 /**< Explicit mantissa bits in IEEE-754 single precision */
#define BR_MAX_NSB_DBL 52 /**< Explicit mantissa bits in IEEE-754 double precision */

#define PUSH_ERR(func, minor, str)                                                                           \
    H5Epush(H5E_DEFAULT, __FILE__, func, __LINE__, H5E_ERR_CLS, H5E_PLINE, minor, str)

static size_t H5Z_filter_bitround(unsigned int flags, size_t cd_nelmts, const unsigned int cd_values[],
                                  size_t nbytes, size_t *buf_size, void **buf);
static herr_t set_local_bitround(hid_t dcpl, hid_t type, hid_t space);
static htri_t can_apply_bitround(hid_t dcpl, hid_t type, hid_t space);

/**
 * The filter class maps the filter ID to the BitRound callbacks.
 */
const H5Z_class2_t H5Z_BITROUND[1] = {{
    H5Z_CLASS_T_VERS,                  /**< H5Z_class_t version */
    (H5Z_filter_t)H5Z_FILTER_BITROUND, /**< Filter ID number */
#ifdef FILTER_DECODE_ONLY
    0, /**< Encoder availability flag */
#else
    1, /**< Encoder availability flag */
#endif
    1,                                          /**< Decoder availability flag */
    BR_FLT_NAME,                                /**< Filter name for debugging */
    (H5Z_can_apply_func_t)(can_apply_bitround), /**< Callback: can the filter be applied? */
    (H5Z_set_local_func_t)(set_local_bitround), /**< Callback: set per-dataset parameters */
    (H5Z_func_t)(H5Z_filter_bitround),          /**< The filter function */
}};

H5PL_type_t
H5PLget_plugin_type(void)
{
    return H5PL_TYPE_FILTER;
}
const void *
H5PLget_plugin_info(void)
{
    return H5Z_BITROUND;
}

/* ------------------------------------------------------------------------- */
/* Little-endian fill-value packing helpers.                                 */
/*                                                                           */
/* The raw fill-value bytes are stashed little-endian in cd_values[3..4].    */
/* Encode and decode run on the same host and decode is a no-op, so this is  */
/* only for portable h5dump display and any future cross-host reader. The    */
/* pack/unpack pair is symmetric, so the encode-side fill-value comparison is */
/* correct on both little- and big-endian hosts.                             */
/* ------------------------------------------------------------------------- */
static int
br_is_big_endian(void)
{
    const uint32_t one = 1u;
    return ((const unsigned char *)&one)[0] == 0;
}

static uint32_t
br_bswap32(uint32_t v)
{
    return ((v & 0x000000FFu) << 24) | ((v & 0x0000FF00u) << 8) | ((v & 0x00FF0000u) >> 8) |
           ((v & 0xFF000000u) >> 24);
}

static uint64_t
br_bswap64(uint64_t v)
{
    return ((uint64_t)br_bswap32((uint32_t)(v & 0xFFFFFFFFu)) << 32) |
           (uint64_t)br_bswap32((uint32_t)(v >> 32));
}

static void
br_pack_f32_le(unsigned int *v, float f)
{
    uint32_t u;
    memcpy(&u, &f, sizeof(u));
    if (br_is_big_endian())
        u = br_bswap32(u);
    v[0] = (unsigned int)u;
}

static float
br_unpack_f32_le(const unsigned int *v)
{
    uint32_t u = (uint32_t)v[0];
    float    f;
    if (br_is_big_endian())
        u = br_bswap32(u);
    memcpy(&f, &u, sizeof(f));
    return f;
}

static void
br_pack_f64_le(unsigned int *v, double d)
{
    uint64_t u;
    memcpy(&u, &d, sizeof(u));
    if (br_is_big_endian())
        u = br_bswap64(u);
    v[0] = (unsigned int)(u & 0xFFFFFFFFu);
    v[1] = (unsigned int)(u >> 32);
}

static double
br_unpack_f64_le(const unsigned int *v)
{
    uint64_t u = ((uint64_t)(uint32_t)v[1] << 32) | (uint32_t)v[0];
    double   d;
    if (br_is_big_endian())
        u = br_bswap64(u);
    memcpy(&d, &u, sizeof(d));
    return d;
}

/**
 * \brief HDF5 BitRound filter function.
 *
 * Encode quantizes the buffer in place; the byte count is unchanged. Decode
 * (H5Z_FLAG_REVERSE) is a no-op: the stored values are already legal IEEE-754
 * numbers that need no dequantization.
 */
size_t
H5Z_filter_bitround(unsigned int flags, size_t cd_nelmts, const unsigned int cd_values[], size_t nbytes,
                    size_t *buf_size, void **buf)
{
    const char fnc_nm[] = "H5Z_filter_bitround()";
    char       errmsg[256];

    (void)buf_size; /* in-place quantization: size is unchanged */

    if (flags & H5Z_FLAG_REVERSE)
        return nbytes;

    /* Read only the slots we own. Validate with >=, never ==. */
    if (cd_nelmts < BR_FLT_PRM_NBR) {
        snprintf(errmsg, sizeof(errmsg), "%s reports too few filter parameters: %lu < %d", fnc_nm,
                 (unsigned long)cd_nelmts, BR_FLT_PRM_NBR);
        PUSH_ERR(fnc_nm, H5E_CALLBACK, errmsg);
        return 0;
    }

    const unsigned int nsb         = cd_values[BR_FLT_PRM_PSN_NSB];
    const size_t       datum_size  = (size_t)cd_values[BR_FLT_PRM_PSN_DATUM_SIZE];
    const int          has_mss_val = (int)cd_values[BR_FLT_PRM_PSN_HAS_MSS_VAL];

    if (datum_size != 4 && datum_size != 8) {
        snprintf(errmsg, sizeof(errmsg), "%s reports invalid datum size = %lu B", fnc_nm,
                 (unsigned long)datum_size);
        PUSH_ERR(fnc_nm, H5E_CALLBACK, errmsg);
        return 0;
    }
    if (nbytes % datum_size != 0) {
        snprintf(errmsg, sizeof(errmsg), "%s reports buffer size %lu B not a multiple of datum size %lu B",
                 fnc_nm, (unsigned long)nbytes, (unsigned long)datum_size);
        PUSH_ERR(fnc_nm, H5E_CALLBACK, errmsg);
        return 0;
    }

    const int bit_xpl = (datum_size == 4) ? BR_MAX_NSB_FLT : BR_MAX_NSB_DBL;

    /* Defensive: set_local() should have removed the filter when no bits would
     * be rounded (NSB >= explicit mantissa bits) or rejected NSB < 1. If we
     * still get such a value, do nothing rather than produce a surprising
     * result. */
    if (nsb < 1 || nsb >= (unsigned int)bit_xpl)
        return nbytes;

    const int    bit_zro = bit_xpl - (int)nsb; /* number of trailing mantissa bits to zero */
    const size_t n       = nbytes / datum_size;
    char        *base    = (char *)(*buf);
    size_t       idx;

    /* Quantize element by element. The float<->integer reinterpretation goes
     * through memcpy (lowered to plain loads/stores by the compiler) so it is
     * free of strict-aliasing UB regardless of the build's -fstrict-aliasing
     * setting. The quantization assumes the stored float byte order matches
     * the host's; see README.md. */
    if (datum_size == 4) {
        /* msk_zro: keep the high bits, zero the low bit_zro mantissa bits.
         * msk_hshv: the single 0.5-ulp bit (MSB of the bits being zeroed),
         * added before the shave to round to nearest. */
        const uint32_t msk_zro  = ~(uint32_t)0 << bit_zro;
        const uint32_t msk_hshv = (~msk_zro) & (msk_zro >> 1);
        const float    fill     = has_mss_val ? br_unpack_f32_le(&cd_values[BR_FLT_PRM_PSN_MSS_VAL]) : NAN;

        /* Skip the fill value, +/-0.0, and NaN; quantize everything else.
         * +/-Inf is intentionally NOT skipped (matches netcdf-c): adding the
         * round bit to the all-ones exponent turns Inf into NaN. See README. */
        for (idx = 0; idx < n; idx++) {
            uint32_t u;
            float    v;
            memcpy(&u, base + idx * sizeof(u), sizeof(u));
            memcpy(&v, &u, sizeof(v));
            if (v != fill && v != 0.0f && !isnan(v)) {
                u += msk_hshv;
                u &= msk_zro;
                memcpy(base + idx * sizeof(u), &u, sizeof(u));
            }
        }
    }
    else { /* datum_size == 8 */
        const uint64_t msk_zro  = ~(uint64_t)0 << bit_zro;
        const uint64_t msk_hshv = (~msk_zro) & (msk_zro >> 1);
        const double   fill     = has_mss_val ? br_unpack_f64_le(&cd_values[BR_FLT_PRM_PSN_MSS_VAL]) : NAN;

        for (idx = 0; idx < n; idx++) {
            uint64_t u;
            double   v;
            memcpy(&u, base + idx * sizeof(u), sizeof(u));
            memcpy(&v, &u, sizeof(v));
            if (v != fill && v != 0.0 && !isnan(v)) {
                u += msk_hshv;
                u &= msk_zro;
                memcpy(base + idx * sizeof(u), &u, sizeof(u));
            }
        }
    }

    return nbytes;
}

/**
 * \brief Callback to determine if the filter can be applied to a dataset.
 *
 * Requires a simple dataspace. Datatype and fill-value handling are left to
 * set_local().
 */
htri_t
can_apply_bitround(hid_t dcpl, hid_t type, hid_t space)
{
    char errmsg[256];

    (void)dcpl;
    (void)type;

    if (H5Sis_simple(space) <= 0) {
        snprintf(errmsg, sizeof(errmsg), "Cannot apply \"%s\": data space is not simple.", BR_FLT_NAME);
        PUSH_ERR("can_apply_bitround", H5E_CALLBACK, errmsg);
        return 0;
    }
    return 1;
}

/**
 * \brief Callback to derive and store the per-dataset filter parameters.
 *
 * Reads the user-supplied NSB, derives the datum size and (optional) fill
 * value from the dataset, range-checks NSB, and commits the populated
 * cd_values back to the property list. On non-float datasets, or when NSB
 * would round zero bits, the filter removes itself (no-op) and returns 1.
 * Genuine errors (unsupported datatype/size/NSB, or a failed HDF5 call) push
 * to the error stack and return a negative value so dataset creation aborts.
 */
herr_t
set_local_bitround(hid_t dcpl, hid_t type, hid_t space)
{
    const char fnc_nm[] = "set_local_bitround()";
    herr_t     rcd;
    char       errmsg[256];

    (void)space;

    /* Derived slots are zeroed (and overwritten below); cd_values[0] (NSB) is
     * mandatory and is read from the user's filter setup -- there is no default. */
    unsigned int cd_values[BR_FLT_PRM_NBR] = {0, 0, 0, 0, 0};
    unsigned int flags                     = 0;
    size_t       cd_nelmts                 = BR_FLT_PRM_NBR;

    /* Retrieve the user-supplied NSB (cd_values[0]); cd_nelmts is set to the
     * number of parameters the user actually provided. */
    rcd = H5Pget_filter_by_id(dcpl, H5Z_FILTER_BITROUND, &flags, &cd_nelmts, cd_values, 0, NULL, NULL);
    if (rcd < 0) {
        snprintf(errmsg, sizeof(errmsg), "%s reports H5Pget_filter_by_id() failed", fnc_nm);
        PUSH_ERR(fnc_nm, H5E_CALLBACK, errmsg);
        return -1;
    }

    /* Quantization is only meaningful for floating-point data. */
    H5T_class_t data_class = H5Tget_class(type);
    if (data_class < 0) {
        snprintf(errmsg, sizeof(errmsg), "%s reports H5Tget_class() returned invalid class", fnc_nm);
        PUSH_ERR(fnc_nm, H5E_CALLBACK, errmsg);
        return -1;
    }
    if (data_class != H5T_FLOAT) {
        /* Silently remove the filter on non-float datasets, like BitGroom. */
        rcd = H5Premove_filter(dcpl, H5Z_FILTER_BITROUND);
        return (rcd < 0) ? -1 : 1;
    }

    /* NSB is mandatory: it is the only user-facing parameter and there is no
     * meaningful default. Reject a filter added with no parameters. */
    if (cd_nelmts < 1) {
        snprintf(errmsg, sizeof(errmsg), "%s reports missing required NSB parameter (cd_values[0])", fnc_nm);
        PUSH_ERR(fnc_nm, H5E_CALLBACK, errmsg);
        return -1;
    }

    /* Datum size must be single or double precision. */
    size_t datum_size = H5Tget_size(type);
    if (datum_size != 4 && datum_size != 8) {
        snprintf(errmsg, sizeof(errmsg), "%s reports unsupported float datum size = %lu B (need 4 or 8)",
                 fnc_nm, (unsigned long)datum_size);
        PUSH_ERR(fnc_nm, H5E_CALLBACK, errmsg);
        return -1;
    }
    cd_values[BR_FLT_PRM_PSN_DATUM_SIZE] = (unsigned int)datum_size;

    /* Range-check NSB. */
    unsigned int nsb     = cd_values[BR_FLT_PRM_PSN_NSB];
    int          bit_xpl = (datum_size == 4) ? BR_MAX_NSB_FLT : BR_MAX_NSB_DBL;
    if (nsb < 1) {
        snprintf(errmsg, sizeof(errmsg), "%s reports invalid NSB = %u (must be >= 1)", fnc_nm, nsb);
        PUSH_ERR(fnc_nm, H5E_CALLBACK, errmsg);
        return -1;
    }
    if (nsb >= (unsigned int)bit_xpl) {
        /* NSB at or above the explicit mantissa width would round zero bits:
         * the filter would be a no-op, so remove it. */
        rcd = H5Premove_filter(dcpl, H5Z_FILTER_BITROUND);
        return (rcd < 0) ? -1 : 1;
    }

    /* Find and pack the dataset fill value, if the user defined one. */
    int              has_mss_val = 0;
    H5D_fill_value_t status;
    rcd = H5Pfill_value_defined(dcpl, &status);
    if (rcd < 0) {
        snprintf(errmsg, sizeof(errmsg), "%s reports H5Pfill_value_defined() failed", fnc_nm);
        PUSH_ERR(fnc_nm, H5E_CALLBACK, errmsg);
        return -1;
    }
    if (status == H5D_FILL_VALUE_USER_DEFINED) {
        if (datum_size == 4) {
            float fv = 0.0f;
            rcd      = H5Pget_fill_value(dcpl, type, &fv);
            if (rcd < 0) {
                snprintf(errmsg, sizeof(errmsg), "%s reports H5Pget_fill_value() failed", fnc_nm);
                PUSH_ERR(fnc_nm, H5E_CALLBACK, errmsg);
                return -1;
            }
            br_pack_f32_le(&cd_values[BR_FLT_PRM_PSN_MSS_VAL], fv);
        }
        else {
            double fv = 0.0;
            rcd       = H5Pget_fill_value(dcpl, type, &fv);
            if (rcd < 0) {
                snprintf(errmsg, sizeof(errmsg), "%s reports H5Pget_fill_value() failed", fnc_nm);
                PUSH_ERR(fnc_nm, H5E_CALLBACK, errmsg);
                return -1;
            }
            br_pack_f64_le(&cd_values[BR_FLT_PRM_PSN_MSS_VAL], fv);
        }
        has_mss_val = 1;
    }
    cd_values[BR_FLT_PRM_PSN_HAS_MSS_VAL] = (unsigned int)has_mss_val;

    /* Commit the populated parameters. */
    rcd = H5Pmodify_filter(dcpl, H5Z_FILTER_BITROUND, flags, BR_FLT_PRM_NBR, cd_values);
    if (rcd < 0) {
        snprintf(errmsg, sizeof(errmsg), "%s reports H5Pmodify_filter() failed", fnc_nm);
        PUSH_ERR(fnc_nm, H5E_CALLBACK, errmsg);
        return -1;
    }

    return 1;
}
