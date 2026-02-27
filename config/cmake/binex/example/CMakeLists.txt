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
cmake_minimum_required (VERSION 3.18)
project (HDF5PL_EX C)

set (dyn_examples)
if (BLOSC_AVAILABLE)
  set (dyn_examples ${dyn_examples} h5ex_d_blosc)
endif ()
if (BLOSC2_AVAILABLE)
  set (dyn_examples ${dyn_examples} h5ex_d_blosc2)
endif ()
if (BITGROOM_AVAILABLE)
  set (dyn_examples ${dyn_examples} h5ex_d_bitgroom)
endif ()
if (BITROUND_AVAILABLE)
  set (dyn_examples ${dyn_examples} h5ex_d_granularbr)
endif ()
if (BSHUF_AVAILABLE)
  set (dyn_examples ${dyn_examples} h5ex_d_bshuf)
endif ()
if (BZIP2_AVAILABLE)
  set (dyn_examples ${dyn_examples} h5ex_d_bzip2)
endif ()
if (LZ4_AVAILABLE)
  set (dyn_examples ${dyn_examples} h5ex_d_lz4)
endif ()
if (LZF_AVAILABLE)
  set (dyn_examples ${dyn_examples} h5ex_d_lzf)
endif ()
if (JPEG_AVAILABLE)
  set (dyn_examples ${dyn_examples} h5ex_d_jpeg)
endif ()
if (ZSTD_AVAILABLE)
  set (extra_examples ${extra_examples} h5ex_d_zstd)
endif ()

set (extra_examples)
if (ZFP_AVAILABLE)
  set (extra_examples ${extra_examples} h5ex_d_zfp)
endif ()

if (USE_SHARED_LIBS)
  add_definitions (-DH5_BUILT_AS_DYNAMIC_LIB)
else ()
  add_definitions (-DH5_BUILT_AS_STATIC_LIB)
endif ()

#run-time loadable library examples
foreach (example ${dyn_examples} ${extra_examples})
  add_executable (${example} ${PROJECT_SOURCE_DIR}/${example}.c)
  target_include_directories(${example} PRIVATE ${H5PL_HDF5_INCLUDE_DIRS})
  TARGET_C_PROPERTIES (${example} ${LIB_TYPE})
  target_link_libraries (${example} PRIVATE ${H5PL_HDF5_LINK_LIBS})
  if (NOT WIN32)
    target_link_libraries (${example} PRIVATE dl)
  endif ()
endforeach ()

