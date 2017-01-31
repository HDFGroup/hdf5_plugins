cmake_minimum_required (VERSION 3.2.2)
project (H5MAFISC C)

#-----------------------------------------------------------------------------
# Basic H5MAFISC stuff here
#-----------------------------------------------------------------------------
set (CMAKE_NO_SYSTEM_FROM_IMPORTED 1)

#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
set (H5MAFISC_RESOURCES_DIR             ${H5MAFISC_SOURCE_DIR}/config/cmake)
set (HDF_RESOURCES_DIR                  ${H5MAFISC_SOURCE_DIR}/config/cmake)
set (H5MAFISC_SRC_DIR                   ${H5MAFISC_SOURCE_DIR}/src)
set (H5MAFISC_SRC_LIBLZMA_DIR           ${H5MAFISC_SOURCE_DIR}/src/liblzma)

#-----------------------------------------------------------------------------
# Set the core names of all the libraries
#-----------------------------------------------------------------------------
set (H5MAFISC_LIB_CORENAME              "h5mafisc")
set (LZMA_LIB_CORENAME              "lzma")

#-----------------------------------------------------------------------------
# Set the true names of all the libraries if customized by external project
#-----------------------------------------------------------------------------
set (H5MAFISC_LIB_NAME              "${H5MAFISC_EXTERNAL_LIB_PREFIX}${H5MAFISC_LIB_CORENAME}")
set (LZMA_LIB_NAME              "${H5MAFISC_EXTERNAL_LIB_PREFIX}${LZMA_LIB_CORENAME}")

#-----------------------------------------------------------------------------
# Set the target names of all the libraries
#-----------------------------------------------------------------------------
set (H5MAFISC_LIB_TARGET              ${H5MAFISC_LIB_CORENAME})
set (LZMA_LIB_TARGET              ${LZMA_LIB_CORENAME})

if (NOT H5MAFISC_INSTALL_BIN_DIR)
  set (H5MAFISC_INSTALL_BIN_DIR bin)
endif ()
if (NOT H5MAFISC_INSTALL_LIB_DIR)
  set (H5MAFISC_INSTALL_LIB_DIR lib/plugin)
endif ()
if (NOT H5MAFISC_INSTALL_INCLUDE_DIR)
  set (H5MAFISC_INSTALL_INCLUDE_DIR include)
endif ()
if (NOT H5MAFISC_INSTALL_DATA_DIR)
  if (NOT WIN32)
    set (H5MAFISC_INSTALL_DATA_DIR share)
    set (H5MAFISC_INSTALL_CMAKE_DIR share/cmake)
  else ()
    set (H5MAFISC_INSTALL_DATA_DIR ".")
    set (H5MAFISC_INSTALL_CMAKE_DIR cmake)
  endif ()
endif ()

set (H5MAFISC_VERS_MAJOR 1)
set (H5MAFISC_VERS_MINOR 0)
set (H5MAFISC_VERS_RELEASE 0)

set (H5MAFISC_SOVERS_MAJOR 1)
set (H5MAFISC_SOVERS_MINOR 0)
set (H5MAFISC_SOVERS_INTERFACE 1)
set (H5MAFISC_SOVERS_RELEASE 0)
math (EXPR H5MAFISC_SOVERS_MAJOR ${H5MAFISC_SOVERS_INTERFACE}-${H5MAFISC_SOVERS_RELEASE})

#-----------------------------------------------------------------------------
set (H5MAFISC_PACKAGE "h5mafisc")
set (H5MAFISC_PACKAGE_NAME "h5mafisc")
set (H5MAFISC_PACKAGE_VERSION "${H5MAFISC_VERS_MAJOR}.${H5MAFISC_VERS_MINOR}")
set (H5MAFISC_PACKAGE_VERSION_STRING "${H5MAFISC_PACKAGE_VERSION}")
set (H5MAFISC_PACKAGE_VERSION_MAJOR "${H5MAFISC_VERS_MAJOR}")
set (H5MAFISC_PACKAGE_VERSION_MINOR "${H5MAFISC_VERS_MINOR}")
set (H5MAFISC_PACKAGE_STRING "${H5MAFISC_PACKAGE_NAME} ${H5MAFISC_PACKAGE_VERSION}")
set (H5MAFISC_PACKAGE_TARNAME "${H5MAFISC_PACKAGE_NAME}{HDF_PACKAGE_EXT}")
set (H5MAFISC_PACKAGE_URL "http://www.hdfgroup.org")
set (H5MAFISC_PACKAGE_BUGREPORT "help@hdfgroup.org")
set (H5MAFISC_PACKAGE_SOVERSION "${H5MAFISC_SOVERS_MAJOR}.${H5MAFISC_SOVERS_RELEASE}.${H5MAFISC_SOVERS_MINOR}")

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${H5MAFISC_RESOURCES_DIR}/H5MAFISCMacros.cmake)

