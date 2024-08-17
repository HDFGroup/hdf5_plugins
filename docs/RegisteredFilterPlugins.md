Registered Filter Plugins
-------------------------
Please be sure to see HDF5 Filter Plugins, a convenience software that packages together many of the commonly used filters that users have created and registered.

Information on Registered Filter Plugins
----------------------------------------
Members of the HDF5 user community can create and register Third-Party (compression or other) filters for use with HDF5.

To register a filter please contact The HDF Helpdesk with the following information:

* Contact information for the developer requesting a new identifier
* Short description of the new filter
* Links to any relevant information including licensing information

Here is the current policy regarding filter identifier assignment:
The filter identifier is designed to be a unique identifier for the filter. Values from `zero` through `32,767` are reserved for filters supported by The HDF Group in the HDF5 library and for filters requested and supported by the 3rd party.
Values from `32768` to `65535` are reserved for non-distributed uses (e.g., internal company usage) or for application usage when testing a feature. The HDF Group does not track or document the usage of filters with identifiers from this range.
Please contact the maintainer of a filter for help with the filter/compression support in HDF5.

List of Filters Registered with The HDF Group
---------------------------------------------
|Filter  |Identifier Name |Short Description|
|--------|----------------|---------------------|
|305     |<a href="#lzo">LZO</a> |LZO lossless compression used by PyTables|
|307     |<a href="#bzip2">BZIP2</a>   |BZIP2 lossless compression used by PyTables|
|32000   |<a href="#lzf">LZF</a> |LZF lossless compression used by H5Py project|
|32001   |<a href="#blosc">BLOSC</a>   |Blosc lossless compression used by PyTables|
|32002   |<a href="#mafisc">MAFISC</a>  |Modified LZMA compression filter, MAFISC (Multidimensional Adaptive Filtering Improved Scientific data Compression)|
|32003   |<a href="#snappy">Snappy</a>  |Snappy lossless compression|
|32004   |<a href="#lz4">LZ4</a> |LZ4 fast lossless compression algorithm|
|32005   |<a href="#apax">APAX</a>    |Samplify’s APAX Numerical Encoding Technology|
|32006   |<a href="#cbf">CBF</a> |All imgCIF/CBF compressions and decompressions, including Canonical, Packed, Packed Version 2, Byte Offset and Nibble Offset|
|32007   |<a href="#jpeg-xr">JPEG-XR</a> |Enables images to be compressed/decompressed with JPEG-XR compression|
|32008   |<a href="#bitshuffle">bitshuffle</a>  |Extreme version of shuffle filter that shuffles data at bit level instead of byte level|
|32009   |<a href="#spdp">SPDP</a>    |SPDP fast lossless compression algorithm for single- and double-precision floating-point data|
|32010   |<a href="#lpc-rice">LPC-Rice</a>    |LPC-Rice multi-threaded lossless compression|
|32011   |<a href="#ccsds-123">CCSDS-123</a>   |ESA CCSDS-123 multi-threaded compression filter|
|32012   |<a href="#jpeg-ls">JPEG-LS</a> |CharLS JPEG-LS multi-threaded compression filter|
|32013   |<a href="#zfp">zfp</a> |Lossy & lossless compression of floating point and integer datasets to meet rate, accuracy, and/or precision targets.|
|32014   |<a href="#fpzip">fpzip</a>   |Fast and Efficient Lossy or Lossless Compressor for Floating-Point Data|
|32015   |<a href="#zstandard">Zstandard</a>   |Real-time compression algorithm with wide range of compression / speed trade-off and fast decoder|
|32016   |<a href="#b³d">B³D</a> |GPU based image compression method developed for light-microscopy applications|
|32017   |<a href="#sz">SZ</a>  |An error-bounded lossy compressor for scientific floating-point data|
|32018   |<a href="#fcidecomp">FCIDECOMP</a>   |EUMETSAT CharLS compression filter for use with netCDF|
|32019   |<a href="#jpeg">JPEG</a>    |Jpeg compression filter|
|32020   |<a href="#vbz">VBZ</a> |Compression filter for raw dna signal data used by Oxford Nanopore|
|32021   |<a href="#fapec">FAPEC</a>  | Versatile and efficient data compressor supporting many kinds of data and using an outlier-resilient entropy coder|
|32022   |<a href="#bitgroom">BitGroom</a>    |The BitGroom quantization algorithm|
|32023   |<a href="#gbr">Granular</a> |BitRound (GBR) The GBG quantization algorithm is a significant improvement to the BitGroom filter|
|32024   |<a href="#sz3">SZ3</a> |A modular error-bounded lossy compression framework for scientific datasets|
|32025   |<a href="#delta-rice">Delta-Rice</a>  |Lossless compression algorithm optimized for digitized analog signals based on delta encoding and rice coding|
|32026   |<a href="#blosc2">BLOSC2</a>   |The recent new-generation version of the Blosc compression library|
|32027   |<a href="#flac">FLAC</a>    |FLAC audio compression filter in HDF5|
|32028   |<a href="#h5z-sperr">H5Z-SPERR</a>    |H5Z-SPERR is the HDF5 filter for SPERR|
|32029   |<a href="#trpx">TERSE/PROLIX</a>    |A lossless and fast compression of the diffraction data|
|32030   |<a href="#ffmpeg">FFMPEG</a>    |A lossy compression filter based on ffmpeg video library|


