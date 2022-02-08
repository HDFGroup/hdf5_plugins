# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (BUILD_TESTING ON CACHE BOOL "Build h5blosc Unit Testing" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5blosc Examples" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (HDF5_NAMESPACE "hdf5::" CACHE STRING "Name space of HDF5 library" FORCE)

set (BLOSC_GIT_URL "https://github.com/Blosc/c-blosc.git" CACHE STRING "Use BLOSC from Github" FORCE)
set (BLOSC_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (BLOSC_TGZ_NAME "c-blosc.tar.gz" CACHE STRING "Use BLOSC from compressed file" FORCE)

set (BLOSC_PACKAGE_NAME "blosc" CACHE STRING "Name of BLOSC package" FORCE)
set (H5BLOSC_CPACK_ENABLE ON CACHE BOOL "Enable the CPACK include and components" FORCE)

set (BLOSC_ZLIB_GIT_URL "https://git@bitbucket.hdfgroup.org/scm/test/zlib.git" CACHE STRING "Use BLOSC_ZLIB from HDF repo" FORCE)
set (BLOSC_ZLIB_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (BLOSC_ZLIB_TGZ_NAME "ZLib.tar.gz" CACHE STRING "Use BLOSC_ZLib from compressed file" FORCE)

set (BLOSC_ZLIB_PACKAGE_NAME "zlib" CACHE STRING "Name of BLOSC_ZLIB package" FORCE)

set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