#-----------------------------------------------------------------------------
# Setup output Directories
#-----------------------------------------------------------------------------
if (NOT H5MAFISC_EXTERNALLY_CONFIGURED)
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
  )
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries"
  )
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries."
  )
else ()
  # if we are externally configured, but the project uses old cmake scripts
  # this may not be set
  if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
  endif ()
endif ()

#-----------------------------------------------------------------------------
# Targets built within this project are exported at Install time for use
# by other projects using FindH5MAFISC.
#-----------------------------------------------------------------------------
if (NOT H5MAFISC_EXPORTED_TARGETS)
  set (H5MAFISC_EXPORTED_TARGETS "H5mafisc-targets")
endif ()

#-----------------------------------------------------------------------------
# To include a library in the list exported by the project AT BUILD TIME,
# add it to this variable. This is NOT used by Make Install, but for projects
# which include h5mafisc as a sub-project within their build tree
#-----------------------------------------------------------------------------
set_global_variable (H5MAFISC_LIBRARIES_TO_EXPORT "")

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
include (${H5MAFISC_RESOURCES_DIR}/ConfigureChecks.cmake)

#-----------------------------------------------------------------------------
# Option to Build Shared/Static libs, only shared is allowed
#-----------------------------------------------------------------------------
option (BUILD_SHARED_LIBS "Build Shared Libraries" ON)
set (LIB_TYPE SHARED)
if (BUILD_SHARED_LIBS)
  set (LIB_TYPE SHARED)
  add_definitions (-DH5MAFISC_BUILT_AS_DYNAMIC_LIB)
else ()
  message (FATAL_ERROR " ${H5MAFISC_PACKAGE_NAME} library must be built as a shared library")
endif ()

#-----------------------------------------------------------------------------
# HDF5 support
#-----------------------------------------------------------------------------
set (CMAKE_MODULE_PATH ${HDF_RESOURCES_DIR} ${CMAKE_MODULE_PATH})
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

#find_package(LibLZMA REQUIRED)

