cmake_minimum_required (VERSION 3.2.2)
project (HDF5ZFP_EX)

set (dyn_examples
    h5ex_d_zfp
)

if (USE_SHARED_LIBS)
  add_definitions (-DH5_BUILT_AS_DYNAMIC_LIB)
else ()
  add_definitions (-DH5_BUILT_AS_STATIC_LIB)
endif ()

#run-time loadable zfp library examples
foreach (example ${dyn_examples})
  add_executable (${example} ${PROJECT_SOURCE_DIR}/${example}.c)
  TARGET_NAMING (${example} ${LIB_TYPE})
  TARGET_C_PROPERTIES (${example} ${LIB_TYPE} " " " ")
  target_link_libraries (${example} PRIVATE ${HDF5_LINK_LIBS})
  if (NOT WIN32)
    target_link_libraries (${example} PRIVATE dl)
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
            -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
            -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
            -P "${H5ZFP_RESOURCES_DIR}/runTest.cmake"
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
              -D "TEST_EXPECT=0"
              -D "TEST_REFERENCE=${testname}.ddl"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5ZFP_RESOURCES_DIR}/runTest.cmake"
      )
      if (NOT "${last_test}" STREQUAL "")
        set_tests_properties (H5DUMP-${testname} PROPERTIES DEPENDS ${last_test})
      endif ()
    endif ()
  endmacro ()

  macro (ADD_H5_UD_TEST testname resultcode resultfile)
    # Remove any output file left over from previous test run
    add_test (
        NAME H5ZFP_UD-${testname}-clearall-objects
        COMMAND    ${CMAKE_COMMAND}
            -E remove
            out-${testname}.${resultfile}
            ${testname}.${resultfile}.out
            ${testname}.${resultfile}.out.err
            ${resultfile}-${testname}.out
            ${resultfile}-${testname}.out.err
    )
    if (NOT "${last_test}" STREQUAL "")
      set_tests_properties (H5ZFP_UD-${testname}-clearall-objects PROPERTIES DEPENDS ${last_test})
    endif ()
    set (last_test "H5ZFP_UD-${testname}-clearall-objects")
    if (WIN32)
      set(${TESTLIBDIR} "${HDF5_TOOLS_DIR}")
    else ()
      set(${TESTLIBDIR} "${HDF5_LIBRARY_PATH}")
    endif ()
    if (HDF5_BUILD_TOOLS)
      add_test (
          NAME H5ZFP_UD-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${HDF5_REPACK_EXECUTABLE}"
              -D "TEST_ARGS:STRING=${ARGN};${PROJECT_BINARY_DIR}/${resultfile};${PROJECT_BINARY_DIR}/out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_FILTER:STRING=O?...ing file[^\n]+\n"
              -D "TEST_OUTPUT=${testname}.${resultfile}.out"
              -D "TEST_REFERENCE=${testname}.${resultfile}.tst"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5ZFP_RESOURCES_DIR}/runTest.cmake"
      )
      if (NOT "${last_test}" STREQUAL "")
        set_tests_properties (H5ZFP_UD-${testname} PROPERTIES DEPENDS ${last_test})
      endif ()
      set (last_test "H5ZFP_UD-${testname}")
      add_test (
          NAME H5ZFP_UD-h5dump-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=${HDF5_DUMP_EXECUTABLE}"
              -D "TEST_ARGS:STRING=-pH;out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_OUTPUT=${resultfile}-${testname}.out"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_REFERENCE=${resultfile}-${testname}.ddl"
              -D "TEST_FILTER:STRING=PARAMS {[ -0-9]*}"
              -D "TEST_FILTER_REPLACE:STRING=PARAMS { XXXX }"
              -D "TEST_LIBRARY_DIRECTORY=${TESTLIBDIR}"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5ZFP_RESOURCES_DIR}/runTest.cmake"
      )
      if (NOT "${last_test}" STREQUAL "")
        set_tests_properties (H5ZFP_UD-h5dump-${testname} PROPERTIES DEPENDS ${last_test})
      endif ()
      set (last_test "H5ZFP_UD-h5dump-${testname}")
    endif ()
  endmacro ()

  # --------------------------------------------------------------------
  # Copy all the HDF5 files from the source directory into the test directory
  # --------------------------------------------------------------------
  set (LIST_HDF5_TEST_FILES
      h5repack_layout.h5
      mesh.h5
  )
  set (LIST_OTHER_TEST_FILES
      h5ex_d_zfp.tst
      h5repack_layout.h5-ud_convert.ddl
      ud_convert.h5repack_layout.h5.tst
      mesh.h5-ud_mesh_convert.ddl
      ud_mesh_convert.mesh.h5.tst
  )

  foreach (h5_file ${LIST_HDF5_TEST_FILES} ${LIST_OTHER_TEST_FILES})
    HDFTEST_COPY_FILE("${PROJECT_SOURCE_DIR}/testfiles/${h5_file}" "${PROJECT_BINARY_DIR}/${h5_file}" "zfp_files")
  endforeach ()
  if (WIN32 AND MSVC_VERSION LESS 1900)
    HDFTEST_COPY_FILE("${PROJECT_SOURCE_DIR}/testfiles/h5ex_d_zfp.wddl" "${PROJECT_BINARY_DIR}/h5ex_d_zfp.ddl" "zfp_files")
  else ()
    HDFTEST_COPY_FILE("${PROJECT_SOURCE_DIR}/testfiles/h5ex_d_zfp.ddl" "${PROJECT_BINARY_DIR}/h5ex_d_zfp.ddl" "zfp_files")
  endif()
  add_custom_target(zfp_files ALL COMMENT "Copying files needed by zfp tests" DEPENDS ${zfp_files_list})

  ADD_H5_TEST (h5ex_d_zfp)

#UD ZFP
  ADD_H5_UD_TEST (ud_convert 0 h5repack_layout.h5 -v -f UD=32013,1,0,0 -l CHUNK=4x8)
#UD ZFP accuracy mode
  ADD_H5_UD_TEST (ud_mesh_convert 0 mesh.h5 --enable-error-stack -v -f UD=32013,0,6,3,0,3539053052,1062232653,0,0
         -l X,Y,Z,Indexes:CHUNK=217
         -l Indexes2:CHUNK=1517
         -l Pressure,Pressure2,Pressure3:CHUNK=10x20x5
         -l Pressure_2D:CHUNK=10x20
         -l Stress,Velocity,Stress2,Velocity2,Stress3,Velocity3:CHUNK=11x21x1x1
         -l VelocityZ,VelocityZ2,VelocityZ3:CHUNK=11x21x1
         -l XY:CHUNK=651x1
         -l XYZ:CHUNK=217x1
         -l XYZ2:CHUNK=1617x1
         -l XYZ3:CHUNK=33x1
  )

endif ()
