# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (USE_SHARED_LIBS ON CACHE BOOL "Use Shared Libraries" FORCE)

set (BUILD_ZFP_LIBRARY_SOURCE ON CACHE BOOL "build the zfp library within the plugin" FORCE)

set (BUILD_TESTING ON CACHE BOOL "Build h5zfp Unit Testing" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5zfp Examples" FORCE)

set (HDF_ENABLE_PARALLEL OFF CACHE BOOL "Enable parallel build (requires MPI)" FORCE)

set (H5ZFP_PACKAGE_NAME "h5zfp" CACHE STRING "Name of h5zfp package" FORCE)

set (H5ZFP_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5ZFP_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)

set (ZFP_PACKAGE_NAME "zfp" CACHE STRING "Name of ZFP package" FORCE)
