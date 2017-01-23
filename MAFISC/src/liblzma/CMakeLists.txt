cmake_minimum_required (VERSION 3.2.2)
project (H5MAFISC_SRC_LIBLZMA C)

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR})
INCLUDE_DIRECTORIES (${CMAKE_BINARY_DIR})

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS} -DHAVE_CONFIG_H)

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
INCLUDE_DIRECTORIES (
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lz
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/rangecoder
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/delta
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/simple
)
#add_definitions (TUKLIB_SYMBOL_PREFIX=lzma_)

#-----------------------------------------------------------------------------
# Define H5MAFISC Library
#-----------------------------------------------------------------------------
set (LZMA_HDRS
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/base.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/bcj.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/block.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/check.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/container.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/delta.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/filter.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/hardware.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/index.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/index_hash.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/lzma.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/stream_flags.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/version.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/api/lzma/vli.h
)
set (LZMA_SRCS
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/tuklib_physmem.c
)
set (LZMA_COMMON_SRCS ${LZMA_COMMON_SRCS}
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/common.c
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/common.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/block_util.c
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/easy_preset.c
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/easy_preset.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/filter_common.c
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/filter_common.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/hardware_physmem.c
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/index.c
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/index.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/stream_flags_common.c
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/stream_flags_common.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/vli_size.c
)
set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_COMMON_SRCS})
if (COND_MAIN_ENCODER)
  set (LZMA_COMMON_ENCODER_SRCS ${LZMA_COMMON_ENCODER_SRCS}
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/alone_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/block_buffer_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/block_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/block_encoder.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/block_header_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/easy_buffer_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/easy_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/easy_encoder_memusage.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/filter_buffer_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/filter_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/filter_encoder.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/filter_flags_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/index_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/index_encoder.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/stream_buffer_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/stream_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/stream_encoder.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/stream_flags_encoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/vli_encoder.c
  )
  set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_COMMON_ENCODER_SRCS})
endif ()
if (COND_MAIN_DECODER)
  set (LZMA_COMMON_DECODER_SRCS ${LZMA_COMMON_DECODER_SRCS}
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/alone_decoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/alone_decoder.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/auto_decoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/block_buffer_decoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/block_decoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/block_decoder.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/block_header_decoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/easy_decoder_memusage.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/filter_buffer_decoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/filter_decoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/filter_decoder.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/filter_flags_decoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/index_decoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/index_hash.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/stream_buffer_decoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/stream_decoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/stream_decoder.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/stream_flags_decoder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/common/vli_decoder.c
  )
  set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_COMMON_DECODER_SRCS})
endif ()

set (LZMA_CHECK_SRCS ${LZMA_CHECK_SRCS}
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check/check.c
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check/check.h
    ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check/crc_macros.h
)
if (COND_CHECK_CRC32)
  set (LZMA_CHECK_SRCS ${LZMA_CHECK_SRCS}
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check/crc32_table.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check/crc32_table_le.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check/crc32_table_be.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check/crc32_fast.c
  )
endif ()
if (COND_CHECK_CRC64)
  set (LZMA_CHECK_SRCS ${LZMA_CHECK_SRCS}
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check/crc64_table.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check/crc64_table_le.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check/crc64_table_be.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check/crc64_fast.c
  )
endif ()
if (COND_CHECK_SHA256)
  set (LZMA_CHECK_SRCS ${LZMA_CHECK_SRCS}
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/check/sha256.c
  )
endif ()
set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_CHECK_SRCS})

if (COND_FILTER_LZ)
  if (COND_ENCODER_LZ)
    set (LZMA_ENCODER_LZ_SRCS ${LZMA_ENCODER_LZ_SRCS}
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lz/lz_encoder.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lz/lz_encoder.h
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lz/lz_encoder_hash.h
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lz/lz_encoder_hash_table.h
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lz/lz_encoder_mf.c
    )
    set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_ENCODER_LZ_SRCS})
  endif ()
  if (COND_DECODER_LZ)
    set (LZMA_DECODER_LZ_SRCS ${LZMA_DECODER_LZ_SRCS}
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lz/lz_decoder.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lz/lz_decoder.h
    )
    set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_DECODER_LZ_SRCS})
  endif ()
