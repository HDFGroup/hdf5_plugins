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

set (ZFP_GIT_URL "https://github.com/LLNL/zfp.git" CACHE STRING "Use ZFP from  GitHub repository" FORCE)
set (ZFP_GIT_BRANCH "develop" CACHE STRING "" FORCE)

set (ZFP_TGZ_ORIGPATH "https://github.com/LLNL/zfp/releases/download/1.0.0" CACHE STRING "Use PLUGINS from original location" FORCE)
set (ZFP_TGZ_NAME "zfp-1.0.1.tar.gz" CACHE STRING "Use ZFP from compressed file" FORCE)
