# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (USE_SHARED_LIBS ON CACHE BOOL "Use Shared Libraries" FORCE)

set (BUILD_TESTING ON CACHE BOOL "Build h5lzf Unit Testing" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5lzf Examples" FORCE)

set (HDF_ENABLE_PARALLEL OFF CACHE BOOL "Enable parallel build (requires MPI)" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (H5LZF_PACKAGE_NAME "h5lzf" CACHE STRING "Name of h5lzf package" FORCE)

set (H5LZF_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5LZF_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)

set (LZF_GIT_URL "https://git@bitbucket.hdfgroup.org/scm/test/lzf.git" CACHE STRING "Use LZF from HDF repository" FORCE)
set (LZF_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (LZF_TGZ_NAME "lzf.tar.gz" CACHE STRING "Use LZF from compressed file" FORCE)

set (LZF_PACKAGE_NAME "lzf" CACHE STRING "Name of LZF package" FORCE)
