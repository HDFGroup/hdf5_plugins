cmake_minimum_required (VERSION 3.12)
project (H5SZF_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup szf source directories
#-----------------------------------------------------------------------------
set(H5SZF_SZ_INC_DIR ${H5SZF_SRC_SOURCE_DIR}/lib/include)
set(H5SZF_SZ_SRC_DIR ${H5SZF_SRC_SOURCE_DIR}/lib/src)
set(H5SZF_ZLIB_INC_DIR ${H5SZF_SRC_SOURCE_DIR}/lib/zlib)
set(H5SZF_ZLIB_SRC_DIR ${H5SZF_SRC_SOURCE_DIR}/lib/zlib)
set(H5SZF_ZSTD_INC_DIR ${H5SZF_SRC_SOURCE_DIR}/lib/zstd)
set(H5SZF_ZSTD_SRC_DIR ${H5SZF_SRC_SOURCE_DIR}/lib/zstd)
set(H5SZF_ZSTD_INC_DIRS
  ${H5SZF_SRC_SOURCE_DIR}/lib/zstd
  ${H5SZF_SRC_SOURCE_DIR}/lib/zstd/common
  ${H5SZF_SRC_SOURCE_DIR}/lib/zstd/compress
  ${H5SZF_SRC_SOURCE_DIR}/lib/zstd/decompress
)
set(H5SZF_ZSTD_SRC_DIRS
  ${H5SZF_SRC_SOURCE_DIR}/lib/zstd
  ${H5SZF_SRC_SOURCE_DIR}/lib/zstd/common
  ${H5SZF_SRC_SOURCE_DIR}/lib/zstd/compress
  ${H5SZF_SRC_SOURCE_DIR}/lib/zstd/decompress
)

#-----------------------------------------------------------------------------
# Setup include directories
#-----------------------------------------------------------------------------
set(H5SZF_INCLUDE_DIRS ${H5SZF_INCLUDE_DIRS}
    ${HDF5_INCLUDE_DIR} ${H5SZF_BINARY_DIR}
    ${H5SZF_SRC_SOURCE_DIR}
    ${H5SZF_SZ_INC_DIR} ${H5SZF_ZLIB_INC_DIR} ${H5SZF_ZSTD_INC_DIRS}
)

#-----------------------------------------------------------------------------
# Define H5SZ Library
#-----------------------------------------------------------------------------
set (H5SZF_HDRS)
set (H5SZF_SRCS
    ${H5SZF_SRC_SOURCE_DIR}/H5Zszf.c
)
if (BUILD_SZF_LIBRARY_SOURCE)
  set (H5SZF_HDRS ${H5SZF_HDRS}
      ${H5SZF_ZLIB_INC_DIR}/zconf.h
      ${H5SZF_ZLIB_INC_DIR}/zlib.h
      ${H5SZF_ZLIB_INC_DIR}/crc32.h
      ${H5SZF_ZLIB_INC_DIR}/deflate.h
      ${H5SZF_ZLIB_INC_DIR}/gzguts.h
      ${H5SZF_ZLIB_INC_DIR}/inffast.h
      ${H5SZF_ZLIB_INC_DIR}/inffixed.h
      ${H5SZF_ZLIB_INC_DIR}/inflate.h
      ${H5SZF_ZLIB_INC_DIR}/inftrees.h
      ${H5SZF_ZLIB_INC_DIR}/trees.h
      ${H5SZF_ZLIB_INC_DIR}/zutil.h
  )
  set (H5SZF_SRCS ${H5SZF_SRCS}
      ${H5SZF_ZLIB_SRC_DIR}/adler32.c
      ${H5SZF_ZLIB_SRC_DIR}/compress.c
      ${H5SZF_ZLIB_SRC_DIR}/crc32.c
      ${H5SZF_ZLIB_SRC_DIR}/deflate.c
      ${H5SZF_ZLIB_SRC_DIR}/gzclose.c
      ${H5SZF_ZLIB_SRC_DIR}/gzlib.c
      ${H5SZF_ZLIB_SRC_DIR}/gzread.c
      ${H5SZF_ZLIB_SRC_DIR}/gzwrite.c
      ${H5SZF_ZLIB_SRC_DIR}/inflate.c
      ${H5SZF_ZLIB_SRC_DIR}/infback.c
      ${H5SZF_ZLIB_SRC_DIR}/inftrees.c
      ${H5SZF_ZLIB_SRC_DIR}/inffast.c
      ${H5SZF_ZLIB_SRC_DIR}/trees.c
      ${H5SZF_ZLIB_SRC_DIR}/uncompr.c
      ${H5SZF_ZLIB_SRC_DIR}/zutil.c

  )
  set (H5SZF_HDRS ${H5SZF_HDRS}
      ${H5SZF_ZSTD_INC_DIR}/compress/hist.h
      ${H5SZF_ZSTD_INC_DIR}/compress/zstd_compress_internal.h
      ${H5SZF_ZSTD_INC_DIR}/compress/zstd_compress_literals.h
      ${H5SZF_ZSTD_INC_DIR}/compress/zstd_compress_sequences.h
      ${H5SZF_ZSTD_INC_DIR}/compress/zstd_compress_superblock.h
      ${H5SZF_ZSTD_INC_DIR}/compress/zstd_cwksp.h
      ${H5SZF_ZSTD_INC_DIR}/compress/zstd_double_fast.h
      ${H5SZF_ZSTD_INC_DIR}/compress/zstd_fast.h
      ${H5SZF_ZSTD_INC_DIR}/compress/zstd_lazy.h
      ${H5SZF_ZSTD_INC_DIR}/compress/zstd_ldm.h
      ${H5SZF_ZSTD_INC_DIR}/compress/zstd_opt.h
      ${H5SZF_ZSTD_INC_DIR}/compress/zstdmt_compress.h
      ${H5SZF_ZSTD_INC_DIR}/common/bitstream.h
      ${H5SZF_ZSTD_INC_DIR}/common/compiler.h
      ${H5SZF_ZSTD_INC_DIR}/common/cpu.h
      ${H5SZF_ZSTD_INC_DIR}/common/debug.h
      ${H5SZF_ZSTD_INC_DIR}/common/error_private.h
      ${H5SZF_ZSTD_INC_DIR}/common/fse.h
      ${H5SZF_ZSTD_INC_DIR}/common/huf.h
      ${H5SZF_ZSTD_INC_DIR}/common/mem.h
      ${H5SZF_ZSTD_INC_DIR}/common/pool.h
      ${H5SZF_ZSTD_INC_DIR}/common/threading.h
      ${H5SZF_ZSTD_INC_DIR}/common/xxhash.h
      ${H5SZF_ZSTD_INC_DIR}/common/zstd_depss.h
      ${H5SZF_ZSTD_INC_DIR}/common/zstd_errors.h
      ${H5SZF_ZSTD_INC_DIR}/common/zstd_internal.h
      ${H5SZF_ZSTD_INC_DIR}/decompress/zstd_ddict.h
      ${H5SZF_ZSTD_INC_DIR}/decompress/zstdmt_decompress_block.h
      ${H5SZF_ZSTD_INC_DIR}/decompress/zstdmt_decompress_internal.h
      ${H5SZF_ZSTD_INC_DIR}/zstd.h
  )
  set (H5SZF_SRCS ${H5SZF_SRCS}
      ${H5SZF_ZSTD_SRC_DIR}/common/debug.c
      ${H5SZF_ZSTD_SRC_DIR}/common/entropy_common.c
      ${H5SZF_ZSTD_SRC_DIR}/common/error_private.c
      ${H5SZF_ZSTD_SRC_DIR}/common/fse_decompress.c
      ${H5SZF_ZSTD_SRC_DIR}/common/pool.c
      ${H5SZF_ZSTD_SRC_DIR}/common/threading.c
      ${H5SZF_ZSTD_SRC_DIR}/common/xxhash.c
      ${H5SZF_ZSTD_SRC_DIR}/common/zstd_common.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/fse_compress.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/hist.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/huf_compress.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/zstd_compress_literals.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/zstd_compress_sequences.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/zstd_compress_superblock.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/zstd_compress.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/zstd_double_fast.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/zstd_fast.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/zstd_lazy.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/zstd_ldm.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/zstd_opt.c
      ${H5SZF_ZSTD_SRC_DIR}/compress/zstdmt_compress.c
      ${H5SZF_ZSTD_SRC_DIR}/decompress/huf_decompress.c
      ${H5SZF_ZSTD_SRC_DIR}/decompress/zstd_ddict.c
      ${H5SZF_ZSTD_SRC_DIR}/decompress/zstd_decompress_block.c
      ${H5SZF_ZSTD_SRC_DIR}/decompress/zstd_decompress.c
  )
  set (H5SZF_HDRS ${H5SZF_HDRS}
      ${H5SZF_SZ_INC_DIR}/MultiLevelCacheTable.h
      ${H5SZF_SZ_INC_DIR}/MultiLevelCacheTableWideInterval.h
      ${H5SZF_SZ_INC_DIR}/CacheTable.h
      ${H5SZF_SZ_INC_DIR}/sz_defines.h
      ${H5SZF_SZ_INC_DIR}/CompressElement.h
      ${H5SZF_SZ_INC_DIR}/DynamicDoubleArray.h
      ${H5SZF_SZ_INC_DIR}/rw.h
      ${H5SZF_SZ_INC_DIR}/conf.h
      ${H5SZF_SZ_INC_DIR}/dataCompression.h
      ${H5SZF_SZ_INC_DIR}/dictionary.h
      ${H5SZF_SZ_INC_DIR}/DynamicFloatArray.h
      ${H5SZF_SZ_INC_DIR}/VarSet.h
      ${H5SZF_SZ_INC_DIR}/sz.h
      ${H5SZF_SZ_INC_DIR}/Huffman.h
      ${H5SZF_SZ_INC_DIR}/ByteToolkit.h
      ${H5SZF_SZ_INC_DIR}/sz_float.h
      ${H5SZF_SZ_INC_DIR}/sz_double.h
      ${H5SZF_SZ_INC_DIR}/callZlib.h
      ${H5SZF_SZ_INC_DIR}/iniparser.h
      ${H5SZF_SZ_INC_DIR}/TypeManager.h
      ${H5SZF_SZ_INC_DIR}/sz_int8.h
      ${H5SZF_SZ_INC_DIR}/sz_int16.h
      ${H5SZF_SZ_INC_DIR}/sz_int32.h
      ${H5SZF_SZ_INC_DIR}/sz_int64.h
      ${H5SZF_SZ_INC_DIR}/szd_int8.h
      ${H5SZF_SZ_INC_DIR}/szd_int16.h
      ${H5SZF_SZ_INC_DIR}/szd_int32.h
      ${H5SZF_SZ_INC_DIR}/szd_int64.h
      ${H5SZF_SZ_INC_DIR}/sz_uint8.h
      ${H5SZF_SZ_INC_DIR}/sz_uint16.h
      ${H5SZF_SZ_INC_DIR}/sz_uint32.h
      ${H5SZF_SZ_INC_DIR}/sz_uint64.h
      ${H5SZF_SZ_INC_DIR}/szd_uint8.h
      ${H5SZF_SZ_INC_DIR}/szd_uint16.h
      ${H5SZF_SZ_INC_DIR}/szd_uint32.h
      ${H5SZF_SZ_INC_DIR}/szd_uint64.h
      ${H5SZF_SZ_INC_DIR}/sz_float_pwr.h
      ${H5SZF_SZ_INC_DIR}/sz_double_pwr.h
      ${H5SZF_SZ_INC_DIR}/szd_float.h
      ${H5SZF_SZ_INC_DIR}/szd_double.h
      ${H5SZF_SZ_INC_DIR}/szd_float_pwr.h
      ${H5SZF_SZ_INC_DIR}/szd_double_pwr.h
      ${H5SZF_SZ_INC_DIR}/sz_float_ts.h
      ${H5SZF_SZ_INC_DIR}/szd_float_ts.h
      ${H5SZF_SZ_INC_DIR}/sz_double_ts.h
      ${H5SZF_SZ_INC_DIR}/szd_double_ts.h
      ${H5SZF_SZ_INC_DIR}/utility.h
      ${H5SZF_SZ_INC_DIR}/sz_opencl.h
      ${H5SZF_SZ_INC_DIR}/DynamicByteArray.h
      ${H5SZF_SZ_INC_DIR}/DynamicIntArray.h
      ${H5SZF_SZ_INC_DIR}/TightDataPointStorageI.h
      ${H5SZF_SZ_INC_DIR}/TightDataPointStorageD.h
      ${H5SZF_SZ_INC_DIR}/TightDataPointStorageF.h
      ${H5SZF_SZ_INC_DIR}/pastriD.h
      ${H5SZF_SZ_INC_DIR}/pastriF.h
      ${H5SZF_SZ_INC_DIR}/pastriGeneral.h
      ${H5SZF_SZ_INC_DIR}/pastri.h
      ${H5SZF_SZ_INC_DIR}/exafelSZ.h
      ${H5SZF_SZ_INC_DIR}/ArithmeticCoding.h
      ${H5SZF_SZ_INC_DIR}/sz_omp.h
  )
  set (H5SZF_SRCS ${H5SZF_SRCS}
      ${H5SZF_SZ_SRC_DIR}/ArithmeticCoding.c
      ${H5SZF_SZ_SRC_DIR}/ByteToolkit.c
      ${H5SZF_SZ_SRC_DIR}/CacheTable.c
      ${H5SZF_SZ_SRC_DIR}/callZlib.c
      ${H5SZF_SZ_SRC_DIR}/CompressElement.c
      ${H5SZF_SZ_SRC_DIR}/conf.c
      ${H5SZF_SZ_SRC_DIR}/dataCompression.c
      ${H5SZF_SZ_SRC_DIR}/dictionary.c
      ${H5SZF_SZ_SRC_DIR}/DynamicByteArray.c
      ${H5SZF_SZ_SRC_DIR}/DynamicDoubleArray.c
      ${H5SZF_SZ_SRC_DIR}/DynamicFloatArray.c
      ${H5SZF_SZ_SRC_DIR}/DynamicIntArray.c
      ${H5SZF_SZ_SRC_DIR}/Huffman.c
      ${H5SZF_SZ_SRC_DIR}/iniparser.c
      ${H5SZF_SZ_SRC_DIR}/MultiLevelCacheTable.c
      ${H5SZF_SZ_SRC_DIR}/MultiLevelCacheTableWideInterval.c
      ${H5SZF_SZ_SRC_DIR}/pastri.c
      ${H5SZF_SZ_SRC_DIR}/exafelSZ.c
      ${H5SZF_SZ_SRC_DIR}/rw.c
      ${H5SZF_SZ_SRC_DIR}/rwf.c
      ${H5SZF_SZ_SRC_DIR}/sz.c
      ${H5SZF_SZ_SRC_DIR}/szd_double.c
      ${H5SZF_SZ_SRC_DIR}/szd_double_pwr.c
      ${H5SZF_SZ_SRC_DIR}/szd_double_ts.c
      ${H5SZF_SZ_SRC_DIR}/szd_float.c
      ${H5SZF_SZ_SRC_DIR}/szd_float_pwr.c
      ${H5SZF_SZ_SRC_DIR}/szd_float_ts.c
      ${H5SZF_SZ_SRC_DIR}/szd_int16.c
      ${H5SZF_SZ_SRC_DIR}/szd_int32.c
      ${H5SZF_SZ_SRC_DIR}/szd_int64.c
      ${H5SZF_SZ_SRC_DIR}/szd_int8.c
      ${H5SZF_SZ_SRC_DIR}/sz_double.c
      ${H5SZF_SZ_SRC_DIR}/sz_double_pwr.c
      ${H5SZF_SZ_SRC_DIR}/sz_double_ts.c
      ${H5SZF_SZ_SRC_DIR}/szd_uint16.c
      ${H5SZF_SZ_SRC_DIR}/szd_uint32.c
      ${H5SZF_SZ_SRC_DIR}/szd_uint64.c
      ${H5SZF_SZ_SRC_DIR}/szd_uint8.c
      ${H5SZF_SZ_SRC_DIR}/szf.c
      ${H5SZF_SZ_SRC_DIR}/sz_float.c
      ${H5SZF_SZ_SRC_DIR}/sz_float_pwr.c
      ${H5SZF_SZ_SRC_DIR}/sz_float_ts.c
      ${H5SZF_SZ_SRC_DIR}/sz_int16.c
      ${H5SZF_SZ_SRC_DIR}/sz_int32.c
      ${H5SZF_SZ_SRC_DIR}/sz_int64.c
      ${H5SZF_SZ_SRC_DIR}/sz_int8.c
      ${H5SZF_SZ_SRC_DIR}/sz_omp.c
      ${H5SZF_SZ_SRC_DIR}/sz_uint16.c
      ${H5SZF_SZ_SRC_DIR}/sz_uint32.c
      ${H5SZF_SZ_SRC_DIR}/sz_uint64.c
      ${H5SZF_SZ_SRC_DIR}/sz_uint8.c
      ${H5SZF_SZ_SRC_DIR}/TightDataPointStorageD.c
      ${H5SZF_SZ_SRC_DIR}/TightDataPointStorageF.c
      ${H5SZF_SZ_SRC_DIR}/TightDataPointStorageI.c
      ${H5SZF_SZ_SRC_DIR}/TypeManager.c
      ${H5SZF_SZ_SRC_DIR}/utility.c
      ${H5SZF_SZ_SRC_DIR}/VarSet.c
  )
endif ()

add_library (${H5SZF_LIB_TARGET} MODULE ${H5SZF_SRCS})
if (DISABLE_H5SZF_ENCODER)
  set_target_properties(${H5SZF_LIB_TARGET} PROPERTIES COMPILE_DEFINITIONS "FILTER_DECODE_ONLY")
endif ()
target_include_directories(${H5SZF_LIB_TARGET} PRIVATE ${H5SZF_INCLUDE_DIRS})
#set_property (TARGET ${H5SZF_LIB_TARGET} APPEND PROPERTY COMPILE_DEFINITIONS H5_SZ_DEBUG)
target_link_libraries (${H5SZF_LIB_TARGET} PRIVATE ${LINK_LIBS})
if (WIN32)
  target_link_libraries (${H5SZF_LIB_TARGET} PRIVATE "ws2_32.lib")
endif ()
set_global_variable (H5SZF_LIBRARIES_TO_EXPORT ${H5SZF_LIB_TARGET})
HDF_SET_LIB_VERSIONS ("H5SZF" ${H5SZF_LIB_TARGET} ${H5SZF_LIB_NAME} MODULE)

if (BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5SZF_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "$<TARGET_FILE:${H5SZF_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5SZF_LIB_TARGET}>"
  )
  #-----------------------------------------------------------------------------
  # Copy plugin config to a plugins folder
  #-----------------------------------------------------------------------------
  add_custom_command (
      TARGET     ${H5SZF_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "${H5SZF_SRC_SOURCE_DIR}/sz.config"
        "${CMAKE_BINARY_DIR}/plugins/sz.config"
  )
endif ()

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5SZF_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5SZF_LIB_TARGET}
      EXPORT
          ${H5SZF_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5SZF_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5SZF_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5SZF_INSTALL_BIN_DIR} COMPONENT libraries
  )
  install (
      FILES
          ${H5SZF_SRC_SOURCE_DIR}/sz.config
      DESTINATION ${H5SZF_INSTALL_LIB_DIR} COMPONENT libraries
  )
endif ()