## <center>The Filters</center>

<h2 id="lzo">LZO Filter</h2>

#### Filter ID: 305

#### Filter Description:
LZO is a portable lossless data compression library written in ANSI C.
Reliable and thoroughly tested. High adoption - each second terrabytes of data are compressed by LZO. No bugs since the first release back in 1996.
Offers pretty fast compression and *extremely* fast decompression.
Includes slower compression levels achieving a quite competitive compression ratio while still decompressing at this very high speed.
Distributed under the terms of the GNU General Public License (GPL v2+). Commercial licenses are available on request.
Military-grade stability and robustness.

#### Filter Information:
http://www.oberhumer.com/opensource/lzo/
http://www.pytables.org

#### Contact Information:
Francesc Alted
Email: faltet at pytables dot org

##

<h2 id="bzip2">BZIP2 Filter</h2>

#### Filter ID: 307

#### Filter Description:
bzip2 is a freely available, patent free, high-quality data compressor. It typically compresses files to within 10% to 15% of the best available techniques (the PPM family of statistical compressors), whilst being around twice as fast at compression and six times faster at decompression.

#### Filter Information:
http://www.bzip.org
http://www.pytables.org

#### Contact Information:
Francesc Alted
Email: faltet at pytables dot org

##

<h2 id="lzf">LZF Filter</h2>

#### Filter ID: 32000

#### Filter Description:
The LZF filter is an alternative DEFLATE-style compressor for HDF5 datasets, using the free LZF library by Marc Alexander Lehmann. Its main benefit over the built-in HDF5 DEFLATE filter is speed; in memory-to-memory operation as part of the filter pipeline, it typically compresses 3x-5x faster than DEFLATE, and decompresses 2x faster, while maintaining 50% to 90% of the DEFLATE compression ratio.

LZF can be used to compress any data type, and requires no compile-time or run-time configuration. HDF5 versions 1.6.5 through 1.8.3 are supported. The filter is written in C and can be included directly in C or C++ applications; it has no external dependencies. The license is 3-clause BSD (virtually unrestricted, including commercial applications).

More information, downloads, and benchmarks, are available at the http://h5py.org/lzf/.

Additional Information:

The LZF filter was developed as part of the h5py project, which implements a general-purpose interface to HDF5 from Python.

#### Filter Information:

The h5py homepage: http://h5py.org

The LZF library homepage: http://home.schmorp.de/marc/liblzf.html

#### Contact Information:

Andrew Collette
Web: http://h5py.org

##

<h2 id="blosc">Blosc Filter</h2>

#### Filter ID: 32001

#### Filter Description:
Blosc is a high performance compressor optimized for binary data. It has been designed to compress data very fast, at the expense of achieving lesser compression ratios than, say, zlib+shuffle. It is mainly meant to not introduce a significant delay when dealing with data that is stored in high-performance I/O systems (like large RAID cabinets, or even the OS filesystem memory cache).

It uses advanced cache-efficient techniques to reduce activity on the memory bus as much as possible. It also leverages SIMD (SSE2) and multi-threading capabilities present in nowadays multi-core processors so as to accelerate the compression/decompression process to a maximum.

#### Filter Information:

http://blosc.org/
http://www.pytables.org

#### Contact Information:

Francesc Alted
Email: faltet at pytables dot org

##

<h2 id="mafisc">MAFISC Filter</h2>

#### Filter ID: 32002

#### Filter Description:
This compressing filter exploits the multidimensionality and smoothness characterizing many scientific data sets. It adaptively applies some filters to preprocess the data and uses lzma as the actual compression step. It significantly outperforms pure lzma compression on most datasets.

