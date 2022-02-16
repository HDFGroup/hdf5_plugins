# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (BUILD_TESTING ON CACHE BOOL "Build h5sz Unit Testing" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5sz Examples" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (HDF5_NAMESPACE "hdf5::" CACHE STRING "Name space of HDF5 library" FORCE)

set (SZ_GIT_URL "https://github.com/disheng222/SZ" CACHE STRING "Use SZ filter from github repository" FORCE)
set (SZ_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (SZ_TGZ_NAME "szf.tar.gz" CACHE STRING "Use SZ filter from compressed file" FORCE)

set (SZ_PACKAGE_NAME "SZ" CACHE STRING "Name of SZ filter package" FORCE)
set (H5SZ_CPACK_ENABLE ON CACHE BOOL "Enable the CPACK include and components" FORCE)

set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
