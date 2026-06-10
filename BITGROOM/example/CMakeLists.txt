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
project (HDF5BITGROOM_EX C)

set (dyn_examples
    h5ex_d_bitgroom
)

if (USE_SHARED_LIBS)
  add_definitions (-DH5_BUILT_AS_DYNAMIC_LIB)
else ()
  add_definitions (-DH5_BUILT_AS_STATIC_LIB)
endif ()

#run-time loadable bitgroom library examples
foreach (example ${dyn_examples})
  add_executable (${example} ${PROJECT_SOURCE_DIR}/${example}.c)
  TARGET_C_PROPERTIES (${example} ${LIB_TYPE})
  target_link_libraries (${example} PRIVATE ${H5PL_HDF5_LINK_LIBS})
  if (NOT WIN32)
    target_link_libraries (${example} PRIVATE ${CMAKE_DL_LIBS})
  endif ()
endforeach ()

if (H5PL_BUILD_TESTING)
  # h5repack test input generator. Target name is plugin-suffixed because the
  # BITROUND and BITGROOM trees share the same CMake project and source file
  # basename (issue #240).
  add_executable (h5repack_floats_bitgroom ${PROJECT_SOURCE_DIR}/h5repack_floats.c)
  TARGET_C_PROPERTIES (h5repack_floats_bitgroom ${LIB_TYPE})
  target_link_libraries (h5repack_floats_bitgroom PRIVATE ${H5PL_HDF5_LINK_LIBS})
  if (NOT WIN32)
    target_link_libraries (h5repack_floats_bitgroom PRIVATE dl)
  endif ()

  # Round-trip test for special floating-point values (NaN, +/- Inf, +/- zero).
  # See the test definition lower in this file for the full description.
  add_executable (h5filter_specials_bitgroom ${PROJECT_SOURCE_DIR}/h5filter_specials_bitgroom.c)
  TARGET_C_PROPERTIES (h5filter_specials_bitgroom ${LIB_TYPE})
  target_link_libraries (h5filter_specials_bitgroom PRIVATE ${H5PL_HDF5_LINK_LIBS})
  if (NOT WIN32)
    target_link_libraries (h5filter_specials_bitgroom PRIVATE dl)
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
    if (DISABLE_H5BITGROOM_ENCODER)
      add_test (
          NAME ${testname}-ERR
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=$<TARGET_FILE:${testname}>"
              -D "TEST_ARGS:STRING=${ARGN}"
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
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5BITGROOM_RESOURCES_DIR}/runTest.cmake"
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
              -D "TEST_ARGS:STRING=${ARGN}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_EXPECT=0"
              -D "TEST_OUTPUT=${testname}.out"
              -D "TEST_REFERENCE=${testname}.tst"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5BITGROOM_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (${testname} PROPERTIES DEPENDS ${testname}-clearall)
    endif ()
    set (last_test "${testname}")
    if (HDF5_BUILD_TOOLS OR HDF5_PROVIDES_TOOLS)
      add_test (
          NAME H5DUMP-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${H5PL_HDF5_DUMP_EXECUTABLE}"
              -D "TEST_ARGS:STRING=--enable-error-stack;-p;${testname}.h5"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_OUTPUT=${testname}.ddl.out"
              -D "TEST_EXPECT=0"
              -D "TEST_REFERENCE=${testname}.ddl"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5BITGROOM_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (H5DUMP-${testname} PROPERTIES
          WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
          DEPENDS ${testname})
      set (last_test "H5DUMP-${testname}")
    endif ()
  endmacro ()

  macro (ADD_H5_UD_TEST testname resultcode resultfile)
    # Remove any output file left over from previous test run
    add_test (
        NAME H5BITGROOM_UD-${testname}-clearall-objects
        COMMAND    ${CMAKE_COMMAND}
            -E remove
            ${resultfile}
            out-${testname}.${resultfile}
            ${testname}.${resultfile}.out
            ${testname}.${resultfile}.out.err
            ${resultfile}-${testname}.out
            ${resultfile}-${testname}.out.err
    )
    if (NOT "${last_test}" STREQUAL "")
      set_tests_properties (H5BITGROOM_UD-${testname}-clearall-objects PROPERTIES DEPENDS ${last_test})
    endif ()
    set (last_test "H5BITGROOM_UD-${testname}-clearall-objects")
    if (WIN32)
      set (TESTLIBDIR "${HDF5_TOOLS_DIR}")
    else ()
      set (TESTLIBDIR "${HDF5_LIBRARY_PATH}")
    endif ()
    # Generate the float-data input file for h5repack. The BitGroom filter's
    # set_local() callback removes the filter from the pipeline on non-float
    # datasets, so the input must contain float data for the test to actually
    # exercise the filter (issue #240). Routed through runTest.cmake so
    # PATH/DYLD_LIBRARY_PATH/LD_LIBRARY_PATH are set up for the helper to find
    # the HDF5 runtime library on every platform (Windows in particular has
    # no rpath equivalent and reports STATUS_DLL_NOT_FOUND if the HDF5 DLL is
    # not on PATH).
    add_test (
        NAME H5BITGROOM_UD-${testname}-gen-input
        COMMAND "${CMAKE_COMMAND}"
            -D "TEST_PROGRAM=$<TARGET_FILE:h5repack_floats_bitgroom>"
            -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
            -D "TEST_EXPECT=0"
            -D "TEST_OUTPUT=h5repack_floats_bitgroom.out"
            -D "TEST_SKIP_COMPARE=1"
            -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
            -P "${H5BITGROOM_RESOURCES_DIR}/runTest.cmake"
    )
    set_tests_properties (H5BITGROOM_UD-${testname}-gen-input PROPERTIES
        WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
        DEPENDS H5BITGROOM_UD-${testname}-clearall-objects)
    set (last_test "H5BITGROOM_UD-${testname}-gen-input")
    if (HDF5_BUILD_TOOLS OR HDF5_PROVIDES_TOOLS)
      add_test (
          NAME H5BITGROOM_UD-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${H5PL_HDF5_REPACK_EXECUTABLE}"
              -D "TEST_ARGS:STRING=${ARGN};${PROJECT_BINARY_DIR}/${resultfile};${PROJECT_BINARY_DIR}/out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_FILTER:STRING=O?...ing file[^\n]+\n"
              -D "TEST_OUTPUT=${testname}.${resultfile}.out"
              -D "TEST_REFERENCE=${testname}.${resultfile}.tst"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5BITGROOM_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (H5BITGROOM_UD-${testname} PROPERTIES
          WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
          DEPENDS H5BITGROOM_UD-${testname}-gen-input)
      add_test (
          NAME H5BITGROOM_UD-h5dump-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${H5PL_HDF5_DUMP_EXECUTABLE}"
              -D "TEST_ARGS:STRING=-pH;out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_OUTPUT=${resultfile}-${testname}.out"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_REFERENCE=${resultfile}-${testname}.ddl"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5BITGROOM_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (H5BITGROOM_UD-h5dump-${testname} PROPERTIES
          WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
          DEPENDS H5BITGROOM_UD-${testname})
      set (last_test "H5BITGROOM_UD-h5dump-${testname}")
      # Verify that the filter actually modifies the dataset values: dump the
      # data (no -pH) and compare against a captured reference of the quantized
      # output. BitGroom's encoding is bit-deterministic across platforms, so
      # the captured reference is portable. The companion BITROUND test suite
      # intentionally omits an equivalent check because the testing indicated
      # the Granular BitRound encoding introduces platform-dependent
      # 1-bit-different stored output.
      add_test (
          NAME H5BITGROOM_UD-h5dump-data-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${H5PL_HDF5_DUMP_EXECUTABLE}"
              -D "TEST_ARGS:STRING=out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_OUTPUT=${resultfile}-${testname}-data.out"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_REFERENCE=${resultfile}-${testname}-data.ddl"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5BITGROOM_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (H5BITGROOM_UD-h5dump-data-${testname} PROPERTIES
          WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
          DEPENDS H5BITGROOM_UD-h5dump-${testname})
      set (last_test "H5BITGROOM_UD-h5dump-data-${testname}")
    endif ()
  endmacro ()

  # --------------------------------------------------------------------
  # Copy all the HDF5 files from the source directory into the test directory
  # --------------------------------------------------------------------
  set (LIST_OTHER_TEST_FILES
      h5ex_d_bitgroom.ddl
      h5ex_d_bitgroom.tst
      h5repack_floats.h5-ud_convert.ddl
      h5repack_floats.h5-ud_convert-data.ddl
      ud_convert.h5repack_floats.h5.tst
  )

  foreach (h5_file ${LIST_OTHER_TEST_FILES})
    HDFTEST_COPY_FILE("${PROJECT_SOURCE_DIR}/testfiles/${h5_file}" "${PROJECT_BINARY_DIR}/${h5_file}" "bitgroom_files")
  endforeach ()
  add_custom_target(bitgroom_files ALL COMMENT "Copying files needed by bitgroom tests" DEPENDS ${bitgroom_files_list})

  ADD_H5_TEST (h5ex_d_bitgroom)

  if (NOT DISABLE_H5BITGROOM_ENCODER)
    #UD BITGROOM
    ADD_H5_UD_TEST (ud_convert 0 h5repack_floats.h5 --enable-error-stack -v -f UD=32022,0,5,3,4,0,0,0 -l CHUNK=4x8)

    # Special-value round-trip test: writes a small chunked dataset containing
    # NaN, +/- Inf, +/- zero, plus normal values; applies the BitGroom filter
    # via H5Pset_filter; reads back; and asserts in C that the special values
    # are preserved bit-exact while at least one normal value was quantized.
    # BitGroom alternates shave (AND) on even indices and set (OR) on odd
    # indices; the special values straddle both. The verdict comes from the
    # program's exit code; no h5dump comparison is involved, so cross-platform
    # NaN/Inf printf formatting cannot affect the result. TESTLIBDIR is
    # already set by ADD_H5_UD_TEST above.
    add_test (
        NAME H5BITGROOM_UD-specials-roundtrip-clearall
        COMMAND ${CMAKE_COMMAND} -E remove
            h5filter_specials_bitgroom.h5
            h5filter_specials_bitgroom.out
            h5filter_specials_bitgroom.out.err
    )
    if (NOT "${last_test}" STREQUAL "")
      set_tests_properties (H5BITGROOM_UD-specials-roundtrip-clearall PROPERTIES DEPENDS ${last_test})
    endif ()
    set (last_test "H5BITGROOM_UD-specials-roundtrip-clearall")
    add_test (
        NAME H5BITGROOM_UD-specials-roundtrip
        COMMAND "${CMAKE_COMMAND}"
            -D "TEST_PROGRAM=$<TARGET_FILE:h5filter_specials_bitgroom>"
            -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
            -D "TEST_EXPECT=0"
            -D "TEST_OUTPUT=h5filter_specials_bitgroom.out"
            -D "TEST_SKIP_COMPARE=1"
            -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
            -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
            -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
            -P "${H5BITGROOM_RESOURCES_DIR}/runTest.cmake"
    )
    set_tests_properties (H5BITGROOM_UD-specials-roundtrip PROPERTIES
        WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
        DEPENDS H5BITGROOM_UD-specials-roundtrip-clearall)
    set (last_test "H5BITGROOM_UD-specials-roundtrip")
  endif ()

endif ()
