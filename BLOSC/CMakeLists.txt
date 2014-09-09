cmake_minimum_required (VERSION 2.8.10)
project (H5BLOSC C)

#-----------------------------------------------------------------------------
# Basic H5BZIP stuff here
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
set (H5BLOSC_RESOURCES_DIR           ${H5BLOSC_SOURCE_DIR}/config/cmake)
set (H5BLOSC_SRC_DIR                 ${H5BLOSC_SOURCE_DIR}/src)

#-----------------------------------------------------------------------------
# Set the core names of all the libraries
#-----------------------------------------------------------------------------
set (H5BLOSC_LIB_CORENAME              "h5blosc")

#-----------------------------------------------------------------------------
# Set the true names of all the libraries if customized by external project
#-----------------------------------------------------------------------------
set (H5BLOSC_LIB_NAME              "${H5BLOSC_EXTERNAL_LIB_PREFIX}${H5BLOSC_LIB_CORENAME}")

#-----------------------------------------------------------------------------
# Set the target names of all the libraries
#-----------------------------------------------------------------------------
set (H5BLOSC_LIB_TARGET              ${H5BLOSC_LIB_CORENAME})

if (NOT H5BLOSC_INSTALL_BIN_DIR)
  set (H5BLOSC_INSTALL_BIN_DIR bin)
endif (NOT H5BLOSC_INSTALL_BIN_DIR)
if (NOT H5BLOSC_INSTALL_LIB_DIR)
  set (H5BLOSC_INSTALL_LIB_DIR lib)
endif (NOT H5BLOSC_INSTALL_LIB_DIR)
if (NOT H5BLOSC_INSTALL_INCLUDE_DIR)
  set (H5BLOSC_INSTALL_INCLUDE_DIR include)
endif (NOT H5BLOSC_INSTALL_INCLUDE_DIR)
if (NOT H5BLOSC_INSTALL_DATA_DIR)
  if (NOT WIN32)
    set (H5BLOSC_INSTALL_DATA_DIR share)
    set (H5BLOSC_INSTALL_CMAKE_DIR share/cmake)
  else (NOT WIN32)
    set (H5BLOSC_INSTALL_DATA_DIR ".")
    set (H5BLOSC_INSTALL_CMAKE_DIR cmake)
  endif (NOT WIN32)
endif (NOT H5BLOSC_INSTALL_DATA_DIR)

set (H5BLOSC_VERS_MAJOR 3)
set (H5BLOSC_VERS_MINOR 1)
set (H5BLOSC_VERS_RELEASE 3)

set (H5BLOSC_SOVERS_MAJOR 3)
set (H5BLOSC_SOVERS_MINOR 1)
set (H5BLOSC_SOVERS_INTERFACE 3)
set (H5BLOSC_SOVERS_RELEASE 0)
math (EXPR H5BLOSC_SOVERS_MAJOR ${H5BLOSC_SOVERS_INTERFACE}-${H5BLOSC_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5BLOSC_PACKAGE "h5blosc")
set (H5BLOSC_PACKAGE_NAME "h5blosc")
set (H5BLOSC_PACKAGE_VERSION "${H5BLOSC_VERS_MAJOR}.${H5BLOSC_VERS_MINOR}.${H5BLOSC_VERS_RELEASE}")
set (H5BLOSC_PACKAGE_VERSION_STRING "${H5BLOSC_PACKAGE_VERSION}")
set (H5BLOSC_PACKAGE_VERSION_MAJOR "${H5BLOSC_VERS_MAJOR}")
set (H5BLOSC_PACKAGE_VERSION_MINOR "${H5BLOSC_VERS_MINOR}")
set (H5BLOSC_PACKAGE_STRING "${H5BLOSC_PACKAGE_NAME} ${H5BLOSC_PACKAGE_VERSION}")
set (H5BLOSC_PACKAGE_TARNAME "${H5BLOSC_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5BLOSC_PACKAGE_URL "http://www.hdfgroup.org")
set (H5BLOSC_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5BLOSC_PACKAGE_SOVERSION "${H5BLOSC_SOVERS_MAJOR}.${H5BLOSC_SOVERS_RELEASE}.${H5BLOSC_SOVERS_MINOR}")

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5BLOSC_RESOURCES_DIR}/HDFMacros.cmake)
include (${H5BLOSC_RESOURCES_DIR}/H5BLOSCMacros.cmake)

