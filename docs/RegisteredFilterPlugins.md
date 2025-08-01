# Registered HDF5 Filter Plugins

This document holds the official information about registered HDF5 filter plugins. It is part of the [HDF5 Filter Plugins](https://github.com/HDFGroup/hdf5_plugins) repository which conveniently hosts the code, as well as build and packaging instructions for a selection of popular HDF5 filter plugins.

Information about HDF5 filters and creating their plugins can be found in the HDF5 [documentation](https://support.hdfgroup.org/documentation/hdf5/latest/_h5_p_l__u_g.html#sec_filter_plugins). How filters fit in the library data I/O operations is described in the [Data Pipeline](https://support.hdfgroup.org/documentation/hdf5/latest/_h5_d__u_g.html#subsubsec_dataset_transfer_pipe) documentation.

## How to Register HDF5 Filter Plugin

Any member of the HDF5 community can register a plugin for their or a third-party filter. To register a filter plugin, please contact The HDF Group's [Helpdesk](mailto:help@hdfgroup.org) with as much of the following information as possible:

* Maintainer's contact information. Minimum an email address, preferably additional information like personal website, GitHub or social network handles. More ways to contact the responsible maintainer is better.
* Filter plugin's respository.
* Description of the new plugin including the specifics of the filter parameters (`cd_nelmts` and `cd_values[]`) supported by the plugin.
* Links to any relevant documentation, including the licensing information.

Upon receiving a request with the above information, HDF Group will register the new plugin by assigning it a filter plugin _identifier_. The current policy for assigning an identifier is explained below:

| Plugin Identifier Values | Description |
|--------------------------|-------------|
| `0`–`255` | These values are reserved for plugins predefined and registered by the HDF5 library and of use to the general public. |
| `256`–`511` | The values in this range are intended for testing only and can be temporarily used by any organization. No attempts are made to resolve numbering conflicts, as all definitions are temporary. |
| `512`–`32,767` | The values within this range are managed by The HDF Group and intended for filter plugins nominally requested, developed, or maintained by third parties. |
| `32,768`–`65,535` | The values in this range are designated for internal company use or application testing when assessing a feature. The HDF Group does not track or document the use of filter plugins within this range. |

## List of Filter Plugins Registered with The HDF Group

| Plugin Identifier | For Filter | Short Description|
|--------|----------------|---------------------|
|`257`     |<a href="#hzip">hzip</a> |hzip compression used in Silo|
|`258`     |<a href="#fpzip">fpzip</a> |Duplicate of plugin `32014`|
|`305`     |<a href="#lzo">LZO</a> |LZO lossless compression used by PyTables|
|`307`     |<a href="#bzip2">BZIP2</a>   |BZIP2 lossless compression used by PyTables|
|`32000`   |<a href="#lzf">LZF</a> |LZF lossless compression used by the h5py package|
|`32001`   |<a href="#blosc">BLOSC</a>   |Blosc lossless compression used by PyTables|
|`32002`   |<a href="#mafisc">MAFISC</a>  |Modified LZMA compression filter, MAFISC (Multidimensional Adaptive Filtering Improved Scientific data Compression)|
|`32003`   |<a href="#snappy">Snappy</a>  |Snappy lossless compression|
|`32004`   |<a href="#lz4">LZ4</a> |LZ4 fast lossless compression algorithm|
|`32005`   |<a href="#apax">APAX</a>    |Samplify’s APAX Numerical Encoding Technology|
|`32006`   |<a href="#cbf">CBF</a> |All imgCIF/CBF compressions and decompressions, including Canonical, Packed, Packed Version 2, Byte Offset and Nibble Offset|
|`32007`   |<a href="#jpeg-xr">JPEG-XR</a> |Enables images to be compressed/decompressed with JPEG-XR compression|
|`32008`   |<a href="#bitshuffle">bitshuffle</a>  |Extreme version of shuffle filter that shuffles data at bit level instead of byte level|
|`32009`   |<a href="#spdp">SPDP</a>    |SPDP fast lossless compression algorithm for single- and double-precision floating-point data|
|`32010`   |<a href="#lpc-rice">LPC-Rice</a>    |LPC-Rice multi-threaded lossless compression|
|`32011`   |<a href="#ccsds-123">CCSDS-123</a>   |ESA CCSDS-123 multi-threaded compression filter|
|`32012`   |<a href="#jpeg-ls">JPEG-LS</a> |CharLS JPEG-LS multi-threaded compression filter|
|`32013`   |<a href="#zfp">zfp</a> |Lossy & lossless compression of floating point and integer datasets to meet rate, accuracy, and/or precision targets.|
|`32014`   |<a href="#fpzip">fpzip</a>   |Fast and Efficient Lossy or Lossless Compressor for Floating-Point Data|
|`32015`   |<a href="#zstandard">Zstandard</a>   |Real-time compression algorithm with wide range of compression / speed trade-off and fast decoder|
|`32016`   |<a href="#b3d">B³D</a> |GPU based image compression method developed for light-microscopy applications|
|`32017`   |<a href="#sz">SZ</a>  |An error-bounded lossy compressor for scientific floating-point data|
|`32018`   |<a href="#fcidecomp">FCIDECOMP</a>   |EUMETSAT CharLS compression filter for use with netCDF|
|`32019`   |<a href="#jpeg">JPEG</a>    |Jpeg compression filter|
|`32020`   |<a href="#vbz">VBZ</a> |Compression filter for raw dna signal data used by Oxford Nanopore|
|`32021`   |<a href="#fapec">FAPEC</a>  | Versatile and efficient data compressor supporting many kinds of data and using an outlier-resilient entropy coder|
|`32022`   |<a href="#bitgroom">BitGroom</a>    |The BitGroom quantization algorithm|
|`32023`   |<a href="#gbr">Granular BitRound (GBR)</a> |The GBR quantization algorithm is a significant improvement to the BitGroom filter|
|`32024`   |<a href="#sz3">SZ3</a> |A modular error-bounded lossy compression framework for scientific datasets|
|`32025`   |<a href="#delta-rice">Delta-Rice</a>  |Lossless compression algorithm optimized for digitized analog signals based on delta encoding and rice coding|
|`32026`   |<a href="#blosc2">BLOSC2</a>   |The recent new-generation version of the Blosc compression library|
|`32027`   |<a href="#flac">FLAC</a>    |FLAC audio compression filter in HDF5|
|`32028`   |<a href="#sperr">SPERR</a>    |SPERR is a lossy scientific (floating-point) data compressor that produces one of the best rate-distortion curves|
|`32029`   |<a href="#trpx">TERSE/PROLIX</a>    |A lossless and fast compression of the diffraction data|
|`32030`   |<a href="#ffmpeg">FFMPEG</a>    |A lossy compression filter based on ffmpeg video library|

> [!NOTE]
> Please contact the maintainer of a filter plugin for help with the plugin or its filter in the HDF5 library.

## Information about Registered Filter Plugins

### hzip <a name="hzip"></a>

#### Filter Description

Hzip is a compression algorithm for lossless compression of structured and unstructured meshes composed of cells with hypercube topology. Hzip was written by Peter Lindstrom at LLNL, and is based on the algorithm described in the paper: Peter Lindstrom and Martin Isenburg, "Lossless Compression of Hexahedral Meshes," IEEE Data Compression Conference, March 2008, 192-201. https://doi.org/10.1109/DCC.2008.12. More information at https://computing.llnl.gov/projects/hzip.

#### Plugin ID `257` Information

See above.

##### Contact

Mark Miller<br/>
Email: miller86 at llnl dot gov

---

### LZO <a name="lzo"></a>

#### Filter Description
LZO is a portable lossless data compression library written in ANSI C. Reliable and thoroughly tested. High adoption - each second, terabytes of data are compressed by LZO. There have been no bugs since the first release back in 1996. Offers pretty fast compression and *extremely* fast decompression. Includes slower compression levels achieving a quite competitive compression ratio while still decompressing at this very high speed. Distributed under the terms of the GNU General Public License (GPL v2+). Commercial licenses are available on request. Military-grade stability and robustness.

http://www.oberhumer.com/opensource/lzo/

#### Plugin ID `305` Information

Number of `cd_values[]` parameters is up to 3 (`cd_nelmts` can be `1`, `2`, or `3`).

| `cd_values[]` | Description |
|---|---|
| `[0]` | Complevel, currently no effect. |
| `[1]` | Table `VERSION` attribute. |
| `[2]` | An enum identifying one of: `Table`, `Array`, `EArray`, `VLArray`, `CArray`. |

`h5repack` filter parameters for the `--filter` option: `<objects list>:UD=305,0,3,<cd_values[0]>,<cd_values[1]>,<cd_values[2]>`.

Documentation: http://www.pytables.org.

##### Contact
Francesc Alted<br/>
Email: faltet at pytables dot org

---

### BZIP2 <a name="bzip2"></a>

#### Filter Description
bzip2 is a freely available, patent-free, high-quality data compressor. It typically compresses files to within 10% to 15% of the best available techniques (the PPM family of statistical compressors), whilst being around twice as fast at compression and six times faster at decompression. More information: http://www.bzip.org.

#### Plugin ID `307` Information

Number of `cd_values[]` parameters is 1 (`cd_nelmts = 1`).

| `cd_values[]` | Description |
|---|---|
| `[0]` | Compression block size in units of 100 kilobytes with range `1`-`9`. |

`h5repack` filter parameters for the `--filter` option: `<objects list>:UD=307,0,1,<cd_values[0]>`.

Documentation: http://www.pytables.org.

##### Contact
Francesc Alted<br/>
Email: faltet at pytables dot org

---

### LZF <a name="lzf"></a>

#### Filter Description
The LZF filter is an alternative DEFLATE-style compressor for HDF5 datasets, using the free LZF library by Marc Alexander Lehmann. Its main benefit over the built-in HDF5 DEFLATE filter is speed; in memory-to-memory operation as part of the filter pipeline, it typically compresses 3x-5x faster than DEFLATE, and decompresses 2x faster, while maintaining 50% to 90% of the DEFLATE compression ratio.

LZF can be used to compress any data type, and requires no compile-time or run-time configuration. The filter is written in C and can be included directly in C or C++ applications; it has no external dependencies. The license is 3-clause BSD (virtually unrestricted, including commercial applications).

The LZF library homepage: http://home.schmorp.de/marc/liblzf.html

#### Plugin ID `32000` Information

Number of `cd_values[]` parameters is 3 (`cd_nelmts = 3`).

| `cd_values[]` | Description |
|---|---|
| `[0]` | Plugin revision number, starting from 1. |
| `[1]` | LZF compression filter version number. |
| `[2]` | Pre-computed chunk size in bytes. |

`h5repack` example for the `--filter` option: `<objects list>:32000,1,3,<cd_values[0]>,<cd_values[1]>,<cd_values[2]>`.

The LZF filter plugin was developed as part of the [h5py](http://h5py.org) project, which implements a general-purpose interface to HDF5 from Python. More information, downloads, and benchmarks, are available at http://h5py.org/lzf/.

##### Contact

Andrew Collette<br/>
http://h5py.org

---

### Blosc <a name="blosc"></a>

#### Filter Description
Blosc is a high-performance compressor optimized for binary data. It has been designed to compress data very fast, at the expense of achieving lesser compression ratios than, say, zlib+shuffle. It is mainly meant to not introduce a significant delay when dealing with data stored in high-performance I/O systems (like large RAID cabinets or even the OS filesystem memory cache). It uses advanced cache-efficient techniques to reduce activity on the memory bus as much as possible. It also leverages SIMD (SSE2) and multi-threading capabilities present in modern multicore processors to accelerate the compression/decompression process to a maximum.

http://blosc.org/<br/>
http://www.pytables.org

#### Plugin ID `32001` Information

Number of `cd_values[]` parameters is up to 7. First four parameters are reserved.

| `cd_values[]` | Description |
|---|---|
| `[0]` | Plugin revision number. (reserved) |
| `[1]` | Blosc format version. (reserved) |
| `[2]` | Datatype size. (reserved) |
| `[3]` | Pre-computed chunk size in bytes. (reserved) |
| `[4]` | `0` (no compression) through `9` (maximum compression). (optional) |
| `[5]` | `0` (no shuffle), `1` (byte-wise shuffle), `2` (bit-wise shuffle). (optional) |
| `[6]` | `0` (BLOSCLZ), `1` (LZ4), `2` (LZ4HC), `3` (SNAPPY), `4` (ZLIB), `5` (ZSTD). (optional) |

`h5repack` example for the `--filter` option: `<objects list>:UD=32001,0,7,<cd_values[0]>,<cd_values[1]>,<cd_values[2]>,<cd_values[3]>,[<cd_values[4]>,<cd_values[5]>,<cd_values[6]>,<cd_values[7]>]`.

Common `h5repack` `--filter` parameters:

|  Filter |  `--filter` option |
|----|-----|
| default | `<objects list>:UD=32001,0,0` |
| BLOSCLZ | `<objects list>:UD=32001,0,7,0,0,0,0,5,1,0` |
| LZ4 | `<objects list>:UD=32001,0,7,0,0,0,0,5,1,1` |
| LZ4HC | `<objects list>:UD=32001,0,7,0,0,0,0,5,1,2` |
| SNAPPY | `<objects list>:UD=32001,0,7,0,0,0,0,5,1,3` |
| ZLIB |  `<objects list>:UD=32001,0,7,0,0,0,0,5,1,4` |
| ZSTD | `<objects list>:UD=32001,0,7,0,0,0,0,5,1,5` |

##### Contact

Francesc Alted<br/>
Email: faltet at pytables dot org

---

### MAFISC <a name="mafisc"></a>

#### Filter Description
This compressing filter exploits the multidimensionality and smoothness characterizing many scientific data sets. It adaptively applies some filters to preprocess the data and uses lzma as the actual compression step. It significantly outperforms pure lzma compression on most datasets.

The software is currently under a rather unrestrictive two-clause BSD-style license: http://wr.informatik.uni-hamburg.de/research/projects/icomex/mafisc.

#### Plugin ID `32002` Information

Number of `cd_values[]` parameters is variable. The first is required and the rest must be in the specific order.

| `cd_values[]` | Description |
|---|---|
| `[0]` | Version with value `0`. (required) |
| `[1]` | `datasetId` (reserved) |
| `[2]` | `dataTypeSize` in bytes. (reserved) |
| `[3]` | `isFloat`: `1` if the datatype is a floating-point type. (reserved) |
| `[4]` | Byte order. Values same as `H5T_order_t` [struct](https://support.hdfgroup.org/documentation/hdf5/latest/_h5_tpublic_8h.html#a2a6a8eb856a0829fecaac60f803c9fd0). (reserved) |
| `[5]` | Chunk rank (number of chunk's dimensions). (reserved) |
| `[6]` and following | Chunk dimension sizes, as many as the chunk rank. (reserved) |

`h5repack` example for the `--filter` option: `<objects list>:UD=32002,0,7,...`.

##### Contact

Nathanael Huebbe<br/>
Email: nathanael.huebbe at informatik dot uni-hamburg dot de

---

### Snappy <a name="snappy"></a>

#### Filter Description
Snappy-CUDA is a compression/decompression library that leverages GPU processing power to compress/decompress data. The Snappy compression algorithm does not aim for maximum compression or compatibility with any other compression library; instead, it aims for very high speeds and reasonable compression. For instance, compared to the fastest mode of zlib, the reference implementation of Snappy on the CPU is an order of magnitude faster for most inputs, but the resulting compressed files are anywhere from 20% to 100% bigger.

https://github.com/google/snappy

#### Plugin ID `32003` Information

https://github.com/lucasvr/snappy-cuda

##### Contact

Lucas C. Villa Real<br/>
Email: lucasvr at gmail dot com

---

### LZ4 <a name="lz4"></a>

#### Filter Description
LZ4 is a very fast lossless compression algorithm, providing compression speed at 300 MB/s per core, scalable with multi-cores CPU. It also features an extremely fast decoder, with speeds up and beyond 1GB/s per core, typically reaching RAM speed limits on multi-core systems.

The filter's source code is available from https://github.com/lz4/lz4.

#### Plugin ID `32004` Information

Number of `cd_values[]` parameters is one (`cd_nelmts = 1`).

| `cd_values[]` | Description |
|---|---|
| `[0]` | Block size in bytes at most 2,113,929,216 bytes. Default is 1 GiB (1,073,741,824 bytes). (optional) |

 The description of the chunk binary format produced by this plugin is at [LZ4_HDF5_format.md](https://github.com/HDFGroup/hdf5_plugins/blob/master/LZ4/LZ4_HDF5_format.md).

`h5repack` example for the `--filter` option: `<objects list>:UD=32004,0,0`.

https://github.com/HDFGroup/hdf5_plugins/tree/master/LZ4/src

##### Contact

Michael Rissi (Dectris Ltd.)<br/>
Email: michael dot rissi at dectris dot com

---

### APAX <a name="apax"></a>

#### Plugin ID `32005` Information

It appears to be no longer available.

---

### CBF <a name="cbf"></a>

#### Filter Description
All imgCIF/CBF compressions and decompressions, including Canonical, Packed, Packed Version 2, Byte Offset and Nibble Offset. License Information: GPL and LGPL.

#### Plugin ID `32006` Information

None.

##### Contact

Herbert J. Bernstein<br/>
Email: yayahjb at gmail dot com

---

### JPEG-XR <a name="jpeg-xr"></a>

#### Filter Description
Filter that allows HDF5 image datasets to be compressed or decompressed using the JPEG-XR compression method.

#### Plugin ID `32007` Information

None.

##### Contact

Marvin Albert<br/>
Email: marvin dot albert at gmail dot com

---

### Bitshuffle <a name="bitshuffle"></a>

#### Filter Description
Bitshuffle is an algorithm that rearranges typed, binary data for improving compression, and is algorithmically similar to HDF5's Shuffle filter except it operates at the bit level instead of the byte level. This does not in itself compress the data, only rearranges it for more efficient compression. The actual compression is performed by one of the LZF, LZ4, or ZSTD libraries.

Arranging a typed data array in to a matrix with the elements as the rows and the bits within the elements as the columns, Bitshuffle "transposes" the matrix, such that all the least-significant-bits are in a row, etc. This transpose is performed within blocks of data roughly 8 kB long to fit comfortably within CPU's L1 cache as well as be well matched window of the LZF compression library.

https://github.com/kiyo-masui/bitshuffle

#### Plugin ID `32008` Information

Number of `cd_values[]` parameters is up to 11 but first 3 are reserved (`cd_nelmts` values: `3`, `4`,...`10`).

| `cd_values[]` | Description |
|---|---|
| `[0]` | Bitshuffle major version (reserved) |
| `[1]` | Bitshuffle minor version (reserved) |
| `[2]` | Datatype size in bytes (reserved) |
| `[3]` | Block size in dataset elements. It needs to be divisible by eight. If `0` the default is a value that fits in 8 KiB. |
| `[4]` | `0` (no compression), `2` (`BSHUF_H5_COMPRESS_LZ4`), `3` (`BSHUF_H5_COMPRESS_ZSTD`) |
| `[5]` | Compression level if `BSHUF_H5_COMPRESS_ZSTD`. See <a href="#zstandard">Zstandard</a>. |

`h5repack` example for the `--filter` option: `<objects list>:UD=32025,0,2,<cd_values[0]>,<cd_values[1]>`.

Repository: https://github.com/HDFGroup/hdf5_plugins/tree/master/BSHUF/src

##### Contact

Kiyoshi Masui<br/>
Email: kiyo at physics dot ubc dot ca

---

### SPDP <a name="spdp"></a>

#### Filter Description
SPDP is a fast, lossless, unified compression/decompression algorithm designed for both 32-bit single-precision (float) and 64-bit double-precision (double) floating-point data. It also works on other data.

https://userweb.cs.txstate.edu/~burtscher/research/SPDPcompressor/

#### Plugin ID `32009` Information

See https://userweb.cs.txstate.edu/~burtscher/research/SPDP/H5Zspdp.c.

##### Contact

Martin Burtscher<br/>
Email: burtscher at txstate dot edu

---

### LPC-Rice <a name="lpc-rice"></a>

#### Filter Description
LPC-Rice is a fast lossless compression codec that employs Linear Predictive Coding together with Rice coding. It supports multi-threading and SSE2 vector instructions, enabling it to exceed compression and decompression speeds of 1 GB/s.

#### Plugin ID `32010` Information

https://sourceforge.net/projects/lpcrice/

##### Contact

Frans van den Bergh<br/>
Email: fvdbergh at csir dot co dot za

Derick Swanepoel<br/>
Email: dswanepoel at gmail dot com

---

### CCSDS-123 <a name="ccsds-123"></a>

#### Filter Description
CCSDS-123 is a multi-threaded HDF5 compression filter using the ESA CCSDS-123 implementation.

#### Plugin ID `32011` Information

https://sourceforge.net/projects/ccsds123-hdf-filter/

##### Contact

Frans van den Bergh<br/>
Email: fvdbergh at csir dot co dot za

Derick Swanepoel<br/>
Email: dswanepoel at gmail dot com

---

### JPEG-LS <a name="jpeg-ls"></a>

#### Filter Description
JPEG-LS is a multi-threaded HDF5 compression filter using the CharLS JPEG-LS implementation.

#### Plugin ID `32012` Information

https://sourceforge.net/projects/jpegls-hdf-filter/

##### Contact

Frans van den Bergh<br/>
Email: fvdbergh at csir dot co dot za

Derick Swanepoel<br/>
Email: dswanepoel at gmail dot com

---

### zfp <a name="zfp"></a>

#### Filter Description
zfp is a BSD licensed open source C++ library for compressed floating-point arrays that support very high throughput read and write random access. zfp was designed to achieve high compression ratios and, therefore, uses lossy but optionally error-bounded compression. Although bit-for-bit lossless compression is not always possible, zfp is usually accurate to within machine epsilon in near-lossless mode, and is often orders of magnitude more accurate and faster than other lossy compressors.

This plugin supports all 4 modes of the ZFP compression library: rate, accuracy, precision and expert. It supports 1, 2 and 3 dimensional datasets of single and double precision integer and floating point data. It can be applied to HDF5 datasets of more than 3 dimensions as long as no more than 3 dimensions of the chunking are of size greater than 1. For more information see: http://computing.llnl.gov/projects/floating-point-compression/.

#### Plugin ID `32013` Information

Number of `cd_values[]` parameters is six  (`cd_nelmts = 6`). The filter will transform (modify) their values after execution. The first parameter determines the function of the other five.

| `cd_values[]` | Description |
|---|---|
| `[0]` | Mode: `1` (rate), `2` (precision), `3` (accuracy), `4` (expert). |

| `cd_values[]` | `cd_values[0] = 1` | `cd_values[0] = 2` | `cd_values[0] = 3` | `cd_values[0] = 4` |
|---|---|---|---|---|
| `[1]` | `0` | `0` | `0` | `0` |
| `[2]` | `rateA` | `prec` | `accA` | `minbits` |
| `[3]` | `rateB` | _unused_ | `accB` | `maxbits` |
| `[4]` | `0` | `0` | `0` | `maxprec` |
| `[5]` | `0` | `0` | `0` | `minexp` |

When `cd_values[0] = 3`, expected floating-point accuracy parameter values:

| Floating-point accuracy | `accA` | `accB` |
|---|---|---|
| 1       | 0 | 1072693248 |
| 0.1     | 2576980378 |  1069128089 |
| 0.01    | 1202590843 |  1065646817 |
| 0.001   | 3539053052 |  1062232653 |
| 0.0001  | 3944497965 |  1058682594 |
| 1e-8    | 3794832442 |  1044740494 |
| 1e-11   | 3782829205 |  1034288511 |

`h5repack` examples for the `--filter` option:

* Default: `<objects list>:UD=32013,1,0,032008,0,0`
* Accuracy 0.001 mode: `<objects list>:UD=32013,0,6,3,0,3539053052,1062232653,0,0`

 Repositories:

* Upstream: https://github.com/LLNL/H5Z-ZFP
* Latest release: https://github.com/HDFGroup/hdf5_plugins/tree/master/ZFP

##### Contact

Mark Miller<br/>
Email: miller86 at llnl dot gov

Peter Lindstrom<br/>
Email: pl at llnl dot gov

---

### fpzip <a name="fpzip"></a>

#### Filter Description
fpzip is a library for lossless or lossy compression of 2D or 3D floating-point scalar fields. Although written in C++, fpzip has a C interface. fpzip was developed by Peter Lindstrom at LLNL.

http://computing.llnl.gov/projects/floating-point-compression/

#### Plugin ID `32014` Information

This plugin in the past used ID `258` and appeared in the Silo headers, but may not have been used in actual HDF5 files.

##### Contact

Peter Lindstrom<br/>
Email: pl at llnl dot gov

---

### Zstandard <a name="zstandard"></a>

#### Filter Description
Zstandard is a real-time compression algorithm that provides high compression
ratios. It offers a very wide range of compression / speed trade-offs, while
being backed by a very fast decoder.  The Zstandard library supports regular
compression levels from 1 up to `ZSTD_maxCLevel()`, which is currently 22.
Levels greater than 20 should be used with caution as they require more memory. The
library also offers negative compression levels, which extend the range of speed
vs. ratio preferences. The lower the level, the faster the speed at the cost of
compression. The Zstandard library is provided as open source software under a BSD license: https://github.com/facebook/zstd.

#### Plugin ID `32015` Information

Number of `cd_values[]` parameters is up to one (`cd_nelmts = 0` or `1`).

| `cd_values[]` | Description |
|---|---|
| `[0]` | Optional compression level, default is `3`. |

Repository: https://github.com/HDFGroup/hdf5_plugins/tree/master/ZSTD

##### Contact

Mark Kittisopikul<br/>
Email: kittisopikulm at janelia dot hhmi dot org
Github: [@mkitti](https://github.com/mkitti/)

Nathan Zimmberberg
Email: nzimmerberg at gmail dot com<br/>
Github: [@nhz2](https://github.com/nhz2)

---

### B³D <a name="b3d"></a>

#### Filter Description
B³D is a fast (~1 GB/s), GPU based image compression method, developed for light-microscopy applications. Alongside lossless compression, it offers a noise dependent lossy compression mode, where the loss can be tuned as a proportion of the inherent image noise (accounting for photon shot noise and camera read noise). It not only allows for fast compression during image, but can achieve compression ratios up 100.

#### Plugin ID `32016` Information

http://www.biorxiv.org/content/early/2017/07/21/164624

---

### SZ <a name="sz"></a>

Filter ID:

#### Filter Description
SZ is a fast and efficient error-bounded lossy compressor for floating-point data. It was developed for scientific applications producing large-scale HPC data sets. SZ supports C, Fortran, and Java and has been tested on Linux and Mac OS X.

For more information, see: https://collab.cels.anl.gov/display/ESR/SZ
GitHub: https://github.com/disheng222/SZ
License: https://www.mcs.anl.gov/~shdi/download/sz-download.html

#### Plugin ID `32017` Information

https://github.com/szcompressor/SZ/tree/master/hdf5-filter/H5Z-SZ

##### Contact

Sheng Di<br/>
Email: sdi1 at anl dot gov

Franck Cappello<br/>
Email: cappello at mcs dot anl dot gov

---

### FCIDECOMP <a name="fcidecomp"></a>

#### Filter Description
FCIDECOMP is a third-party compression filter used at EUMETSAT to compress netCDF-4 files. It is a codec implementing JPEG-LS using CharLS for satellite imagery.

#### Plugin ID `32018` Information

All software and documentation can be found at this link:
ftp://ftp.eumetsat.int/pub/OPS/out/test-data/Test-data-for-External-Users/MTG_FCI_L1c_Compressed-Datasets_and_Decompression-Plugin_April2017/Decompression_Plugin/

##### Contact

Dr. Daniel Lee<br/>
Email: daniel dot lee at eumetsat dot int

---

### JPEG <a name="jpeg"></a>

#### Filter Description
This is a lossy compression filter. It provides a user-specified "quality factor" to control the trade-off of size versus accuracy. Libjpeg library is available as a package for most Linux distributions, and the source code is available from https://www.ijg.org/.

#### Plugin ID `32019` Information

Only 8-bit unsigned data arrays are supported. Arrays must be either: 2D monochromatic [NumColumns, NumRows] or  3D RGB [3, NumColumns, NumRows].
Chunking must be set to the size of one entire image so the filter is called once for each image.

Number of `cd_values[]` parameters is 4 (`cd_nelmts = 4`).

| `cd_values[]` | Description |
|---|---|
| `[0]` | quality factor (1-100) |
| `[1]` | numColumns |
| `[2]` | numRows |
| `[3]` | `0` (Mono) or `1` (RGB) |

`h5repack` example for the `--filter` option: `<objects list>:UD=32019,0,4,<cd_values[0]>,<cd_values[1]>,<cd_values[2]>,<cd_values[3]>`.

Repository: https://github.com/HDFGroup/hdf5_plugins/tree/master/JPEG.

##### Contact

Mark Rivers<br/>
Email: rivers at cars dot uchicago dot edu

---

### VBZ <a name="vbz"></a>

#### Filter Description
Oxford Nanopore uses this filter specifically to compress raw DNA signal data (signed integer). To achieve this, it uses both:

streamvbyte (https://github.com/lemire/streamvbyte)

zstd  (https://github.com/facebook/zstd)

#### Plugin ID `32020` Information

https://github.com/nanoporetech/vbz_compression/tree/master/vbz_plugin

##### Contact

George Pimm

---

### FAPEC <a name="fapec"></a>

#### Filter Description

FAPEC is a versatile and efficient data compressor, initially designed for satellite payloads but later extended for ground applications. It relies on an outlier-resilient entropy coding core with similar ratios and speeds than CCSDS 121.0 (adaptive Rice).

FAPEC has a large variety of pre-processing stages and options: images (greyscale, colour, hyperspectral); time series or waveforms (including interleaving, e.g. for multidimensional or interleaved time series or tabular data); floating point (single+double precision); text (including LZW compression and our faster FAPECLZ); tabulated text (CSV); genomics (FastQ); geophysics (Kongsberg's water column datagrams); etc.

Most stages support samples of 8 to 24 bits (big/little endian, signed/unsigned), and lossless/lossy options. It can be extended with new, tailored pre-processing stages. It includes encryption options (AES-256 based on OpenSSL, and our own XXTEA implementation).

The FAPEC library and CLI run on Linux, Windows and Mac. The HDF5 user must request and install the library separately, allowing upgrades without requiring changes in HDF5 code.


https://www.dapcom.es/fapec/
https://www.dapcom.es/get-fapec/
https://www.dapcom.es/resources/FAPEC_EndUserLicenseAgreement.pdf

#### Plugin ID `32021` Information

None.

##### Contact

Jordi Portell i de Mora (DAPCOM Data Services S.L.)<br/>
Email: fapec at dapcom dot es

---

### BitGroom <a name="bitgroom"></a>

#### Filter Description

The BitGroom quantization algorithm is documented in:

Zender, C. S. (2016), Bit Grooming: Statistically accurate precision-preserving quantization with compression, evaluated in the netCDF Operators (NCO, v4.4.8+), Geosci. Model Dev., 9, 3199-3211, doi:10.5194/gmd-9-3199-2016.

#### Plugin ID `32022` Information

The filter is documented and maintained in the Community Codec Repository (https://github.com/ccr/ccr).

##### Contact

Charlie Zender  (University of California, Irvine)

---

### Granular BitRound (GBR) <a name="gbr"></a>

#### Filter Description

The GBG quantization algorithm is a significant improvement to the BitGroom filter documented in:

Zender, C. S. (2016), Bit Grooming: Statistically accurate precision-preserving quantization with compression, evaluated in the netCDF Operators (NCO, v4.4.8+), Geosci. Model Dev., 9, 3199-3211, doi:10.5194/gmd-9-3199-2016.

#### Plugin ID `32023` Information

This filter is documented, implemented, and maintained in the Community Codec Repository (https://github.com/ccr/ccr).

##### Contact

Charlie Zender  (University of California, Irvine)

---

### SZ3 <a name="sz3"></a>

#### Filter Description

SZ3 is a modular error-bounded lossy compression framework for scientific datasets. It allows users to customize their own compression pipeline to adapt to diverse datasets and user requirements. Compared with <a href="#sz">SZ2</a>, SZ3 has integrated a more effective prediction, such that its compression qualities/ratios are much higher than those of SZ2 in most cases.

#### Plugin ID `32024` Information

This filter is documented, implemented, and maintained at: https://github.com/szcompressor/SZ3/tree/master/tools/H5Z-SZ3.

License: https://github.com/szcompressor/SZ/blob/master/copyright-and-BSD-license.txt

##### Contact

Sheng Di<br/>
Email: sdi1 at anl dot gov

Franck Cappello<br/>
Email: cappello at mcs dot anl dot gov

---

### Delta-Rice <a name="delta-rice"></a>

#### Filter Description

Lossless compression algorithm based on Delta encoding and Rice coding optimized for digitized analog signals as signed 16-bit integers. Any other precision data is cast to 16-bit.

#### Plugin ID `32025` Information

Number of `cd_values[]` parameters is up to 3 (`cd_nelmts` values: `0`, `1`, `2`, `3`). Default values are used when no parameters.

| `cd_values[]` | Description |
|---|---|
| `[0]` | Tuning parameter. Default: `8`. |
| `[1]` | Waveform length. Default: `-1`. |
| `[2]` | Filter length. Default: `2`. This parameter determines the number of additional `cd_values[]` but they are not specified in the current plugin implementation. |

`h5repack` example for the `--filter` option: `<objects list>:UD=32025,0,2,<cd_values[0]>,<cd_values[1]>`.

This filter is documented, implemented, and maintained at: https://gitlab.com/dgma224/deltarice.

##### Contact

David Mathews<br/>
Email: david dot mathews dot 1994 at gmail dot com

---

### Blosc2 <a name="blosc2"></a>

#### Filter Description

Blosc2 is a high-performance compressor optimized for binary data (e.g., floating-point numbers, integers and booleans). It has been designed to transmit data to the processor cache faster than the traditional, non-compressed, direct memory fetch approach via a memcpy() OS call. Blosc's main goal is not just to reduce the size of large datasets on disk or in memory but also to accelerate memory-bound computations.

C-Blosc2 is the new major version of C-Blosc, and tries hard to be backward compatible with both the C-Blosc1 API and its in-memory format.

Blosc project: https://www.blosc.org<br/>
C-Blosc2 docs: https://www.blosc.org/c-blosc2/c-blosc2.html<br/>
License: https://github.com/Blosc/c-blosc2/blob/main/LICENSE.txt

#### Plugin ID `32026` Information

Number of `cd_values[]` parameters is up to 7. First four parameters are reserved.

| `cd_values[]` | Description |
|---|---|
| `[0]` | Plugin revision number. (reserved) |
| `[1]` | Unused. (reserved) |
| `[2]` | Datatype size in bytes. (reserved) |
| `[3]` | Chunk size in bytes. (reserved) |
| `[4]` | `0` (no compression) through `9` (maximum compression). (optional) |
| `[5]` | `0` (no shuffle), `1` (byte-wise shuffle), `2` (bit-wise shuffle). (optional) |
| `[6]` | `0` (BLOSCLZ, default), `1` (LZ4), `2` (LZ4HC), `4` (ZLIB), `5` (ZSTD). (optional) |
| `[7]` | Chunk rank. (optional) |
| `[8]` and beyond | If the chunk rank is specified, the rest of the `cd_values[]` are chunk dimensions. |

Repository: https://github.com/HDFGroup/hdf5_plugins/tree/master/BLOSC2.

##### Contact

Francesc Alted<br/>
Email: faltet at gmail dot org (BDFL for the Blosc project)

---

### FLAC <a name="flac"></a>

Filter ID: `32027`

#### Filter Description

FLAC is an audio compression filter in HDF5. (Our ultimate goal is to use it via h5py in the hdf5plugin library: https://github.com/silx-kit/hdf5plugin). The FLAC filter is open source: https://github.com/xiph/flac and libFLAC has BSD-like license: https://github.com/xiph/flac/blob/master/CONTRIBUTING.md.

#### Plugin ID `32027` Information

None.

##### Contact

Laurie Stephey<br/>
Email: lastephey at lbl dot gov

---

### SPERR <a name="sperr"></a>

#### Filter Description

SPERR is a wavelet-based lossy compressor for 2D, 3D, and 4D floating-point scientific data. It achieves one of the best compression ratios given a user-prescribed error tolerance (i.e., maximum point-wise error). SPERR also supports two distinctive decoding modes, namely "flexible-rate decoding" and "multi-resolution decoding," that facilitate data analysis with various constraints. More details are available on SPERR Github repository: https://github.com/NCAR/SPERR.

#### Plugin ID `32028` Information

Number of `cd_values[]` parameters is up to 4 (`cd_nelmts <= 4`).

| `cd_values[]` | Description |
|---|---|
| `[0]` | Compression mode, quality. (mandatory) |
| `[1]` | Missing value mode: `0` (no missing value), `1` (any `NAN` is a missing value), `2` (any value where `abs(value) >= 1e35` is a missing value), `3` (use a single 32-bit float as the missing value; **not implemented**), `4` (use a single 64-bit double as the missing value; **not implemented**). (optional) |
| `[2]` | 32-bit float to use as missing value when `cd_values[1] = 3`. (optional) |
| `[3]` | 64-bit float to use as missing value when `cd_values[1] = 4`.. (optional) |

Repository : https://github.com/NCAR/H5Z-SPERR

##### Contact
Samuel Li<br/>
Email: shaomeng at ucar dot edu

---

### TERSE/PROLIX (TRPX) <a name="trpx"></a>

Filter ID:

#### Filter Description

A new compression algorithm (https://www.ncbi.nlm.nih.gov/pmc/articles/PMC10626653/), which is specifically tailored for the lossless and fast compression of the diffraction data.

GitHub repo of the algorithm: https://github.com/Senikm/trpx

#### Plugin ID `32029` Information

None.

##### Contact
Jan Pieter Abrahams<br/>
Email: jp.abrahams at unibas dot ch

Senik Matinyan<br/>
Email: senik.matinyan at unibas dot ch

---

### FFMPEG <a name="ffmpeg"></a>

#### Filter Description

A lossy compression filter based on ffmpeg video library.

#### Plugin ID `32030` Information

https://github.com/Cai-Lab-at-University-of-Michigan/ffmpeg_HDF5_filter

License: Under MIT License

##### Contact
Cai Lab at University of Michigan: https://www.cai-lab.org
