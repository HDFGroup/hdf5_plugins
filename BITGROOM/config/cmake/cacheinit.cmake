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

set (H5PL_BUILD_TESTING ON CACHE BOOL "Build h5bitgroom Unit Testing" FORCE)

set (H5PL_BUILD_EXAMPLES ON CACHE BOOL "Build h5bitgroom Examples" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (HDF5_NAMESPACE "hdf5::" CACHE STRING "Name space of HDF5 library" FORCE)

set (BITGROOM_GIT_URL "https://no_url/bitgroom.git" CACHE STRING "Use BITGROOM from repository" FORCE)
set (BITGROOM_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (BITGROOM_TGZ_ORIGPATH "https://no_url" CACHE STRING "Use PLUGINS from original location" FORCE)
set (BITGROOM_TGZ_NAME "bitgroom-1.0.tar.gz" CACHE STRING "Use BITGROOM from compressed file" FORCE)

set (BITGROOM_PACKAGE_NAME "bitgroom" CACHE STRING "Name of BITGROOM package" FORCE)
set (H5BITGROOM_CPACK_ENABLE ON CACHE BOOL "Enable the CPACK include and components" FORCE)

set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
