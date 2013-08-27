cmake_minimum_required (VERSION 2.8.10)
PROJECT (HDF5BZ2_EX)

SET (dyn_examples
    h5ex_d_bzip2
)

#run-time loadable bzip library examples
FOREACH (example ${dyn_examples})
  ADD_EXECUTABLE (${example} ${PROJECT_SOURCE_DIR}/${example}.c)
  TARGET_NAMING (${example} ${LIB_TYPE})
  TARGET_C_PROPERTIES (${example} " " " ")
  TARGET_LINK_LIBRARIES (${example} ${HDF5_LIBRARIES})
  IF (NOT WIN32)
    TARGET_LINK_LIBRARIES (${example} dl)
  ENDIF (NOT WIN32)
ENDFOREACH (example ${dyn_examples})

IF (BUILD_TESTING)
  MACRO (ADD_H5_TEST testname)
    ADD_TEST (
        NAME ${testname}-clearall
        COMMAND    ${CMAKE_COMMAND}
            -E remove 
            ${testname}.out
            ${testname}.out.err
            ${testname}.ddl.out
            ${testname}.ddl.out.err
            ${testname}.h5
    )
    ADD_TEST (
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
            -P "${H5BZ2_RESOURCES_DIR}/runTest.cmake"
    )
    SET_TESTS_PROPERTIES(${testname} PROPERTIES DEPENDS ${testname}-clearall)
    IF (HDF5_BUILD_TOOLS)
      ADD_TEST (
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
              -P "${H5BZ2_RESOURCES_DIR}/runTest.cmake"
      )
      SET_TESTS_PROPERTIES(H5DUMP-${testname} PROPERTIES DEPENDS ${testname})
    ENDIF (HDF5_BUILD_TOOLS)
  ENDMACRO (ADD_H5_TEST)

  MACRO (ADD_H5_UD_TEST testname resultcode resultfile)
    # Remove any output file left over from previous test run
    ADD_TEST (
        NAME H5BZ2_UD-clearall-objects
        COMMAND    ${CMAKE_COMMAND}
            -E remove 
            out-${testname}.${resultfile}
            ${testname}.${resultfile}.out
            ${testname}.${resultfile}.out.err
            ${resultfile}-${testname}.out
            ${resultfile}-${testname}.out.err
    )
    IF (HDF5_BUILD_TOOLS)
      ADD_TEST (
          NAME H5BZ2_UD-${testname}
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
              -P "${H5BZ2_RESOURCES_DIR}/runTest.cmake"
      )
      SET_TESTS_PROPERTIES (H5BZ2_UD-${testname} PROPERTIES DEPENDS H5BZ2_UD-clearall-objects)
      ADD_TEST (
          NAME H5BZ2_UD-h5dump-${testname}
          COMMAND "${CMAKE_COMMAND}"
              -D "TEST_PROGRAM=$<TARGET_FILE:h5dump>"
              -D "TEST_ARGS:STRING=-pH;out-${testname}.${resultfile}"
              -D "TEST_FOLDER=${PROJECT_BINARY_DIR}"
              -D "TEST_OUTPUT=${resultfile}-${testname}.out"
              -D "TEST_EXPECT=${resultcode}"
              -D "TEST_REFERENCE=${resultfile}-${testname}.ddl"
              -D "TEST_ENV_VAR=HDF5_PLUGIN_PATH"
              -D "TEST_ENV_VALUE=${CMAKE_BINARY_DIR}/plugins"
              -P "${H5BZ2_RESOURCES_DIR}/runTest.cmake"
      )
      SET_TESTS_PROPERTIES (H5BZ2_UD-h5dump-${testname} PROPERTIES DEPENDS "H5BZ2_UD-${testname}")
    ENDIF (HDF5_BUILD_TOOLS)
  ENDMACRO (ADD_H5_UD_TEST)

  # --------------------------------------------------------------------
  # Copy all the HDF5 files from the source directory into the test directory
  # --------------------------------------------------------------------
  SET (LIST_HDF5_TEST_FILES
      ${PROJECT_SOURCE_DIR}/testfiles/h5repack_layout.h5
  )
  SET (LIST_OTHER_TEST_FILES
      ${PROJECT_SOURCE_DIR}/testfiles/h5ex_d_bzip2.ddl
      ${PROJECT_SOURCE_DIR}/testfiles/h5ex_d_bzip2.tst
      ${PROJECT_SOURCE_DIR}/testfiles/h5repack_layout.h5-ud_convert.ddl
      ${PROJECT_SOURCE_DIR}/testfiles/ud_convert.h5repack_layout.h5.tst
  )

  FOREACH (h5_file ${LIST_HDF5_TEST_FILES} ${LIST_OTHER_TEST_FILES})
    GET_FILENAME_COMPONENT(fname "${h5_file}" NAME)
    SET (dest "${PROJECT_BINARY_DIR}/${fname}")
    #MESSAGE (STATUS " Copying ${h5_file}")
    ADD_CUSTOM_COMMAND (
        TARGET     h5ex_d_bzip2
        POST_BUILD
        COMMAND    ${CMAKE_COMMAND}
        ARGS       -E copy_if_different ${h5_file} ${dest}
    )
  ENDFOREACH (h5_file ${LIST_HDF5_TEST_FILES} ${LIST_OTHER_TEST_FILES})


  ADD_H5_TEST (h5ex_d_bzip2)

#UD BZIP2
  ADD_H5_UD_TEST (ud_convert 0 h5repack_layout.h5 -v -f UD=307,1,9)
    
ENDIF (BUILD_TESTING)
  