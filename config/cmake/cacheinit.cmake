# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (USE_SHARED_LIBS ON CACHE BOOL "Use Shared Libraries" FORCE)

set (BUILD_TESTING ON CACHE BOOL "Build h5pl Unit Testing" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5pl Examples" FORCE)

set (HDF_ENABLE_PARALLEL OFF CACHE BOOL "Enable parallel build (requires MPI)" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (H5PL_PACKAGE_NAME "h5pl" CACHE STRING "Name of h5plugin package" FORCE)

set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)

set (H5PL_GIT_URL "https://git@bitbucket.hdfgroup.org/scm/test/h5plugin.git" CACHE STRING "Use plugins from HDF repository" FORCE)
set (H5PL_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (H5PL_TGZ_NAME "h5pl.tar.gz" CACHE STRING "Use plugins from compressed file" FORCE)

set (H5PL_PACKAGE_NAME "h5pl" CACHE STRING "Name of plugins package" FORCE)
