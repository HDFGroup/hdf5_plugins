# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (BUILD_TESTING ON CACHE BOOL "Build h5lzf Unit Testing" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5lzf Examples" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (HDF5_NAMESPACE "hdf5::" CACHE STRING "Name space of HDF5 library" FORCE)

set (LZF_GIT_URL "https://git@bitbucket.hdfgroup.org/scm/test/lzf.git" CACHE STRING "Use LZF from HDF repository" FORCE)
set (LZF_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (LZF_TGZ_NAME "lzf.tar.gz" CACHE STRING "Use LZF from compressed file" FORCE)

set (LZF_PACKAGE_NAME "lzf" CACHE STRING "Name of LZF package" FORCE)
set (H5LZF_CPACK_ENABLE ON CACHE BOOL "Enable the CPACK include and components" FORCE)

set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
