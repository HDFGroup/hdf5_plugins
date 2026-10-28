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
project (HDFPL_EX C)

set (dyn_examples
    h5ex_d_bitround
)

if (USE_SHARED_LIBS)
  add_definitions (-DH5_BUILT_AS_DYNAMIC_LIB)
else ()
  add_definitions (-DH5_BUILT_AS_STATIC_LIB)
endif ()

# run-time loadable bitround library examples
foreach (example ${dyn_examples})
  add_executable (${example} ${PROJECT_SOURCE_DIR}/${example}.c)
  target_include_directories (${example} PRIVATE ${H5PL_HDF5_INCLUDE_DIRS})
  TARGET_C_PROPERTIES (${example} ${LIB_TYPE})
  target_link_libraries (${example} PRIVATE ${H5PL_HDF5_LINK_LIBS})
  if (NOT WIN32)
    target_link_libraries (${example} PRIVATE ${CMAKE_DL_LIBS})
  endif ()
endforeach ()

if (H5PL_BUILD_TESTING)
  if (WIN32)
    set (TESTLIBDIR "${HDF5_TOOLS_DIR}")
  else ()
    set (TESTLIBDIR "${HDF5_LIBRARY_PATH}")
  endif ()

  # h5repack test-input generator. Target name is plugin-suffixed because the
  # BITROUND and BITGROOM trees share the same CMake project and source file
  # basename (issue #240).
  add_executable (h5repack_floats_bitround ${PROJECT_SOURCE_DIR}/h5repack_floats.c)
  target_include_directories (h5repack_floats_bitround PRIVATE ${H5PL_HDF5_INCLUDE_DIRS})
  TARGET_C_PROPERTIES (h5repack_floats_bitround ${LIB_TYPE})
  target_link_libraries (h5repack_floats_bitround PRIVATE ${H5PL_HDF5_LINK_LIBS})
  if (NOT WIN32)
    target_link_libraries (h5repack_floats_bitround PRIVATE ${CMAKE_DL_LIBS})
  endif ()

  # Register an exit-code-only test: run the program against the installed
  # plugin found through HDF5_PLUGIN_PATH, and require exit code 0. The program
  # is self-checking (round-trip plus error-bound assertions in C), so no
  # reference file (.ddl/.tst) comparison is needed and cross-platform float
  # formatting cannot affect the result.
  macro (ADD_BR_EXIT_TEST testname prog)
    add_test (
        NAME ${testname}-clearall
        COMMAND ${CMAKE_COMMAND} -E remove ${testname}.out ${testname}.out.err
    )
    if (NOT "${last_test}" STREQUAL "")
      set_tests_properties (${testname}-clearall PROPERTIES DEPENDS ${last_test})
    endif ()
    add_test (
        NAME ${testname}
        COMMAND "${CMAKE_COMMAND}"
            -D "TEST_PROGRAM=$<TARGET_FILE:${prog}>"
            -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
            -D "TEST_EXPECT=0"
            -D "TEST_OUTPUT=${testname}.out"
            -D "TEST_SKIP_COMPARE=1"
            -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
            -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
    )
    set_tests_properties (${testname} PROPERTIES
        WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
        DEPENDS ${testname}-clearall)
    set (last_test "${testname}")
  endmacro ()

  # Reference file h5repack round-trip test: generate a chunked float dataset,
  # repack it through the BitRound UD filter, then h5dump the result and compare
  # against captured references. BitRound is fully deterministic (integer
  # add-and-shave, no libm), so both the filter metadata (-pH) and the quantized
  # data dumps are bit-reproducible across platforms and make safe reference
  # files.
  macro (ADD_H5_UD_TEST testname resultcode resultfile)
    add_test (
        NAME H5BITROUND_UD-${testname}-clearall-objects
        COMMAND ${CMAKE_COMMAND}
            -E remove
            ${resultfile}
            out-${testname}.${resultfile}
            ${testname}.${resultfile}.out
            ${testname}.${resultfile}.out.err
            ${resultfile}-${testname}.out
            ${resultfile}-${testname}.out.err
            ${resultfile}-${testname}-data.out
            ${resultfile}-${testname}-data.out.err
    )
    if (NOT "${last_test}" STREQUAL "")
      set_tests_properties (H5BITROUND_UD-${testname}-clearall-objects PROPERTIES DEPENDS ${last_test})
    endif ()
    set (last_test "H5BITROUND_UD-${testname}-clearall-objects")

    # Generate the float-data input for h5repack. Routed through runTest.cmake
    # so the runtime library path is set up for the helper on every platform.
    add_test (
        NAME H5BITROUND_UD-${testname}-gen-input
        COMMAND "${CMAKE_COMMAND}"
            -D "TEST_PROGRAM=$<TARGET_FILE:h5repack_floats_bitround>"
            -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
            -D "TEST_EXPECT=0"
            -D "TEST_OUTPUT=h5repack_floats_bitround.out"
            -D "TEST_SKIP_COMPARE=1"
            -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
            -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
    )
    set_tests_properties (H5BITROUND_UD-${testname}-gen-input PROPERTIES
        WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
        DEPENDS H5BITROUND_UD-${testname}-clearall-objects)
    set (last_test "H5BITROUND_UD-${testname}-gen-input")

    if (HDF5_BUILD_TOOLS OR HDF5_PROVIDES_TOOLS)
      add_test (
          NAME H5BITROUND_UD-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${H5PL_HDF5_REPACK_EXECUTABLE}"
              -D "TEST_ARGS:STRING=${ARGN};${PROJECT_BINARY_DIR}/${resultfile};${PROJECT_BINARY_DIR}/out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_FILTER:STRING=O?...ing file[^\n]+\n"
              -D "TEST_OUTPUT=${testname}.${resultfile}.out"
              -D "TEST_REFERENCE=${testname}.${resultfile}.tst"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (H5BITROUND_UD-${testname} PROPERTIES
          WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
          DEPENDS H5BITROUND_UD-${testname}-gen-input)

      # Filter metadata (filter id, name, cd_values) recorded in the file.
      add_test (
          NAME H5BITROUND_UD-h5dump-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${H5PL_HDF5_DUMP_EXECUTABLE}"
              -D "TEST_ARGS:STRING=-pH;out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_OUTPUT=${resultfile}-${testname}.out"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_REFERENCE=${resultfile}-${testname}.ddl"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (H5BITROUND_UD-h5dump-${testname} PROPERTIES
          WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
          DEPENDS H5BITROUND_UD-${testname})
      set (last_test "H5BITROUND_UD-h5dump-${testname}")

      # Quantized data: confirms the filter actually changed the values and
      # the output is bit-reproducible (BitRound is deterministic, unlike the
      # Granular BitRound sibling, whose suite omits this check).
      add_test (
          NAME H5BITROUND_UD-h5dump-data-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${H5PL_HDF5_DUMP_EXECUTABLE}"
              -D "TEST_ARGS:STRING=out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_OUTPUT=${resultfile}-${testname}-data.out"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_REFERENCE=${resultfile}-${testname}-data.ddl"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (H5BITROUND_UD-h5dump-data-${testname} PROPERTIES
          WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
          DEPENDS H5BITROUND_UD-h5dump-${testname})
      set (last_test "H5BITROUND_UD-h5dump-data-${testname}")
    endif ()
  endmacro ()

  # Copy the reference files into the test directory.
  set (LIST_OTHER_TEST_FILES
      h5repack_floats.h5-ud_convert.ddl
      h5repack_floats.h5-ud_convert-data.ddl
      ud_convert.h5repack_floats.h5.tst
  )
  foreach (h5_file ${LIST_OTHER_TEST_FILES})
    HDFTEST_COPY_FILE("${PROJECT_SOURCE_DIR}/testfiles/${h5_file}" "${PROJECT_BINARY_DIR}/${h5_file}" "example_files")
  endforeach ()
  add_custom_target (example_files ALL COMMENT "Copying files needed by example tests" DEPENDS ${example_files_list})

  # The filter is encode-only. Under a decode-only build these write-side tests
  # are not registered (the plugin still loads for reading, since decode is a
  # no-op).
  if (NOT DISABLE_H5PL_ENCODER)
    ADD_BR_EXIT_TEST (H5BITROUND-roundtrip h5ex_d_bitround)

    # Reference file h5repack round-trip (NSB=7, chunk 4x8).
    ADD_H5_UD_TEST (ud_convert 0 h5repack_floats.h5 --enable-error-stack -v -f UD=32032,0,5,7,0,0,0,0 -l CHUNK=4x8)
  endif ()
endif ()
