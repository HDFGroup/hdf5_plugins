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
# This is the CacheURLs file.

########################
# EXTERNAL cache entries
########################

set (ZSTD_GIT_URL "https://github.com/facebook/zstd.git" CACHE STRING "Use ZSTD from  GitHub repository" FORCE)
set (ZSTD_GIT_BRANCH "dev" CACHE STRING "" FORCE)

set (ZSTD_TGZ_ORIGPATH "https://github.com/facebook/zstd/releases/download/v1.5.7" CACHE STRING "Use PLUGINS from original location" FORCE)
set (ZSTD_TGZ_NAME "zstd-1.5.7.tar.gz" CACHE STRING "Use ZSTD from compressed file" FORCE)
