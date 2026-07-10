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
interpreted as a signed int. It follows the same signed compression-level
convention as liblz4's lz4frame API, so a value means the same thing here
as it does everywhere else in the LZ4 ecosystem:

    >= 2  : LZ4HC level (2..12); values above 12 are clamped to 12.
            9 is the LZ4HC default and the recommended archival setting.
   0 or 1 : default fast encoder (LZ4_compress_default; acceleration 1).
     < 0  : fast encoder with acceleration = -cd_values[1] + 1, so -1 is
            acceleration 2, -2 is acceleration 3, and so on. Values below
            -65536 are clamped to -65536 (acceleration LZ4_ACCELERATION_MAX,
            currently 65537).

Note the negative side is offset by one: acceleration 1 is the default
(0 or 1), so the first faster-than-default step is -1 (acceleration 2).
To request a specific acceleration A (A >= 2), set cd_values[1] = 1 - A.
For example, acceleration 8 -> cd_values[1] = -7; acceleration 9 -> -8.

Because the slot is unsigned on disk, a negative value is stored as its
two's-complement bit pattern. For example, cd_values[1] = -8 (acceleration
9) is stored as 4294967288, which is what "h5dump -p" displays. The plugin
casts the slot back to int when reading.

cd_values[1] only affects writes. The on-disk chunk format is unchanged:
LZ4HC, accelerated fast LZ4, and the default encoder all produce LZ4
Block payloads that the existing reader (LZ4_decompress_safe) decodes
without modification. Files written by the older plugin (cd_nelmts <= 1)
are read by the new plugin identically.
