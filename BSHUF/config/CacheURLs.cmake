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

set (LZ4_GIT_URL "https://github.com/lz4/lz4.git" CACHE STRING "Use LZ4 from  GitHub repository" FORCE)
set (LZ4_GIT_BRANCH "dev" CACHE STRING "" FORCE)

set (LZ4_TGZ_ORIGPATH "https://github.com/lz4/lz4/releases/download/v1.10.0" CACHE STRING "Use PLUGINS from original location" FORCE)
set (LZ4_TGZ_NAME "lz4-1.10.0.tar.gz" CACHE STRING "Use LZ4 from compressed file" FORCE)

set (BSHUF_GIT_URL "https://github.com/kiyo-masui/bitshuffle.git" CACHE STRING "Use BSHUF from HDF repository" FORCE)
set (BSHUF_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (BSHUF_TGZ_ORIGPATH "https://github.com/kiyo-masui/bitshuffle/archive/refs/tags" CACHE STRING "Use PLUGINS from original location" FORCE)
set (BSHUF_TGZ_NAME "bitshuffle-0.5.2.tar.gz" CACHE STRING "Use BSHUF from compressed file" FORCE)
