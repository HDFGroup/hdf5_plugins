# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (USE_SHARED_LIBS ON CACHE BOOL "Use Shared Libraries" FORCE)

set (BUILD_TESTING ON CACHE BOOL "Build h5blosc Unit Testing" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5blosc Examples" FORCE)

set (HDF_ENABLE_PARALLEL OFF CACHE BOOL "Enable parallel build (requires MPI)" FORCE)

set (H5BLOSC_PACKAGE_NAME "h5blosc" CACHE STRING "Name of h5blosc package" FORCE)

set (H5BLOSC_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO SVN TGZ)" FORCE)
set_property (CACHE H5BLOSC_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO SVN TGZ)

set (BLOSC_SVN_URL "http://svn.hdfgroup.uiuc.edu/blosc/trunk" CACHE STRING "Use BLOSC from HDF repository" FORCE)

set (BLOSC_TGZ_NAME "c-blosc.tar.gz" CACHE STRING "Use BLOSC from compressed file" FORCE)

set (BLOSC_PACKAGE_NAME "blosc" CACHE STRING "Name of BLOSC package" FORCE)
