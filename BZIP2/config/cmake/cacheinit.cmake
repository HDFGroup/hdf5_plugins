# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

SET (USE_SHARED_LIBS ON CACHE BOOL "Use Shared Libraries" FORCE)

SET (BUILD_TESTING OFF CACHE BOOL "Build h5bz2 Unit Testing" FORCE)

SET (HDF_ENABLE_PARALLEL OFF CACHE BOOL "Enable parallel build (requires MPI)" FORCE)

SET (H5BZ2_PACKAGE_NAME "h5bz2" CACHE STRING "Name of h5bz2 package" FORCE)

set (H5BZ2_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO SVN TGZ)" FORCE)
set_property (CACHE H5BZ2_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO SVN TGZ)

set (BZ2_SVN_URL "http://svn.hdfgroup.uiuc.edu/bzip2/trunk" CACHE STRING "Use BZ2 from HDF repository" FORCE)

set (BZ2_TGZ_NAME "BZ2.tar.gz" CACHE STRING "Use BZ2 from compressed file" FORCE)

set (BZ2_PACKAGE_NAME "bz2" CACHE STRING "Name of BZ2 package" FORCE)
