# HDF5 BitRound filter and its plugin ID 32032

## Usage

BitRound takes a single user parameter, NSB, and must be applied before a
lossless compressor (it does not change the byte count on its own):

```c
unsigned int nsb = 7;   /* keep 7 significant mantissa bits */
H5Pset_chunk(dcpl, rank, chunk);
H5Pset_filter(dcpl, 32032, H5Z_FLAG_MANDATORY, 1, &nsb);
H5Pset_deflate(dcpl, 6);   /* or zstd, etc. */
```

## cd_values layout

The filter stores 5 `cd_values`. Only slot `[0]` is user-supplied; the rest are
filled in by the filter's `set_local()` callback from the dataset itself (the
shape matches the sibling BitGroom / Granular BitRound plugins):

| Slot  | Meaning                                          | Source  |
|-------|--------------------------------------------------|---------|
| `[0]` | NSB — number of significant mantissa bits to keep | user    |
| `[1]` | datum size — 4 (float) or 8 (double)             | derived |
| `[2]` | has-fill — 1 if a user fill value is defined     | derived |
| `[3]` | raw fill-value bytes, low 32 bits (little-endian) | derived |
| `[4]` | raw fill-value bytes, high 32 bits (little-endian) | derived |

So an `h5repack` invocation that pins all five slots looks like:

```sh
h5repack -f UD=32032,0,7 -f GZIP=6 in.h5 out.h5
```

`set_local()` rewrites the trailing slots from the destination dataset, so the
five values shown by `h5dump -p` reflect the dataset, not just what was passed.

**Valid NSB range:** 1–22 for float (datum size 4), 1–51 for double (datum
size 8). NSB at or above the explicit mantissa width would round zero bits, so
the filter removes itself (no-op). On non-float datasets the filter also
removes itself. NSB is mandatory and has no default; a filter added with no
parameters, or with an out-of-range NSB (`< 1`), is rejected and fails dataset
creation.

## Special-value handling

| Value                     | Treatment                                        |
|---------------------------|--------------------------------------------------|
| `_FillValue` (if defined) | skipped — preserved bit-exact                    |
| `±0.0`                    | skipped — preserved bit-exact                    |
| `NaN`                     | skipped — preserved bit-exact                    |
| `±Inf`                    | **not** skipped — quantized like any other value |

Adding the round bit to the all-ones exponent turns `±Inf` into a NaN. This
matches netcdf-c. If you need Inf preserved, mask or replace it before writing.

## Encode-only / decode

BitRound is an encode-side (write) filter. Decode (read) is a no-op: the stored
values are already legal IEEE-754 numbers. A decode-only build
(`-DDISABLE_H5BITROUND_ENCODER=ON`) still loads and reads BitRound data.

## Byte order

The quantization operates on the in-pipeline buffer assuming the stored float
datatype is in the host's native byte order. This holds for the usual case
(native-order or little-endian float datasets on a little-endian host) and
matches the sibling BitGroom plugin and netcdf-c. Writing an opposite-endian
float dataset on a host of the other endianness would round the wrong mantissa
bits; if that case ever matters, byte-swap to native order before writing.

## References

**Algorithm reference:** Klöwer M, Razinger M, Dominguez JJ, Düben PD, Palmer
TN. "Compressing atmospheric data into its real information content."
*Nat. Comput. Sci.* **1**(11), 713–724 (2021).
[doi:10.1038/s43588-021-00156-2](https://doi.org/10.1038/s43588-021-00156-2)

**Upstream implementation:** netcdf-c (`libsrc4/nc4var.c`,
`NC_QUANTIZE_BITROUND`). This plugin's quantization core ("add half-ulp then
shave") is bit-compatible with netcdf-c's
`nc_def_var_quantize(..., NC_QUANTIZE_BITROUND, nsb)`.