The software is currently under a rather unrestrictive two clause BSD style license.

#### Filter Information:

http://wr.informatik.uni-hamburg.de/research/projects/icomex/mafisc

#### Contact Information:

Nathanael Huebbe 
Email: nathanael.huebbe at informatik dot uni-hamburg dot de 

##

<h2 id="snappy">Snappy Filter</h2>

#### Filter ID: 32003

#### Filter Description:
Snappy-CUDA is a compression/decompression library that leverages GPU processing power to compress/decompress data. The Snappy compression algorithm does not aim for maximum compression or compatibility with any other compression library; instead, it aims for very high speeds and reasonable compression. For instance, compared to the fastest mode of zlib, the reference implementation of Snappy on the CPU is an order of magnitude faster for most inputs, but the resulting compressed files are anywhere from 20% to 100% bigger.

#### Filter Information:

https://github.com/lucasvr/snappy-cuda
https://github.com/google/snappy

#### Contact Information:

Lucas C. Villa Real
Email: lucasvr at gmail dot com

##

<h2 id="lz4">LZ4 Filter</h2>

#### Filter ID: 32004

#### Filter Description:
LZ4 is a very fast lossless compression algorithm, providing compression speed at 300 MB/s per core, scalable with multi-cores CPU. It also features an extremely fast decoder, with speeds up and beyond 1GB/s per core, typically reaching RAM speed limits on multi-core systems. For a format description of the LZ4 compression filter in HDF5, see HDF5_LZ4.pdf.

#### Filter Information:

LZ4 Algorithm:   https://github.com/nexusformat/HDF5-External-Filter-Plugins/tree/master/LZ4

LZ4 Code:

Although the LZ4 software is not supported by The HDF Group, it is included in The HDF Group SVN repository so that it can be tested regularly with HDF5. For convenience, users can obtain it from SVN with the following command:
   svn checkout https://svn.hdfgroup.org/hdf5_plugins/trunk/LZ4 LZ4

#### Contact Information:

Michael Rissi (Dectris Ltd.)
Email: michael dot rissi at dectris dot com

##

<h2 id="apax">APAX</h2>

#### Filter ID: 32005

Appears to be no longer available

##

<h2 id="cbf">CBF</h2>

#### Filter ID: 32006

#### Filter Description:
All imgCIF/CBF compressions and decompressions, including Canonical, Packed, Packed Vesrsion 2, Byte Offset and Nibble Offset. 
License Information: GPL and LGPL

#### Contact Information:

Herbert J. Bernstein
Email: yayahjb at gmail dot com

##

<h2 id="jpeg-xr">JPEG-XR</h2>

#### Filter ID: 32007

#### Filter Description:
Filter that allows HDF5 image datasets to be compressed or decompressed using the JPEG-XR compression method.

#### Filter Information:

JPEG-XR Compression Method
JPEG-XR Filter for HDF5

#### Contact Information:

Marvin Albert 
Email: marvin dot albert at gmail dot com

##

<h2 id="bitshuffle">bitshuffle</h2>

#### Filter ID: 32008

#### Filter Description:
This filter shuffles data at the bit level to improve compression. CHIME uses this filter for data acquisition.

#### Filter Information:

bitshuffle
CHIME

#### Contact Information:

Kiyoshi Masui
Email: kiyo at physics dot ubc dot ca

##

<h2 id="spdp">SPDP</h2>

#### Filter ID: 32009

#### Filter Description:
SPDP is a fast, lossless, unified compression/decompression algorithm designed for both 32-bit single-precision (float) and 64-bit double-precision (double) floating-point data. It also works on other data.

#### Filter Information:

http://cs.txstate.edu/~burtscher/research/SPDP/

#### Contact Information:

Martin Burtscher
Email: burtscher at txstate dot edu

##

<h2 id="lpc-rice">LPC-Rice</h2>

#### Filter ID: 32010

#### Filter Description:
LPC-Rice is a fast lossless compression codec that employs Linear Predictive Coding together with Rice coding. It supports multi-threading and SSE2 vector instructions, enabling it to exceed compression and decompression speeds of 1 GB/s.

#### Filter Information:

https://sourceforge.net/projects/lpcrice/

#### Contact Information:

Frans van den Bergh
Email: fvdbergh at csir dot co dot za

Derick Swanepoel
Email: dswanepoel at gmail dot com

##

<h2 id="ccsds-123">CCSDS-123</h2>

