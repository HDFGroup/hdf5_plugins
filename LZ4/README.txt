Building this filter/example requires knowledge of the hdf5 and the lz4 
installation.

For autotools configure, you must supply these using
    --with-hdf5 and --with-lz4lib.
Example (in the build folder):
    ../configure --with-hdf5=/temp/hdf5 --with-lz4lib=/temp/lz4

For CMake, you must supply the location of the cmake configuration files 
    in environment variables.
    In addition, CMake options "H5PL_BUILD_EXAMPLES" and "H5PL_BUILD_TESTING" must
    be set "ON" in order to build the example and run the tests.
Example:
    set(ENV{HDF5_ROOT} "/temp/hdf5/")
    set(ENV{LZ4_ROOT} "/temp/lz4/")
    set(ENV{LD_LIBRARY_PATH} "/temp/lz4/lib:/temp/hdf5/lib")
    set(ADD_BUILD_OPTIONS "-DH5PL_BUILD_EXAMPLES:BOOL=ON -DH5PL_BUILD_TESTING:BOOL=ON")

    For non-cmake built hdf5 or lz4, use the location of the include/lib
    folders:
    set(ENV{HDF5_ROOT} "/temp/hdf5")
    set(ENV{LZ4_ROOT} "/temp/lz4")

Filter parameters (cd_values[])
-------------------------------

cd_values[0] -- block size (unchanged). 0 or absent selects the plugin
default (1 GiB).

cd_values[1] -- encoder selector (optional). Stored as unsigned int but
interpreted as a signed int:

      0  : LZ4_compress_default       (legacy behavior, the default)
    > 0  : LZ4HC level, clamped to [LZ4HC_CLEVEL_MIN, LZ4HC_CLEVEL_MAX]
           (currently [2, 12]); 9 is the LZ4HC default.
    < 0  : fast-encoder acceleration = -cd_values[1], lower-bounded at 1
           and clamped by liblz4 internally at LZ4_ACCELERATION_MAX
           (currently 65537).

Because the slot is unsigned on disk, a negative encoder value is stored
as its two's-complement bit pattern. For example, acceleration 8 is
written as cd_values[1] = -8 and "h5dump -p" displays the slot as the
unsigned decimal 4294967288. The plugin casts back to int when reading.

cd_values[1] only affects writes. The on-disk chunk format is unchanged:
LZ4HC, accelerated fast LZ4, and the default encoder all produce LZ4
Block payloads that the existing reader (LZ4_decompress_safe) decodes
without modification. Files written by the older plugin (cd_nelmts <= 1)
are read by the new plugin identically.