if (H5PL_BUILD_TESTING)
  # Set plugin path relative to PROJECT_SOURCE_DIR
  # Windows: HDF5/HDFPLExamples/example -> ../../lib/plugin
  # Unix/Mac: HDF5/share/HDFPLExamples/example -> ../../../lib/plugin
  if (WIN32)
    set (_plugin_path "${PROJECT_SOURCE_DIR}/../../lib/plugin")
  else ()
    set (_plugin_path "${PROJECT_SOURCE_DIR}/../../../lib/plugin")
    set (_plugin_path "${_plugin_path}:${PROJECT_SOURCE_DIR}/../../../lib64/plugin")
    set (_plugin_path "${_plugin_path}:${PROJECT_SOURCE_DIR}/../../../plugin")
  endif ()

  macro (ADD_H5_TEST testname)
    add_test (
        NAME ${testname}-clearall
        COMMAND    ${CMAKE_COMMAND}
            -E remove
            ${testname}.out
            ${testname}.out.err
            ${testname}.ddl.out
            ${testname}.ddl.out.err
            ${testname}.h5
    )
    if (NOT "${last_test}" STREQUAL "")
      set_tests_properties (${testname}-clearall PROPERTIES DEPENDS ${last_test})
    endif ()
    set (last_test "${testname}-clearall")
    if (WIN32)
      set (TESTLIBDIR "${HDF5_TOOLS_DIR}")
    else ()
      set (TESTLIBDIR "${HDF5_LIBRARY_PATH}")
    endif ()
    if (DISABLE_H5PL_ENCODER)
      add_test (
          NAME ${testname}-ERR
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=$<TARGET_FILE:${testname}>"
              -D "TEST_ARGS:STRING="
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_EXPECT=1"
              -D "TEST_MASK_ERROR=true"
              -D "TEST_OUTPUT=${testname}.out"
              -D "TEST_REFERENCE=${testname}.tst"
              -D "TEST_SKIP_COMPARE=1"
              -D "TEST_ERRREF=1"
              -D "GREP_ERRREF=Filter present but encoding disabled"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${_plugin_path}"
              -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (${testname}-ERR PROPERTIES DEPENDS ${testname}-clearall)
      add_test (
          NAME ${testname}
          COMMAND "${CMAKE_COMMAND}"
              -E copy_if_different
              "${PROJECT_SOURCE_DIR}/testfiles/${testname}.h5" "${PROJECT_BINARY_DIR}/${testname}.h5"
      )
      set_tests_properties (${testname} PROPERTIES DEPENDS ${testname}-ERR)
    else ()
      add_test (
          NAME ${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=$<TARGET_FILE:${testname}>"
              -D "TEST_ARGS:STRING="
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_EXPECT=0"
              -D "TEST_OUTPUT=${testname}.out"
              -D "TEST_REFERENCE=${testname}.tst"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${_plugin_path}"
              -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (${testname} PROPERTIES DEPENDS ${testname}-clearall)
    endif ()
    set (last_test "${testname}")
    if (HDF5_BUILD_TOOLS OR HDF5_PROVIDES_TOOLS)
      if (NOT "${ARGN}" STREQUAL "FILTERALL")
        add_test (
            NAME H5DUMP-${testname}
            COMMAND "${CMAKE_COMMAND}"
               -D "TEST_PROGRAM=${H5PL_HDF5_DUMP_EXECUTABLE}"
                -D "TEST_ARGS:STRING=--enable-error-stack;-p;${testname}.h5"
                -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
                -D "TEST_OUTPUT=${testname}.ddl.out"
                -D "TEST_FILTER=PARAMS { ([0-9]) [-]?[0-9]+ ([0-9] [0-9] [0-9] [0-9] [0-9] [0-9]) }\n"
                -D "TEST_FILTER_REPLACE=PARAMS { \\1 XXXX \\2 }\n"
                -D "TEST_EXPECT=0"
                -D "TEST_REFERENCE=${testname}.ddl"
                -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
                -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
                -D "TEST_ENV_VALUE=${_plugin_path}"
                -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
        )
      else ()
        if (NOT "${ARGN}" STREQUAL "TEST_MASK_MOD")
          add_test (
              NAME H5DUMP-${testname}
              COMMAND "${CMAKE_COMMAND}"
                  -D "TEST_PROGRAM=${H5PL_HDF5_DUMP_EXECUTABLE}"
                  -D "TEST_ARGS:STRING=--enable-error-stack;-p;${testname}.h5"
                  -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
                  -D "TEST_OUTPUT=${testname}.ddl.out"
                  -D "TEST_FILTER:STRING=PARAMS {[ -0-9]*}"
                  -D "TEST_FILTER_REPLACE:STRING=PARAMS { XXXX }"
                  -D "TEST_EXPECT=0"
                  -D "TEST_REFERENCE=${testname}.ddl"
                  -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
                  -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
                  -D "TEST_ENV_VALUE=${_plugin_path}"
                  -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
          )
        else ()
          add_test (
              NAME H5DUMP-${testname}
              COMMAND "${CMAKE_COMMAND}"
                  -D "TEST_PROGRAM=${H5PL_HDF5_DUMP_EXECUTABLE}"
                  -D "TEST_ARGS:STRING=--enable-error-stack;-p;${testname}.h5"
                  -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
                  -D "TEST_OUTPUT=${testname}.ddl.out"
                  -D "TEST_FILTER:STRING=PARAMS {[ -0-9]*}"
                  -D "TEST_FILTER_REPLACE:STRING=PARAMS { XXXX }"
                  -D "TEST_EXPECT=0"
                  -D "TEST_MASK_MOD=1"
                  -D "TEST_REFERENCE=${testname}.ddl"
                  -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
                  -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
                  -D "TEST_ENV_VALUE=${_plugin_path}"
                  -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
          )
        endif ()
      endif ()
      set_tests_properties (H5DUMP-${testname} PROPERTIES
          WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
          DEPENDS ${testname})
      # This test currently fails on ARM64 Windows and needs more investigation
      if ("H5DUMP-${testname}" MATCHES "H5DUMP-h5ex_d_granularbr" AND MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
        if (CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|AARCH64" OR CMAKE_C_COMPILER_ARCHITECTURE_ID MATCHES "ARM64")
          set_tests_properties (H5DUMP-${testname} PROPERTIES DISABLED TRUE)
        endif ()
      endif ()
      set (last_test "H5DUMP-${testname}")
    endif ()
  endmacro ()

  macro (ADD_H5_UD_TEST testname resultcode resultfile deptest)
    # Remove any output file left over from previous test run
    add_test (
        NAME H5PL_UD-${testname}-clearall-objects
        COMMAND    ${CMAKE_COMMAND}
            -E remove
            out-${testname}.${resultfile}
            ${testname}.${resultfile}.out
            ${testname}.${resultfile}.out.err
            ${resultfile}-${testname}.out
            ${resultfile}-${testname}.out.err
    )
    if (NOT "${deptest}" STREQUAL "NONE")
      set_tests_properties (H5PL_UD-${testname}-clearall-objects PROPERTIES DEPENDS H5PL_UD-${deptest})
    endif ()
    set (last_test "H5PL_UD-${testname}-clearall-objects")
    if (WIN32)
      set (${TESTLIBDIR} "${HDF5_TOOLS_DIR}")
    else ()
      set (${TESTLIBDIR} "${HDF5_LIBRARY_PATH}")
    endif ()
    if (HDF5_BUILD_TOOLS OR HDF5_PROVIDES_TOOLS)
      if ("${deptest}" STREQUAL "NONE")
        set (testfile ${resultfile})
      else ()
        set (testfile out-${deptest}.${resultfile})
      endif ()
      add_test (
          NAME H5PL_UD-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${H5PL_HDF5_REPACK_EXECUTABLE}"
              -D "TEST_ARGS:STRING=--enable-error-stack;${ARGN};${PROJECT_BINARY_DIR}/${testfile};${PROJECT_BINARY_DIR}/out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_FILTER:STRING=O?...ing file[^\n]+\n"
              -D "TEST_OUTPUT=${testname}.${resultfile}.out"
              -D "TEST_REFERENCE=${testname}.${resultfile}.tst"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${_plugin_path}"
              -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (H5PL_UD-${testname} PROPERTIES DEPENDS H5PL_UD-${testname}-clearall-objects)
      set (deptest "H5PL_UD-${testname}")
    endif ()
  endmacro ()

  macro (ADD_H5_UD_VERIFY testname resultcode resultfile deptest)
    # Remove any output file left over from previous test run
    if (HDF5_BUILD_TOOLS OR HDF5_PROVIDES_TOOLS)
      if (NOT "${ARGN}" STREQUAL "FILTERALL")
        add_test (
            NAME H5PL_UD-h5dump-${testname}
            COMMAND "${CMAKE_COMMAND}"
                -D "TEST_PROGRAM=${H5PL_HDF5_DUMP_EXECUTABLE}"
                -D "TEST_ARGS:STRING=--enable-error-stack;-pH;out-${deptest}.${resultfile}"
                -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
                -D "TEST_OUTPUT=${resultfile}-${testname}.out"
                -D "TEST_EXPECT=${resultcode}"
                -D "TEST_REFERENCE=${resultfile}-${testname}.ddl"
                -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
                -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
                -D "TEST_ENV_VALUE=${_plugin_path}"
                -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
        )
      else ()
        add_test (
            NAME H5PL_UD-h5dump-${testname}
            COMMAND "${CMAKE_COMMAND}"
                -D "TEST_PROGRAM=${H5PL_HDF5_DUMP_EXECUTABLE}"
                -D "TEST_ARGS:STRING=--enable-error-stack;-pH;out-${deptest}.${resultfile}"
                -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
                -D "TEST_OUTPUT=${resultfile}-${testname}.out"
                -D "TEST_FILTER:STRING=PARAMS {[ -0-9]*}"
                -D "TEST_FILTER_REPLACE:STRING=PARAMS { XXXX }"
                -D "TEST_EXPECT=${resultcode}"
                -D "TEST_REFERENCE=${resultfile}-${testname}.ddl"
                -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
                -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
                -D "TEST_ENV_VALUE=${_plugin_path}"
                -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
        )
      endif ()
      if (NOT "${last_test}" STREQUAL "")
        set_tests_properties (H5PL_UD-h5dump-${testname} PROPERTIES
          WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
          DEPENDS H5PL_UD-${deptest})
      endif ()
      set (last_test "H5PL_UD-h5dump-${testname}")
    endif ()
  endmacro ()

  # --------------------------------------------------------------------
  # Copy all the HDF5 files from the source directory into the test directory
  # --------------------------------------------------------------------
  set (LIST_HDF5_TEST_FILES
      h5repack_layout.h5
  )
  set (LIST_OTHER_TEST_FILES
      h5repack_layout.h5-ud_pl_convert.ddl
      h5repack_layout.h5-ud_pl_lz4_convert.ddl
      h5repack_layout.h5-ud_pl_blosc_convert.ddl
      h5repack_layout.h5-ud_pl_bz2_convert.ddl
      ud_convert.h5repack_layout.h5.tst
      ud_bz2_convert.h5repack_layout.h5.tst
      ud_blosc_convert.h5repack_layout.h5.tst
      ud_lz4_convert.h5repack_layout.h5.tst
      ud_mafisc_convert.h5repack_layout.h5.tst
  )

  foreach (h5_file ${dyn_examples})
    HDFTEST_COPY_FILE("${PROJECT_SOURCE_DIR}/testfiles/${h5_file}.tst" "${PROJECT_BINARY_DIR}/${h5_file}.tst" "example_files")
    HDFTEST_COPY_FILE("${PROJECT_SOURCE_DIR}/testfiles/${h5_file}.ddl" "${PROJECT_BINARY_DIR}/${h5_file}.ddl" "example_files")
  endforeach ()
  foreach (h5_file ${LIST_HDF5_TEST_FILES} ${LIST_OTHER_TEST_FILES})
    HDFTEST_COPY_FILE("${PROJECT_SOURCE_DIR}/testfiles/${h5_file}" "${PROJECT_BINARY_DIR}/${h5_file}" "example_files")
  endforeach ()
  add_custom_target(example_files ALL COMMENT "Copying files needed by example tests" DEPENDS ${example_files_list})

  foreach (h5_file ${dyn_examples})
    ADD_H5_TEST (${h5_file})
  endforeach ()
  if (SZ_AVAILABLE)
    ## special test compare
    ADD_H5_TEST (h5ex_d_sz TEST_MASK_MOD)
  endif ()
  ## special filter
  ADD_H5_TEST (h5ex_d_zfp FILTERALL)

  if (NOT DISABLE_H5PL_ENCODER)
    #UD BZ2$
    ADD_H5_UD_TEST (ud_bz2_convert 0 h5repack_layout.h5 NONE -v -f /dset1:UD=307,0,1,9)
    ADD_H5_UD_VERIFY (ud_pl_bz2_convert 0 h5repack_layout.h5 ud_bz2_convert)
    #UD BLOSC$
    ADD_H5_UD_TEST (ud_blosc_convert 0 h5repack_layout.h5 ud_bz2_convert -v -f /dset2:UD=32001,0,7,0,0,0,0,5,1,0)
    ADD_H5_UD_VERIFY (ud_pl_blosc_convert 0 h5repack_layout.h5 ud_blosc_convert)
    #UD LZ4$
    ADD_H5_UD_TEST (ud_lz4_convert 0 h5repack_layout.h5 ud_blosc_convert -v -f /dset3:UD=32004,0,1,3 -l /dset1:CHUNK=4x8)
    ADD_H5_UD_VERIFY (ud_pl_lz4_convert 0 h5repack_layout.h5 ud_lz4_convert)
    if (MAFISC_AVAILABLE)
      #UD MAFISC$
      ADD_H5_UD_TEST (ud_mafisc_convert 0 h5repack_layout.h5 ud_lz4_convert -v -f /dset4:UD=32013,1,0,0 -l CHUNK=10x10)
      #UD VERIFY
      ADD_H5_UD_VERIFY (ud_pl_convert 0 h5repack_layout.h5 ud_mafisc_convert FILTERALL)
    endif ()
  endif ()

endif ()
