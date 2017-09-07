Building this filter/example requires knowledge of the hdf5 and the zfp
installation.

For autotools configure, you must supply these using
    --with-hdf5 and --with-zfplib.
Example (in the build folder):
    ../configure --with-hdf5=/temp/hdf5 --with-zfplib=/temp/zfp

For CMake, you must supply the location of the cmake configuration files
    in environment variables.
    In addition, CMake options "BUILD_EXAMPLES" and "BUILD_TESTING" must
    be set "ON" in order to build the example and run the tests.
Example:
    set(ENV{HDF5_DIR} "/temp/hdf5/share/cmake/hdf5")
    set(ENV{ZFP_DIR} "/temp/zfp/share/cmake/zfp")
    set(ENV{LD_LIBRARY_PATH} "/temp/zfp/lib:/temp/hdf5/lib")
    set(ADD_BUILD_OPTIONS "-DBUILD_EXAMPLES:BOOL=ON -DBUILD_TESTING:BOOL=ON")

    For non-cmake built hdf5 or zfp, use the location of the include/lib
    folders:
    set(ENV{HDF5_ROOT} "/temp/hdf5")
    set(ENV{ZFP_ROOT} "/temp/zfp")
