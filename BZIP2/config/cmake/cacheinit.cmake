# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (BUILD_TESTING ON CACHE BOOL "Build h5bz2 Unit Testing" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5bz2 Examples" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (BZ2_GIT_URL "https://git@bitbucket.hdfgroup.org/scm/test/bzip2.git" CACHE STRING "Use BZ2 from HDF repository" FORCE)
set (BZ2_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (BZ2_TGZ_NAME "BZ2.tar.gz" CACHE STRING "Use BZ2 from compressed file" FORCE)

set (BZ2_PACKAGE_NAME "bz2" CACHE STRING "Name of BZ2 package" FORCE)

set (H5BZ2_PACKAGE_NAME "h5bz2" CACHE STRING "Name of h5bz2 package" FORCE)

set (H5BZ2_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5BZ2_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
