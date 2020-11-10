cmake_minimum_required (VERSION 3.12)
########################################################
# This dashboard is maintained by The HDF Group
# For any comments please contact cdashhelp@hdfgroup.org
#
########################################################
# -----------------------------------------------------------
# -- Get environment
# -----------------------------------------------------------
if (NOT SITE_OS_NAME)
  ## machine name not provided - attempt to discover with uname
  ## -- set hostname
  ## --------------------------
  find_program (HOSTNAME_CMD NAMES hostname)
  exec_program (${HOSTNAME_CMD} ARGS OUTPUT_VARIABLE HOSTNAME)
  set (CTEST_SITE  "${HOSTNAME}${CTEST_SITE_EXT}")
  find_program (UNAME NAMES uname)
  macro (getuname name flag)
    exec_program ("${UNAME}" ARGS "${flag}" OUTPUT_VARIABLE "${name}")
  endmacro ()

  getuname (osname -s)
  getuname (osrel  -r)
  getuname (cpu    -m)
  message (STATUS "Dashboard script uname output: ${osname}-${osrel}-${cpu}\n")

  set (CTEST_BUILD_NAME  "${osname}-${osrel}-${cpu}")
else ()
  ## machine name provided
  ## --------------------------
  if (CMAKE_HOST_UNIX)
    set (CTEST_BUILD_NAME "${SITE_OS_NAME}-${SITE_OS_VERSION}-${SITE_OS_BITS}-${SITE_COMPILER_NAME}-${SITE_COMPILER_VERSION}")
  else ()
    set (CTEST_BUILD_NAME "${SITE_OS_NAME}-${SITE_OS_VERSION}-${SITE_COMPILER_NAME}")
  endif ()
endif ()
if (SITE_BUILDNAME_SUFFIX)
  set (CTEST_BUILD_NAME  "${SITE_BUILDNAME_SUFFIX}-${CTEST_BUILD_NAME}")
