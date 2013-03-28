cmake_minimum_required (VERSION 2.8.10)
PROJECT (HDF5BZ2_EX)

SET (dyn_examples
    h5ex_d_bzip2
)

#run-time loadable bzip library examples
FOREACH (example ${dyn_examples})
  ADD_EXECUTABLE (${example} ${PROJECT_SOURCE_DIR}/${example}.c)
  TARGET_NAMING (${example} ${LIB_TYPE})
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

  IF (HDF5_BUILD_TOOLS)
    ADD_CUSTOM_COMMAND (
        TARGET     h5ex_d_bzip2
        POST_BUILD
        COMMAND    ${XLATE_UTILITY}
        ARGS       ${PROJECT_SOURCE_DIR}/testfiles/h5ex_d_bzip2.ddl h5ex_d_bzip2.ddl -l4
    )
    ADD_CUSTOM_COMMAND (
        TARGET     h5ex_d_bzip2
        POST_BUILD
        COMMAND    ${XLATE_UTILITY}
        ARGS       ${PROJECT_SOURCE_DIR}/testfiles/h5ex_d_bzip2.tst h5ex_d_bzip2.tst -l4
    )
  ENDIF (HDF5_BUILD_TOOLS)

  ADD_H5_TEST (h5ex_d_bzip2)
    
ENDIF (BUILD_TESTING)
  