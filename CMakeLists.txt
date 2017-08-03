cmake_minimum_required(VERSION 3.2.2)
PROJECT ( HDF5PL C )

#-----------------------------------------------------------------------------
# Instructions for use : Normal Build
#
# For standard build of HDF5PL libraries,tests and tools.
# Run cmake using the HDF5PL source tree to generate a build tree.
# Enable/Disable options according to requirements and
# set CMAKE_INSTALL_PREFIX to the required install path.
# Make install can be used to install all components for system-wide use.
#
#-----------------------------------------------------------------------------
# Instructions for use : Sub-Project Build
#
# To include HDF5PL as a sub-project within another project.
# Set HDF5PL_EXTERNALLY_CONFIGURED to 1 in the parent project and
# supply values for the following variables...
#
# HDF5PL_EXPORTED_TARGETS :
#   Set this to the name of the targets variable which controls exports
#   If unset (because parent project does not support/use the
#   install (EXPORT target...) syntax), then targets are not configured
#   for export during install.
#
# HDF5PL_LIB_DEPENDENCIES :
#   If the build of HDF5PL libs is being customized, then rules for the
#   dependencies of the HDF5PL libs may be 'incomplete', add additional
#   dependencies to this variable so that external projects pick them up
#
# HDF5PL_EXTERNAL_LIB_PREFIX :
#   If the parent project needs to install hdf libraries, but avoid
#   name conflicts with system versions, then a prefix may be added
#   to ensure that the correct versions configured are used.
#
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Define some CMake variables for use later in the project
#-----------------------------------------------------------------------------
set (HDF_RESOURCES_DIR           ${HDF5PL_SOURCE_DIR}/config/cmake)

if (NOT HDF5PL_INSTALL_BIN_DIR)
  set (HDF5PL_INSTALL_BIN_DIR bin)
endif ()
if (NOT HDF5PL_INSTALL_BIN_DIR)
  set (HDF5PL_INSTALL_LIB_DIR lib)
endif ()
if (NOT HDF5PL_INSTALL_INCLUDE_DIR)
  set (HDF5PL_INSTALL_INCLUDE_DIR include)
endif ()
if (NOT HDF5PL_INSTALL_DATA_DIR)
  if (NOT WIN32)
    set (HDF5PL_INSTALL_DATA_DIR share)
    set (HDF5PL_INSTALL_CMAKE_DIR share/cmake)
    set (HDF5PL_INSTALL_EXTRA_DIR share)
  else ()
    set (HDF5PL_INSTALL_DATA_DIR ".")
    set (HDF5PL_INSTALL_CMAKE_DIR cmake)
    set (HDF5PL_INSTALL_EXTRA_DIR ".")
  endif ()
endif ()

#-----------------------------------------------------------------------------
# Basic HDF5PL stuff here
#-----------------------------------------------------------------------------
set (HDF5PL_PACKAGE "hdf5-plugins")
set (HDF5PL_PACKAGE_NAME "HDF5PL")
set (HDF5PL_PACKAGE_VERSION "${HDF5PL_VERS_MAJOR}.${HDF5PL_VERS_MINOR}.${HDF5PL_VERS_RELEASE}")
set (HDF5PL_PACKAGE_VERSION_STRING "${HDF5PL_PACKAGE_VERSION}")
set (HDF5PL_PACKAGE_VERSION_MAJOR "${HDF5PL_VERS_MAJOR}")
set (HDF5PL_PACKAGE_VERSION_MINOR "${HDF5PL_VERS_MINOR}")
set (HDF5PL_PACKAGE_STRING "${HDF5PL_PACKAGE_NAME}")
set (HDF5PL_PACKAGE_TARNAME "${HDF5PL_PACKAGE_NAME}${HDF_PACKAGE_EXT}")
set (HDF5PL_PACKAGE_URL "https://support.hdfgroup.org/hdf5/")
set (HDF5PL_PACKAGE_BUGREPORT "help@hdfgroup.org")

set (HDF5PL_VERS_MAJOR "1")
set (HDF5PL_VERS_MINOR "0")
set (HDF5PL_VERS_RELEASE "0")

#-----------------------------------------------------------------------------
# Include some macros for reusable code
#-----------------------------------------------------------------------------
include (${HDF_RESOURCES_DIR}/HDFMacros.cmake)
include (${HDF_RESOURCES_DIR}/HDFLibMacros.cmake)
include (${HDF_RESOURCES_DIR}/HDF45Macros.cmake)
include (${HDF_RESOURCES_DIR}/HDF5PLMacros.cmake)

