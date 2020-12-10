Building these filter/example requires knowledge of the hdf5 and the
compression library installation. Out-of-source build process is expected.

For autotools configure, you must supply these using
    --with-hdf5 and --with-{filter}lib.
Example for blosc (in the build folder):
    ../configure --with-hdf5=/temp/hdf5 --with-blosclib=/temp/blosc


For CMake, you must supply the location of the CMake-built HDF5 configuration files
    in environment variables.
    In addition, CMake options "BUILD_EXAMPLES" and "BUILD_TESTING" must
    be set "ON" in order to build the example and run the tests.
    The external compression library sources are provided in the PLUGIN_SOURCE folder.
    These can be used with the H5PL_ALLOW_EXTERNAL_SUPPORT option.
    Consult CMake documentation for setting CMAKE_GENERATOR
Example:
    set(ENV{HDF5_DIR} "/temp/hdf5/share/cmake/hdf5")
    set(BUILD_OPTIONS "-DUSE_SHARED_LIBS:BOOL=ON -DBUILD_SHARED_LIBS:BOOL=ON")
    set(BUILD_OPTIONS "${BUILD_OPTIONS} -DTGZPATH:PATH=${PLUGIN_SOURCE}/libs -DH5PL_ALLOW_EXTERNAL_SUPPORT:STRING=\"TGZ\"")
    cmake -C ${PLUGIN_SOURCE}/config/cmake/cacheinit.cmake -DCMAKE_BUILD_TYPE:STRING=Release ${BUILD_OPTIONS} "-G${CMAKE_GENERATOR}" ${PLUGIN_SOURCE}

    For non-cmake built hdf5, use the location of the include/lib folders:
    set(ENV{HDF5_ROOT} "/temp/hdf5")
