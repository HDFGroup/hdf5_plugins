# This is the CMakeCache file.

########################
# EXTERNAL cache entries
########################

set (USE_SHARED_LIBS ON CACHE BOOL "Use Shared Libraries" FORCE)

set (BUILD_TESTING ON CACHE BOOL "Build h5blosc Unit Testing" FORCE)
set (H5PL_BUILD_TESTING ON CACHE BOOL "Enable h5pl examples" FORCE)

set (BUILD_EXAMPLES ON CACHE BOOL "Build h5pl Examples" FORCE)

set (HDF_ENABLE_PARALLEL OFF CACHE BOOL "Enable parallel build (requires MPI)" FORCE)

set (HDF5_PACKAGE_NAME "hdf5" CACHE STRING "Name of HDF5 package" FORCE)

set (HDF5_NAMESPACE "hdf5::" CACHE STRING "Name space of HDF5 library" FORCE)

set (H5PL_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)" FORCE)
set_property (CACHE H5PL_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)

set (H5PL_GIT_URL "https://github.com/HDFGroup/h5plugin.git" CACHE STRING "Use plugins from HDF repository" FORCE)
set (H5PL_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (H5PL_TGZ_NAME "h5pl.tar.gz" CACHE STRING "Use plugins from compressed file" FORCE)

set (PL_PACKAGE_NAME "pl" CACHE STRING "Name of plugins package" FORCE)
set (H5PL_CPACK_ENABLE ON CACHE BOOL "Enable the CPACK include and components" FORCE)

############################# bitshuffle ################################################

set (BSHUF_GIT_URL "https://git@bitbucket.hdfgroup.org/scm/test/bitshuffle.git" CACHE STRING "Use BSHUF from HDF repository" FORCE)
set (BSHUF_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (BSHUF_TGZ_NAME "bitshuffle.tar.gz" CACHE STRING "Use BSHUF from compressed file" FORCE)

set (BSHUF_PACKAGE_NAME "bshuf" CACHE STRING "Name of BSHUF package" FORCE)

############################# blosc ################################################

set (BLOSC_GIT_URL "https://github.com/Blosc/c-blosc.git" CACHE STRING "Use BLOSC from Github" FORCE)
set (BLOSC_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (BLOSC_TGZ_NAME "c-blosc.tar.gz" CACHE STRING "Use BLOSC from compressed file" FORCE)

set (BLOSC_PACKAGE_NAME "blosc" CACHE STRING "Name of BLOSC package" FORCE)

set (ZLIB_GIT_URL "https://git@bitbucket.hdfgroup.org/scm/test/zlib.git" CACHE STRING "Use ZLIB from HDF repo" FORCE)
set (ZLIB_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (ZLIB_TGZ_NAME "ZLib.tar.gz" CACHE STRING "Use ZLib from compressed file" FORCE)

set (ZLIB_PACKAGE_NAME "zlib" CACHE STRING "Name of ZLIB package" FORCE)

############################# bzip2 ################################################

set (BZ2_GIT_URL "https://git@bitbucket.hdfgroup.org/scm/test/bzip2.git" CACHE STRING "Use BZ2 from HDF repository" FORCE)
set (BZ2_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (BZ2_TGZ_NAME "BZ2.tar.gz" CACHE STRING "Use BZ2 from compressed file" FORCE)

set (BZ2_PACKAGE_NAME "bz2" CACHE STRING "Name of BZ2 package" FORCE)

############################# fpzip ################################################

set (FPZIP_GIT_URL "https://https://github.com/LLNL/fpzip" CACHE STRING "Use FPZIP from github repository" FORCE)
set (FPZIP_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (FPZIP_TGZ_NAME "fpzip.tar.gz" CACHE STRING "Use FPZIP from compressed file" FORCE)

set (FPZIP_PACKAGE_NAME "fpzip" CACHE STRING "Name of FPZIP package" FORCE)

############################# jpeg ################################################

set (JPEG_GIT_URL "https://git@bitbucket.hdfgroup.org/scm/test/jpeg.git" CACHE STRING "Use JPEG from HDF repository" FORCE)
set (JPEG_GIT_BRANCH "jpeg9d" CACHE STRING "" FORCE)

#set (JPEG_TGZ_NAME "JPEG9d.tar.gz" CACHE STRING "Use JPEG from compressed file" FORCE)
set (JPEG_TGZ_NAME "JPEG.tar.gz" CACHE STRING "Use JPEG from compressed file" FORCE)

set (JPEG_PACKAGE_NAME "jpeg" CACHE STRING "Name of JPEG package" FORCE)

############################# lz4 ################################################

set (BUILD_LZ4_LIBRARY_SOURCE ON CACHE BOOL "build the lz4 library within the plugin" FORCE)

set (LZ4_GIT_URL "https://git@bitbucket.hdfgroup.org/scm/test/lz4.git" CACHE STRING "Use LZ4 from HDF repository" FORCE)
set (LZ4_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (LZ4_TGZ_NAME "lz4.tar.gz" CACHE STRING "Use LZ4 from compressed file" FORCE)

set (LZ4_PACKAGE_NAME "lz4" CACHE STRING "Name of LZ4 package" FORCE)

############################# lzf ################################################

set (LZF_GIT_URL "https://git@bitbucket.hdfgroup.org/scm/test/lzf.git" CACHE STRING "Use LZF from HDF repository" FORCE)
set (LZF_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (LZF_TGZ_NAME "lzf.tar.gz" CACHE STRING "Use LZF from compressed file" FORCE)

set (LZF_PACKAGE_NAME "lzf" CACHE STRING "Name of LZF package" FORCE)

############################# mafisc ################################################

#set (BUILD_MAFISC_LIBRARY_SOURCE OFF CACHE BOOL "build the mafisc library within the plugin" FORCE)

#set (MAFISC_PACKAGE_NAME "mafisc" CACHE STRING "Name of MAFISC package" FORCE)

############################# sz ################################################

set (SZF_GIT_URL "https://github.com/disheng222/SZ" CACHE STRING "Use SZ filter from github repository" FORCE)
set (SZF_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (SZF_TGZ_NAME "szf.tar.gz" CACHE STRING "Use SZ filter from compressed file" FORCE)

set (SZF_PACKAGE_NAME "szf" CACHE STRING "Name of SZ filter package" FORCE)

############################# zfp ################################################

set (ZFP_GIT_URL "https://github.com/LLNL/zfp.git" CACHE STRING "Use ZFP from Github" FORCE)
set (ZFP_GIT_BRANCH "master" CACHE STRING "" FORCE)

set (ZFP_TGZ_NAME "zfp.tar.gz" CACHE STRING "Use ZFP from compressed file" FORCE)

set (ZFP_PACKAGE_NAME "zfp" CACHE STRING "Name of ZFP package" FORCE)
