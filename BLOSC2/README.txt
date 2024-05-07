Building this filter/example requires knowledge of the hdf5 and the blosc2
installation.

For autotools configure, you must supply these using
    --with-hdf5 and --with-blosc2lib.
Example (in the build folder):
    ../configure --with-hdf5=/temp/hdf5 --with-blosc2lib=/temp/blosc2

For CMake, you must supply the location of the cmake configuration files
    in environment variables.
    In addition, CMake options "H5PL_BUILD_EXAMPLES" and "H5PL_BUILD_TESTING" must
    be set "ON" in order to build the example and run the tests.
Example:
    set(ENV{HDF5_ROOT} "/temp/hdf5")
    set(ENV{BLOSC2_ROOT} "/temp/blosc2")
    set(ENV{LD_LIBRARY_PATH} "/temp/blosc2/lib:/temp/hdf5/lib")
    set(ADD_BUILD_OPTIONS "-DH5PL_BUILD_EXAMPLES:BOOL=ON -DH5PL_BUILD_TESTING:BOOL=ON")

    For non-cmake built hdf5 or blosc2, use the location of the include/lib
    folders:
    set(ENV{HDF5_ROOT} "/temp/hdf5")
    set(ENV{BLOSC2_ROOT} "/temp/blosc2")
