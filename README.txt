Building these filter/example requires knowledge of the hdf5 and the
compression library installation.

For autotools configure, you must supply these using
    --with-hdf5 and --with-{filter}lib.
Example for blosc (in the build folder):
    ../configure --with-hdf5=/temp/hdf5 --with-blosclib=/temp/blosc

For CMake, you must supply the location of the cmake configuration files
    in environment variables.
    In addition, CMake options "BUILD_EXAMPLES" and "BUILD_TESTING" must
    be set "ON" in order to build the example and run the tests.
Example for blosc:
    set(ENV{HDF5_DIR} "/temp/hdf5/share/cmake/hdf5")
    set(ENV{BLOSC_DIR} "/temp/blosc/share/cmake/blosc")
    set(ENV{LD_LIBRARY_PATH} "/temp/blosc/lib:/temp/hdf5/lib")
    set(ADD_BUILD_OPTIONS "-DENABLE_BLOSC:BOOL=ON -DBUILD_EXAMPLES:BOOL=ON -DBUILD_TESTING:BOOL=ON")

    For non-cmake built hdf5 or blosc, use the location of the include/lib
    folders:
    set(ENV{HDF5_ROOT} "/temp/hdf5")
    set(ENV{BLOSC_ROOT} "/temp/blosc")