#### Filter ID: 32011

#### Filter Description:
CCSDS-123 is a multi-threaded HDF5 compression filter using the ESA CCSDS-123 implementation.

#### Filter Information:

https://sourceforge.net/projects/ccsds123-hdf-filter/

#### Contact Information:

Frans van den Bergh
Email: fvdbergh at csir dot co dot za

Derick Swanepoel
Email: dswanepoel at gmail dot com

##

<h2 id="jpeg-ls">JPEG-LS</h2>

#### Filter ID: 32012

#### Filter Description:
JPEG-LS is a multi-threaded HDF5 compression filter using the CharLS JPEG-LS implementation.

#### Filter Information:

https://sourceforge.net/projects/jpegls-hdf-filter/

#### Contact Information:

Frans van den Bergh
Email: fvdbergh at csir dot co dot za

Derick Swanepoel
Email: dswanepoel at gmail dot com

##

<h2 id="zfp">zfp</h2>

#### Filter ID: 32013

#### Filter Description:
zfp is a BSD licensed open source C++ library for compressed floating-point arrays that support very high throughput read and write random access. zfp was designed to achieve high compression ratios and therefore uses lossy but optionally error-bounded compression. Although bit-for-bit lossless compression is not always possible, zfp is usually accurate to within machine epsilon in near-lossless mode, and is often orders of magnitude more accurate and faster than other lossy compressors.

#### Filter Information:

https://github.com/LLNL/H5Z-ZFP

For more information see: http://computation.llnl.gov/projects/floating-point-compression/

#### Contact Information:

Mark Miller
Email: miller86 at llnl dot gov

Peter Lindstrom
Email: pl at llnl dot gov

##

<h2 id="fpzip">fpzip</h2>

#### Filter ID: 32014

#### Filter Description:
fpzip is a library for lossless or lossy compression of 2D or 3D floating-point scalar fields. Although written in C++, fpzip has a C interface. fpzip was developed by Peter Lindstrom at LLNL.

#### Filter Information:

For more information see: http://computation.llnl.gov/projects/floating-point-compression/

#### Contact Information:

Peter Lindstrom
Email: pl at llnl dot gov

##

<h2 id="zstandard">Zstandard</h2>

#### Filter ID: 32015

#### Filter Description:
Zstandard is a real-time compression algorithm, providing high compression ratios. It offers a very wide range of compression / speed trade-offs, while being backed by a very fast decoder. The Zstandard library is provided as open source software using a BSD license.

#### Filter Information:

https://github.com/aparamon/HDF5Plugin-Zstandard

#### Contact Information:

Andrey Paramonov
Email: paramon at acdlabs dot ru

##

<h2 id="b³d">B³D</h2>

#### Filter ID: 32016

#### Filter Description:
B³D is a fast (~1 GB/s), GPU based image compression method, developed for light-microscopy applications. Alongside lossless compression, it offers a noise dependent lossy compression mode, where the loss can be tuned as a proportion of the inherent image noise (accounting for photon shot noise and camera read noise). It not only allows for fast compression during image, but can achieve compression ratios up 100.

