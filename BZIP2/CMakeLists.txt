cmake_minimum_required (VERSION 2.8.10)
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
SET (H5BZ2_PACKAGE_NAME "h5bzip2")
SET (H5BZ2_PACKAGE_VERSION "${H5BZ2_VERS_MAJOR}.${H5BZ2_VERS_MINOR}.${H5BZ2_VERS_RELEASE}")
SET (H5BZ2_PACKAGE_VERSION_STRING "${H5BZ2_PACKAGE_VERSION}")
SET (H5BZ2_PACKAGE_VERSION_MAJOR "${H5BZ2_VERS_MAJOR}")
SET (H5BZ2_PACKAGE_VERSION_MINOR "${H5BZ2_VERS_MINOR}")
SET (H5BZ2_PACKAGE_STRING "${H5BZ2_PACKAGE_NAME} ${H5BZ2_PACKAGE_VERSION}")
SET (H5BZ2_PACKAGE_TARNAME "${H5BZ2_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
SET (H5BZ2_PACKAGE_URL "http://www.hdfgroup.org")
SET (H5BZ2_PACKAGE_BUGREPORT "help@hdfgroup.org")

#-----------------------------------------------------------------------------
# Set Install folder value
#-----------------------------------------------------------------------------
IF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  IF(CMAKE_HOST_UNIX)
    SET(CMAKE_INSTALL_PREFIX "/HDF_Group/${H5BZ2_PACKAGE_NAME}/${H5BZ2_PACKAGE_VERSION}"
      CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
  ELSE(CMAKE_HOST_UNIX)
    GetDefaultWindowsPrefixBase(CMAKE_GENERIC_PROGRAM_FILES)
    SET(CMAKE_INSTALL_PREFIX
      "${CMAKE_GENERIC_PROGRAM_FILES}/HDF_Group/${H5BZ2_PACKAGE_NAME}/${H5BZ2_PACKAGE_VERSION}"
      CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
    SET(CMAKE_GENERIC_PROGRAM_FILES)
  ENDIF(CMAKE_HOST_UNIX)
ENDIF (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

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
# Targets built within this project are exported at Install time for use
# by other projects using FindH5BZ2. 
#-----------------------------------------------------------------------------
IF (NOT H5BZ2_EXPORTED_TARGETS)
  SET (H5BZ2_EXPORTED_TARGETS "H5bz2-targets")
ENDIF (NOT H5BZ2_EXPORTED_TARGETS)

#-----------------------------------------------------------------------------
# To include a library in the list exported by the project AT BUILD TIME,
# add it to this variable. This is NOT used by Make Install, but for projects
# which include h5bzip2 as a sub-project within their build tree
#-----------------------------------------------------------------------------
SET_GLOBAL_VARIABLE(H5BZ2_LIBRARIES_TO_EXPORT "")

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
  MESSAGE (FATAL_ERROR " ${H5BZ2_PACKAGE_NAME} library must be built as a shared library")
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
      MESSAGE (FATAL_ERROR " SHARED HDF5 is Required for ${H5BZ2_PACKAGE_NAME} library")
    ENDIF (HDF5_BUILD_SHARED_LIBS)
  ELSE (HDF5_FOUND)
    MESSAGE (FATAL_ERROR " HDF5 is Required for ${H5BZ2_PACKAGE_NAME} library")
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
    MESSAGE (FATAL_ERROR " bz2 is Required for ${H5BZ2_PACKAGE_NAME} library")
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
 
#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
IF (NOT H5BZ2_EXTERNALLY_CONFIGURED)
  INSTALL (
      EXPORT ${H5BZ2_EXPORTED_TARGETS}
      DESTINATION ${H5BZ2_INSTALL_CMAKE_DIR}/${H5BZ2_PACKAGE}
      FILE ${H5BZ2_PACKAGE}${H5BZ2_PACKAGE_EXT}-targets.cmake
      COMPONENT configinstall
  )
ENDIF (NOT H5BZ2_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Export all exported targets to the build tree for use by parent project
#-----------------------------------------------------------------------------
IF (NOT H5BZ2_EXTERNALLY_CONFIGURED)
  EXPORT (
      TARGETS ${H5BZ2_LIBRARIES_TO_EXPORT} ${H5BZ2_LIB_DEPENDENCIES}
      FILE ${H5BZ2_PACKAGE}${H5BZ2_PACKAGE_EXT}-targets.cmake
  )
ENDIF (NOT H5BZ2_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Configure the bzip2-config.cmake file for the build directory
#-----------------------------------------------------------------------------
SET (H5BZ2_INCLUDES_BUILD_TIME
    ${H5BZ2_SRC_DIR} ${H5BZ2_BINARY_DIR}
)
SET (H5BZ2_VERSION_STRING @H5BZ2_PACKAGE_VERSION@)
SET (H5BZ2_VERSION_MAJOR  @H5BZ2_PACKAGE_VERSION_MAJOR@)
SET (H5BZ2_VERSION_MINOR  @H5BZ2_PACKAGE_VERSION_MINOR@)

CONFIGURE_FILE (
    ${H5BZ2_RESOURCES_DIR}/h5bzip2-config.cmake.build.in 
    ${H5BZ2_BINARY_DIR}/${H5BZ2_PACKAGE}${H5BZ2_PACKAGE_EXT}-config.cmake @ONLY
)

#-----------------------------------------------------------------------------
# Configure the bzip2-config.cmake file for the install directory
#-----------------------------------------------------------------------------
IF (NOT H5BZ2_EXTERNALLY_CONFIGURED)
  CONFIGURE_FILE (
      ${H5BZ2_RESOURCES_DIR}/h5bzip2-config.cmake.install.in
      ${H5BZ2_BINARY_DIR}/CMakeFiles/${H5BZ2_PACKAGE}${H5BZ2_PACKAGE_EXT}-config.cmake @ONLY
  )
  INSTALL (
      FILES ${H5BZ2_BINARY_DIR}/CMakeFiles/${H5BZ2_PACKAGE}${H5BZ2_PACKAGE_EXT}-config.cmake
      DESTINATION ${H5BZ2_INSTALL_CMAKE_DIR}/${H5BZ2_PACKAGE}
      COMPONENT configinstall
  )
ENDIF (NOT H5BZ2_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Configure the bzip2-config-version.cmake file for the install directory
#-----------------------------------------------------------------------------
IF (NOT H5BZ2_EXTERNALLY_CONFIGURED)
  CONFIGURE_FILE (
      ${H5BZ2_RESOURCES_DIR}/h5bzip2-config-version.cmake.in
      ${H5BZ2_BINARY_DIR}/CMakeFiles/${H5BZ2_PACKAGE}${H5BZ2_PACKAGE_EXT}-config-version.cmake @ONLY
  )
  INSTALL (
      FILES ${H5BZ2_BINARY_DIR}/CMakeFiles/${H5BZ2_PACKAGE}${H5BZ2_PACKAGE_EXT}-config-version.cmake
      DESTINATION ${H5BZ2_INSTALL_CMAKE_DIR}/${H5BZ2_PACKAGE}
      COMPONENT configinstall
  )
ENDIF (NOT H5BZ2_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Configure the README.txt file for the binary package
#-----------------------------------------------------------------------------
SET (BINARY_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
SET (BINARY_PLATFORM "${CMAKE_SYSTEM_NAME}")
IF (WIN32)
  SET (BINARY_EXAMPLE_ENDING "zip")
  SET (BINARY_INSTALL_ENDING "exe")
  IF (CMAKE_CL_64)
    SET (BINARY_SYSTEM_NAME "win64")
  ELSE (CMAKE_CL_64)
    SET (BINARY_SYSTEM_NAME "win32")
  ENDIF (CMAKE_CL_64)
  IF (${CMAKE_SYSTEM_VERSION} MATCHES "6.1")
    SET (BINARY_PLATFORM "${BINARY_PLATFORM} 7")
  ELSEIF (${CMAKE_SYSTEM_VERSION} MATCHES "6.2")
    SET (BINARY_PLATFORM "${BINARY_PLATFORM} 8")
  ENDIF (${CMAKE_SYSTEM_VERSION} MATCHES "6.1")
  SET (BINARY_PLATFORM "${BINARY_PLATFORM} ${MSVC_C_ARCHITECTURE_ID}")
  IF (${CMAKE_C_COMPILER_VERSION} MATCHES "16.*")
    SET (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2010")
  ELSEIF (${CMAKE_C_COMPILER_VERSION} MATCHES "15.*")
    SET (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2008")
  ELSEIF (${CMAKE_C_COMPILER_VERSION} MATCHES "17.*")
    SET (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2012")
  ELSE (${CMAKE_C_COMPILER_VERSION} MATCHES "16.*")
    SET (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO ${CMAKE_C_COMPILER_VERSION}")
  ENDIF (${CMAKE_C_COMPILER_VERSION} MATCHES "16.*")
ELSEIF (APPLE)
  SET (BINARY_EXAMPLE_ENDING "tar.gz")
  SET (BINARY_INSTALL_ENDING "dmg")
  SET (BINARY_PLATFORM "${BINARY_PLATFORM} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR}")
  SET (BINARY_PLATFORM "${BINARY_PLATFORM}, using ${CMAKE_C_COMPILER_ID} C ${CMAKE_C_COMPILER_VERSION}")
ELSE (WIN32)
  SET (BINARY_EXAMPLE_ENDING "tar.gz")
  SET (BINARY_INSTALL_ENDING "sh")
  SET (BINARY_PLATFORM "${BINARY_PLATFORM} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR}")
  SET (BINARY_PLATFORM "${BINARY_PLATFORM}, using ${CMAKE_C_COMPILER_ID} C ${CMAKE_C_COMPILER_VERSION}")
ENDIF (WIN32)

CONFIGURE_FILE (
    ${H5BZ2_RESOURCES_DIR}/README.txt.cmake.in 
    ${H5BZ2_BINARY_DIR}/README.txt @ONLY
)


#-----------------------------------------------------------------------------
# Set the cpack variables
#-----------------------------------------------------------------------------
IF (NOT H5BZ2_EXTERNALLY_CONFIGURED)
  SET (CPACK_PACKAGE_VENDOR "HDF_Group")
  SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY "h5bzip2 Installation")
  SET (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
  SET (CPACK_PACKAGE_NAME "${H5BZ2_PACKAGE_NAME}")
  SET (CPACK_PACKAGE_VERSION "${H5BZ2_PACKAGE_VERSION}")
  SET (CPACK_PACKAGE_VERSION_MAJOR "${H5BZ2_PACKAGE_VERSION_MAJOR}")
  SET (CPACK_PACKAGE_VERSION_MINOR "${H5BZ2_PACKAGE_VERSION_MINOR}")
  SET (CPACK_PACKAGE_VERSION_PATCH "")
  SET (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}/${CPACK_PACKAGE_NAME}/${CPACK_PACKAGE_VERSION}")
  #SET (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
  SET (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
  SET (CPACK_PACKAGE_RELOCATABLE TRUE)

  SET (CPACK_GENERATOR "TGZ") 
  IF (WIN32)
    LIST (APPEND CPACK_GENERATOR "NSIS") 
    # Installers for 32- vs. 64-bit CMake:
    #  - Root install directory (displayed to end user at installer-run time)
    #  - "NSIS package/display name" (text used in the installer GUI)
    #  - Registry key used to store info about the installation
    IF (CMAKE_CL_64)
      SET (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
      SET (CPACK_NSIS_PACKAGE_NAME "${H5BZ2_PACKAGE_STRING} (Win64)")
      SET (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${H5BZ2_PACKAGE_STRING}-${LIB_TYPE} (Win64)")
    ELSE (CMAKE_CL_64)
      SET (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
      SET (CPACK_NSIS_PACKAGE_NAME "${H5BZ2_PACKAGE_STRING}")
      SET (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${H5BZ2_PACKAGE_STRING}-${LIB_TYPE}")
    ENDIF (CMAKE_CL_64)
    SET (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_NAME}\\\\${CPACK_PACKAGE_VERSION}")
    SET (CPACK_MONOLITHIC_INSTALL ON)
    SET (CPACK_NSIS_CONTACT "${H5BZ2_PACKAGE_BUGREPORT}")
    SET (CPACK_NSIS_MODIFY_PATH ON)
  ELSEIF (APPLE)
    LIST (APPEND CPACK_GENERATOR "DragNDrop") 
    SET (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
    SET (CPACK_PACKAGE_DEFAULT_LOCATION "/opt/${CPACK_PACKAGE_NAME}")
    SET (CPACK_PACKAGING_INSTALL_PREFIX "/")
#    SET (CPACK_PACKAGE_ICON "${H5BZ2_RESOURCES_DIR}/h5bz2.gif")
    SET (CPACK_SET_DESTDIR TRUE) # Required when packaging, and set CMAKE_INSTALL_PREFIX to "/".
  ELSE (WIN32)
    LIST (APPEND CPACK_GENERATOR "STGZ") 
    SET (CPACK_PACKAGING_INSTALL_PREFIX "/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
    SET (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)

    SET (CPACK_DEBIAN_PACKAGE_SECTION "Libraries")
    SET (CPACK_DEBIAN_PACKAGE_MAINTAINER "${H5BZ2_PACKAGE_BUGREPORT}")
    
#      LIST (APPEND CPACK_GENERATOR "RPM") 
    SET (CPACK_RPM_PACKAGE_RELEASE "1")
    SET (CPACK_RPM_COMPONENT_INSTALL ON)
    SET (CPACK_RPM_PACKAGE_RELOCATABLE ON)
  ENDIF (WIN32)
  

  # By default, do not warn when built on machines using only VS Express:
  IF (NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
    SET (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
  ENDIF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
  INCLUDE(InstallRequiredSystemLibraries)

  SET (CPACK_INSTALL_CMAKE_PROJECTS "${H5BZ2_BINARY_DIR};h5bz2;ALL;/")
  
  INCLUDE (CPack)

  CPACK_ADD_COMPONENT (libraries 
      DISPLAY_NAME "h5bzip2 Libraries"
      GROUP Runtime
  )
  CPACK_ADD_COMPONENT (headers 
      DISPLAY_NAME "h5bzip2 Headers" 
      DEPENDS libraries
      GROUP Development
  )
  CPACK_ADD_COMPONENT (hdfdocuments 
      DISPLAY_NAME "h5bzip2 Documents"
      GROUP Documents
  )
  CPACK_ADD_COMPONENT (configinstall 
      DISPLAY_NAME "h5bzip2 CMake files" 
      DEPENDS libraries
      GROUP Development
  )
  
ENDIF (NOT H5BZ2_EXTERNALLY_CONFIGURED)
  