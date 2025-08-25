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

set (JPEG_GIT_URL "https://github.com/libjpeg-turbo/libjpeg-turbo.git" CACHE STRING "Use JPEG from TurboJPEG" FORCE)
set (JPEG_GIT_BRANCH "jpeg-9e" CACHE STRING "" FORCE)

set (JPEG_TGZ_ORIGPATH "https://www.ijg.org/files" CACHE STRING "Use PLUGINS from original location" FORCE)
set (JPEG_TGZ_NAME "jpegsrc.v9e.tar.gz" CACHE STRING "Use JPEG from compressed file" FORCE)