#-----------------------------------------------------------------------------
# Setup output Directories
#-----------------------------------------------------------------------------
if (NOT H5BLOSC_EXTERNALLY_CONFIGURED)
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
  )
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries"
  )
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries."
  )
else (NOT H5BLOSC_EXTERNALLY_CONFIGURED)
  # if we are externally configured, but the project uses old cmake scripts
  # this may not be set
  if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
  endif (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
endif (NOT H5BLOSC_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Targets built within this project are exported at Install time for use
# by other projects using FindH5BLOSC. 
#-----------------------------------------------------------------------------
if (NOT H5BLOSC_EXPORTED_TARGETS)
  set (H5BLOSC_EXPORTED_TARGETS "H5blosc-targets")
endif (NOT H5BLOSC_EXPORTED_TARGETS)

#-----------------------------------------------------------------------------
# To include a library in the list exported by the project AT BUILD TIME,
# add it to this variable. This is NOT used by Make Install, but for projects
# which include h5blosc as a sub-project within their build tree
#-----------------------------------------------------------------------------
SET_GLOBAL_VARIABLE (H5BLOSC_LIBRARIES_TO_EXPORT "")

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5BLOSC_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# Option to Build Shared/Static libs, only shared is allowed
#-----------------------------------------------------------------------------
option (BUILD_SHARED_LIBS "Build Shared Libraries" ON)
set (LIB_TYPE SHARED)
if (BUILD_SHARED_LIBS)
  set (LIB_TYPE SHARED)
  ADD_DEFINITIONS (-DH5BLOSC_BUILT_AS_DYNAMIC_LIB)
else (BUILD_SHARED_LIBS)
  message (FATAL_ERROR " ${H5BLOSC_PACKAGE_NAME} library must be built as a shared library")
endif (BUILD_SHARED_LIBS)

#-----------------------------------------------------------------------------
# blosc requires pthreads
# CMAKE_THREAD_LIBS_INIT        - the thread library to link into an application (-lpthread )
# CMAKE_USE_SPROC_INIT          - are we using sproc?
# CMAKE_USE_WIN32_THREADS_INIT  - are we using WIN32 threads
# CMAKE_USE_PTHREADS_INIT       - are we using pthreads
# CMAKE_HP_PTHREADS_INIT        - are we using hp pthreads
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
set (CMAKE_MODULE_PATH ${HDF_RESOURCES_DIR} ${CMAKE_MODULE_PATH})

set (HDF5_USE_SHARED_LIBRARIES 1)
ADD_DEFINITIONS (-DH5_USE_18_API)

if (NOT HDF5_HDF5_HEADER)
  find_package (HDF5 NAMES ${HDF5_PACKAGE_NAME})
  if (NOT HDF5_FOUND)
    find_package (HDF5) # Legacy find
    set (HDF5_BUILD_SHARED_LIBS 1)
  endif (NOT HDF5_FOUND)
  if (HDF5_FOUND)
    set (HDF5_HAVE_H5PUBCONF_H 1)
    set (HDF5_HAVE_HDF5 1)
    set (HDF5_HDF5_HEADER "h5pubconf.h")
    set (HDF5_INCLUDE_DIR_GEN ${HDF5_INCLUDE_DIR})
    message (STATUS "HDF5 found: INC=${HDF5_INCLUDE_DIR} TOOLS=${HDF5_TOOLS_DIR}")
    if ((NOT USE_SHARED_LIBS AND HDF5_BUILD_SHARED_LIBS) OR (USE_SHARED_LIBS AND NOT HDF5_BUILD_SHARED_LIBS))
      message (STATUS "HDF5 build type does match requested type")
    endif ((NOT USE_SHARED_LIBS AND HDF5_BUILD_SHARED_LIBS) OR (USE_SHARED_LIBS AND NOT HDF5_BUILD_SHARED_LIBS))
    if (HDF5_BUILD_SHARED_LIBS)
      ADD_DEFINITIONS (-DH5_BUILT_AS_DYNAMIC_LIB)
    else (HDF5_BUILD_SHARED_LIBS)
      ADD_DEFINITIONS (-DH5_BUILT_AS_STATIC_LIB)
    endif (HDF5_BUILD_SHARED_LIBS)
  else (HDF5_FOUND)
    message (FATAL_ERROR " HDF5 is Required for ${H5BLOSC_PACKAGE_NAME} library")
  endif (HDF5_FOUND)
else (NOT HDF5_HDF5_HEADER)
  # This project is being called from within another and HDF5 is already configured
  set (HDF5_HAVE_H5PUBCONF_H 1)
  set (HDF5_HAVE_HDF5 1)
endif (NOT HDF5_HDF5_HEADER)
add_executable (h5repack IMPORTED)
add_executable (h5dump IMPORTED)
if (USE_SHARED_LIBS AND WIN32)
  set_property (TARGET h5repack PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repackdll")
  set_property (TARGET h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dumpdll")
else (USE_SHARED_LIBS AND WIN32)
  set_property (TARGET h5repack PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repack")
  set_property (TARGET h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dump")
endif (USE_SHARED_LIBS AND WIN32)
set (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:h5dump>)
#set (LINK_LIBS ${LINK_LIBS} ${HDF5_LIBRARIES})
INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIRS})

include (ExternalProject)
set (H5BLOSC_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO SVN TGZ)")
set_property (CACHE H5BLOSC_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO SVN TGZ)
if (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  option (BLOSC_USE_EXTERNAL "Use External Library Building for BLOSC" 1)
  if (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN")
    set (BLOSC_URL ${BLOSC_SVN_URL})
  elseif (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${H5BLOSC_SOURCE_DIR})
    endif (NOT TGZPATH)
    set (BLOSC_URL ${TGZPATH}/${BLOSC_TGZ_NAME})
  else (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN")
    set (BLOSC_USE_EXTERNAL 0)
  endif (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN")
endif (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
#-----------------------------------------------------------------------------
# blosc Library Settings
#-----------------------------------------------------------------------------
if (NOT BLOSC_USE_EXTERNAL)
  find_package (BLOSC NAMES ${BLOSC_PACKAGE_NAME})
  if (NOT BLOSC_FOUND)
    find_package (BLOSC) # Legacy find
  endif (NOT BLOSC_FOUND)
endif (NOT BLOSC_USE_EXTERNAL)
if (BLOSC_FOUND)
  set (H5BLOSC_HAVE_BLOSC_H 1)
  set (H5BLOSC_HAVE_BLOSC 1)
  set (H5BLOSC_BLOSC_HEADER "blosc.h")
  set (BLOSC_INCLUDE_DIR_GEN ${BLOSC_INCLUDE_DIR})
  set (BLOSC_INCLUDE_DIRS ${BLOSC_INCLUDE_DIR})
else (BLOSC_FOUND)
  if (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    EXTERNAL_BLOSC_LIBRARY (${H5BLOSC_ALLOW_EXTERNAL_SUPPORT} ${LIB_TYPE})
    set (H5BLOSC_HAVE_BLOSC_H 1)
    set (H5BLOSC_HAVE_BLOSC 1)
    message (STATUS "Filter BLOSC is built")
  else (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    message (FATAL_ERROR " blosc is Required for ${H5BLOSC_PACKAGE_NAME} library")
  endif (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
endif (BLOSC_FOUND)
message (STATUS "blosc found: INC=${BLOSC_INCLUDE_DIR} LIBS=${BLOSC_LIBRARIES}")
set (LINK_LIBS ${LINK_LIBS} ${BLOSC_LIBRARIES})
INCLUDE_DIRECTORIES (${BLOSC_INCLUDE_DIRS})

if (BLOSC_FOUND)
  PACKAGE_BLOSC_LIBRARY (${H5BLOSC_ALLOW_EXTERNAL_SUPPORT})
endif (BLOSC_FOUND)

if (MSVC)
  set (CMAKE_MFC_FLAG 0)
endif (MSVC)

set (MAKE_SYSTEM)
if (CMAKE_BUILD_TOOL MATCHES "make")
  set (MAKE_SYSTEM 1)
endif (CMAKE_BUILD_TOOL MATCHES "make")

set (CFG_INIT "/${CMAKE_CFG_INTDIR}")
if (MAKE_SYSTEM)
  set (CFG_INIT "")
endif (MAKE_SYSTEM)

#-----------------------------------------------------------------------------
# Compiler specific flags : Shouldn't there be compiler tests for these
#-----------------------------------------------------------------------------
if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_C_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_C_FLAGS} -std=c99 -fomit-frame-pointer -finline-functions -fno-common")
endif (CMAKE_COMPILER_IS_GNUCC)
if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -fno-common")
endif (CMAKE_COMPILER_IS_GNUCXX)

#-----------------------------------------------------------------------------
# This is in here to help some of the GCC based IDES like Eclipse
# and code blocks parse the compiler errors and warnings better.
#-----------------------------------------------------------------------------
if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
endif (CMAKE_COMPILER_IS_GNUCC)
if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
endif (CMAKE_COMPILER_IS_GNUCXX)

#-----------------------------------------------------------------------------
# Generate the config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5BLOSC_RESOURCES_DIR}/config.h.in ${H5BLOSC_BINARY_DIR}/config.h @ONLY)

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5blosc Unit Testing" OFF)
if (BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE INTEGER
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing ()
  include (CTest)
endif (BUILD_TESTING)

add_subdirectory (${H5BLOSC_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
if (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  if (BLOSC_FOUND AND BLOSC_USE_EXTERNAL)
    ADD_DEPENDENCIES (${H5BLOSC_LIB_TARGET} BLOSC)
  endif (BLOSC_FOUND AND BLOSC_USE_EXTERNAL)
  if (HDF5_FOUND AND HDF5_USE_EXTERNAL)
    ADD_DEPENDENCIES (${H5BLOSC_LIB_TARGET} HDF5)
  endif (HDF5_FOUND AND HDF5_USE_EXTERNAL)
endif (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")

if (BUILD_EXAMPLES)
  add_subdirectory (${H5BLOSC_SOURCE_DIR}/example ${PROJECT_BINARY_DIR}/example)
endif (BUILD_EXAMPLES)
 
#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (NOT H5BLOSC_EXTERNALLY_CONFIGURED)
  install (
      EXPORT ${H5BLOSC_EXPORTED_TARGETS}
      DESTINATION ${H5BLOSC_INSTALL_CMAKE_DIR}/${H5BLOSC_PACKAGE}
      FILE ${H5BLOSC_PACKAGE}${H5BLOSC_PACKAGE_EXT}-targets.cmake
      COMPONENT configinstall
  )
endif (NOT H5BLOSC_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Export all exported targets to the build tree for use by parent project
#-----------------------------------------------------------------------------
if (NOT H5BLOSC_EXTERNALLY_CONFIGURED)
  export (
      TARGETS ${H5BLOSC_LIBRARIES_TO_EXPORT} ${H5BLOSC_LIB_DEPENDENCIES}
      FILE ${H5BLOSC_PACKAGE}${H5BLOSC_PACKAGE_EXT}-targets.cmake
  )
endif (NOT H5BLOSC_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Configure the blosc-config.cmake file for the build directory
#-----------------------------------------------------------------------------
set (H5BLOSC_INCLUDES_BUILD_TIME
    ${H5BLOSC_SRC_DIR} ${H5BLOSC_BINARY_DIR}
)
set (H5BLOSC_VERSION_STRING @H5BLOSC_PACKAGE_VERSION@)
set (H5BLOSC_VERSION_MAJOR  @H5BLOSC_PACKAGE_VERSION_MAJOR@)
set (H5BLOSC_VERSION_MINOR  @H5BLOSC_PACKAGE_VERSION_MINOR@)

configure_file (
    ${H5BLOSC_RESOURCES_DIR}/h5blosc-config.cmake.build.in 
    ${H5BLOSC_BINARY_DIR}/${H5BLOSC_PACKAGE}${H5BLOSC_PACKAGE_EXT}-config.cmake @ONLY
)

#-----------------------------------------------------------------------------
# Configure the blosc-config.cmake file for the install directory
#-----------------------------------------------------------------------------
if (NOT H5BLOSC_EXTERNALLY_CONFIGURED)
  configure_file (
      ${H5BLOSC_RESOURCES_DIR}/h5blosc-config.cmake.install.in
      ${H5BLOSC_BINARY_DIR}/CMakeFiles/${H5BLOSC_PACKAGE}${H5BLOSC_PACKAGE_EXT}-config.cmake @ONLY
  )
  install (
      FILES ${H5BLOSC_BINARY_DIR}/CMakeFiles/${H5BLOSC_PACKAGE}${H5BLOSC_PACKAGE_EXT}-config.cmake
      DESTINATION ${H5BLOSC_INSTALL_CMAKE_DIR}/${H5BLOSC_PACKAGE}
      COMPONENT configinstall
  )
endif (NOT H5BLOSC_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Configure the blosc-config-version.cmake file for the install directory
#-----------------------------------------------------------------------------
if (NOT H5BLOSC_EXTERNALLY_CONFIGURED)
  configure_file (
      ${H5BLOSC_RESOURCES_DIR}/h5blosc-config-version.cmake.in
      ${H5BLOSC_BINARY_DIR}/CMakeFiles/${H5BLOSC_PACKAGE}${H5BLOSC_PACKAGE_EXT}-config-version.cmake @ONLY
  )
  install (
      FILES ${H5BLOSC_BINARY_DIR}/CMakeFiles/${H5BLOSC_PACKAGE}${H5BLOSC_PACKAGE_EXT}-config-version.cmake
      DESTINATION ${H5BLOSC_INSTALL_CMAKE_DIR}/${H5BLOSC_PACKAGE}
      COMPONENT configinstall
  )
endif (NOT H5BLOSC_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Configure the README.txt file for the binary package
#-----------------------------------------------------------------------------
set (BINARY_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
set (BINARY_PLATFORM "${CMAKE_SYSTEM_NAME}")
if (WIN32)
  set (BINARY_EXAMPLE_ENDING "zip")
  set (BINARY_INSTALL_ENDING "exe")
  if (CMAKE_CL_64)
    set (BINARY_SYSTEM_NAME "win64")
  else (CMAKE_CL_64)
    set (BINARY_SYSTEM_NAME "win32")
  endif (CMAKE_CL_64)
  if (${CMAKE_SYSTEM_VERSION} MATCHES "6.1")
    set (BINARY_PLATFORM "${BINARY_PLATFORM} 7")
  elseif (${CMAKE_SYSTEM_VERSION} MATCHES "6.2")
    set (BINARY_PLATFORM "${BINARY_PLATFORM} 8")
  endif (${CMAKE_SYSTEM_VERSION} MATCHES "6.1")
  set (BINARY_PLATFORM "${BINARY_PLATFORM} ${MSVC_C_ARCHITECTURE_ID}")
  if (${CMAKE_C_COMPILER_VERSION} MATCHES "16.*")
    set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2010")
  elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "15.*")
    set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2008")
  elseif (${CMAKE_C_COMPILER_VERSION} MATCHES "17.*")
    set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO 2012")
  else (${CMAKE_C_COMPILER_VERSION} MATCHES "16.*")
    set (BINARY_PLATFORM "${BINARY_PLATFORM}, using VISUAL STUDIO ${CMAKE_C_COMPILER_VERSION}")
  endif (${CMAKE_C_COMPILER_VERSION} MATCHES "16.*")
elseif (APPLE)
  set (BINARY_EXAMPLE_ENDING "tar.gz")
  set (BINARY_INSTALL_ENDING "dmg")
  set (BINARY_PLATFORM "${BINARY_PLATFORM} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR}")
  set (BINARY_PLATFORM "${BINARY_PLATFORM}, using ${CMAKE_C_COMPILER_ID} C ${CMAKE_C_COMPILER_VERSION}")
else (WIN32)
  set (BINARY_EXAMPLE_ENDING "tar.gz")
  set (BINARY_INSTALL_ENDING "sh")
  set (BINARY_PLATFORM "${BINARY_PLATFORM} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR}")
  set (BINARY_PLATFORM "${BINARY_PLATFORM}, using ${CMAKE_C_COMPILER_ID} C ${CMAKE_C_COMPILER_VERSION}")
endif (WIN32)

configure_file (
    ${H5BLOSC_RESOURCES_DIR}/README.txt.cmake.in 
    ${H5BLOSC_BINARY_DIR}/README.txt @ONLY
)


#-----------------------------------------------------------------------------
# Set the cpack variables
#-----------------------------------------------------------------------------
if (NOT H5BLOSC_EXTERNALLY_CONFIGURED)
  set (CPACK_PACKAGE_VENDOR "HDF_Group")
  set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "h5blosc Installation")
  set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
  set (CPACK_PACKAGE_NAME "${H5BLOSC_PACKAGE_NAME}")
  if (CDASH_LOCAL)
    set (CPACK_PACKAGE_VERSION "${H5BLOSC_PACKAGE_VERSION}")
  else (CDASH_LOCAL)
    set (CPACK_PACKAGE_VERSION "${H5BLOSC_PACKAGE_VERSION_STRING}")
  endif (CDASH_LOCAL)
  set (CPACK_PACKAGE_VERSION "${H5BLOSC_PACKAGE_VERSION}")
  set (CPACK_PACKAGE_VERSION_MAJOR "${H5BLOSC_PACKAGE_VERSION_MAJOR}")
  set (CPACK_PACKAGE_VERSION_MINOR "${H5BLOSC_PACKAGE_VERSION_MINOR}")
  set (CPACK_PACKAGE_VERSION_PATCH "")
  set (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}/${CPACK_PACKAGE_NAME}/${CPACK_PACKAGE_VERSION}")
  #set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
  set (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
  set (CPACK_PACKAGE_RELOCATABLE TRUE)

  set (CPACK_GENERATOR "TGZ") 
  if (WIN32)
    list (APPEND CPACK_GENERATOR "NSIS") 
    # Installers for 32- vs. 64-bit CMake:
    #  - Root install directory (displayed to end user at installer-run time)
    #  - "NSIS package/display name" (text used in the installer GUI)
    #  - Registry key used to store info about the installation
    if (CMAKE_CL_64)
      set (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
      set (CPACK_NSIS_PACKAGE_NAME "${H5BLOSC_PACKAGE_STRING} (Win64)")
      set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION} (Win64)")
    else (CMAKE_CL_64)
      set (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
      set (CPACK_NSIS_PACKAGE_NAME "${H5BLOSC_PACKAGE_STRING}")
      set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
    endif (CMAKE_CL_64)
    set (CPACK_NSIS_DISPLAY_NAME "@CPACK_NSIS_PACKAGE_NAME@, is a dynamic filter for HDF5")
    set (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_NAME}\\\\${CPACK_PACKAGE_VERSION}")
    set (CPACK_MONOLITHIC_INSTALL ON)
    set (CPACK_NSIS_CONTACT "${H5BLOSC_PACKAGE_BUGREPORT}")
    set (CPACK_NSIS_MODIFY_PATH ON)
  elseif (APPLE)
    list (APPEND CPACK_GENERATOR "DragNDrop") 
    set (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
    set (CPACK_PACKAGING_INSTALL_PREFIX "/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
#    set (CPACK_PACKAGE_ICON "${H5BLOSC_RESOURCES_DIR}/h5blosc.gif")
  else (WIN32)
    list (APPEND CPACK_GENERATOR "STGZ") 
    set (CPACK_PACKAGING_INSTALL_PREFIX "/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
    set (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)

    set (CPACK_DEBIAN_PACKAGE_SECTION "Libraries")
    set (CPACK_DEBIAN_PACKAGE_MAINTAINER "${H5BLOSC_PACKAGE_BUGREPORT}")
    
#      list (APPEND CPACK_GENERATOR "RPM") 
    set (CPACK_RPM_PACKAGE_RELEASE "1")
    set (CPACK_RPM_COMPONENT_INSTALL ON)
    set (CPACK_RPM_PACKAGE_RELOCATABLE ON)
  endif (WIN32)
  

  # By default, do not warn when built on machines using only VS Express:
  if (NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
    set (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
  endif (NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
  include (InstallRequiredSystemLibraries)

  set (CPACK_INSTALL_CMAKE_PROJECTS "${H5BLOSC_BINARY_DIR};h5blosc;ALL;/")
  
  if (H5BLOSC_PACKAGE_EXTLIBS)
    if (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (BLOSC_FOUND AND BLOSC_USE_EXTERNAL)
        set (CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${BLOSC_INCLUDE_DIR_GEN};BLOSC;libraries;/")
        set (CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${BLOSC_INCLUDE_DIR_GEN};BLOSC;headers;/")
        set (CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${BLOSC_INCLUDE_DIR_GEN};BLOSC;configinstall;/")
      endif (BLOSC_FOUND AND BLOSC_USE_EXTERNAL)
    endif (H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  endif (H5BLOSC_PACKAGE_EXTLIBS)
  
  include (CPack)

  cpack_add_component (libraries 
      DISPLAY_NAME "h5blosc Libraries"
      GROUP Runtime
  )
  cpack_add_component (headers 
      DISPLAY_NAME "h5blosc Headers" 
      DEPENDS libraries
      GROUP Development
  )
  cpack_add_component (hdfdocuments 
      DISPLAY_NAME "h5blosc Documents"
      GROUP Documents
  )
  cpack_add_component (configinstall 
      DISPLAY_NAME "h5blosc CMake files" 
      DEPENDS libraries
      GROUP Development
  )
  
endif (NOT H5BLOSC_EXTERNALLY_CONFIGURED)
  