[Information](http://www.biorxiv.org/content/early/2017/07/21/164624)

##

<h2 id="sz">SZ</h2>

#### Filter ID: 32017

#### Filter Description:
SZ is a fast and efficient error-bounded lossy compressor for floating-point data. It was developed for scientific applications producing large-scale HPC data sets. SZ supports C, Fortran, and Java and has been tested on Linux and Mac OS X.

#### Filter Information:

[Information](https://collab.cels.anl.gov/display/ESR/SZ)
[github](https://github.com/disheng222/SZ)
[License](https://www.mcs.anl.gov/~shdi/download/sz-download.html)

#### Contact Information:

Sheng Di
Email: sdi1 at anl dot gov

Franck Cappello
Email: cappello at mcs dot anl dot gov

##

<h2 id="fcidecomp">FCIDECOMP</h2>

#### Filter ID: 32018

#### Filter Description:
FCIDECOMP is a third-party compression filter used at EUMETSAT for the compression of netCDF-4 files. It is a codec implementing JPEG-LS using CharLS used for satellite imagery.

#### Filter Information:

All software and documentation can be found at this link:

ftp://ftp.eumetsat.int/pub/OPS/out/test-data/Test-data-for-External-Users/MTG_FCI_L1c_Compressed-Datasets_and_Decompression-Plugin_April2017/Decompression_Plugin/

#### Contact Information:

Dr. Daniel Lee
Email: daniel dot lee at eumetsat dot int

##

<h2 id="jpeg">JPEG</h2>

#### Filter ID: 32019

#### Filter Description:
This is a lossy compression filter. It provides a user-specified "quality factor" to control the trade-off of size versus accuracy.

#### Filter Information:

Information
Github
License

libjpeg: This library is available as a package for most Linux distributions, and source code is available from https://www.ijg.org/.

Restrictions:

Only 8-bit unsigned data arrays are supported.
Arrays must be either:
 2-D monochromatic [NumColumns, NumRows]
 3-D RGB [3, NumColumns, NumRows]
Chunking must be set to the size of one entire image so the filter is called once for each image.
Using the JPEG filter in your application:

HDF5 only supports compression for "chunked" datasets; this just means that you need to call H5Pset_chunk to specify a chunk size. The chunking must be set to the size of a single image for the JPEG filter to work properly.

When calling H5Pset_filter for compression it must be called with cd_nelmts=4 and cd_values as follows:

    cd_values[0] = quality factor (1-100)

    cd_values[1] = numColumns

    cd_values[2] = numRows

    cd_values[3] = 0=Mono, 1=RGB

Common h5repack parameter: UD=32019,0,4,q,c,r,t

#### Contact Information:

Mark Rivers , University of Chicago (rivers at cars.uchicago.edu)

##

<h2 id="vbz">VBZ</h2>

#### Filter ID: 32020

#### Filter Description:
This filter is used by Oxford Nanopore specifically to compress raw dna signal data (signed integer). To achieve this it uses both:

streamvbyte (https://github.com/lemire/streamvbyte)

zstd  (https://github.com/facebook/zstd) 

#### Contact Information:

George Pimm

##

<h2 id="fapec">FAPEC</h2>

#### Filter ID:  32021

#### Filter Description:

FAPEC is a versatile and efficient data compressor, initially designed for satellite payloads but later extended for ground applications. It relies on an outlier-resilient entropy coding core with similar ratios and speeds than CCSDS 121.0 (adaptive Rice).

FAPEC has a large variety of pre-processing stages and options: images (greyscale, colour, hyperspectral); time series or waveforms (including interleaving, e.g. for multidimensional or interleaved time series or tabular data); floating point (single+double precision); text (including LZW compression and our faster FAPECLZ); tabulated text (CSV); genomics (FastQ); geophysics (Kongsberg's water column datagrams); etc.

Most stages support samples of 8 to 24 bits (big/little endian, signed/unsigned), and lossless/lossy options. It can be extended with new, tailored pre-processing stages. It includes encryption options (AES-256 based on OpenSSL, and our own XXTEA implementation).

The FAPEC library and CLI runs on Linux, Windows and Mac. The HDF5 user must request and install the library separately, thus allowing to upgrade it without requiring changes in your HDF5 code.

#### Filter Information:

https://www.dapcom.es/fapec/
https://www.dapcom.es/get-fapec/
https://www.dapcom.es/resources/FAPEC_EndUserLicenseAgreement.pdf

#### Contact Information:

Jordi Portell i de Mora (DAPCOM Data Services S.L.)

fapec at dapcom dot es

##

<h2 id="bitgroom">BitGroom</h2>

#### Filter ID:  32022

#### Filter Description:

The BitGroom quantization algorithm is documented in:

Zender, C. S. (2016), Bit Grooming: Statistically accurate precision-preserving quantization with compression, evaluated in the netCDF Operators (NCO, v4.4.8+), Geosci. Model Dev., 9, 3199-3211, doi:10.5194/gmd-9-3199-2016.

#### Filter Information:

The filter is documented and maintained in the Community Codec Repository (https://github.com/ccr/ccr).

#### Contact Information:

Charlie Zender  (University of California, Irvine)

##

<h2 id="gbr">Granular BitRound (GBR)</h2>

#### Filter ID:  32023

#### Filter Description:

The GBG quantization algorithm is a significant improvement the BitGroom filter documented in:

Zender, C. S. (2016), Bit Grooming: Statistically accurate precision-preserving quantization with compression, evaluated in the netCDF Operators (NCO, v4.4.8+), Geosci. Model Dev., 9, 3199-3211, doi:10.5194/gmd-9-3199-2016.

#### Filter Information:

This filter is documented, implemented, and maintained in the Community Codec Repository (https://github.com/ccr/ccr).

#### Contact Information:

Charlie Zender  (University of California, Irvine)

##

<h2 id="sz3">SZ3</h2>

#### Filter ID:  32024

#### Filter Description:

SZ3 is a modular error-bounded lossy compression framework for scientific datasets, which allows users to customize their own compression pipeline to adapt to diverse datasets and user-requirements. Compared with SZ2 (filter id: 32017), SZ3 has integrated a more effective prediction such that its compression qualities/ratios are much higher than that of SZ2 in most of cases.  

#### Filter Information:

This filter is documented, implemented, and maintained in github: https://github.com/szcompressor/SZ3.

License: https://github.com/szcompressor/SZ/blob/master/copyright-and-BSD-license.txt 

#### Contact Information:

Sheng Di
Email: sdi1 at anl dot gov

Franck Cappello
Email: cappello at mcs dot anl dot gov

##

<h2 id="delta-rice">Delta-Rice</h2>

#### Filter ID:  32025

#### Filter Description:

Lossless compression algorithm optimized for digitized analog signals based on delta encoding and rice coding.  

#### Filter Information:

This filter is documented, implemented, and maintained at: https://gitlab.com/dgma224/deltarice.

#### Contact Information:

David Mathews
Email: david dot mathews dot 1994 at gmail dot com 

##

<h2 id="blosc2">Blosc2 Filter</h2>

#### Filter ID: 32026

#### Filter Description:

Blosc is a high performance compressor optimized for binary data (i.e. floating point numbers, integers and booleans). It has been designed to transmit data to the processor cache faster than the traditional, non-compressed, direct memory fetch approach via a memcpy() OS call. Blosc main goal is not just to reduce the size of large datasets on-disk or in-memory, but also to accelerate memory-bound computations.

C-Blosc2 is the new major version of C-Blosc, and tries hard to be backward compatible with both the C-Blosc1 API and its in-memory format.

#### Filter Information:

Blosc project: https://www.blosc.org

C-Blosc2 docs: https://www.blosc.org/c-blosc2/c-blosc2.html

License: https://github.com/Blosc/c-blosc2/blob/main/LICENSE.txt

#### Contact Information:

Francesc Alted
Email: faltet at gmail dot org (BDFL for the Blosc project)

##

<h2 id="flac">FLAC Filter</h2>

#### Filter ID: 32027

#### Filter Description:

FLAC is an audio compression filter in HDF5. (Our ultimate goal is to use it via h5py in the hdf5plugin library: https://github.com/silx-kit/hdf5plugin).

#### Filter Information:

The FLAC filter is open source: https://github.com/xiph/flac

libFLAC has BSD-like license: https://github.com/xiph/flac/blob/master/CONTRIBUTING.md

#### Contact Information:

Laurie Stephey
Email: lastephey at lbl dot gov

##

<h2 id="h5z-sperr">H5Z-SPERR Filter</h2>

#### Filter ID: 32028

#### Filter Description:

SPERR is a wavelet-based lossy compressor for floating-point scientific data; it achieves one of the best compression ratios given a user-prescribed error tolerance (i.e., maximum point-wise error). SPERR also supports two distinctive decoding modes, namely "flexible-rate decoding" and "multi-resolution decoding," that facilitate data analysis with various constraints. More details are available on SPERR Github repository: https://github.com/NCAR/SPERR.

#### Filter Information:

H5Z-SPERR is the HDF5 filter for SPERR. It is also available on Github: https://github.com/NCAR/H5Z-SPERR

#### Contact Information:
Samuel Li
Email: shaomeng at ucar dot edu

##

<h2 id="trpx">TERSE/PROLIX (or TRPX) Filter</h2>

#### Filter ID: 32029

#### Filter Description:

A new compression algorithm (https://www.ncbi.nlm.nih.gov/pmc/articles/PMC10626653/), which is specifically tailored for the lossless and fast compression of the diffraction data.

#### Filter Information:

GitHub repo of the algorithm: https://github.com/Senikm/trpx

#### Contact Information:
Jan Pieter Abrahams
Email: jp.abrahams at unibas dot ch

Senik Matinyan
Email: senik.matinyan at unibas dot ch

##

<h2 id="ffmpeg">FFMPEG Filter</h2>

#### Filter ID: 32030

#### Filter Description:

A lossy compression filter based on ffmpeg video library.

#### Filter Information:

https://github.com/Cai-Lab-at-University-of-Michigan/ffmpeg_HDF5_filter

License: Under MIT License

#### Contact Information:
Cai Lab at University of Michigan: https://www.cai-lab.org

