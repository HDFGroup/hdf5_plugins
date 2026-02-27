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
    h5ex_d_granularbr
)

if (USE_SHARED_LIBS)
  add_definitions (-DH5_BUILT_AS_DYNAMIC_LIB)
else ()
  add_definitions (-DH5_BUILT_AS_STATIC_LIB)
endif ()

#run-time loadable granularbr library examples
foreach (example ${dyn_examples})
  add_executable (${example} ${PROJECT_SOURCE_DIR}/${example}.c)
  target_include_directories(${example} PRIVATE ${H5PL_HDF5_INCLUDE_DIRS})
  TARGET_C_PROPERTIES (${example} ${LIB_TYPE})
  target_link_libraries (${example} PRIVATE ${H5PL_HDF5_LINK_LIBS})
  if (NOT WIN32)
    target_link_libraries (${example} PRIVATE dl)
  endif ()
endforeach ()

if (H5PL_BUILD_TESTING)
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
      # This test currently fails on ARM64 Windows and needs more investigation
      if ("H5DUMP-${testname}" MATCHES "H5DUMP-h5ex_d_granularbr" AND MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
        if (CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|AARCH64" OR CMAKE_C_COMPILER_ARCHITECTURE_ID MATCHES "ARM64")
          set_tests_properties (H5DUMP-${testname} PROPERTIES DISABLED TRUE)
        endif ()
      endif ()
      set (last_test "H5DUMP-${testname}")
    endif ()
  endmacro ()

  macro (ADD_H5_UD_TEST testname resultcode resultfile)
    # Remove any output file left over from previous test run
    add_test (
        NAME H5BITROUND_UD-${testname}-clearall-objects
        COMMAND    ${CMAKE_COMMAND}
            -E remove
            out-${testname}.${resultfile}
            ${testname}.${resultfile}.out
            ${testname}.${resultfile}.out.err
            ${resultfile}-${testname}.out
            ${resultfile}-${testname}.out.err
    )
    if (NOT "${last_test}" STREQUAL "")
      set_tests_properties (H5BITROUND_UD-${testname}-clearall-objects PROPERTIES DEPENDS ${last_test})
    endif ()
    set (last_test "H5BITROUND_UD-${testname}-clearall-objects")
    if (WIN32)
      set (${TESTLIBDIR} "${HDF5_TOOLS_DIR}")
    else ()
      set (${TESTLIBDIR} "${HDF5_LIBRARY_PATH}")
    endif ()
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
      set_tests_properties (H5BITROUND_UD-${testname} PROPERTIES DEPENDS H5BITROUND_UD-${testname}-clearall-objects)
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
    endif ()
  endmacro ()

  # --------------------------------------------------------------------
  # Copy all the HDF5 files from the source directory into the test directory
  # --------------------------------------------------------------------
  set (LIST_HDF5_TEST_FILES
      h5repack_layout.h5
  )
  set (LIST_OTHER_TEST_FILES
      h5ex_d_granularbr.ddl
      h5ex_d_granularbr.tst
      h5repack_layout.h5-ud_convert.ddl
      ud_convert.h5repack_layout.h5.tst
  )

  foreach (h5_file ${LIST_HDF5_TEST_FILES} ${LIST_OTHER_TEST_FILES})
    HDFTEST_COPY_FILE("${PROJECT_SOURCE_DIR}/testfiles/${h5_file}" "${PROJECT_BINARY_DIR}/${h5_file}" "example_files")
  endforeach ()
  add_custom_target(example_files ALL COMMENT "Copying files needed by example tests" DEPENDS ${example_files_list})

  ADD_H5_TEST (h5ex_d_granularbr)

  if (NOT DISABLE_H5PL_ENCODER)
    #UD BITROUND
    ADD_H5_UD_TEST (ud_convert 0 h5repack_layout.h5 -v -f UD=32023,1,5,3,4,0,0,0 -l CHUNK=4x8)
  endif ()

endif ()
