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

set (H5PL_BUILD_TESTING ON CACHE BOOL "Build h5blosc Unit Testing" FORCE)

set (H5PL_BUILD_EXAMPLES ON CACHE BOOL "Build h5blosc Examples" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (HDF5_NAMESPACE "hdf5::" CACHE STRING "Name space of HDF5 library" FORCE)

set (BLOSC_GIT_URL "https://github.com/Blosc/c-blosc.git" CACHE STRING "Use BLOSC from Github" FORCE)
set (BLOSC_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (BLOSC_TGZ_ORIGPATH "https://github.com/Blosc/c-blosc/archive/refs/tags" CACHE STRING "Use PLUGINS from original location" FORCE)
set (BLOSC_TGZ_NAME "c-blosc-1.21.5.tar.gz" CACHE STRING "Use BLOSC from compressed file" FORCE)

set (BLOSC_PACKAGE_NAME "blosc" CACHE STRING "Name of BLOSC package" FORCE)
set (H5BLOSC_CPACK_ENABLE ON CACHE BOOL "Enable the CPACK include and components" FORCE)

set (BLOSC_ZLIB_GIT_URL "https://github.com/madler/zlib.git" CACHE STRING "Use ZLIB from  GitHub repository" FORCE)
set (BLOSC_ZLIB_GIT_BRANCH "develop" CACHE STRING "" FORCE)

set (BLOSC_ZLIB_TGZ_ORIGPATH "https://github.com/madler/zlib/releases/download/v1.3" CACHE STRING "Use PLUGINS from original location" FORCE)
set (BLOSC_ZLIB_TGZ_NAME "zlib-1.3.tar.gz" CACHE STRING "Use ZLib from compressed file" FORCE)

set (BLOSC_ZLIB_PACKAGE_NAME "zlib" CACHE STRING "Name of BLOSC_ZLIB package" FORCE)

set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
