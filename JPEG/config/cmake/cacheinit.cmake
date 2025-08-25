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

set (H5PL_BUILD_TESTING ON CACHE BOOL "Build h5jpeg Unit Testing" FORCE)

set (H5PL_BUILD_EXAMPLES ON CACHE BOOL "Build h5jpeg Examples" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (HDF5_NAMESPACE "hdf5::" CACHE STRING "Name space of HDF5 library" FORCE)

set (JPEG_PACKAGE_NAME "jpeg" CACHE STRING "Name of JPEG package" FORCE)
set (H5JPEG_CPACK_ENABLE ON CACHE BOOL "Enable the CPACK include and components" FORCE)

set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
