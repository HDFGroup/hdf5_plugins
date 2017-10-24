cmake_minimum_required (VERSION 3.2.2)
project (HDF5PL_EX)

set (dyn_examples
    h5ex_d_blosc
    h5ex_d_bzip2
    h5ex_d_lz4
    h5ex_d_lzf
    h5ex_d_mafisc
    h5ex_d_zfp
)

if (USE_SHARED_LIBS)
  add_definitions (-DH5_BUILT_AS_DYNAMIC_LIB)
else ()
  add_definitions (-DH5_BUILT_AS_STATIC_LIB)
endif ()

#run-time loadable library examples
foreach (example ${dyn_examples})
  add_executable (${example} ${PROJECT_SOURCE_DIR}/${example}.c)
  TARGET_NAMING (${example} ${LIB_TYPE})
  TARGET_C_PROPERTIES (${example} ${LIB_TYPE} " " " ")
  target_link_libraries (${example} ${LINK_LIBS})
  if (NOT WIN32)
    target_link_libraries (${example} dl)
  endif ()
endforeach ()

if (BUILD_TESTING)
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
      set(TESTLIBDIR "${HDF5_TOOLS_DIR}")
    else ()
      set(TESTLIBDIR "${HDF5_LIBRARY_PATH}")
    endif ()
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
    if (NOT "${last_test}" STREQUAL "")
      set_tests_properties (${testname} PROPERTIES DEPENDS ${last_test})
    endif ()
    set (last_test "${testname}")
    if (HDF5_BUILD_TOOLS)
      add_test (
          NAME H5DUMP-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${HDF5_DUMP_EXECUTABLE}"
              -D "TEST_ARGS:STRING=--enable-error-stack;-p;${testname}.h5"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_OUTPUT=${testname}.ddl.out"
              -D "TEST_FILTER=PARAMS { ([0-9]) [-]?[0-9]+ ([0-9] [0-9] [0-9] [0-9] [0-9] [0-9]) }\n"
              -D "TEST_FILTER_REPLACE=PARAMS { \\1 XXXX \\2 }\n"
              -D "TEST_EXPECT=0"
              -D "TEST_REFERENCE=${testname}.ddl"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
      )
      if (NOT "${last_test}" STREQUAL "")
        set_tests_properties (H5DUMP-${testname} PROPERTIES DEPENDS ${last_test})
      endif ()
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
    if (WIN32)
      set(${TESTLIBDIR} "${HDF5_TOOLS_DIR}")
    else ()
      set(${TESTLIBDIR} "${HDF5_LIBRARY_PATH}")
    endif ()
    if (HDF5_BUILD_TOOLS)
      if ("${deptest}" STREQUAL "NONE")
        set (testfile ${resultfile})
      else ()
        set (testfile out-${deptest}.${resultfile})
      endif ()
      add_test (
          NAME H5PL_UD-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${HDF5_REPACK_EXECUTABLE}"
              -D "TEST_ARGS:STRING=--enable-error-stack;${ARGN};${PROJECT_BINARY_DIR}/${testfile};${PROJECT_BINARY_DIR}/out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_FILTER:STRING=O?...ing file[^\n]+\n"
              -D "TEST_OUTPUT=${testname}.${resultfile}.out"
              -D "TEST_REFERENCE=${testname}.${resultfile}.tst"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (H5PL_UD-${testname} PROPERTIES DEPENDS H5PL_UD-${testname}-clearall-objects)
      set (deptest "H5PL_UD-${testname}")
    endif ()
  endmacro ()

  macro (ADD_H5_UD_VERIFY testname resultcode resultfile deptest)
    # Remove any output file left over from previous test run
    if (HDF5_BUILD_TOOLS)
      add_test (
          NAME H5PL_UD-h5dump-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${HDF5_DUMP_EXECUTABLE}"
              -D "TEST_ARGS:STRING=--enable-error-stack;-pH;out-${deptest}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_OUTPUT=${resultfile}-${testname}.out"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_REFERENCE=${resultfile}-${testname}.ddl"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -P "${H5PL_RESOURCES_DIR}/runTest.cmake"
      )
      if (NOT "${last_test}" STREQUAL "")
        set_tests_properties (H5PL_UD-h5dump-${testname} PROPERTIES DEPENDS H5PL_UD-${deptest})
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
  if (WIN32 AND MSVC_VERSION LESS 1900)
    HDFTEST_COPY_FILE("${PROJECT_SOURCE_DIR}/testfiles/h5ex_d_zfp.wddl" "${PROJECT_BINARY_DIR}/h5ex_d_zfp.ddl" "example_files")
  endif()
  add_custom_target(example_files ALL COMMENT "Copying files needed by example tests" DEPENDS ${example_files_list})

  foreach (h5_file ${dyn_examples})
    ADD_H5_TEST (${h5_file})
  endforeach ()

#UD BZ2$
  ADD_H5_UD_TEST (ud_bz2_convert 0 h5repack_layout.h5 NONE -v -f /dset1:UD=307,0,1,9)
  ADD_H5_UD_VERIFY (ud_pl_bz2_convert 0 h5repack_layout.h5 ud_bz2_convert)
#UD BLOSC$
  ADD_H5_UD_TEST (ud_blosc_convert 0 h5repack_layout.h5 ud_bz2_convert -v -f /dset2:UD=32001,0,7,0,0,0,0,5,1,0)
  ADD_H5_UD_VERIFY (ud_pl_blosc_convert 0 h5repack_layout.h5 ud_blosc_convert)
#UD LZ4$
  ADD_H5_UD_TEST (ud_lz4_convert 0 h5repack_layout.h5 ud_blosc_convert -v -f /dset3:UD=32004,0,1,3 -l CHUNK=4x8)
  ADD_H5_UD_VERIFY (ud_pl_lz4_convert 0 h5repack_layout.h5 ud_lz4_convert)
#UD MAFISC$
  ADD_H5_UD_TEST (ud_mafisc_convert 0 h5repack_layout.h5 ud_lz4_convert -v -f /dset4:UD=32013,1,0,0 -l CHUNK=4x8)
#UD VERIFY
  ADD_H5_UD_VERIFY (ud_pl_convert 0 h5repack_layout.h5 ud_mafisc_convert)

endif ()
