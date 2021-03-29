# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (BUILD_TESTING ON CACHE BOOL "Build h5bitgroom Unit Testing" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5bitgroom Examples" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (BITGROOM_GIT_URL "https://github.com/facebook/bitgroom.git" CACHE STRING "Use BITGROOM from HDF repository" FORCE)
set (BITGROOM_GIT_BRANCH "bitgroom" CACHE STRING "" FORCE)

set (BITGROOM_TGZ_NAME "BITGROOM.tar.gz" CACHE STRING "Use BITGROOM from compressed file" FORCE)

set (BITGROOM_PACKAGE_NAME "bitgroom" CACHE STRING "Name of BITGROOM package" FORCE)
set (H5BITGROOM_CPACK_ENABLE ON CACHE BOOL "Enable the CPACK include and components" FORCE)

set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