if (NOT HDF5_HDF5_HEADER)
  if (USE_SHARED_LIBS)
    set (FIND_HDF_COMPONENTS C shared)
  else ()
    set (FIND_HDF_COMPONENTS C static)
  endif ()
  message (STATUS "HDF5 find comps: ${FIND_HDF_COMPONENTS}")
  set (SEARCH_PACKAGE_NAME ${HDF5_PACKAGE_NAME})

  find_package (HDF5 NAMES ${SEARCH_PACKAGE_NAME} COMPONENTS ${FIND_HDF_COMPONENTS})
  message (STATUS "HDF5 C libs:${HDF5_FOUND} static:${HDF5_static_C_FOUND} and shared:${HDF5_shared_C_FOUND}")
  if (HDF5_FOUND)
    if (USE_SHARED_LIBS)
      if (NOT TARGET hdf5::h5repack-shared)
        add_executable (hdf5::h5repack-shared IMPORTED)
      endif ()
      if (NOT TARGET hdf5::h5dump-shared)
        add_executable (hdf5::h5dump-shared IMPORTED)
      endif ()
      set (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:hdf5::h5dump-shared>)
      set (HDF5_REPACK_EXECUTABLE $<TARGET_FILE:hdf5::h5repack-shared>)
    else ()
      if (NOT TARGET hdf5::h5repack)
        add_executable (hdf5::h5repack IMPORTED)
      endif()
      if (NOT TARGET hdf5::h5dump)
        add_executable (hdf5::h5dump IMPORTED)
      endif()
      set (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:hdf5::h5dump>)
      set (HDF5_REPACK_EXECUTABLE $<TARGET_FILE:hdf5::h5repack>)
    endif()
    if (NOT HDF5_static_C_FOUND AND NOT HDF5_shared_C_FOUND)
      #find library from non-dual-binary package
      set (FIND_HDF_COMPONENTS C)
      message (STATUS "HDF5 find comps: ${FIND_HDF_COMPONENTS}")

      find_package (HDF5 NAMES ${SEARCH_PACKAGE_NAME} COMPONENTS ${FIND_HDF_COMPONENTS})
      message (STATUS "HDF5 libs:${HDF5_FOUND} C:${HDF5_C_FOUND}")
      set (HDF5_LINK_LIBS ${HDF5_LINK_LIBS} ${HDF5_LIBRARIES})
      #mafisc source needs to be linked with HDF5
      set (LINK_LIBS ${LINK_LIBS} ${HDF5_LIBRARIES})
      if (HDF5_BUILD_SHARED_LIBS)
        add_definitions (-DH5_BUILT_AS_DYNAMIC_LIB)
      else ()
        add_definitions (-DH5_BUILT_AS_STATIC_LIB)
      endif ()
      if (USE_SHARED_LIBS AND WIN32)
        set_property (TARGET hdf5::h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dumpdll")
        set_property (TARGET hdf5::h5repack PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repackdll")
      else ()
        set_property (TARGET hdf5::h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dump")
        set_property (TARGET hdf5::h5repack PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repack")
      endif ()
      set (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:hdf5::h5dump>)
      set (HDF5_REPACK_EXECUTABLE $<TARGET_FILE:hdf5::h5repack>)
    else (NOT HDF5_static_C_FOUND AND NOT HDF5_shared_C_FOUND)
      if (USE_SHARED_LIBS AND HDF5_shared_C_FOUND)
        set (HDF5_LINK_LIBS ${HDF5_LINK_LIBS} ${HDF5_C_SHARED_LIBRARY})
        set (HDF5_LIBRARY_PATH ${PACKAGE_PREFIX_DIR}/lib)
        #mafisc source needs to be linked with HDF5
        set (LINK_LIBS ${LINK_LIBS} ${HDF5_C_SHARED_LIBRARY})
      else (HDF5_static_C_FOUND)
        set (HDF5_LINK_LIBS ${HDF5_LINK_LIBS} ${HDF5_C_STATIC_LIBRARY})
        #mafisc source needs to be linked with HDF5
        set (LINK_LIBS ${LINK_LIBS} ${HDF5_C_STATIC_LIBRARY})
      endif (USE_SHARED_LIBS AND HDF5_shared_C_FOUND)
      set_property (TARGET hdf5::h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dump")
      set_property (TARGET hdf5::h5repack PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repack")
      set (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:hdf5::h5dump>)
      set (HDF5_REPACK_EXECUTABLE $<TARGET_FILE:hdf5::h5repack>)
    endif (NOT HDF5_static_C_FOUND AND NOT HDF5_shared_C_FOUND)
  else ()
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
    #mafisc source needs to be linked with HDF5
    set (LINK_LIBS ${LINK_LIBS} ${HDF5_LIBRARIES})

    add_executable (hdf5::h5repack IMPORTED)
    add_executable (hdf5::h5dump IMPORTED)
    set_property (TARGET hdf5::h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repack")
    set_property (TARGET hdf5::h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dump")
    set (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:hdf5::h5repack>)
    set (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:hdf5::h5dump>)
  endif ()
  set (HDF5_PACKAGE_NAME ${SEARCH_PACKAGE_NAME})

  if (HDF5_FOUND)
    set (HDF5_HAVE_H5PUBCONF_H 1)
    set (HDF5_HAVE_HDF5 1)
    set (HDF5_HDF5_HEADER "h5pubconf.h")
    set (HDF5_INCLUDE_DIR_GEN ${HDF5_INCLUDE_DIR})
    message (STATUS "HDF5 found: INC=${HDF5_INCLUDE_DIR} TOOLS=${HDF5_TOOLS_DIR}")
  else ()
    message (FATAL_ERROR " HDF5 is Required for ${H5MAFISC_PACKAGE_NAME} library")
  endif ()
else ()
  # This project is being called from within another and HDF5 is already configured
  set (HDF5_HAVE_H5PUBCONF_H 1)
  set (HDF5_HAVE_HDF5 1)
endif ()
message (STATUS "HDF5 link libs: ${HDF5_LINK_LIBS}")

if (USE_SHARED_LIBS)
  set (H5_LIB_TYPE SHARED)
else ()
  set (H5_LIB_TYPE STATIC)
endif ()

INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Generate the config.h file containing user settings needed by compilation
#-----------------------------------------------------------------------------
configure_file (${H5MAFISC_RESOURCES_DIR}/config.h.in ${H5MAFISC_BINARY_DIR}/config.h @ONLY)

option (BUILD_MAFISC_LIBRARY_SOURCE "build the mafisc library within the plugin" OFF)
if (NOT BUILD_MAFISC_LIBRARY_SOURCE)
  include (ExternalProject)
  set (H5MAFISC_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
  set_property (CACHE H5MAFISC_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)
  if (H5MAFISC_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5MAFISC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    option (MAFISC_USE_EXTERNAL "Use External Library Building for MAFISC" 1)
    if (H5MAFISC_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
      set (MAFISC_URL ${MAFISC_GIT_URL})
    elseif (H5MAFISC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (NOT TGZPATH)
        set (TGZPATH ${H5MAFISC_SOURCE_DIR})
      endif ()
      set (MAFISC_URL ${TGZPATH}/${MAFISC_TGZ_NAME})
    else ()
      set (MAFISC_USE_EXTERNAL 0)
    endif ()
  endif ()
  #-----------------------------------------------------------------------------
  # mafisc Library Settings
  #-----------------------------------------------------------------------------
  if (NOT MAFISC_USE_EXTERNAL)
    find_package (MAFISC NAMES ${MAFISC_PACKAGE_NAME})
    if (NOT MAFISC_FOUND)
      find_package (MAFISC) # Legacy find
      if (NOT MAFISC_FOUND)
        message (FATAL_ERROR " mafisc is Required for ${H5MAFISC_PACKAGE_NAME} library")
      endif ()
    endif ()
  endif ()
  if (MAFISC_FOUND)
    set (H5MAFISC_HAVE_MAFISC_H 1)
    set (H5MAFISC_HAVE_MAFISC 1)
    set (H5MAFISC_MAFISC_HEADER "mafisc.h")
    set (H5MAFISC_INCLUDE_DIR_GEN ${MAFISC_INCLUDE_DIR})
    set (H5MAFISC_INCLUDE_DIRS ${MAFISC_INCLUDE_DIR})
  else ()
    if (H5MAFISC_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5MAFISC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      EXTERNAL_MAFISC_LIBRARY (${H5MAFISC_ALLOW_EXTERNAL_SUPPORT} STATIC)
      set (H5MAFISC_HAVE_MAFISC_H 1)
      set (H5MAFISC_HAVE_MAFISC 1)
      message (STATUS "Filter MAFISC is built")
    else ()
      message (FATAL_ERROR " MAFISC is Required for ${H5MAFISC_PACKAGE_NAME} library")
    endif ()
  endif ()
  message (STATUS "mafisc found: INC=${MAFISC_INCLUDE_DIR} TOOLS=${MAFISC_LIBRARIES}")
  set (LINK_LIBS ${LINK_LIBS} ${MAFISC_LIBRARIES})
  INCLUDE_DIRECTORIES (${H5MAFISC_INCLUDE_DIRS})

  if (MAFISC_FOUND)
    PACKAGE_MAFISC_LIBRARY (${H5MAFISC_ALLOW_EXTERNAL_SUPPORT})
  endif ()
endif ()

if (MSVC)
  set (CMAKE_MFC_FLAG 0)
endif ()

set (MAKE_SYSTEM)
if (CMAKE_BUILD_TOOL MATCHES "make")
  set (MAKE_SYSTEM 1)
endif ()

set (CFG_INIT "/${CMAKE_CFG_INTDIR}")
if (MAKE_SYSTEM)
  set (CFG_INIT "")
endif ()

#-----------------------------------------------------------------------------
# Compiler specific flags : Shouldn't there be compiler tests for these
#-----------------------------------------------------------------------------
if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_C_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_C_FLAGS} -std=c99 -fomit-frame-pointer -finline-functions -fno-common")
endif ()
if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_ANSI_CFLAGS} ${CMAKE_CXX_FLAGS} -fomit-frame-pointer -finline-functions -fno-common")
endif ()

#-----------------------------------------------------------------------------
# This is in here to help some of the GCC based IDES like Eclipse
# and code blocks parse the compiler errors and warnings better.
#-----------------------------------------------------------------------------
if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
endif ()
if (CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
endif ()

#-----------------------------------------------------------------------------
# Dashboard and Testing Settings
#-----------------------------------------------------------------------------
option (BUILD_TESTING "Build h5mafisc Unit Testing" OFF)
if (BUILD_TESTING)
  set (DART_TESTING_TIMEOUT 1200 CACHE INTEGER
       "Timeout in seconds for each test (default 1200=20minutes)")
  enable_testing ()
  include (CTest)
  include (${PROJECT_SOURCE_DIR}/CTestConfig.cmake)
  configure_file (${HDF_RESOURCES_DIR}/CTestCustom.cmake ${PROJECT_BINARY_DIR}/CTestCustom.ctest @ONLY)
endif ()

add_subdirectory (${H5MAFISC_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)

if (H5MAFISC_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5MAFISC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  if (MAFISC_FOUND AND MAFISC_USE_EXTERNAL)
    add_dependencies (${H5MAFISC_LIB_TARGET} MAFISC)
  endif ()
endif ()

option (BUILD_EXAMPLES "Build h5mafisc Examples" OFF)
if (BUILD_EXAMPLES)
  add_subdirectory (${H5MAFISC_SOURCE_DIR}/example ${PROJECT_BINARY_DIR}/example)
endif ()

#-----------------------------------------------------------------------------
# Add file(s) to CMake Install
#-----------------------------------------------------------------------------

include (CMakePackageConfigHelpers)

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (NOT H5MAFISC_EXTERNALLY_CONFIGURED)
  install (
      EXPORT ${H5MAFISC_EXPORTED_TARGETS}
      DESTINATION ${H5MAFISC_INSTALL_CMAKE_DIR}
      FILE ${H5MAFISC_PACKAGE}${H5MAFISC_PACKAGE_EXT}-targets.cmake
      COMPONENT configinstall
  )
endif ()

#-----------------------------------------------------------------------------
# Export all exported targets to the build tree for use by parent project
#-----------------------------------------------------------------------------
if (NOT H5MAFISC_EXTERNALLY_CONFIGURED)
  export (
      TARGETS ${H5MAFISC_LIBRARIES_TO_EXPORT} ${H5MAFISC_LIB_DEPENDENCIES}
      FILE ${H5MAFISC_PACKAGE}${H5MAFISC_PACKAGE_EXT}-targets.cmake
  )
endif ()

#-----------------------------------------------------------------------------
# Set includes needed for build
#-----------------------------------------------------------------------------
set (H5MAFISC_INCLUDES_BUILD_TIME
    ${H5MAFISC_SRC_DIR} ${H5MAFISC_BINARY_DIR}
)
#-----------------------------------------------------------------------------
# Set variables needed for installation
#-----------------------------------------------------------------------------
set (H5MAFISC_VERSION_STRING ${H5MAFISC_PACKAGE_VERSION})
set (H5MAFISC_VERSION_MAJOR  ${H5MAFISC_PACKAGE_VERSION_MAJOR})
set (H5MAFISC_VERSION_MINOR  ${H5MAFISC_PACKAGE_VERSION_MINOR})

#-----------------------------------------------------------------------------
# Configure the h5mafisc-config.cmake file for the build directory
#-----------------------------------------------------------------------------
set(INCLUDE_INSTALL_DIR ${H5MAFISC_INSTALL_INCLUDE_DIR} )
set(SHARE_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/${H5MAFISC_INSTALL_CMAKE_DIR}" )
set(CURRENT_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}" )
configure_package_config_file (
    ${H5MAFISC_RESOURCES_DIR}/h5mafisc-config.cmake.in
    "${H5MAFISC_BINARY_DIR}/${H5MAFISC_PACKAGE}${H5MAFISC_PACKAGE_EXT}-config.cmake"
    INSTALL_DESTINATION "${H5MAFISC_INSTALL_CMAKE_DIR}"
    PATH_VARS INCLUDE_INSTALL_DIR SHARE_INSTALL_DIR CURRENT_BUILD_DIR
    INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
)

#-----------------------------------------------------------------------------
# Configure the h5mafisc-config.cmake file for the install directory
#-----------------------------------------------------------------------------
set(INCLUDE_INSTALL_DIR ${H5MAFISC_INSTALL_INCLUDE_DIR} )
set(SHARE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${H5MAFISC_INSTALL_CMAKE_DIR}" )
set(CURRENT_BUILD_DIR "${CMAKE_INSTALL_PREFIX}" )
configure_package_config_file (
    ${H5MAFISC_RESOURCES_DIR}/h5mafisc-config.cmake.in
    "${H5MAFISC_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${H5MAFISC_PACKAGE}${H5MAFISC_PACKAGE_EXT}-config.cmake"
    INSTALL_DESTINATION "${H5MAFISC_INSTALL_CMAKE_DIR}"
    PATH_VARS INCLUDE_INSTALL_DIR SHARE_INSTALL_DIR CURRENT_BUILD_DIR
)
if (NOT H5MAFISC_EXTERNALLY_CONFIGURED)
  install (
      FILES ${H5MAFISC_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${H5MAFISC_PACKAGE}${H5MAFISC_PACKAGE_EXT}-config.cmake
      DESTINATION ${H5MAFISC_INSTALL_CMAKE_DIR}
      COMPONENT configinstall
  )
endif ()

#-----------------------------------------------------------------------------
# Configure the h5mafisc-config-version.cmake file for the install directory
#-----------------------------------------------------------------------------
if (NOT H5MAFISC_EXTERNALLY_CONFIGURED)
  configure_file (
      ${H5MAFISC_RESOURCES_DIR}/h5mafisc-config-version.cmake.in
      ${H5MAFISC_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${H5MAFISC_PACKAGE}${H5MAFISC_PACKAGE_EXT}-config-version.cmake @ONLY
  )
  install (
      FILES ${H5MAFISC_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${H5MAFISC_PACKAGE}${H5MAFISC_PACKAGE_EXT}-config-version.cmake
      DESTINATION ${H5MAFISC_INSTALL_CMAKE_DIR}
      COMPONENT configinstall
  )
endif ()

#-----------------------------------------------------------------------------
# Configure the HDF5PL_Examples.cmake file and the examples
#-----------------------------------------------------------------------------
configure_file (
    ${HDF_RESOURCES_DIR}/HDF5PL_Examples.cmake.in
    ${H5MAFISC_BINARY_DIR}/HDF5PL_Examples.cmake @ONLY
)
install (
    FILES ${H5MAFISC_BINARY_DIR}/HDF5PL_Examples.cmake
    DESTINATION ${H5MAFISC_INSTALL_DATA_DIR}
    COMPONENT hdfdocuments
)
execute_process(
     COMMAND ${CMAKE_COMMAND} -E copy_directory ${HDF_RESOURCES_DIR}/binex ${H5MAFISC_BINARY_DIR}/HDFPLExamples
)
install (
  DIRECTORY ${H5MAFISC_BINARY_DIR}/HDFPLExamples
  DESTINATION ${H5MAFISC_INSTALL_DATA_DIR}
  USE_SOURCE_PERMISSIONS
  COMPONENT hdfdocuments
)

#-----------------------------------------------------------------------------
# Configure the README.txt file for the binary package
#-----------------------------------------------------------------------------
H5MAFISC_README_PROPERTIES("")

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
endif ()

#-----------------------------------------------------------------------------
# Set the cpack variables
#-----------------------------------------------------------------------------
if (NOT H5MAFISC_EXTERNALLY_CONFIGURED)
  set (CPACK_PACKAGE_VENDOR "HDF_Group")
  set (CPACK_PACKAGE_NAME "${H5MAFISC_PACKAGE_NAME}")
  if (CDASH_LOCAL)
    set (CPACK_PACKAGE_VERSION "${H5MAFISC_PACKAGE_VERSION}")
  else ()
    set (CPACK_PACKAGE_VERSION "${H5MAFISC_PACKAGE_VERSION_STRING}")
  endif ()
  set (CPACK_PACKAGE_VERSION_MAJOR "${H5MAFISC_PACKAGE_VERSION_MAJOR}")
  set (CPACK_PACKAGE_VERSION_MINOR "${H5MAFISC_PACKAGE_VERSION_MINOR}")
  set (CPACK_PACKAGE_VERSION_PATCH "")
  #set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
  set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
  set (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
  set (CPACK_PACKAGE_RELOCATABLE TRUE)
  set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "h5mafisc Installation")
  set (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}/${CPACK_PACKAGE_NAME}/${CPACK_PACKAGE_VERSION}")

  set (CPACK_GENERATOR "TGZ")
  if (WIN32)
    set (CPACK_GENERATOR "ZIP")

    if (NSIS_EXECUTABLE)
      list (APPEND CPACK_GENERATOR "NSIS")
    endif ()
    # Installers for 32- vs. 64-bit CMake:
    #  - Root install directory (displayed to end user at installer-run time)
    #  - "NSIS package/display name" (text used in the installer GUI)
    #  - Registry key used to store info about the installation
    set (CPACK_NSIS_PACKAGE_NAME "${H5MAFISC_PACKAGE_STRING}")
    if (CMAKE_CL_64)
      set (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
      set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION} (Win64)")
    else ()
      set (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
      set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
    endif ()
    # set the install/unistall icon used for the installer itself
    # There is a bug in NSI that does not handle full unix paths properly.
    #set (CPACK_NSIS_MUI_ICON "${H5MAFISC_RESOURCES_DIR}\\\\H5MAFISC.ico")
    #set (CPACK_NSIS_MUI_UNIICON "${H5MAFISC_RESOURCES_DIR}\\\\H5MAFISC.ico")
    # set the package header icon for MUI
    #set (CPACK_PACKAGE_ICON "${H5MAFISC_RESOURCES_DIR}\\\\H5MAFISC.bmp")
    set (CPACK_NSIS_DISPLAY_NAME "@CPACK_NSIS_PACKAGE_NAME@, is a dynamic filter for HDF5")
    #set (CPACK_PACKAGE_INSTALL_DIRECTORY "plugins")
    set (CPACK_NSIS_CONTACT "${H5MAFISC_PACKAGE_BUGREPORT}")
    set (CPACK_NSIS_MODIFY_PATH ON)

    if (WIX_EXECUTABLE)
      list (APPEND CPACK_GENERATOR "WIX")
    endif ()
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
#    set(CPACK_WIX_PRODUCT_ICON "${H5MAFISC_RESOURCES_EXT_DIR}\\\\H5MAFISC.ico")
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
    list (APPEND CPACK_GENERATOR "STGZ")
    list (APPEND CPACK_GENERATOR "DragNDrop")
    set (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
    set (CPACK_PACKAGING_INSTALL_PREFIX "/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
#    set (CPACK_PACKAGE_ICON "${H5MAFISC_RESOURCES_DIR}/h5mafisc.gif")

    option (H5MAFISC_PACK_MACOSX_BUNDLE  "Package the H5MAFISC Library in a Bundle" OFF)
    if (H5MAFISC_PACK_MACOSX_BUNDLE)
      list (APPEND CPACK_GENERATOR "Bundle")
      set (CPACK_BUNDLE_NAME "${H5MAFISC_PACKAGE_STRING}")
      set (CPACK_BUNDLE_LOCATION "/")    # make sure CMAKE_INSTALL_PREFIX ends in /
      set (CMAKE_INSTALL_PREFIX "/${CPACK_BUNDLE_NAME}.framework/Versions/${CPACK_PACKAGE_VERSION}/${CPACK_PACKAGE_NAME}/")
#      set (CPACK_BUNDLE_ICON "${H5MAFISC_RESOURCES_DIR}/H5MAFISC.icns")
#      set (CPACK_BUNDLE_PLIST "${H5MAFISC_BINARY_DIR}/CMakeFiles/Info.plist")
      set (CPACK_APPLE_GUI_INFO_STRING "H5MAFISC Plugin Library")
      set (CPACK_APPLE_GUI_COPYRIGHT "Copyright © ???. All rights reserved.")
      set (CPACK_SHORT_VERSION_STRING "${CPACK_PACKAGE_VERSION}")
      set (CPACK_APPLE_GUI_BUNDLE_NAME "${H5MAFISC_PACKAGE_STRING}")
      set (CPACK_APPLE_GUI_VERSION_STRING "${CPACK_PACKAGE_VERSION_STRING}")
      set (CPACK_APPLE_GUI_SHORT_VERSION_STRING "${CPACK_PACKAGE_VERSION}")
      #-----------------------------------------------------------------------------
      # Configure the Info.plist file for the install bundle
      #-----------------------------------------------------------------------------
#      configure_file (
#          ${H5MAFISC_RESOURCES_DIR}/CPack.Info.plist.in
#          ${H5MAFISC_BINARY_DIR}/CMakeFiles/Info.plist @ONLY
#      )
#      configure_file (
#          ${H5MAFISC_RESOURCES_DIR}/PkgInfo.in
#          ${H5MAFISC_BINARY_DIR}/CMakeFiles/PkgInfo @ONLY
#      )
#      configure_file (
#          ${H5MAFISC_RESOURCES_DIR}/version.plist.in
#          ${H5MAFISC_BINARY_DIR}/CMakeFiles/version.plist @ONLY
#      )
#      install (
#          FILES ${H5MAFISC_BINARY_DIR}/CMakeFiles/PkgInfo
#                ${H5MAFISC_BINARY_DIR}/CMakeFiles/version.plist
#          DESTINATION ..
#      )
    endif (H5MAFISC_PACK_MACOSX_BUNDLE)
  else (WIN32)
    list (APPEND CPACK_GENERATOR "STGZ")
    set (CPACK_PACKAGING_INSTALL_PREFIX "/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
    set (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)

    set (CPACK_DEBIAN_PACKAGE_SECTION "Libraries")
    set (CPACK_DEBIAN_PACKAGE_MAINTAINER "${H5MAFISC_PACKAGE_BUGREPORT}")

#      list (APPEND CPACK_GENERATOR "RPM")
    set (CPACK_RPM_PACKAGE_RELEASE "1")
    set (CPACK_RPM_COMPONENT_INSTALL ON)
    set (CPACK_RPM_PACKAGE_RELOCATABLE ON)
  endif (WIN32)

  set (CPACK_INSTALL_CMAKE_PROJECTS "${H5MAFISC_BINARY_DIR};h5mafisc;ALL;/")

  if (H5MAFISC_PACKAGE_EXTLIBS)
    if (H5MAFISC_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5MAFISC_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (MAFISC_FOUND AND MAFISC_USE_EXTERNAL)
        set (CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${MAFISC_INCLUDE_DIR_GEN};MAFISC;libraries;/")
      endif ()
    endif ()
  endif ()

  set (CPACK_ALL_INSTALL_TYPES Full User)
  set (CPACK_INSTALL_TYPE_FULL_DISPLAY_NAME "Everything")

  include (CPack)

  cpack_add_component_group(Runtime)

  cpack_add_component (libraries
      DISPLAY_NAME "h5mafisc Libraries"
      GROUP Runtime
      INSTALL_TYPES Full User
  )
  cpack_add_component_group(hdfdocuments
      DISPLAY_NAME "h5mafisc Documents"
      GROUP Documents
      INSTALL_TYPES Full User
  )

endif (NOT H5MAFISC_EXTERNALLY_CONFIGURED)
