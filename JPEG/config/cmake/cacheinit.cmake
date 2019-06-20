# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (BUILD_TESTING ON CACHE BOOL "Build h5jpeg Unit Testing" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5jpeg Examples" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (JPEG_GIT_URL "https://git@bitbucket.hdfgroup.org/scm/test/jpeg.git" CACHE STRING "Use JPEG from HDF repository" FORCE)
set (JPEG_GIT_BRANCH "jpeg9c" CACHE STRING "" FORCE)

set (JPEG_TGZ_NAME "JPEG.tar.gz" CACHE STRING "Use JPEG from compressed file" FORCE)

set (JPEG_PACKAGE_NAME "jpeg" CACHE STRING "Name of JPEG package" FORCE)
set (H5JPEG_CPACK_ENABLE ON CACHE BOOL "Enable the CPACK include and components" FORCE)

set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
