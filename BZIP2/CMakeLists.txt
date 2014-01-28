cmake_minimum_required (VERSION 2.8.4)
PROJECT (H5BZ2 C)

#-----------------------------------------------------------------------------
# Basic H5BZIP stuff here
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
SET (H5BZ2_RESOURCES_DIR           ${H5BZ2_SOURCE_DIR}/config/cmake)
SET (H5BZ2_SRC_DIR                 ${H5BZ2_SOURCE_DIR}/src)

#-----------------------------------------------------------------------------
# Set the core names of all the libraries
#-----------------------------------------------------------------------------
SET (H5BZ2_LIB_CORENAME              "h5bz2")

#-----------------------------------------------------------------------------
# Set the true names of all the libraries if customized by external project
#-----------------------------------------------------------------------------
SET (H5BZ2_LIB_NAME              "${H5BZ2_EXTERNAL_LIB_PREFIX}${H5BZ2_LIB_CORENAME}")

#-----------------------------------------------------------------------------
# Set the target names of all the libraries
#-----------------------------------------------------------------------------
SET (H5BZ2_LIB_TARGET              ${H5BZ2_LIB_CORENAME})

IF (NOT H5BZ2_INSTALL_BIN_DIR)
  SET (H5BZ2_INSTALL_BIN_DIR bin)
ENDIF (NOT H5BZ2_INSTALL_BIN_DIR)
IF (NOT H5BZ2_INSTALL_LIB_DIR)
  SET (H5BZ2_INSTALL_LIB_DIR lib)
ENDIF (NOT H5BZ2_INSTALL_LIB_DIR)
IF (NOT H5BZ2_INSTALL_INCLUDE_DIR)
  SET (H5BZ2_INSTALL_INCLUDE_DIR include)
ENDIF (NOT H5BZ2_INSTALL_INCLUDE_DIR)
IF (NOT H5BZ2_INSTALL_DATA_DIR)
  IF (NOT WIN32)
    SET (H5BZ2_INSTALL_DATA_DIR share)
    SET (H5BZ2_INSTALL_CMAKE_DIR share/cmake)
  ELSE (NOT WIN32)
    SET (H5BZ2_INSTALL_DATA_DIR ".")
    SET (H5BZ2_INSTALL_CMAKE_DIR cmake)
  ENDIF (NOT WIN32)
ENDIF (NOT H5BZ2_INSTALL_DATA_DIR)

SET (H5BZ2_VERS_MAJOR 1)
SET (H5BZ2_VERS_MINOR 0)
SET (H5BZ2_VERS_RELEASE 0)

#-----------------------------------------------------------------------------
SET (H5BZ2_PACKAGE "h5bz2")
SET (H5BZ2_PACKAGE_NAME "h5bz2")
SET (H5BZ2_PACKAGE_VERSION "${H5BZ2_VERS_MAJOR}.${H5BZ2_VERS_MINOR}")
SET (H5BZ2_PACKAGE_VERSION_MAJOR "${H5BZ2_VERS_MAJOR}")
SET (H5BZ2_PACKAGE_VERSION_MINOR "${H5BZ2_VERS_MINOR}")
SET (H5BZ2_PACKAGE_STRING "${H5BZ2_PACKAGE_NAME} ${H5BZ2_PACKAGE_VERSION}")
SET (H5BZ2_PACKAGE_TARNAME "h5bz2{HDF_PACKAGE_EXT}")
SET (H5BZ2_PACKAGE_URL "http://www.hdfgroup.org")
SET (H5BZ2_PACKAGE_BUGREPORT "help@hdfgroup.org")

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
INCLUDE (${H5BZ2_RESOURCES_DIR}/HDFMacros.cmake)
INCLUDE (${H5BZ2_RESOURCES_DIR}/H5BZ2Macros.cmake)

#-----------------------------------------------------------------------------
# Setup output Directories
#-----------------------------------------------------------------------------
IF (NOT H5BZ2_EXTERNALLY_CONFIGURED)
  SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
  )
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries"
  )
  SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries."
  )
