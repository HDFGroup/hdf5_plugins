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

#run-time loadable bitround library examples
foreach (example ${dyn_examples})
  add_executable (${example} ${PROJECT_SOURCE_DIR}/${example}.c)
  target_include_directories(${example} PRIVATE ${H5PL_HDF5_INCLUDE_DIRS})
  TARGET_C_PROPERTIES (${example} ${LIB_TYPE})
  target_link_libraries (${example} PRIVATE ${H5PL_HDF5_LINK_LIBS})
  if (NOT WIN32)
    target_link_libraries (${example} PRIVATE ${CMAKE_DL_LIBS})
  endif ()
endforeach ()

if (H5PL_BUILD_TESTING)
  # h5repack test input generator. Target name is plugin-suffixed because the
  # GRANULAR_BITROUND and BITROUND trees share the same CMake project and source file
  # basename (issue #240).
  add_executable (h5repack_floats_bitround ${PROJECT_SOURCE_DIR}/h5repack_floats.c)
  target_include_directories(h5repack_floats_bitround PRIVATE ${H5PL_HDF5_INCLUDE_DIRS})
  TARGET_C_PROPERTIES (h5repack_floats_bitround ${LIB_TYPE})
  target_link_libraries (h5repack_floats_bitround PRIVATE ${H5PL_HDF5_LINK_LIBS})
  if (NOT WIN32)
    target_link_libraries (h5repack_floats_bitround PRIVATE ${CMAKE_DL_LIBS})
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
              -D "TEST_ARGS:STRING=${ARGN}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_EXPECT=0"
              -D "TEST_OUTPUT=${testname}.out"
              -D "TEST_REFERENCE=${testname}.tst"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (${testname} PROPERTIES DEPENDS ${testname}-clearall)
    endif ()
    set (last_test "${testname}")
    if (HDF5_BUILD_TOOLS OR HDF5_PROVIDES_TOOLS)
      add_test (
          NAME H5DUMP-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${H5PL_HDF5_DUMP_EXECUTABLE}"
              -D "TEST_ARGS:STRING=--enable-error-stack;-pH;${testname}.h5"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_OUTPUT=${testname}.ddl.out"
              -D "TEST_EXPECT=0"
              -D "TEST_REFERENCE=${testname}.ddl"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
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
        NAME H5BITROUND_UD-${testname}-clearall-objects
        COMMAND    ${CMAKE_COMMAND}
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
    if (WIN32)
      set (TESTLIBDIR "${HDF5_TOOLS_DIR}")
    else ()
      set (TESTLIBDIR "${HDF5_LIBRARY_PATH}")
    endif ()
    # Generate the float-data input file for h5repack. The BitRound filter's
    # set_local() callback removes the filter from the pipeline on non-float
    # datasets, so the input must contain float data for the test to actually
    # exercise the filter (issue #240). Routed through runTest.cmake so
    # PATH/DYLD_LIBRARY_PATH/LD_LIBRARY_PATH are set up for the helper to find
    # the HDF5 runtime library on every platform (Windows in particular has
    # no rpath equivalent and reports STATUS_DLL_NOT_FOUND if the HDF5 DLL is
    # not on PATH).
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
      set_tests_properties (H5BITROUND_UD-${testname} PROPERTIES DEPENDS H5BITROUND_UD-${testname}-gen-input)
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
      # Verify that the filter actually modifies the dataset values: dump the
      # data (no -pH) and compare against a captured reference of the
      # quantized output. BitRound's encoding is bit-deterministic across
      # platforms (its inner loop is pure integer bit ops, with the bit-zero
      # count derived once per call from NSD via ceil() of a constant
      # expression), so the captured reference is portable.
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

  # --------------------------------------------------------------------
  # Copy all the HDF5 files from the source directory into the test directory
  # --------------------------------------------------------------------
  set (LIST_OTHER_TEST_FILES
      h5ex_d_bitround.ddl
      h5ex_d_bitround.tst
      h5repack_floats.h5-ud_convert.ddl
      h5repack_floats.h5-ud_convert-data.ddl
      ud_convert.h5repack_floats.h5.tst
  )

  foreach (h5_file ${LIST_OTHER_TEST_FILES})
    HDFTEST_COPY_FILE("${PROJECT_SOURCE_DIR}/testfiles/${h5_file}" "${PROJECT_BINARY_DIR}/${h5_file}" "example_files")
  endforeach ()
  add_custom_target(example_files ALL COMMENT "Copying files needed by example tests" DEPENDS ${example_files_list})

  ADD_H5_TEST (h5ex_d_bitround)

  if (NOT DISABLE_H5PL_ENCODER)
    #UD BITROUND
    ADD_H5_UD_TEST (ud_convert 0 h5repack_floats.h5 --enable-error-stack -v -f UD=32032,0,5,3,4,0,0,0 -l CHUNK=4x8)
  endif ()

endif ()
