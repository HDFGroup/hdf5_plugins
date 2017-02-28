# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (USE_SHARED_LIBS ON CACHE BOOL "Use Shared Libraries" FORCE)

set (BUILD_MAFISC_LIBRARY_SOURCE ON CACHE BOOL "build the mafisc library within the plugin" FORCE)

set (BUILD_TESTING ON CACHE BOOL "Build h5mafisc Unit Testing" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5mafisc Examples" FORCE)

set (HDF_ENABLE_PARALLEL OFF CACHE BOOL "Enable parallel build (requires MPI)" FORCE)

set (H5MAFISC_PACKAGE_NAME "h5mafisc" CACHE STRING "Name of h5mafisc package" FORCE)

set (H5MAFISC_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5MAFISC_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)

set (MAFISC_PACKAGE_NAME "mafisc" CACHE STRING "Name of MAFISC package" FORCE)
