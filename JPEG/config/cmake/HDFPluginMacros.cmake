#-------------------------------------------------------------------------------
macro (BASIC_SETTINGS varname)
  string(TOUPPER ${varname} PLUGIN_PACKAGE_VARNAME)
  string(TOLOWER ${varname} PLUGIN_VARNAME)
  set (H5${PLUGIN_PACKAGE_VARNAME}_PACKAGE "h5${PLUGIN_VARNAME}")
  set (H5${PLUGIN_PACKAGE_VARNAME}_PACKAGE_NAME "h5${PLUGIN_VARNAME}")
  string(TOUPPER ${H5${PLUGIN_PACKAGE_VARNAME}_PACKAGE_NAME} PLUGIN_PACKAGE_NAME)
  string(TOLOWER ${H5${PLUGIN_PACKAGE_VARNAME}_PACKAGE_NAME} PLUGIN_NAME)
  set (CMAKE_NO_SYSTEM_FROM_IMPORTED 1)

  #-----------------------------------------------------------------------------
  # Define some CMake variables for use later in the project
  #-----------------------------------------------------------------------------
  set (${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR           ${${PLUGIN_PACKAGE_NAME}_SOURCE_DIR}/config/cmake)
  set (${PLUGIN_PACKAGE_NAME}_SRC_DIR                 ${${PLUGIN_PACKAGE_NAME}_SOURCE_DIR}/src)

  #-----------------------------------------------------------------------------
  # Set the core names of all the libraries
  #-----------------------------------------------------------------------------
    set (${PLUGIN_PACKAGE_NAME}_LIB_CORENAME              "${PLUGIN_NAME}")

  #-----------------------------------------------------------------------------
  # Set the true names of all the libraries if customized by external project
  #-----------------------------------------------------------------------------
  set (${PLUGIN_PACKAGE_NAME}_LIB_NAME              "${${PLUGIN_PACKAGE_NAME}_EXTERNAL_LIB_PREFIX}${${PLUGIN_PACKAGE_NAME}_LIB_CORENAME}")

  #-----------------------------------------------------------------------------
  # Set the target names of all the libraries
  #-----------------------------------------------------------------------------
  set (${PLUGIN_PACKAGE_NAME}_LIB_TARGET              ${${PLUGIN_PACKAGE_NAME}_LIB_CORENAME})

  if (NOT ${PLUGIN_PACKAGE_NAME}_INSTALL_BIN_DIR)
    set (${PLUGIN_PACKAGE_NAME}_INSTALL_BIN_DIR bin)
  endif ()
  if (NOT ${PLUGIN_PACKAGE_NAME}_INSTALL_LIB_DIR)
    set (${PLUGIN_PACKAGE_NAME}_INSTALL_LIB_DIR lib/plugin)
  endif ()
  if (NOT ${PLUGIN_PACKAGE_NAME}_INSTALL_INCLUDE_DIR)
    set (${PLUGIN_PACKAGE_NAME}_INSTALL_INCLUDE_DIR include)
  endif ()
  if (NOT ${PLUGIN_PACKAGE_NAME}_INSTALL_DATA_DIR)
    if (NOT WIN32)
      set (${PLUGIN_PACKAGE_NAME}_INSTALL_DATA_DIR share)
      set (${PLUGIN_PACKAGE_NAME}_INSTALL_CMAKE_DIR share/cmake/${PLUGIN_NAME})
    else ()
      set (${PLUGIN_PACKAGE_NAME}_INSTALL_DATA_DIR ".")
      set (${PLUGIN_PACKAGE_NAME}_INSTALL_CMAKE_DIR cmake)
    endif ()
  endif ()

  #-----------------------------------------------------------------------------
  # Setup output Directories
  #-----------------------------------------------------------------------------
  if (NOT ${PLUGIN_PACKAGE_NAME}_EXTERNALLY_CONFIGURED)
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
  # by other projects using Find${PLUGIN_PACKAGE_NAME}.
  #-----------------------------------------------------------------------------
  if (NOT ${PLUGIN_PACKAGE_NAME}_EXPORTED_TARGETS AND NOT "${PLUGIN_NAME}" MATCHES "h5pl")
    set (${PLUGIN_PACKAGE_NAME}_EXPORTED_TARGETS "${PLUGIN_NAME}-targets")
  endif ()

  #-----------------------------------------------------------------------------
  # To include a library in the list exported by the project AT BUILD TIME,
  # add it to this variable. This is NOT used by Make Install, but for projects
  # which include ${PLUGIN_NAME} as a sub-project within their build tree
  #-----------------------------------------------------------------------------
  set_global_variable (${PLUGIN_PACKAGE_NAME}_LIBRARIES_TO_EXPORT "")

  #-----------------------------------------------------------------------------
  # Plugins are MODULEs that must be built as Shared libs
  #-----------------------------------------------------------------------------
  set (BUILD_SHARED_LIBS ON CACHE BOOL "Build Shared Libraries" FORCE)
  set (LIB_TYPE SHARED)
  add_definitions (-D${PLUGIN_PACKAGE_NAME}_BUILT_AS_DYNAMIC_LIB)

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

  option (DISABLE_${PLUGIN_PACKAGE_NAME}_ENCODER "build the ${PLUGIN_NAME} library without encoding" OFF)
  if (DISABLE_${PLUGIN_PACKAGE_NAME}_ENCODER)
    set (FILTER_DECODE_ONLY 1)
  endif ()

  #-----------------------------------------------------------------------------
  # Set includes needed for build
  #-----------------------------------------------------------------------------
  set (${PLUGIN_PACKAGE_NAME}_INCLUDES_BUILD_TIME
      ${${PLUGIN_PACKAGE_NAME}_SRC_DIR} ${${PLUGIN_PACKAGE_NAME}_BINARY_DIR}
  )
endmacro ()

macro (HDF5_SUPPORT link_hdf)
  set (CMAKE_MODULE_PATH ${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR} ${CMAKE_MODULE_PATH})
  set (CMAKE_POSITION_INDEPENDENT_CODE ON)
  # plugins must use SHARED libraries
  option (USE_SHARED_LIBS "Use Shared Libraries" ON)

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
        if (NOT TARGET hdf5::h5dump-shared)
          add_executable (hdf5::h5dump-shared IMPORTED)
        endif ()
        if (NOT TARGET hdf5::h5repack-shared)
          add_executable (hdf5::h5repack-shared IMPORTED)
        endif ()
        set (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:hdf5::h5dump-shared>)
        set (HDF5_REPACK_EXECUTABLE $<TARGET_FILE:hdf5::h5repack-shared>)
      else ()
        if (NOT TARGET hdf5::h5dump)
          add_executable (hdf5::h5dump IMPORTED)
        endif()
        if (NOT TARGET hdf5::h5repack)
          add_executable (hdf5::h5repack IMPORTED)
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
        if (${link_hdf})
          #plugin source needs to be linked with HDF5
          set (LINK_LIBS ${LINK_LIBS} ${HDF5_LIBRARIES})
          if (HDF5_BUILD_SHARED_LIBS)
            add_definitions (-DH5_BUILT_AS_DYNAMIC_LIB)
          else ()
            add_definitions (-DH5_BUILT_AS_STATIC_LIB)
          endif ()
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
      else ()
        if (USE_SHARED_LIBS AND HDF5_shared_C_FOUND)
          set (HDF5_LINK_LIBS ${HDF5_LINK_LIBS} ${HDF5_C_SHARED_LIBRARY})
          set (HDF5_LIBRARY_PATH ${PACKAGE_PREFIX_DIR}/lib)
          if (${link_hdf})
            #plugin source needs to be linked with HDF5
            set (LINK_LIBS ${LINK_LIBS} ${HDF5_C_SHARED_LIBRARY})
          endif ()
          set_property (TARGET hdf5::h5dump-shared PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dump-shared")
          set_property (TARGET hdf5::h5repack-shared PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repack-shared")
        else ()
          set (HDF5_LINK_LIBS ${HDF5_LINK_LIBS} ${HDF5_C_STATIC_LIBRARY})
          if (${link_hdf})
            #plugin source needs to be linked with HDF5
            set (LINK_LIBS ${LINK_LIBS} ${HDF5_C_STATIC_LIBRARY})
          endif ()
          set_property (TARGET hdf5::h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dump")
          set_property (TARGET hdf5::h5repack PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repack")
        endif ()
      endif ()
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
      if (${link_hdf})
        #plugin source needs to be linked with HDF5
        set (LINK_LIBS ${LINK_LIBS} ${HDF5_LIBRARIES})
      endif ()
      add_executable (hdf5::h5dump IMPORTED)
      add_executable (hdf5::h5repack IMPORTED)
      set_property (TARGET hdf5::h5dump PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5dump")
      set_property (TARGET hdf5::h5repack PROPERTY IMPORTED_LOCATION "${HDF5_TOOLS_DIR}/h5repack")
      set (HDF5_DUMP_EXECUTABLE $<TARGET_FILE:hdf5::h5dump>)
      set (HDF5_REPACK_EXECUTABLE $<TARGET_FILE:hdf5::h5repack>)
    endif ()

    set (HDF5_PACKAGE_NAME ${SEARCH_PACKAGE_NAME})

    if (HDF5_FOUND)
      set (HDF5_HAVE_H5PUBCONF_H 1)
      set (HDF5_HAVE_HDF5 1)
      set (HDF5_HDF5_HEADER "h5pubconf.h")
      set (HDF5_INCLUDE_DIRS ${HDF5_INCLUDE_DIR})
      message (STATUS "HDF5 found: INC=${HDF5_INCLUDE_DIR} TOOLS=${HDF5_TOOLS_DIR}")
    else ()
      message (FATAL_ERROR " HDF5 is Required for plugin library")
    endif ()
  else ()
    # This project is being called from within another and HDF5 is already configured
    set (HDF5_HAVE_H5PUBCONF_H 1)
    set (HDF5_HAVE_HDF5 1)
    if (${link_hdf})
      #plugin source needs to be linked with HDF5
      set (LINK_LIBS ${LINK_LIBS} ${HDF5_LINK_LIBS})
    endif ()
  endif ()
  set (H5PL_INCLUDE_DIRS ${HDF5_INCLUDE_DIR})
  message (STATUS "HDF5 link libs: ${HDF5_LINK_LIBS} Includes: ${H5PL_INCLUDE_DIRS}")

  if (USE_SHARED_LIBS)
    set (H5_LIB_TYPE SHARED)
  else ()
    set (H5_LIB_TYPE STATIC)
  endif ()
endmacro ()

macro (INSTALL_SUPPORT varname)
  string(TOUPPER ${varname} PLUGIN_PACKAGE_NAME)
  string(TOLOWER ${varname} PLUGIN_NAME)

  #-----------------------------------------------------------------------------
  # Set variables needed for installation
  #-----------------------------------------------------------------------------
  set (${PLUGIN_PACKAGE_NAME}_VERSION_STRING ${${PLUGIN_PACKAGE_NAME}_PACKAGE_VERSION})
  set (${PLUGIN_PACKAGE_NAME}_VERSION_MAJOR  ${${PLUGIN_PACKAGE_NAME}_PACKAGE_VERSION_MAJOR})
  set (${PLUGIN_PACKAGE_NAME}_VERSION_MINOR  ${${PLUGIN_PACKAGE_NAME}_PACKAGE_VERSION_MINOR})

  #-----------------------------------------------------------------------------
  # Configure the H5PL_Examples.cmake file and the examples
  #-----------------------------------------------------------------------------
  configure_file (
      ${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/H5PL_Examples.cmake.in
      ${${PLUGIN_PACKAGE_NAME}_BINARY_DIR}/H5PL_Examples.cmake @ONLY
  )
  install (
      FILES
          ${${PLUGIN_PACKAGE_NAME}_BINARY_DIR}/H5PL_Examples.cmake
          ${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/CTestScript.cmake
          ${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/HDFoptions.cmake
          DESTINATION ${${PLUGIN_PACKAGE_NAME}_INSTALL_DATA_DIR}
      COMPONENT hdfdocuments
  )
  execute_process(
       COMMAND ${CMAKE_COMMAND} -E copy_directory ${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/binex ${${PLUGIN_PACKAGE_NAME}_BINARY_DIR}/HDFPLExamples
  )
  install (
    DIRECTORY ${${PLUGIN_PACKAGE_NAME}_BINARY_DIR}/HDFPLExamples
    DESTINATION ${${PLUGIN_PACKAGE_NAME}_INSTALL_DATA_DIR}
    USE_SOURCE_PERMISSIONS
    COMPONENT hdfdocuments
  )

  #-----------------------------------------------------------------------------
  # Configure the README.txt file for the binary package
  #-----------------------------------------------------------------------------
  PLUGIN_README_PROPERTIES (${PLUGIN_PACKAGE_NAME})

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
  if (NOT ${PLUGIN_PACKAGE_NAME}_EXTERNALLY_CONFIGURED)
    set (CPACK_PACKAGE_VENDOR "HDF_Group")
    set (CPACK_PACKAGE_NAME "${${PLUGIN_PACKAGE_NAME}_PACKAGE_NAME}")
    if (CDASH_LOCAL)
      set (CPACK_PACKAGE_VERSION "${${PLUGIN_PACKAGE_NAME}_PACKAGE_VERSION}")
    else ()
      set (CPACK_PACKAGE_VERSION "${${PLUGIN_PACKAGE_NAME}_PACKAGE_VERSION_STRING}")
    endif ()
    set (CPACK_PACKAGE_VERSION_MAJOR "${${PLUGIN_PACKAGE_NAME}_PACKAGE_VERSION_MAJOR}")
    set (CPACK_PACKAGE_VERSION_MINOR "${${PLUGIN_PACKAGE_NAME}_PACKAGE_VERSION_MINOR}")
    set (CPACK_PACKAGE_VERSION_PATCH "")
    #set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
    set (CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
    set (CPACK_PACKAGE_RELOCATABLE TRUE)
    set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PLUGIN_NAME} Installation")
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
      set (CPACK_NSIS_PACKAGE_NAME "${${PLUGIN_PACKAGE_NAME}_PACKAGE_STRING}")
      if (CMAKE_CL_64)
        set (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
        set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION} (Win64)")
      else ()
        set (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
        set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
      endif ()
      # set the install/unistall icon used for the installer itself
      # There is a bug in NSI that does not handle full unix paths properly.
      #set (CPACK_NSIS_MUI_ICON "${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}\\\\${PLUGIN_PACKAGE_NAME}.ico")
      #set (CPACK_NSIS_MUI_UNIICON "${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}\\\\${PLUGIN_PACKAGE_NAME}.ico")
      # set the package header icon for MUI
      #set (CPACK_PACKAGE_ICON "${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}\\\\${PLUGIN_PACKAGE_NAME}.bmp")
      set (CPACK_NSIS_DISPLAY_NAME "${CPACK_NSIS_PACKAGE_NAME}, is a dynamic filter for HDF5")
      #set (CPACK_PACKAGE_INSTALL_DIRECTORY "plugins")
      set (CPACK_NSIS_CONTACT "${${PLUGIN_PACKAGE_NAME}_PACKAGE_BUGREPORT}")
      set (CPACK_NSIS_MODIFY_PATH ON)

      if (WIX_EXECUTABLE)
        list (APPEND CPACK_GENERATOR "WIX")
      endif ()
      #WiX variables
      set (CPACK_WIX_UNINSTALL "1")
#   .. variable:: CPACK_WIX_LICENSE_RTF
#    RTF License File
#
#    If CPACK_RESOURCE_FILE_LICENSE has an .rtf extension it is used as-is.
#
#    If CPACK_RESOURCE_FILE_LICENSE has an .txt extension it is implicitly
#    converted to RTF by the WiX Generator.
#    The expected encoding of the .txt file is UTF-8.
#
#    With CPACK_WIX_LICENSE_RTF you can override the license file used by the
#    WiX Generator in case CPACK_RESOURCE_FILE_LICENSE is in an unsupported
#    format or the .txt -> .rtf conversion does not work as expected.
      set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/README.txt")
#   .. variable:: CPACK_WIX_PRODUCT_ICON
#    The Icon shown next to the program name in Add/Remove programs.
#      set(CPACK_WIX_PRODUCT_ICON "${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}\\\\${PLUGIN_PACKAGE_NAME}.ico")
#
#    .. variable:: CPACK_WIX_UI_BANNER
#
#    The bitmap will appear at the top of all installer pages other than the
#    welcome and completion dialogs.
#
#    If set, this image will replace the default banner image.
#
#    This image must be 493 by 58 pixels.
#
#   .. variable:: CPACK_WIX_UI_DIALOG
#
#    Background bitmap used on the welcome and completion dialogs.
#
#    If this variable is set, the installer will replace the default dialog
#    image.
#
#    This image must be 493 by 312 pixels.
#
    elseif (APPLE)
      list (APPEND CPACK_GENERATOR "STGZ")
      list (APPEND CPACK_GENERATOR "DragNDrop")
      set (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)
      set (CPACK_PACKAGING_INSTALL_PREFIX "/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
#      set (CPACK_PACKAGE_ICON "${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/h5jpeg.gif")

      option (${PLUGIN_PACKAGE_NAME}_PACK_MACOSX_BUNDLE  "Package the ${PLUGIN_PACKAGE_NAME} Library in a Bundle" OFF)
      if (${PLUGIN_PACKAGE_NAME}_PACK_MACOSX_BUNDLE)
        list (APPEND CPACK_GENERATOR "Bundle")
        set (CPACK_BUNDLE_NAME "${${PLUGIN_PACKAGE_NAME}_PACKAGE_STRING}")
        set (CPACK_BUNDLE_LOCATION "/")    # make sure CMAKE_INSTALL_PREFIX ends in /
        set (CMAKE_INSTALL_PREFIX "/${CPACK_BUNDLE_NAME}.framework/Versions/${CPACK_PACKAGE_VERSION}/${CPACK_PACKAGE_NAME}/")
#        set (CPACK_BUNDLE_ICON "${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/H5JPEG.icns")
#        set (CPACK_BUNDLE_PLIST "${${PLUGIN_PACKAGE_NAME}_BINARY_DIR}/CMakeFiles/Info.plist")
        set (CPACK_APPLE_GUI_INFO_STRING "${PLUGIN_PACKAGE_NAME} Plugin Library")
        set (CPACK_APPLE_GUI_COPYRIGHT "Copyright © ???. All rights reserved.")
        set (CPACK_SHORT_VERSION_STRING "${CPACK_PACKAGE_VERSION}")
        set (CPACK_APPLE_GUI_BUNDLE_NAME "${${PLUGIN_PACKAGE_NAME}_PACKAGE_STRING}")
        set (CPACK_APPLE_GUI_VERSION_STRING "${CPACK_PACKAGE_VERSION_STRING}")
        set (CPACK_APPLE_GUI_SHORT_VERSION_STRING "${CPACK_PACKAGE_VERSION}")
        #-----------------------------------------------------------------------------
        # Configure the Info.plist file for the install bundle
        #-----------------------------------------------------------------------------
#        configure_file (
#            ${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/CPack.Info.plist.in
#            ${${PLUGIN_PACKAGE_NAME}_BINARY_DIR}/CMakeFiles/Info.plist @ONLY
#        )
#        configure_file (
#            ${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/PkgInfo.in
#            ${${PLUGIN_PACKAGE_NAME}_BINARY_DIR}/CMakeFiles/PkgInfo @ONLY
#        )
#        configure_file (
#            ${${PLUGIN_PACKAGE_NAME}_RESOURCES_DIR}/version.plist.in
#            ${${PLUGIN_PACKAGE_NAME}_BINARY_DIR}/CMakeFiles/version.plist @ONLY
#        )
#        install (
#            FILES ${${PLUGIN_PACKAGE_NAME}_BINARY_DIR}/CMakeFiles/PkgInfo
#                  ${${PLUGIN_PACKAGE_NAME}_BINARY_DIR}/CMakeFiles/version.plist
#            DESTINATION ..
#        )
      endif ()
    else (WIN32)
      list (APPEND CPACK_GENERATOR "STGZ")
      set (CPACK_PACKAGING_INSTALL_PREFIX "/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
      set (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)

      set (CPACK_DEBIAN_PACKAGE_SECTION "Libraries")
      set (CPACK_DEBIAN_PACKAGE_MAINTAINER "${${PLUGIN_PACKAGE_NAME}_PACKAGE_BUGREPORT}")

#        list (APPEND CPACK_GENERATOR "RPM")
      set (CPACK_RPM_PACKAGE_RELEASE "1")
      set (CPACK_RPM_COMPONENT_INSTALL ON)
      set (CPACK_RPM_PACKAGE_RELOCATABLE ON)
    endif ()

    if (${PLUGIN_PACKAGE_NAME}_CPACK_ENABLE)
      set (CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${${PLUGIN_PACKAGE_NAME}_BINARY_DIR};${PLUGIN_NAME};ALL;/")
    else ()
      set (CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${${PLUGIN_PACKAGE_NAME}_BINARY_DIR};${PLUGIN_NAME};libraries;/")
      set (CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${${PLUGIN_PACKAGE_NAME}_BINARY_DIR};${PLUGIN_NAME};headers;/")
    endif ()

    set (CPACK_ALL_INSTALL_TYPES Full User)
    set (CPACK_INSTALL_TYPE_FULL_DISPLAY_NAME "Everything")

  endif ()

  if (${PLUGIN_PACKAGE_NAME}_CPACK_ENABLE)
    include (CPack)

    cpack_add_component_group(Runtime)

    cpack_add_component (libraries
        DISPLAY_NAME "${PLUGIN_NAME} Libraries"
        GROUP Runtime
    )
    cpack_add_component_group(hdfdocuments
        DISPLAY_NAME "${PLUGIN_NAME} Documents"
        GROUP Documents
        INSTALL_TYPES Full User
    )
  endif ()
endmacro ()
