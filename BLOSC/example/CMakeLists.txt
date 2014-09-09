cmake_minimum_required (VERSION 2.8.10)
PROJECT (HDF5BLOSC_EX)

set (dyn_examples
    h5ex_d_blosc
)

#run-time loadable bzip library examples
foreach (example ${dyn_examples})
  add_executable (${example} ${PROJECT_SOURCE_DIR}/${example}.c)
  TARGET_NAMING (${example} ${LIB_TYPE})
  TARGET_C_PROPERTIES (${example} " " " ")
  target_link_libraries (${example} ${HDF5_LIBRARIES})
  if (NOT WIN32)
    target_link_libraries (${example} dl)
  endif (NOT WIN32)
endforeach (example ${dyn_examples})

if (BUILD_TESTING)
  MACRO (ADD_H5_TEST testname)
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
    add_test (
        NAME ${testname}
        COMMAND "${CMAKE_COMMAND}"
            -D "TEST_PROGRAM=$<TARGET_FILE:${testname}>"
            -D "TEST_ARGS:STRING=${ARGN}"
            -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
            -D "TEST_EXPECT=0"
            -D "TEST_OUTPUT=${testname}.out"
            -D "TEST_REFERENCE=${testname}.tst"
            -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
            -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
            -P "${H5BLOSC_RESOURCES_DIR}/runTest.cmake"
    )
    set_tests_properties(${testname} PROPERTIES DEPENDS ${testname}-clearall)
    if (HDF5_BUILD_TOOLS)
      add_test (
          NAME H5DUMP-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=$<TARGET_FILE:h5dump>"
              -D "TEST_ARGS:STRING=--enable-error-stack;${testname}.h5"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_OUTPUT=${testname}.ddl.out"
              -D "TEST_EXPECT=0"
              -D "TEST_REFERENCE=${testname}.ddl"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5BLOSC_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties(H5DUMP-${testname} PROPERTIES DEPENDS ${testname})
    endif (HDF5_BUILD_TOOLS)
  ENDMACRO (ADD_H5_TEST)

  MACRO (ADD_H5_UD_TEST testname resultcode resultfile)
    # Remove any output file left over from previous test run
    add_test (
        NAME H5BLOSC_UD-clearall-objects
        COMMAND    ${CMAKE_COMMAND}
            -E remove 
            out-${testname}.${resultfile}
            ${testname}.${resultfile}.out
            ${testname}.${resultfile}.out.err
            ${resultfile}-${testname}.out
            ${resultfile}-${testname}.out.err
    )
    if (HDF5_BUILD_TOOLS)
      add_test (
          NAME H5BLOSC_UD-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=$<TARGET_FILE:h5repack>"
              -D "TEST_ARGS:STRING=${ARGN};${PROJECT_BINARY_DIR}/${resultfile};${PROJECT_BINARY_DIR}/out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_FILTER:STRING=O?...ing file[^\n]+\n"
              -D "TEST_OUTPUT=${testname}.${resultfile}.out"
              -D "TEST_REFERENCE=${testname}.${resultfile}.tst"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5BLOSC_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (H5BLOSC_UD-${testname} PROPERTIES DEPENDS H5BLOSC_UD-clearall-objects)
      add_test (
          NAME H5BLOSC_UD-h5dump-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=$<TARGET_FILE:h5dump>"
              -D "TEST_ARGS:STRING=-pH;out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_OUTPUT=${resultfile}-${testname}.out"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_REFERENCE=${resultfile}-${testname}.ddl"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5BLOSC_RESOURCES_DIR}/runTest.cmake"
      )
      set_tests_properties (H5BLOSC_UD-h5dump-${testname} PROPERTIES DEPENDS
"H5BLOSC_UD-${testname}")
    endif (HDF5_BUILD_TOOLS)
  endmacro (ADD_H5_UD_TEST)

  # --------------------------------------------------------------------
  # Copy all the HDF5 files from the source directory into the test directory
  # --------------------------------------------------------------------
  set (LIST_HDF5_TEST_FILES
      ${PROJECT_SOURCE_DIR}/testfiles/h5repack_layout.h5
  )
  set (LIST_OTHER_TEST_FILES
      ${PROJECT_SOURCE_DIR}/testfiles/h5ex_d_blosc.ddl
      ${PROJECT_SOURCE_DIR}/testfiles/h5ex_d_blosc.tst
      ${PROJECT_SOURCE_DIR}/testfiles/h5repack_layout.h5-ud_convert.ddl
      ${PROJECT_SOURCE_DIR}/testfiles/ud_convert.h5repack_layout.h5.tst
  )

  foreach (h5_file ${LIST_HDF5_TEST_FILES} ${LIST_OTHER_TEST_FILES})
    get_filename_component(fname "${h5_file}" NAME)
    set (dest "${PROJECT_BINARY_DIR}/${fname}")
    #message (STATUS " Copying ${h5_file}")
    add_custom_command (
        TARGET     h5ex_d_blosc
        POST_BUILD
        COMMAND    ${CMAKE_COMMAND}
        ARGS       -E copy_if_different ${h5_file} ${dest}
    )
  endforeach (h5_file ${LIST_HDF5_TEST_FILES} ${LIST_OTHER_TEST_FILES})


  ADD_H5_TEST (h5ex_d_blosc)

#UD BLOSC
#  ADD_H5_UD_TEST (ud_convert 0 h5repack_layout.h5 -v -f UD=32001,1,...,n)
    
endif (BUILD_TESTING)
  
