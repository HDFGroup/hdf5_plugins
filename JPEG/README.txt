Building this filter/example requires knowledge of the hdf5 and the jpeg
installation.

For autotools configure, you must supply these using
    --with-hdf5 and --with-jpeglib.
Example (in the build folder):
    ../configure --with-hdf5=/temp/hdf5 --with-jpeglib=/temp/jpeg

For CMake, you must supply the location of the cmake configuration files 
    in environment variables.
    In addition, CMake options "BUILD_EXAMPLES" and "BUILD_TESTING" must
    be set "ON" in order to build the example and run the tests.
Example:
    set(ENV{HDF5_ROOT} "/temp/hdf5/")
    set(ENV{JPEG_ROOT} "/temp/jpeg/")
    set(ENV{LD_LIBRARY_PATH} "/temp/jpeg/lib:/temp/hdf5/lib")
    set(ADD_BUILD_OPTIONS "-DBUILD_EXAMPLES:BOOL=ON -DBUILD_TESTING:BOOL=ON")

    For non-cmake built hdf5 or jpeg, use the location of the include/lib
    folders:
    set(ENV{HDF5_ROOT} "/temp/hdf5")
    set(ENV{JPEG_ROOT} "/temp/jpeg")
