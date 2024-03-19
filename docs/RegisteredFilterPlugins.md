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
|305 |LZO |LZO lossless compression used by PyTables|
|307 |BZIP2   |BZIP2 lossless compression used by PyTables|
|32000   |LZF |LZF lossless compression used by H5Py project|
|32001   |BLOSC   |Blosc lossless compression used by PyTables|
|32002   |MAFISC  |Modified LZMA compression filter, MAFISC (Multidimensional Adaptive Filtering Improved Scientific data Compression)|
|32003   |Snappy  |Snappy lossless compression|
|32004   |LZ4 |LZ4 fast lossless compression algorithm|
|32005   |APAX    |Samplify’s APAX Numerical Encoding Technology|
|32006   |CBF |All imgCIF/CBF compressions and decompressions, including Canonical, Packed, Packed Version 2, Byte Offset and Nibble Offset|
|32007   |JPEG-XR |Enables images to be compressed/decompressed with JPEG-XR compression|
|32008   |bitshuffle  |Extreme version of shuffle filter that shuffles data at bit level instead of byte level|
|32009   |SPDP    |SPDP fast lossless compression algorithm for single- and double-precision floating-point data|
|32010   |LPC-Rice    |LPC-Rice multi-threaded lossless compression|
|32011   |CCSDS-123   |ESA CCSDS-123 multi-threaded compression filter|
|32012   |JPEG-LS |CharLS JPEG-LS multi-threaded compression filter|
|32013   |zfp |Lossy & lossless compression of floating point and integer datasets to meet rate, accuracy, and/or precision targets.|
|32014   |fpzip   |Fast and Efficient Lossy or Lossless Compressor for Floating-Point Data|
|32015   |Zstandard   |Real-time compression algorithm with wide range of compression / speed trade-off and fast decoder|
|32016   |B³D |GPU based image compression method developed for light-microscopy applications|
|32017   |SZ  |An error-bounded lossy compressor for scientific floating-point data|
|32018   |FCIDECOMP   |EUMETSAT CharLS compression filter for use with netCDF|
|32019   |JPEG    |Jpeg compression filter|
|32020   |VBZ |Compression filter for raw dna signal data used by Oxford Nanopore|
|32021   |FAPEC  | Versatile and efficient data compressor supporting many kinds of data and using an outlier-resilient entropy coder|
|32022   |BitGroom    |The BitGroom quantization algorithm|
|32023   |Granular |BitRound (GBR) The GBG quantization algorithm is a significant improvement to the BitGroom filter|
|32024   |SZ3 |A modular error-bounded lossy compression framework for scientific datasets|
|32025   |Delta-Rice  |Lossless compression algorithm optimized for digitized analog signals based on delta encoding and rice coding|
|32026   |BLOSC   |The recent new-generation version of the Blosc compression library|
|32027   |FLAC    |FLAC audio compression filter in HDF5|
