#
# Copyright by The HDF Group.
# All rights reserved.
#
# This file is part of HDF5.  The full HDF5 copyright notice, including
# terms governing use, modification, and redistribution, is contained in
# the COPYING file, which can be found at the root of the source code
# distribution tree, or in https://www.hdfgroup.org/licenses.
# If you do not have access to either file, you may request a copy from
# help@hdfgroup.org.
#
# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (H5PL_BUILD_TESTING ON CACHE BOOL "Build h5zstd Unit Testing" FORCE)

set (H5PL_BUILD_EXAMPLES ON CACHE BOOL "Build h5zstd Examples" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (HDF5_NAMESPACE "hdf5::" CACHE STRING "Name space of HDF5 library" FORCE)

set (ZSTD_GIT_URL "https://github.com/facebook/zstd.git" CACHE STRING "Use ZSTD from  GitHub repository" FORCE)
set (ZSTD_GIT_BRANCH "dev" CACHE STRING "" FORCE)

set (ZSTD_TGZ_ORIGPATH "https://github.com/facebook/zstd/releases/download/v1.5.5" CACHE STRING "Use PLUGINS from original location" FORCE)
set (ZSTD_TGZ_NAME "zstd-1.5.5.tar.gz" CACHE STRING "Use ZSTD from compressed file" FORCE)

set (ZSTD_PACKAGE_NAME "zstd" CACHE STRING "Name of ZSTD package" FORCE)
set (H5ZSTD_CPACK_ENABLE ON CACHE BOOL "Enable the CPACK include and components" FORCE)

set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