endif ()

if (COND_FILTER_LZMA1)
  set (LZMA_SRCS ${LZMA_SRCS}
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma_common.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/rangecoder/range_common.h
  )
  if (COND_ENCODER_LZMA1)
    set (LZMA_ENCODER_LZMA1_SRCS ${LZMA_ENCODER_LZMA1_SRCS}
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/fastpos.h
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma_encoder.h
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma_encoder.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma_encoder_presets.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma_encoder_private.h
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma_encoder_optimum_fast.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma_encoder_optimum_normal.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/fastpos_table.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/rangecoder/range_encoder.h
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/rangecoder/price.h
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/rangecoder/price_table.c
    )
    set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_ENCODER_LZMA1_SRCS})
  endif ()
  if (COND_DECODER_LZMA1)
    set (LZMA_DECODER_LZMA1_SRCS ${LZMA_DECODER_LZMA1_SRCS}
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma_decoder.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma_decoder.h
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/rangecoder/range_decoder.h
    )
    set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_DECODER_LZMA1_SRCS})
  endif ()
  if (COND_ENCODER_LZMA2)
    set (LZMA_ENCODER_LZMA2_SRCS ${LZMA_ENCODER_LZMA2_SRCS}
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma2_encoder.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma2_encoder.h
    )
    set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_ENCODER_LZMA2_SRCS})
  endif ()
  if (COND_DECODER_LZMA2)
    set (LZMA_DECODER_LZMA2_SRCS ${LZMA_DECODER_LZMA2_SRCS}
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma2_decoder.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/lzma/lzma2_decoder.h
    )
    set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_DECODER_LZMA2_SRCS})
  endif ()
endif ()

if (COND_FILTER_DELTA)
  set (LZMA_SRCS ${LZMA_SRCS}
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/delta/delta_common.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/delta/delta_common.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/delta/delta_private.h
  )
  if (COND_ENCODER_DELTA)
    set (LZMA_ENCODER_DELTA_SRCS ${LZMA_ENCODER_DELTA_SRCS}
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/delta/delta_encoder.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/delta/delta_encoder.h
    )
    set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_ENCODER_DELTA_SRCS})
  endif ()
  if (COND_DECODER_DELTA)
    set (LZMA_DECODER_DELTA_SRCS ${LZMA_DECODER_DELTA_SRCS}
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/delta/delta_decoder.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/delta/delta_decoder.h
    )
    set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_DECODER_DELTA_SRCS})
  endif ()
endif ()

if (COND_FILTER_SIMPLE)
  set (LZMA_SRCS ${LZMA_SRCS}
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/simple/simple_coder.c
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/simple/simple_coder.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/simple/simple_private.h
      ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/simple/x86.c
  )
  if (COND_ENCODER_SIMPLE)
    set (LZMA_ENCODER_SIMPLE_SRCS ${LZMA_ENCODER_SIMPLE_SRCS}
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/simple/simple_encoder.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/simple/simple_encoder.h
    )
    set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_ENCODER_SIMPLE_SRCS})
  endif ()
  if (COND_DECODER_SIMPLE)
    set (LZMA_DECODER_SIMPLE_SRCS ${LZMA_DECODER_SIMPLE_SRCS}
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/simple/simple_decoder.c
        ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/simple/simple_decoder.h
    )
    set (LZMA_SRCS ${LZMA_SRCS} ${LZMA_DECODER_SIMPLE_SRCS})
  endif ()
endif ()

if (WIN32)
  set (LZMA_SRCS ${LZMA_SRCS} ${H5MAFISC_SRC_LIBLZMA_SOURCE_DIR}/liblzma_w32res.rc)
endif ()

add_library (${LZMA_LIB_TARGET} OBJECT ${LZMA_SRCS})
