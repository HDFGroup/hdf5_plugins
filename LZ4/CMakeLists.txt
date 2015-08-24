cmake_minimum_required (VERSION 3.1.0)
project (H5LZ4 C)

#-----------------------------------------------------------------------------
# Basic H5LZ4 stuff here
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
set (H5LZ4_RESOURCES_DIR           ${H5LZ4_SOURCE_DIR}/config/cmake)
set (HDF_RESOURCES_DIR             ${H5LZ4_SOURCE_DIR}/config/cmake)
set (H5LZ4_SRC_DIR                 ${H5LZ4_SOURCE_DIR}/src)

#-----------------------------------------------------------------------------
# Set the core names of all the libraries
#-----------------------------------------------------------------------------
set (H5LZ4_LIB_CORENAME              "h5lz4")

#-----------------------------------------------------------------------------
# Set the true names of all the libraries if customized by external project
#-----------------------------------------------------------------------------
set (H5LZ4_LIB_NAME              "${H5LZ4_EXTERNAL_LIB_PREFIX}${H5LZ4_LIB_CORENAME}")

#-----------------------------------------------------------------------------
# Set the target names of all the libraries
#-----------------------------------------------------------------------------
set (H5LZ4_LIB_TARGET              ${H5LZ4_LIB_CORENAME})

if (NOT H5LZ4_INSTALL_BIN_DIR)
  set (H5LZ4_INSTALL_BIN_DIR bin)
endif (NOT H5LZ4_INSTALL_BIN_DIR)
if (NOT H5LZ4_INSTALL_LIB_DIR)
  set (H5LZ4_INSTALL_LIB_DIR lib/plugin)
endif (NOT H5LZ4_INSTALL_LIB_DIR)
if (NOT H5LZ4_INSTALL_INCLUDE_DIR)
  set (H5LZ4_INSTALL_INCLUDE_DIR include)
endif (NOT H5LZ4_INSTALL_INCLUDE_DIR)
if (NOT H5LZ4_INSTALL_DATA_DIR)
  if (NOT WIN32)
    set (H5LZ4_INSTALL_DATA_DIR share)
    set (H5LZ4_INSTALL_CMAKE_DIR share/cmake)
  else (NOT WIN32)
    set (H5LZ4_INSTALL_DATA_DIR ".")
    set (H5LZ4_INSTALL_CMAKE_DIR cmake)
  endif (NOT WIN32)
endif (NOT H5LZ4_INSTALL_DATA_DIR)

set (H5LZ4_VERS_MAJOR 1)
set (H5LZ4_VERS_MINOR 0)
set (H5LZ4_VERS_RELEASE 0)

set (H5LZ4_SOVERS_MAJOR 1)
set (H5LZ4_SOVERS_MINOR 0)
set (H5LZ4_SOVERS_INTERFACE 1)
set (H5LZ4_SOVERS_RELEASE 0)
math (EXPR H5LZ4_SOVERS_MAJOR ${H5LZ4_SOVERS_INTERFACE}-${H5LZ4_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5LZ4_PACKAGE "h5lz4")
set (H5LZ4_PACKAGE_NAME "h5lz4")
set (H5LZ4_PACKAGE_VERSION "${H5LZ4_VERS_MAJOR}.${H5LZ4_VERS_MINOR}")
set (H5LZ4_PACKAGE_VERSION_STRING "${H5LZ4_PACKAGE_VERSION}")
set (H5LZ4_PACKAGE_VERSION_MAJOR "${H5LZ4_VERS_MAJOR}")
set (H5LZ4_PACKAGE_VERSION_MINOR "${H5LZ4_VERS_MINOR}")
set (H5LZ4_PACKAGE_STRING "${H5LZ4_PACKAGE_NAME} ${H5LZ4_PACKAGE_VERSION}")
set (H5LZ4_PACKAGE_TARNAME "${H5LZ4_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5LZ4_PACKAGE_URL "http://www.hdfgroup.org")
set (H5LZ4_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5LZ4_PACKAGE_SOVERSION "${H5LZ4_SOVERS_MAJOR}.${H5LZ4_SOVERS_RELEASE}.${H5LZ4_SOVERS_MINOR}")

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5LZ4_RESOURCES_DIR}/H5LZ4Macros.cmake)