endif ()
set (BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DSITE:STRING=${CTEST_SITE} -DBUILDNAME:STRING=${CTEST_BUILD_NAME}")

# Launchers work only with Makefile and Ninja generators.
if(NOT "${CTEST_CMAKE_GENERATOR}" MATCHES "Make|Ninja")
  set(CTEST_USE_LAUNCHERS 0)
  set(ENV{CTEST_USE_LAUNCHERS_DEFAULT} 0)
  set(BUILD_OPTIONS "${BUILD_OPTIONS} -DCTEST_USE_LAUNCHERS:BOOL=OFF")
else()
  set(CTEST_USE_LAUNCHERS 1)
  set(ENV{CTEST_USE_LAUNCHERS_DEFAULT} 1)
  set(BUILD_OPTIONS "${BUILD_OPTIONS} -DCTEST_USE_LAUNCHERS:BOOL=ON")
endif()

#-----------------------------------------------------------------------------
# MAC machines need special option
#-----------------------------------------------------------------------------
if (APPLE)
  # Compiler choice
  execute_process (COMMAND xcrun --find cc OUTPUT_VARIABLE XCODE_CC OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process (COMMAND xcrun --find c++ OUTPUT_VARIABLE XCODE_CXX OUTPUT_STRIP_TRAILING_WHITESPACE)
  set (ENV{CC} "${XCODE_CC}")
  set (ENV{CXX} "${XCODE_CXX}")

  set (BUILD_OPTIONS "${BUILD_OPTIONS} -DCTEST_USE_LAUNCHERS:BOOL=ON -DCMAKE_BUILD_WITH_INSTALL_RPATH:BOOL=OFF")
endif ()

#-----------------------------------------------------------------------------
## Clear the build directory
## --------------------------
set (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)
if (NOT EXISTS "${CTEST_BINARY_DIRECTORY}")
  file (MAKE_DIRECTORY "${CTEST_BINARY_DIRECTORY}")
else ()
  ctest_empty_binary_directory (${CTEST_BINARY_DIRECTORY})
endif ()

# Use multiple CPU cores to build
include (ProcessorCount)
ProcessorCount (N)
if (NOT N EQUAL 0)
  if (MAX_PROC_COUNT)
    if (N GREATER MAX_PROC_COUNT)
      set (N ${MAX_PROC_COUNT})
    endif ()
  endif ()
  if (NOT WIN32)
    set (CTEST_BUILD_FLAGS -j${N})
  endif ()
  set (ctest_test_args ${ctest_test_args} PARALLEL_LEVEL ${N})
endif ()

#-----------------------------------------------------------------------------
# Send the main script as a note.
list (APPEND CTEST_NOTES_FILES
    "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}"
    "${CMAKE_CURRENT_LIST_FILE}"
    "${CTEST_SOURCE_DIRECTORY}/config/cmake/cacheinit.cmake"
)

#-----------------------------------------------------------------------------
# Check for required variables.
# --------------------------
foreach (req
    CTEST_CMAKE_GENERATOR
    CTEST_SITE
    CTEST_BUILD_NAME
  )
  if (NOT DEFINED ${req})
    message (FATAL_ERROR "The containing script must set ${req}")
  endif ()
endforeach ()

#-----------------------------------------------------------------------------
# Initialize the CTEST commands
#------------------------------
if (CMAKE_GENERATOR_TOOLSET)
  set (CTEST_CONFIGURE_TOOLSET  "-T${CMAKE_GENERATOR_TOOLSET}")
else ()
  set (CTEST_CONFIGURE_TOOLSET  "")
endif()
if (CMAKE_GENERATOR_ARCHITECTURE)
  set (CTEST_CONFIGURE_ARCHITECTURE  "-A${CMAKE_GENERATOR_ARCHITECTURE}")
else ()
  set (CTEST_CONFIGURE_ARCHITECTURE  "")
endif()
set (CTEST_CONFIGURE_COMMAND
    "${CTEST_CMAKE_COMMAND} -C \"${CTEST_SOURCE_DIRECTORY}/config/cmake/cacheinit.cmake\" -DCMAKE_BUILD_TYPE:STRING=${CTEST_CONFIGURATION_TYPE} ${BUILD_OPTIONS} \"-G${CTEST_CMAKE_GENERATOR}\" \"${CTEST_CONFIGURE_ARCHITECTURE}\" \"${CTEST_CONFIGURE_TOOLSET}\" \"${CTEST_SOURCE_DIRECTORY}\""
)

#-----------------------------------------------------------------------------
## -- set output to english
set ($ENV{LC_MESSAGES}  "en_EN")

# Print summary information.
foreach (v
    CTEST_SITE
    CTEST_BUILD_NAME
    CTEST_SOURCE_DIRECTORY
    CTEST_BINARY_DIRECTORY
    CTEST_CMAKE_GENERATOR
    CTEST_CONFIGURATION_TYPE
    CTEST_CONFIGURE_COMMAND
    CTEST_SCRIPT_DIRECTORY
    CTEST_USE_LAUNCHERS
  )
  set (vars "${vars}  ${v}=[${${v}}]\n")
endforeach ()
message (STATUS "Dashboard script configuration:\n${vars}\n")

#-----------------------------------------------------------------------------

###################################################################
#########       Following is for submission to CDash   ############
###################################################################
if (NOT DEFINED MODEL)
  set (MODEL "Experimental")
endif ()

#-----------------------------------------------------------------------------
  ## NORMAL process
  ## -- LOCAL_SUBMIT reports to CDash server
  ## --------------------------
  ctest_start (${MODEL} TRACK ${MODEL})
  configure_file (${CTEST_SOURCE_DIRECTORY}/config/cmake/CTestCustom.cmake ${CTEST_BINARY_DIRECTORY}/CTestCustom.cmake)
  ctest_read_custom_files ("${CTEST_BINARY_DIRECTORY}")
  ctest_configure (BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE res)
  if (LOCAL_SUBMIT)
    ctest_submit (PARTS Update Configure Notes)
  endif ()
  if (${res} LESS 0 OR ${res} GREATER 0)
    file (APPEND ${CTEST_SCRIPT_DIRECTORY}/FailedCTest.txt "Failed Configure: ${res}\n")
  endif ()

  ctest_build (BUILD "${CTEST_BINARY_DIRECTORY}" APPEND RETURN_VALUE res NUMBER_ERRORS errval)
  if (LOCAL_SUBMIT)
    ctest_submit (PARTS Build)
  endif ()
  if (${res} LESS 0 OR ${res} GREATER 0 OR ${errval} GREATER 0)
    file (APPEND ${CTEST_SCRIPT_DIRECTORY}/FailedCTest.txt "Failed ${errval} Build: ${res}\n")
  endif ()

  ctest_test (BUILD "${CTEST_BINARY_DIRECTORY}" APPEND ${ctest_test_args} RETURN_VALUE res)
  if (LOCAL_SUBMIT)
    ctest_submit (PARTS Test)
  endif ()
  if (${res} LESS 0 OR ${res} GREATER 0)
    file (APPEND ${CTEST_SCRIPT_DIRECTORY}/FailedCTest.txt "Failed Tests: ${res}\n")
  endif ()
#-----------------------------------------------------------------------------
