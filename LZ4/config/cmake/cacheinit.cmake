# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (USE_SHARED_LIBS ON CACHE BOOL "Use Shared Libraries" FORCE)

set (BUILD_LZ4_LIBRARY_SOURCE ON CACHE BOOL "build the lz4 library within the plugin" FORCE)

set (BUILD_TESTING ON CACHE BOOL "Build h5lz4 Unit Testing" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5lz4 Examples" FORCE)

set (HDF_ENABLE_PARALLEL OFF CACHE BOOL "Enable parallel build (requires MPI)" FORCE)

set (H5LZ4_PACKAGE_NAME "h5lz4" CACHE STRING "Name of h5lz4 package" FORCE)

set (H5LZ4_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO SVN TGZ)" FORCE)
set_property (CACHE H5LZ4_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO SVN TGZ)

set (LZ4_SVN_URL "http://lz4.googlecode.com/svn/trunk" CACHE STRING "Use LZ4 from main repository" FORCE)

set (LZ4_TGZ_NAME "lz4.tar.gz" CACHE STRING "Use LZ4 from compressed file" FORCE)

set (LZ4_PACKAGE_NAME "lz4" CACHE STRING "Name of LZ4 package" FORCE)
