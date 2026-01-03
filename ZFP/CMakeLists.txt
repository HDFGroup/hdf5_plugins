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

# Wrapper CMakeLists.txt for H5Z-ZFP subtree integration
cmake_minimum_required(VERSION 3.18)
project(H5ZFP_WRAPPER)

# Verify H5Z-ZFP directory exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/H5Z-ZFP/CMakeLists.txt")
    message(FATAL_ERROR
        "H5Z-ZFP not found at ${CMAKE_CURRENT_SOURCE_DIR}/H5Z-ZFP\n"
        "The upstream H5Z-ZFP code should be present via git subtree.")
endif()

message(STATUS "Using H5Z-ZFP from subtree at ${CMAKE_CURRENT_SOURCE_DIR}/H5Z-ZFP")

# H5Z-ZFP expects these variables to be set
# HDF5_ROOT should already be set by the parent project
# ZFP_ROOT or ZFP_DIR should point to the ZFP compression library

# Pass through HDF5 configuration
if(DEFINED ENV{HDF5_ROOT} AND NOT DEFINED HDF5_ROOT)
    set(HDF5_ROOT $ENV{HDF5_ROOT})
endif()

# Handle ZFP library location (compression library, not the filter)
# The parent build system may have already fetched/built ZFP library
if(H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    # ZFP library will be handled by the parent build system
    # Set ZFP_DIR to where it will be installed
    message(STATUS "ZFP library will be fetched via H5PL_ALLOW_EXTERNAL_SUPPORT=${H5PL_ALLOW_EXTERNAL_SUPPORT}")
endif()

# Add the upstream H5Z-ZFP project
add_subdirectory(H5Z-ZFP)

# Note: The H5Z-ZFP CMakeLists.txt will create the h5zfp target
# and handle installation to the appropriate plugin directory