#-----------------------------------------------------------------------------
# Setup output Directories
#-----------------------------------------------------------------------------
if (NOT H5LZ4_EXTERNALLY_CONFIGURED)
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
  )
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries"
  )
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries."
  )
else (NOT H5LZ4_EXTERNALLY_CONFIGURED)
  # if we are externally configured, but the project uses old cmake scripts
  # this may not be set
  if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
  endif (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
endif (NOT H5LZ4_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Targets built within this project are exported at Install time for use
# by other projects using FindH5LZ4. 
#-----------------------------------------------------------------------------
if (NOT H5LZ4_EXPORTED_TARGETS)
  set (H5LZ4_EXPORTED_TARGETS "H5lz4-targets")
endif (NOT H5LZ4_EXPORTED_TARGETS)

#-----------------------------------------------------------------------------
# To include a library in the list exported by the project AT BUILD TIME,
# add it to this variable. This is NOT used by Make Install, but for projects
# which include h5lz4 as a sub-project within their build tree
#-----------------------------------------------------------------------------
set_global_variable (H5LZ4_LIBRARIES_TO_EXPORT "")

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5LZ4_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# Option to Build Shared/Static libs, only shared is allowed
#-----------------------------------------------------------------------------
option (BUILD_SHARED_LIBS "Build Shared Libraries" ON)
set (LIB_TYPE SHARED)
if (BUILD_SHARED_LIBS)
  set (LIB_TYPE SHARED)
  add_definitions (-DH5LZ4_BUILT_AS_DYNAMIC_LIB)
else (BUILD_SHARED_LIBS)
  message (FATAL_ERROR " ${H5LZ4_PACKAGE_NAME} library must be built as a shared library")
endif (BUILD_SHARED_LIBS)

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
set (CMAKE_MODULE_PATH ${HDF_RESOURCES_DIR} ${CMAKE_MODULE_PATH})

add_definitions (-DH5_USE_18_API)

if (NOT HDF5_HDF5_HEADER)
  if (USE_SHARED_LIBS)
    set (FIND_HDF_COMPONENTS C shared)
  else (USE_SHARED_LIBS)
    set (FIND_HDF_COMPONENTS C static)
  endif (USE_SHARED_LIBS)
  message (STATUS "HDF5 find comps: ${FIND_HDF_COMPONENTS}")
  set (SEARCH_PACKAGE_NAME ${HDF5_PACKAGE_NAME})

  find_package (HDF5 NAMES ${SEARCH_PACKAGE_NAME} COMPONENTS ${FIND_HDF_COMPONENTS})
  message (STATUS "HDF5 C libs:${HDF5_FOUND} static:${HDF5_static_C_FOUND} and shared:${HDF5_shared_C_FOUND}")
  if (HDF5_FOUND)
    add_executable (h5repack IMPORTED)
    add_executable (h5dump IMPORTED)
    if (NOT HDF5_static_C_FOUND AND NOT HDF5_shared_C_FOUND)
      #find library from non-dual-binary package
      set (FIND_HDF_COMPONENTS C)
      message (STATUS "HDF5 find comps: ${FIND_HDF_COMPONENTS}")

      find_package (HDF5 NAMES ${SEARCH_PACKAGE_NAME} COMPONENTS ${FIND_HDF_COMPONENTS})
      message (STATUS "HDF5 libs:${HDF5_FOUND} C:${HDF5_C_FOUND}")
      set (HDF5_LINK_LIBS ${HDF5_LINK_LIBS} ${HDF5_LIBRARIES})
      if (USE_SHARED_LIBS AND WIN32)
        set_property (TARGET h5repack PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repackdll")
        set_property (TARGET h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dumpdll")
      else (USE_SHARED_LIBS AND WIN32)
        set_property (TARGET h5repack PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repack")
        set_property (TARGET h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dump")
      endif (USE_SHARED_LIBS AND WIN32)
    else (NOT HDF5_static_C_FOUND AND NOT HDF5_shared_C_FOUND)
      if (USE_SHARED_LIBS AND HDF5_shared_C_FOUND)
        set (HDF5_LINK_LIBS ${HDF5_LINK_LIBS} ${HDF5_C_SHARED_LIBRARY})
      else (HDF5_static_C_FOUND)
        set (HDF5_LINK_LIBS ${HDF5_LINK_LIBS} ${HDF5_C_STATIC_LIBRARY})
      endif (USE_SHARED_LIBS AND HDF5_shared_C_FOUND)
      set_property (TARGET h5repack PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repack")
      set_property (TARGET h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dump")
    endif (NOT HDF5_static_C_FOUND AND NOT HDF5_shared_C_FOUND)
    set (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:h5dump>)
    set (HDF5_REPACK_EXECUTABLE $<TARGET_FILE:h5repack>)
  else (HDF5_FOUND)
    find_package (HDF5) # Legacy find
#Legacy find_package does not set HDF5_TOOLS_DIR, so we set it here
    set(HDF5_TOOLS_DIR ${HDF5_LIBRARY_DIRS}/../bin)
#Legacy find_package does not set HDF5_BUILD_SHARED_LIBS, so we set it here
    if (USE_SHARED_LIBS AND EXISTS "${HDF5_LIBRARY_DIRS}/libhdf5.so")
      set (HDF5_BUILD_SHARED_LIBS 1)
    else ()
      set (HDF5_BUILD_SHARED_LIBS 0)
    endif ()
    set (HDF5_LINK_LIBS ${HDF5_LINK_LIBS} ${HDF5_LIBRARIES})

    add_executable (h5repack IMPORTED)
    add_executable (h5dump IMPORTED)
    set_property (TARGET h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repack")
    set_property (TARGET h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dump")
    set (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:h5repack>)
    set (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:h5dump>)
  endif (HDF5_FOUND)
  set (HDF5_PACKAGE_NAME ${SEARCH_PACKAGE_NAME})

  if (HDF5_FOUND)
    set (HDF5_HAVE_H5PUBCONF_H 1)
    set (HDF5_HAVE_HDF5 1)
    set (HDF5_HDF5_HEADER "h5pubconf.h")
    set (HDF5_INCLUDE_DIR_GEN ${HDF5_INCLUDE_DIR})
    message (STATUS "HDF5 found: INC=${HDF5_INCLUDE_DIR} TOOLS=${HDF5_TOOLS_DIR}")
  else (HDF5_FOUND)
    message (FATAL_ERROR " HDF5 is Required for ${H5LZ4_PACKAGE_NAME} library")
  endif (HDF5_FOUND)
else (NOT HDF5_HDF5_HEADER)
  # This project is being called from within another and HDF5 is already configured
  set (HDF5_HAVE_H5PUBCONF_H 1)
  set (HDF5_HAVE_HDF5 1)
endif (NOT HDF5_HDF5_HEADER)
message (STATUS "HDF5 link libs: ${HDF5_LINK_LIBS}")

if (USE_SHARED_LIBS)
  set (H5_LIB_TYPE SHARED)
else (USE_SHARED_LIBS)
  set (H5_LIB_TYPE STATIC)
endif (USE_SHARED_LIBS)

INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIR})

option (BUILD_LZ4_LIBRARY_SOURCE "build the lz4 library within the plugin" OFF)
if (NOT BUILD_LZ4_LIBRARY_SOURCE)
  include (ExternalProject)
  set (H5LZ4_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO SVN TGZ)")
  set_property (CACHE H5LZ4_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO SVN TGZ)
  if (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    option (LZ4_USE_EXTERNAL "Use External Library Building for LZ4" 1)
    if (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN")
      set (LZ4_URL ${LZ4_SVN_URL})
    elseif (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5LZ4_SOURCE_DIR})
      endif (NOT TGZPATH)
      set (LZ4_URL ${TGZPATH}/${LZ4_TGZ_NAME})
    else (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN")
      set (LZ4_USE_EXTERNAL 0)
    endif (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN")
  endif (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  #-----------------------------------------------------------------------------
  # lz4 Library Settings
  #-----------------------------------------------------------------------------
  if (NOT LZ4_USE_EXTERNAL)
    find_package (LZ4 NAMES ${LZ4_PACKAGE_NAME})
    if (NOT LZ4_FOUND)
      find_package (LZ4) # Legacy find
      if (NOT LZ4_FOUND)
        message (FATAL_ERROR " lz4 is Required for ${H5LZ4_PACKAGE_NAME} library")
      endif (NOT LZ4_FOUND)
    endif (NOT LZ4_FOUND)
  endif (NOT LZ4_USE_EXTERNAL)
  if (LZ4_FOUND)
    set (H5LZ4_HAVE_LZ4_H 1)
    set (H5LZ4_HAVE_LZ4 1)
    set (H5LZ4_LZ4_HEADER "lz4.h")
    set (H5LZ4_INCLUDE_DIR_GEN ${LZ4_INCLUDE_DIR})
    set (H5LZ4_INCLUDE_DIRS ${LZ4_INCLUDE_DIR})
  else (LZ4_FOUND)
    if (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      EXTERNAL_LZ4_LIBRARY (${H5LZ4_ALLOW_EXTERNAL_SUPPORT} ${LIB_TYPE})
      set (H5LZ4_HAVE_LZ4_H 1)
      set (H5LZ4_HAVE_LZ4 1)
      message (STATUS "Filter LZ4 is built")
    else (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      message (FATAL_ERROR " LZ4 is Required for ${H5LZ4_PACKAGE_NAME} library")
    endif (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  endif (LZ4_FOUND)
  message (STATUS "lz4 found: INC=${LZ4_INCLUDE_DIR} TOOLS=${LZ4_LIBRARIES}")
  set (LINK_LIBS ${LINK_LIBS} ${LZ4_LIBRARIES})
  INCLUDE_DIRECTORIES (${H5LZ4_INCLUDE_DIRS})

  if (LZ4_FOUND)
    PACKAGE_LZ4_LIBRARY (${H5LZ4_ALLOW_EXTERNAL_SUPPORT})
  endif (LZ4_FOUND)
endif (NOT BUILD_LZ4_LIBRARY_SOURCE)

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
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5lz4 Unit Testing" OFF)
if (BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE INTEGER
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing ()
  include (CTest)
endif (BUILD_TESTING)

add_subdirectory (${H5LZ4_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
if (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  if (LZ4_FOUND AND LZ4_USE_EXTERNAL)
    ADD_DEPENDENCIES (${H5LZ4_LIB_TARGET} LZ4)
  endif (LZ4_FOUND AND LZ4_USE_EXTERNAL)
  if (HDF5_FOUND AND HDF5_USE_EXTERNAL)
    ADD_DEPENDENCIES (${H5LZ4_LIB_TARGET} HDF5)
  endif (HDF5_FOUND AND HDF5_USE_EXTERNAL)
endif (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")

option (BUILD_EXAMPLES "Build h5lz4 Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (${H5LZ4_SOURCE_DIR}/example ${PROJECT_BINARY_DIR}/example)
endif (BUILD_EXAMPLES)

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------

include (CMakePackageConfigHelpers)

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (NOT H5LZ4_EXTERNALLY_CONFIGURED)
  install (
      EXPORT ${H5LZ4_EXPORTED_TARGETS}
      DESTINATION ${H5LZ4_INSTALL_CMAKE_DIR}
      FILE ${H5LZ4_PACKAGE}${H5LZ4_PACKAGE_EXT}-targets.cmake
      COMPONENT configinstall
  )
endif (NOT H5LZ4_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Export all exported targets to the build tree for use by parent project
#-----------------------------------------------------------------------------
if (NOT H5LZ4_EXTERNALLY_CONFIGURED)
  export (
      TARGETS ${H5LZ4_LIBRARIES_TO_EXPORT} ${H5LZ4_LIB_DEPENDENCIES}
      FILE ${H5LZ4_PACKAGE}${H5LZ4_PACKAGE_EXT}-targets.cmake
  )
endif (NOT H5LZ4_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Set includes needed for build
#-----------------------------------------------------------------------------
set (H5LZ4_INCLUDES_BUILD_TIME
    ${H5LZ4_SRC_DIR} ${H5LZ4_BINARY_DIR}
)
#-----------------------------------------------------------------------------
# Set variables needed for installation
#-----------------------------------------------------------------------------
set (H5LZ4_VERSION_STRING ${H5LZ4_PACKAGE_VERSION})
set (H5LZ4_VERSION_MAJOR  ${H5LZ4_PACKAGE_VERSION_MAJOR})
set (H5LZ4_VERSION_MINOR  ${H5LZ4_PACKAGE_VERSION_MINOR})

#-----------------------------------------------------------------------------
# Configure the h5lz4-config.cmake file for the build directory
#-----------------------------------------------------------------------------
set(INCLUDE_INSTALL_DIR ${H5LZ4_INSTALL_INCLUDE_DIR} )
set(SHARE_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/${H5LZ4_INSTALL_CMAKE_DIR}" )
set(CURRENT_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}" )
configure_package_config_file (
    ${H5LZ4_RESOURCES_DIR}/h5lz4-config.cmake.in
    "${H5LZ4_BINARY_DIR}/${H5LZ4_PACKAGE}${H5LZ4_PACKAGE_EXT}-config.cmake"
    INSTALL_DESTINATION "${H5LZ4_INSTALL_CMAKE_DIR}"
    PATH_VARS INCLUDE_INSTALL_DIR SHARE_INSTALL_DIR CURRENT_BUILD_DIR
    INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
)

#-----------------------------------------------------------------------------
# Configure the h5lz4-config.cmake file for the install directory
#-----------------------------------------------------------------------------
set(INCLUDE_INSTALL_DIR ${H5LZ4_INSTALL_INCLUDE_DIR} )
set(SHARE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${H5LZ4_INSTALL_CMAKE_DIR}" )
set(CURRENT_BUILD_DIR "${CMAKE_INSTALL_PREFIX}" )
configure_package_config_file (
    ${H5LZ4_RESOURCES_DIR}/h5lz4-config.cmake.in
    "${H5LZ4_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${H5LZ4_PACKAGE}${H5LZ4_PACKAGE_EXT}-config.cmake"
    INSTALL_DESTINATION "${H5LZ4_INSTALL_CMAKE_DIR}"
    PATH_VARS INCLUDE_INSTALL_DIR SHARE_INSTALL_DIR CURRENT_BUILD_DIR
)
if (NOT H5LZ4_EXTERNALLY_CONFIGURED)
  install (
      FILES ${H5LZ4_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${H5LZ4_PACKAGE}${H5LZ4_PACKAGE_EXT}-config.cmake
      DESTINATION ${H5LZ4_INSTALL_CMAKE_DIR}
      COMPONENT configinstall
  )
endif (NOT H5LZ4_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Configure the h5lz4-config-version.cmake file for the install directory
#-----------------------------------------------------------------------------
if (NOT H5LZ4_EXTERNALLY_CONFIGURED)
  configure_file (
      ${H5LZ4_RESOURCES_DIR}/h5lz4-config-version.cmake.in
      ${H5LZ4_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${H5LZ4_PACKAGE}${H5LZ4_PACKAGE_EXT}-config-version.cmake @ONLY
  )
  install (
      FILES ${H5LZ4_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${H5LZ4_PACKAGE}${H5LZ4_PACKAGE_EXT}-config-version.cmake
      DESTINATION ${H5LZ4_INSTALL_CMAKE_DIR}
      COMPONENT configinstall
  )
endif (NOT H5LZ4_EXTERNALLY_CONFIGURED)

#-----------------------------------------------------------------------------
# Configure the README.txt file for the binary package
#-----------------------------------------------------------------------------
H5LZ4_README_PROPERTIES("")

#-----------------------------------------------------------------------------
# Check for Installation Utilities
#-----------------------------------------------------------------------------
if (WIN32)
  set (PF_ENV_EXT "(x86)")
  find_program (NSIS_EXECUTABLE NSIS.exe PATHS "$ENV{ProgramFiles}\\NSIS" "$ENV{ProgramFiles${PF_ENV_EXT}}\\NSIS")
  if(NOT CPACK_WIX_ROOT)
    file(TO_CMAKE_PATH "$ENV{WIX}" CPACK_WIX_ROOT)
  endif()
  find_program (WIX_EXECUTABLE candle  PATHS "${CPACK_WIX_ROOT}/bin")
endif (WIN32)

#-----------------------------------------------------------------------------
# Set the cpack variables
#-----------------------------------------------------------------------------
if (NOT H5LZ4_EXTERNALLY_CONFIGURED)
  set (CPACK_PACKAGE_VENDOR "HDF_Group")
  set (CPACK_PACKAGE_NAME "${H5LZ4_PACKAGE_NAME}")
  if (CDASH_LOCAL)
    set (CPACK_PACKAGE_VERSION "${H5LZ4_PACKAGE_VERSION}")
  else (CDASH_LOCAL)
    set (CPACK_PACKAGE_VERSION "${H5LZ4_PACKAGE_VERSION_STRING}")
  endif (CDASH_LOCAL)
  set (CPACK_PACKAGE_VERSION_MAJOR "${H5LZ4_PACKAGE_VERSION_MAJOR}")
  set (CPACK_PACKAGE_VERSION_MINOR "${H5LZ4_PACKAGE_VERSION_MINOR}")
  set (CPACK_PACKAGE_VERSION_PATCH "")
  #set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
  set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
  set (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
  set (CPACK_PACKAGE_RELOCATABLE TRUE)
  set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "h5lz4 Installation")
  set (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}/${CPACK_PACKAGE_NAME}/${CPACK_PACKAGE_VERSION}")

  set (CPACK_GENERATOR "TGZ") 
  if (WIN32)
    set (CPACK_GENERATOR "ZIP") 

    if (NSIS_EXECUTABLE)    
      list (APPEND CPACK_GENERATOR "NSIS") 
    endif (NSIS_EXECUTABLE)    
    # Installers for 32- vs. 64-bit CMake:
    #  - Root install directory (displayed to end user at installer-run time)
    #  - "NSIS package/display name" (text used in the installer GUI)
    #  - Registry key used to store info about the installation
    set (CPACK_NSIS_PACKAGE_NAME "${H5LZ4_PACKAGE_STRING}")
    if (CMAKE_CL_64)
      set (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
      set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION} (Win64)")
    else (CMAKE_CL_64)
      set (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
      set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
    endif (CMAKE_CL_64)
    # set the install/unistall icon used for the installer itself
    # There is a bug in NSI that does not handle full unix paths properly.
    #set (CPACK_NSIS_MUI_ICON "${H5LZ4_RESOURCES_DIR}\\\\H5LZ4.ico")
    #set (CPACK_NSIS_MUI_UNIICON "${H5LZ4_RESOURCES_DIR}\\\\H5LZ4.ico")
    # set the package header icon for MUI
    #set (CPACK_PACKAGE_ICON "${H5LZ4_RESOURCES_DIR}\\\\H5LZ4.bmp")
    set (CPACK_NSIS_DISPLAY_NAME "@CPACK_NSIS_PACKAGE_NAME@, is a dynamic filter for HDF5")
    #set (CPACK_PACKAGE_INSTALL_DIRECTORY "plugins")
    set (CPACK_NSIS_CONTACT "${H5LZ4_PACKAGE_BUGREPORT}")
    set (CPACK_NSIS_MODIFY_PATH ON)

    if (WIX_EXECUTABLE)    
      list (APPEND CPACK_GENERATOR "WIX") 
    endif (WIX_EXECUTABLE)    
#WiX variables
    set (CPACK_WIX_UNINSTALL "1")
# .. variable:: CPACK_WIX_LICENSE_RTF
#  RTF License File
#
#  If CPACK_RESOURCE_FILE_LICENSE has an .rtf extension it is used as-is.
#
#  If CPACK_RESOURCE_FILE_LICENSE has an .txt extension it is implicitly
#  converted to RTF by the WiX Generator.
#  The expected encoding of the .txt file is UTF-8.
#
#  With CPACK_WIX_LICENSE_RTF you can override the license file used by the
#  WiX Generator in case CPACK_RESOURCE_FILE_LICENSE is in an unsupported
#  format or the .txt -> .rtf conversion does not work as expected.
    set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
# .. variable:: CPACK_WIX_PRODUCT_ICON
#  The Icon shown next to the program name in Add/Remove programs.
#    set(CPACK_WIX_PRODUCT_ICON "${H5LZ4_RESOURCES_EXT_DIR}\\\\H5LZ4.ico")
#
# .. variable:: CPACK_WIX_UI_BANNER
#
#  The bitmap will appear at the top of all installer pages other than the
#  welcome and completion dialogs.
#
#  If set, this image will replace the default banner image.
#
#  This image must be 493 by 58 pixels.
#
# .. variable:: CPACK_WIX_UI_DIALOG
#
#  Background bitmap used on the welcome and completion dialogs.
#
#  If this variable is set, the installer will replace the default dialog
#  image.
#
#  This image must be 493 by 312 pixels.
#
  elseif (APPLE)
    list (APPEND CPACK_GENERATOR "DragNDrop") 
    set (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
    set (CPACK_PACKAGING_INSTALL_PREFIX "/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
#    set (CPACK_PACKAGE_ICON "${H5LZ4_RESOURCES_DIR}/h5lz4.gif")

    option (H5LZ4_PACK_MACOSX_BUNDLE  "Package the H5LZ4 Library in a Bundle" OFF)
    if (H5LZ4_PACK_MACOSX_BUNDLE)
      list (APPEND CPACK_GENERATOR "Bundle")
      set (CPACK_BUNDLE_NAME "${H5LZ4_PACKAGE_STRING}")
      set (CPACK_BUNDLE_LOCATION "/")    # make sure CMAKE_INSTALL_PREFIX ends in /
      set (CMAKE_INSTALL_PREFIX "/${CPACK_BUNDLE_NAME}.framework/Versions/${CPACK_PACKAGE_VERSION}/${CPACK_PACKAGE_NAME}/")
#      set (CPACK_BUNDLE_ICON "${H5LZ4_RESOURCES_DIR}/H5LZ4.icns")
#      set (CPACK_BUNDLE_PLIST "${H5LZ4_BINARY_DIR}/CMakeFiles/Info.plist")
      set (CPACK_APPLE_GUI_INFO_STRING "H5LZ4 Plugin Library")
      set (CPACK_APPLE_GUI_COPYRIGHT "Copyright © ???. All rights reserved.")
      set (CPACK_SHORT_VERSION_STRING "${CPACK_PACKAGE_VERSION}")
      set (CPACK_APPLE_GUI_BUNDLE_NAME "${H5LZ4_PACKAGE_STRING}")
      set (CPACK_APPLE_GUI_VERSION_STRING "${CPACK_PACKAGE_VERSION_STRING}")
      set (CPACK_APPLE_GUI_SHORT_VERSION_STRING "${CPACK_PACKAGE_VERSION}")
      #-----------------------------------------------------------------------------
      # Configure the Info.plist file for the install bundle
      #-----------------------------------------------------------------------------
#      configure_file (
#          ${H5LZ4_RESOURCES_DIR}/CPack.Info.plist.in
#          ${H5LZ4_BINARY_DIR}/CMakeFiles/Info.plist @ONLY
#      )
#      configure_file (
#          ${H5LZ4_RESOURCES_DIR}/PkgInfo.in
#          ${H5LZ4_BINARY_DIR}/CMakeFiles/PkgInfo @ONLY
#      )
#      configure_file (
#          ${H5LZ4_RESOURCES_DIR}/version.plist.in
#          ${H5LZ4_BINARY_DIR}/CMakeFiles/version.plist @ONLY
#      )
#      install (
#          FILES ${H5LZ4_BINARY_DIR}/CMakeFiles/PkgInfo
#                ${H5LZ4_BINARY_DIR}/CMakeFiles/version.plist
#          DESTINATION ..
#      )
    endif (H5LZ4_PACK_MACOSX_BUNDLE)
  else (WIN32)
    list (APPEND CPACK_GENERATOR "STGZ") 
    set (CPACK_PACKAGING_INSTALL_PREFIX "/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
    set (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)

    set (CPACK_DEBIAN_PACKAGE_SECTION "Libraries")
    set (CPACK_DEBIAN_PACKAGE_MAINTAINER "${H5LZ4_PACKAGE_BUGREPORT}")
    
#      list (APPEND CPACK_GENERATOR "RPM") 
    set (CPACK_RPM_PACKAGE_RELEASE "1")
    set (CPACK_RPM_COMPONENT_INSTALL ON)
    set (CPACK_RPM_PACKAGE_RELOCATABLE ON)
  endif (WIN32)

  set (CPACK_INSTALL_CMAKE_PROJECTS "${H5LZ4_BINARY_DIR};h5lz4;libraries;/")
  
  if (H5LZ4_PACKAGE_EXTLIBS)
    if (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (LZ4_FOUND AND LZ4_USE_EXTERNAL)
        set (CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${LZ4_INCLUDE_DIR_GEN};LZ4;libraries;/")
      endif (LZ4_FOUND AND LZ4_USE_EXTERNAL)
    endif (H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "SVN" OR H5LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  endif (H5LZ4_PACKAGE_EXTLIBS)

  set (CPACK_ALL_INSTALL_TYPES Full User)
  set (CPACK_INSTALL_TYPE_FULL_DISPLAY_NAME "Everything")

  set(CPACK_COMPONENTS_ALL libraries)
  
  include (CPack)

  cpack_add_component_group(Runtime)

  cpack_add_component (libraries 
      DISPLAY_NAME "h5lz4 Libraries"
      GROUP Runtime
      INSTALL_TYPES Full User
  )
  
endif (NOT H5LZ4_EXTERNALLY_CONFIGURED)
  