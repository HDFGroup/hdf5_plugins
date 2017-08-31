cmake_minimum_required (VERSION 3.2.2)
project (H5ZFP_SRC_LIBZFP C)

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${H5ZFP_SRC_LIBZFP_SOURCE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR})

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS} -DHAVE_CONFIG_H)

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (
    ${H5ZFP_SRC_LIBZFP_SOURCE_DIR}/inc
)

#-----------------------------------------------------------------------------
# Define H5ZFP Library
#-----------------------------------------------------------------------------
if (BUILD_ZFP_LIBRARY_SOURCE)
  set (ZFP_SRCS ${H5ZFP_SRCS}
      ${H5ZFP_SRC_SOURCE_DIR}/src/bitstream.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/decode1f.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/decode1d.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/encode1f.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/encode1d.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/decode2f.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/decode2d.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/encode2f.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/encode2d.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/decode3f.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/decode3d.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/encode3f.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/encode3d.c
      ${H5ZFP_SRC_SOURCE_DIR}/src/zfp.c

  )
endif (BUILD_ZFP_LIBRARY_SOURCE)

add_library (${ZFP_LIB_TARGET} OBJECT ${ZFP_SRCS})