#-----------------------------------------------------------------------------
# Setup output Directories
#-----------------------------------------------------------------------------
if (NOT HDF5PL_EXTERNALLY_CONFIGURED)
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
  )
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries"
  )
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries."
  )
  if (WIN32)
    set (CMAKE_TEST_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
  else ()
    set (CMAKE_TEST_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  endif ()
else ()
  # if we are externally configured, but the project uses old cmake scripts
  # this may not be set
  if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
  endif ()
endif ()

#-----------------------------------------------------------------------------
# Targets built within this project are exported at Install time for use
# by other projects using FindHDF5PL.
#-----------------------------------------------------------------------------
if (NOT HDF5PL_EXPORTED_TARGETS)
  set (HDF5PL_EXPORTED_TARGETS "HDF5PL-targets")
endif ()

#-----------------------------------------------------------------------------
# To include a library in the list exported by the project AT BUILD TIME,
# add it to this variable. This is NOT used by Make Install, but for projects
# which include HDF5PL as a sub-project within their build tree
#-----------------------------------------------------------------------------
SET_GLOBAL_VARIABLE (HDF5PL_LIBRARIES_TO_EXPORT "")
SET_GLOBAL_VARIABLE (HDF5PL_SOURCE_PACKAGES "")
SET_GLOBAL_VARIABLE (HDF5PL_OBJECT_JARS "")

#-----------------------------------------------------------------------------
# External hdf5 target for the HDF5PL super project.
#-----------------------------------------------------------------------------
include (ExternalProject)
set (HDF_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE HDF_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)

option (HDF5_USE_EXTERNAL "Use External Library Building for HDF5" 0)
if (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  set (HDF5_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for HDF5" FORCE)
  if (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (HDF5_URL ${HDF5_GIT_URL})
    set (HDF5_BRANCH ${HDF5_GIT_BRANCH})
  elseif (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${HDF5PL_SOURCE_DIR})
    endif ()
    set (HDF5_URL ${TGZPATH}/${HDF5_TGZ_NAME})
  endif ()
endif ()

#-----------------------------------------------------------------------------
# External plugin targets for the HDF5PL super project.
#-----------------------------------------------------------------------------
include (ExternalProject)
set (HDF_ALLOW_EXTERNAL_SUPPORT "NO" CACHE STRING "Allow External Library Building (NO GIT TGZ)")
set_property (CACHE HDF_ALLOW_EXTERNAL_SUPPORT PROPERTY STRINGS NO GIT TGZ)

# BLOSC filter
option (HDF_BLOSC_USE_EXTERNAL "Use External Library Building for blosc plugin" 0)
if (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  set (HDF_BLOSC_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for blosc plugin" FORCE)
  if (HDF_BLOSC_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (HDF_BLOSC_URL ${HDF_BLOSC_GIT_URL})
    set (HDF_BLOSC_BRANCH ${HDF_BLOSC_GIT_BRANCH})
  elseif (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${HDF5PL_SOURCE_DIR})
    endif ()
    set (HDF_BLOSC_URL ${TGZPATH}/${HDF_BLOSC_TGZ_NAME})
  endif ()
endif ()

# BZIP2 filter
option (HDF_BZIP2_USE_EXTERNAL "Use External Library Building for bzip2 plugin" 0)
if (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  set (HDF_BZIP2_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for bzip2 plugin" FORCE)
  if (HDF_BZIP2_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (HDF_BZIP2_URL ${HDF_BZIP2_GIT_URL})
    set (HDF_BZIP2_BRANCH ${HDF_BZIP2_GIT_BRANCH})
  elseif (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${HDF5PL_SOURCE_DIR})
    endif ()
    set (HDF_BZIP2_URL ${TGZPATH}/${HDF_BZIP2_TGZ_NAME})
  endif ()
endif ()

# LZ4 filter
option (HDF_LZ4_USE_EXTERNAL "Use External Library Building for lz4 plugin" 0)
if (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  set (HDF_LZ4_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for lz4 plugin" FORCE)
  if (HDF_LZ4_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (HDF_LZ4_URL ${HDF_LZ4_GIT_URL})
    set (HDF_LZ4_BRANCH ${HDF_LZ4_GIT_BRANCH})
  elseif (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${HDF5PL_SOURCE_DIR})
    endif ()
    set (HDF_LZ4_URL ${TGZPATH}/${HDF_LZ4_TGZ_NAME})
  endif ()
endif ()

# LZF filter
option (HDF_LZF_USE_EXTERNAL "Use External Library Building for lzf plugin" 0)
if (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  set (HDF_LZF_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for lzf plugin" FORCE)
  if (HDF_LZF_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (HDF_LZF_URL ${HDF_LZF_GIT_URL})
    set (HDF_LZF_BRANCH ${HDF_LZF_GIT_BRANCH})
  elseif (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${HDF5PL_SOURCE_DIR})
    endif ()
    set (HDF_LZF_URL ${TGZPATH}/${HDF_LZF_TGZ_NAME})
  endif ()
endif ()

# MAFISC filter
option (HDF_MAFISC_USE_EXTERNAL "Use External Library Building for mafisc plugin" 0)
if (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
  set (HDF_MAFISC_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for mafisc plugin" FORCE)
  if (HDF_MAFISC_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
    set (HDF_MAFISC_URL ${HDF_MAFISC_GIT_URL})
    set (HDF_MAFISC_BRANCH ${HDF_MAFISC_GIT_BRANCH})
  elseif (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
    if (NOT TGZPATH)
      set (TGZPATH ${HDF5PL_SOURCE_DIR})
    endif ()
    set (HDF_MAFISC_URL ${TGZPATH}/${HDF_MAFISC_TGZ_NAME})
  endif ()
endif ()

include (CMakePackageConfigHelpers)

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
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (NOT HDF5PL_EXTERNALLY_CONFIGURED)
  install (
      EXPORT ${HDF5PL_EXPORTED_TARGETS}
      DESTINATION ${HDF5PL_INSTALL_CMAKE_DIR}
      FILE ${HDF5PL_PACKAGE}${HDF_PACKAGE_EXT}-targets.cmake
  )
endif ()

#-----------------------------------------------------------------------------
# Export all exported targets to the build tree for use by parent project
#-----------------------------------------------------------------------------
if (NOT HDF5PL_EXTERNALLY_CONFIGURED)
  export (
      TARGETS ${HDF5PL_LIBRARIES_TO_EXPORT} ${HDF5PL_LIB_DEPENDENCIES}
      FILE ${HDF5PL_PACKAGE}${HDF_PACKAGE_EXT}-targets.cmake
  )
endif ()

#-----------------------------------------------------------------------------
# Set variables needed for installation
#-----------------------------------------------------------------------------
set (HDF5PL_VERSION_STRING ${HDF5PL_PACKAGE_VERSION})
set (HDF5PL_VERSION_MAJOR  ${HDF5PL_PACKAGE_VERSION_MAJOR})
set (HDF5PL_VERSION_MINOR  ${HDF5PL_PACKAGE_VERSION_MINOR})

#-----------------------------------------------------------------------------
# Configure the HDF5PL-config.cmake file for the build directory
#-----------------------------------------------------------------------------
set (INCLUDE_INSTALL_DIR ${HDF5PL_INSTALL_INCLUDE_DIR})
set (SHARE_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/${HDF5PL_INSTALL_CMAKE_DIR}" )
set (CURRENT_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}" )
configure_package_config_file (
    ${HDF_RESOURCES_DIR}/HDF5PL-config.cmake.in
    "${HDF5PL_BINARY_DIR}/${HDF5PL_PACKAGE}${HDF_PACKAGE_EXT}-config.cmake"
    INSTALL_DESTINATION "${HDF5PL_INSTALL_CMAKE_DIR}"
    PATH_VARS INCLUDE_INSTALL_DIR SHARE_INSTALL_DIR CURRENT_BUILD_DIR
    INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
)

#-----------------------------------------------------------------------------
# Configure the HDF5PL-config.cmake file for the install directory
#-----------------------------------------------------------------------------
set (INCLUDE_INSTALL_DIR ${HDF5PL_INSTALL_INCLUDE_DIR})
set (SHARE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${HDF5PL_INSTALL_CMAKE_DIR}" )
set (CURRENT_BUILD_DIR "${CMAKE_INSTALL_PREFIX}" )
configure_package_config_file (
    ${HDF_RESOURCES_DIR}/HDF5PL-config.cmake.in
    "${HDF5PL_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${HDF5PL_PACKAGE}${HDF_PACKAGE_EXT}-config.cmake"
    INSTALL_DESTINATION "${HDF5PL_INSTALL_CMAKE_DIR}"
    PATH_VARS INCLUDE_INSTALL_DIR SHARE_INSTALL_DIR CURRENT_BUILD_DIR
)
if (NOT HDF5PL_EXTERNALLY_CONFIGURED)
  install (
      FILES ${HDF5PL_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${HDF5PL_PACKAGE}${HDF_PACKAGE_EXT}-config.cmake
      DESTINATION ${HDF5PL_INSTALL_CMAKE_DIR}
  )
endif ()

#-----------------------------------------------------------------------------
# Configure the HDF5PL-config-version .cmake file for the install directory
#-----------------------------------------------------------------------------
if (NOT HDF5PL_EXTERNALLY_CONFIGURED)
  configure_file (
      ${HDF_RESOURCES_DIR}/HDF5PL-config-version.cmake.in
      ${HDF5PL_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${HDF5PL_PACKAGE}${HDF_PACKAGE_EXT}-config-version.cmake @ONLY
  )
  install (
      FILES ${HDF5PL_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${HDF5PL_PACKAGE}${HDF_PACKAGE_EXT}-config-version.cmake
      DESTINATION ${HDF5PL_INSTALL_CMAKE_DIR}
  )
endif ()

#-----------------------------------------------------------------------------
# Configure the README.txt file for the binary package
#-----------------------------------------------------------------------------
HDF_README_PROPERTIES (0)

#-----------------------------------------------------------------------------
# Configure the COPYING.txt file for the windows binary package
#-----------------------------------------------------------------------------
if (WIN32)
  configure_file (${HDF5PL_SOURCE_DIR}/COPYING ${HDF5PL_BINARY_DIR}/COPYING.txt @ONLY)
endif ()

#-----------------------------------------------------------------------------
# Set the cpack variables
#-----------------------------------------------------------------------------
set (CPACK_PACKAGE_VENDOR "HDF_Group")
set (CPACK_PACKAGE_NAME "${HDF5PL_PACKAGE_NAME}")
if (CDASH_LOCAL)
  set (CPACK_PACKAGE_VERSION "${HDF5PL_PACKAGE_VERSION}")
else ()
  set (CPACK_PACKAGE_VERSION "${HDF5PL_PACKAGE_VERSION_STRING}")
endif ()
set (CPACK_PACKAGE_VERSION_MAJOR "${HDF5PL_PACKAGE_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${HDF5PL_PACKAGE_VERSION_MINOR}")
set (CPACK_PACKAGE_VERSION_PATCH "")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Readme.txt")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "HDF5PL Installation")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/Readme.txt")
set (CPACK_PACKAGE_RELOCATABLE TRUE)
set (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}/${CPACK_PACKAGE_NAME}/${CPACK_PACKAGE_VERSION}")
set (CPACK_PACKAGE_ICON "${HDF_RESOURCES_DIR}/hdf.bmp")

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
  set (CPACK_NSIS_PACKAGE_NAME "${HDF5PL_PACKAGE_STRING}-${CPACK_PACKAGE_VERSION}")
  if (CMAKE_CL_64)
    set (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION} (Win64)")
  else ()
    set (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
    set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
  endif ()
  # set the install/unistall icon used for the installer itself
  # There is a bug in NSI that does not handle full unix paths properly.
  set (CPACK_NSIS_MUI_ICON "${HDF_RESOURCES_DIR}\\\\hdf.ico")
  set (CPACK_NSIS_MUI_UNIICON "${HDF_RESOURCES_DIR}\\\\hdf.ico")
  # set the package header icon for MUI
  set (CPACK_PACKAGE_ICON "${HDF_RESOURCES_DIR}\\\\hdf.bmp")
  set (CPACK_NSIS_DISPLAY_NAME "${CPACK_NSIS_PACKAGE_NAME}, a visual tool for browsing and editing HDF files")
  set (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_NAME}\\\\${CPACK_PACKAGE_VERSION}")
  set (CPACK_NSIS_CONTACT "${HDF5PL_PACKAGE_BUGREPORT}")
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
    set (CPACK_RESOURCE_FILE_LICENSE "${HDF5PL_BINARY_DIR}/COPYING.txt")
# .. variable:: CPACK_WIX_PRODUCT_ICON
#  The Icon shown next to the program name in Add/Remove programs.
    set(CPACK_WIX_PRODUCT_ICON "${HDF_RESOURCES_DIR}\\\\hdf.ico")
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
    set(CPACK_WIX_PROPERTY_ARPCOMMENTS "HDF5PL Software Libraries")
    set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "${HDF5PL_PACKAGE_URL}")
    set(CPACK_WIX_PROPERTY_ARPHELPLINK "${HDF5PL_PACKAGE_BUGREPORT}")
    if (BUILD_SHARED_LIBS)
      set(CPACK_WIX_PATCH_FILE "${HDF_RESOURCES_DIR}/patch.xml")
    endif ()
elseif (APPLE)
  list (APPEND CPACK_GENERATOR "STGZ")
  list (APPEND CPACK_GENERATOR "DragNDrop")
  set (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
  set (CPACK_PACKAGING_INSTALL_PREFIX "/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  set (CPACK_PACKAGE_ICON "${HDF_RESOURCES_DIR}/hdf.icns")

  if (HDF5PL_PACK_MACOSX_FRAMEWORK AND HDF5PL_BUILD_FRAMEWORKS)
    set (CPACK_BUNDLE_NAME "${HDF5PL_PACKAGE_STRING}")
    set (CPACK_BUNDLE_LOCATION "/")    # make sure CMAKE_INSTALL_PREFIX ends in "/"
    set (CMAKE_INSTALL_PREFIX "/${CPACK_BUNDLE_NAME}.framework/Versions/${CPACK_PACKAGE_VERSION}/${CPACK_PACKAGE_NAME}/")
    set (CPACK_BUNDLE_ICON "${HDF_RESOURCES_DIR}/hdf.icns")
    set (CPACK_BUNDLE_PLIST "${HDF5PL_BINARY_DIR}/CMakeFiles/Info.plist")
    #-----------------------------------------------------------------------------
    # Configure the Info.plist file for the install bundle
    #-----------------------------------------------------------------------------
    configure_file (
        ${HDF_RESOURCES_DIR}/CPack.Info.plist.in
        ${HDF5PL_BINARY_DIR}/CMakeFiles/Info.plist @ONLY
    )
    configure_file (
        ${HDF_RESOURCES_DIR}/PkgInfo.in
        ${HDF5PL_BINARY_DIR}/CMakeFiles/PkgInfo @ONLY
    )
    configure_file (
        ${HDF_RESOURCES_DIR}/version.plist.in
        ${HDF5PL_BINARY_DIR}/CMakeFiles/version.plist @ONLY
    )
    install (
        FILES ${HDF5PL_BINARY_DIR}/CMakeFiles/PkgInfo
              ${HDF5PL_BINARY_DIR}/CMakeFiles/version.plist
        DESTINATION ..
    )
  endif ()
else ()
  list (APPEND CPACK_GENERATOR "STGZ")
  set (CPACK_PACKAGING_INSTALL_PREFIX "/${CPACK_PACKAGE_VENDOR}/${CPACK_PACKAGE_NAME}/${CPACK_PACKAGE_VERSION}")
  set (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)

  set (CPACK_DEBIAN_PACKAGE_SECTION "Science")
  set (CPACK_DEBIAN_PACKAGE_MAINTAINER "${HDF5PL_PACKAGE_BUGREPORT}")

  set (CPACK_RPM_COMPONENT_INSTALL ON)
  set (CPACK_RPM_PACKAGE_RELOCATABLE ON)
  set (CPACK_RPM_PACKAGE_GROUP "Libraries")
  set (CPACK_RPM_PACKAGE_LICENSE "BSD-style")
  set (CPACK_RPM_PACKAGE_URL "${HDF5PL_PACKAGE_URL}")

  set (CPACK_STRIP_FILES "${HDF5PL_LIBRARIES_TO_EXPORT};${EXTERNAL_LIBRARY_LIST}")
endif ()

# By default, do not warn when built on machines using only VS Express:
if (NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
  set (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
endif()
include (InstallRequiredSystemLibraries)

set (CPACK_INSTALL_CMAKE_PROJECTS "${HDF5PL_BINARY_DIR};HDF5PL;ALL;/")

if (HDF_PACKAGE_EXTLIBS)
  if (HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR HDF_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      if (HDF5_FOUND AND HDF5_USE_EXTERNAL)
        set (CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${HDF5_INCLUDE_DIR_GEN};HDF5;libraries;/")
        set (CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${HDF5_INCLUDE_DIR_GEN};HDF5;configinstall;/")
      endif ()
  endif ()
endif ()

include (CPack)

cpack_add_install_type(Full DISPLAY_NAME "Everything")
cpack_add_install_type(Developer)

cpack_add_component_group(Runtime)

cpack_add_component_group(Documents
    EXPANDED
    DESCRIPTION "Release notes for using HDF5PL library plugins"
)

#-----------------------------------------------------------------------------
# Now list the cpack commands
#-----------------------------------------------------------------------------
cpack_add_component (libraries
    DISPLAY_NAME "HDF5PL Libraries"
    GROUP Runtime
    INSTALL_TYPES Full Developer User
)
cpack_add_component (headers
    DISPLAY_NAME "HDF5PL Headers"
    DEPENDS libraries
    GROUP Development
    INSTALL_TYPES Full Developer
)
cpack_add_component (docs
    DISPLAY_NAME "HDF5PL Documents"
    DEPENDS libraries
    GROUP Documents
    INSTALL_TYPES Full Developer
)