ELSE (NOT H5BZ2_EXTERNALLY_CONFIGURED)
  # if we are externally configured, but the project uses old cmake scripts
  # this may not be set
  IF (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
  ENDIF (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
ENDIF(NOT H5BZ2_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
INCLUDE (${H5BZ2_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# Option to Build Shared/Static libs, only shared is allowed
#-----------------------------------------------------------------------------
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" ON)
SET (LIB_TYPE SHARED)
IF (BUILD_SHARED_LIBS)
  SET (LIB_TYPE SHARED)
  ADD_DEFINITIONS (-DH5BZ2_BUILT_AS_DYNAMIC_LIB)
ELSE (BUILD_SHARED_LIBS)
  MESSAGE (FATAL_ERROR " h5bz2 library must be built as a shared library")
ENDIF (BUILD_SHARED_LIBS)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
SET (CMAKE_MODULE_PATH ${HDF_RESOURCES_DIR} ${CMAKE_MODULE_PATH})

SET (HDF5_USE_SHARED_LIBRARIES 1)
ADD_DEFINITIONS (-DH5_USE_18_API)

IF (NOT HDF5_HDF5_HEADER)
  FIND_PACKAGE (HDF5 NAMES ${HDF5_PACKAGE_NAME})
  IF (NOT HDF5_FOUND)
    FIND_PACKAGE (HDF5) # Legacy find
    SET (HDF5_BUILD_SHARED_LIBS 1)
  ENDIF (NOT HDF5_FOUND)
  IF (HDF5_FOUND)
    SET (HDF5_HAVE_H5PUBCONF_H 1)
    SET (HDF5_HAVE_HDF5 1)
    SET (HDF5_HDF5_HEADER "h5pubconf.h")
    SET (HDF5_INCLUDE_DIR_GEN ${HDF5_INCLUDE_DIR})
    MESSAGE (STATUS "HDF5 found: INC=${HDF5_INCLUDE_DIR} TOOLS=${HDF5_TOOLS_DIR}")
    IF ((NOT USE_SHARED_LIBS AND HDF5_BUILD_SHARED_LIBS) OR (USE_SHARED_LIBS AND NOT HDF5_BUILD_SHARED_LIBS))
      MESSAGE (STATUS "HDF5 build type does match requested type")
    ENDIF ((NOT USE_SHARED_LIBS AND HDF5_BUILD_SHARED_LIBS) OR (USE_SHARED_LIBS AND NOT HDF5_BUILD_SHARED_LIBS))
    IF (HDF5_BUILD_SHARED_LIBS)
      ADD_DEFINITIONS (-DH5_BUILT_AS_DYNAMIC_LIB)
    ELSE (HDF5_BUILD_SHARED_LIBS)
      MESSAGE (FATAL_ERROR " SHARED HDF5 is Required for h5bz2 library")
    ENDIF (HDF5_BUILD_SHARED_LIBS)
  ELSE (HDF5_FOUND)
    MESSAGE (FATAL_ERROR " HDF5 is Required for h5bz2 library")
  ENDIF (HDF5_FOUND)
ELSE (NOT HDF5_HDF5_HEADER)
  # This project is being called from within another and HDF5 is already configured
  SET (HDF5_HAVE_H5PUBCONF_H 1)
  SET (HDF5_HAVE_HDF5 1)
ENDIF (NOT HDF5_HDF5_HEADER)
ADD_EXECUTABLE (h5repack IMPORTED)
ADD_EXECUTABLE (h5dump IMPORTED)
IF (USE_SHARED_LIBS AND WIN32)
  SET_PROPERTY (TARGET h5repack PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repackdll")
  SET_PROPERTY (TARGET h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dumpdll")
ELSE (USE_SHARED_LIBS AND WIN32)
  SET_PROPERTY (TARGET h5repack PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repack")
  SET_PROPERTY (TARGET h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dump")
ENDIF (USE_SHARED_LIBS AND WIN32)
SET (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:h5dump>)
#SET (LINK_LIBS ${LINK_LIBS} ${HDF5_LIBRARIES})
INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# bzip2 Library Settings
#-----------------------------------------------------------------------------
FIND_PACKAGE (BZ2 NAMES ${BZ2_PACKAGE_NAME})
IF (NOT BZ2_FOUND)
  FIND_PACKAGE (BZ2) # Legacy find
  IF (NOT BZ2_FOUND)
    MESSAGE (FATAL_ERROR " bz2 is Required for h5bz2 library")
  ENDIF (NOT BZ2_FOUND)
ENDIF (NOT BZ2_FOUND)
MESSAGE (STATUS "bz2 found: INC=${BZ2_INCLUDE_DIR} TOOLS=${BZ2_LIBRARIES}")
#SET (LINK_LIBS ${LINK_LIBS} ${BZ2_LIBRARIES})
INCLUDE_DIRECTORIES (${BZ2_INCLUDE_DIR})

IF (MSVC)
  SET (CMAKE_MFC_FLAG 0)
ENDIF (MSVC)

SET (MAKE_SYSTEM)
IF (CMAKE_BUILD_TOOL MATCHES "make")
  SET (MAKE_SYSTEM 1)
ENDIF (CMAKE_BUILD_TOOL MATCHES "make")

SET (CFG_INIT "/${CMAKE_CFG_INTDIR}")
IF (MAKE_SYSTEM)
  SET (CFG_INIT "")
ENDIF (MAKE_SYSTEM)

#-----------------------------------------------------------------------------
# Compiler specific flags : Shouldn't there be compiler tests for these
#-----------------------------------------------------------------------------
IF (CMAKE_COMPILER_IS_GNUCC)
  SET (CMAKE_C_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_C_FLAGS} -std=c99 -fomit-frame-pointer -finline-functions -fno-common")
ENDIF (CMAKE_COMPILER_IS_GNUCC)
IF (CMAKE_COMPILER_IS_GNUCXX)
  SET (CMAKE_CXX_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -fno-common")
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

#-----------------------------------------------------------------------------
# This is in here to help some of the GCC based IDES like Eclipse
# and code blocks parse the compiler errors and warnings better.
#-----------------------------------------------------------------------------
IF (CMAKE_COMPILER_IS_GNUCC)
  SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF (CMAKE_COMPILER_IS_GNUCC)
IF (CMAKE_COMPILER_IS_GNUCXX)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
OPTION (BUILD_TESTING "Build h5bz2 Unit Testing" OFF)
IF (BUILD_TESTING)
  SET (DART_TESTING_TIMEOUT 1200 CACHE INTEGER
       "Timeout in seconds for each test (default 1200=20minutes)")
  ENABLE_TESTING ()
  INCLUDE (CTest)
ENDIF (BUILD_TESTING)

ADD_SUBDIRECTORY (${H5BZ2_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
IF (BUILD_EXAMPLES)
  ADD_SUBDIRECTORY (${H5BZ2_SOURCE_DIR}/example ${PROJECT_BINARY_DIR}/example)
ENDIF (BUILD_EXAMPLES)
  