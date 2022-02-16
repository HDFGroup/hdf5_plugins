#
# Copyright by The HDF Group.
# All rights reserved.
#
# This file is part of HDF5.  The full HDF5 copyright notice, including
# terms governing use, modification, and redistribution, is contained in
# the COPYING file, which can be found at the root of the source code
# distribution tree, or in https://www.hdfgroup.org/licenses.
# If you do not have access to either file, you may request a copy from
# help@hdfgroup.org.
#
#-------------------------------------------------------------------------------
# Plugins must be built SHARED
#-------------------------------------------------------------------------------
macro (EXTERNAL_PL_LIBRARY plname compress_type disable_encoder)
  string(TOLOWER ${plname} PLUGIN_NAME)
  if (${compress_type} MATCHES "GIT")
    EXTERNALPROJECT_ADD (${plname}
        GIT_REPOSITORY ${${plname}_URL}
        GIT_TAG ${${plname}_BRANCH}
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DUSED_SHARED_LIBS:BOOL="ON"
            -DBUILD_SHARED_LIBS:BOOL="ON"
            -D${plname}_PACKAGE_NAME:STRING="${PLUGIN_NAME}"
            -DH5PL_ALLOW_EXTERNAL_SUPPORT:STRING=${H5PL_ALLOW_EXTERNAL_SUPPORT}
            -D${plname}_PACKAGE_EXT:STRING=${${plname}_PACKAGE_EXT}
            -D${plname}_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DBUILD_TESTING:STRING=${BUILD_TESTING}
            -DBUILD_EXAMPLES:STRING=${BUILD_EXAMPLES}
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_PDB_OUTPUT_DIRECTORY:PATH=${CMAKE_PDB_OUTPUT_DIRECTORY}
            -DDISABLE_${plname}_ENCODER:BOOL=${disable_encoder}
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    )
  elseif (${compress_type} MATCHES "TGZ")
    EXTERNALPROJECT_ADD (${plname}
        URL ${${plname}_URL}
        URL_MD5 ""
        INSTALL_COMMAND ""
        CMAKE_ARGS
            -DUSED_SHARED_LIBS:BOOL="ON"
            -DBUILD_SHARED_LIBS:BOOL="ON"
            -D${plname}_PACKAGE_NAME:STRING="${PLUGIN_NAME}"
            -DH5PL_ALLOW_EXTERNAL_SUPPORT:STRING=${H5PL_ALLOW_EXTERNAL_SUPPORT}
            -D${plname}_PACKAGE_EXT:STRING=${${plname}_PACKAGE_EXT}
            -D${plname}_EXTERNALLY_CONFIGURED:BOOL=OFF
            -DBUILD_TESTING:STRING=${BUILD_TESTING}
            -DBUILD_EXAMPLES:STRING=${BUILD_EXAMPLES}
            -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
            -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
            -DCMAKE_PDB_OUTPUT_DIRECTORY:PATH=${CMAKE_PDB_OUTPUT_DIRECTORY}
            -DDISABLE_${plname}_ENCODER:BOOL=${disable_encoder}
            -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
            -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_TOOLCHAIN_FILE}
    )
  endif ()
  externalproject_get_property (${plname} BINARY_DIR SOURCE_DIR)

  # Create imported target ${plname}
  add_library (${plname} "SHARED" IMPORTED)
  HDF_IMPORT_SET_LIB_OPTIONS (${plname} "${plname}" "SHARED" "${${plname}_SOVERS_MAJOR}")
  add_dependencies (H5PL ${plname})

#  include (${BINARY_DIR}/${plname}-targets.cmake)
  set (${plname}_LIBRARY "${plname}")

  set (${plname}_INCLUDE_DIR_GEN "${BINARY_DIR}")
  set (${plname}_INCLUDE_DIR "${SOURCE_DIR}")
  set (${plname}_FOUND 1)
  set (${plname}_LIBRARIES ${${plname}_LIBRARY})
  set (${plname}_INCLUDE_DIRS ${${plname}_INCLUDE_DIR_GEN} ${${plname}_INCLUDE_DIR})
endmacro ()

#-------------------------------------------------------------------------------
macro (FILTER_OPTION plname)
  string(TOLOWER ${plname} PLUGIN_NAME)
  option (ENABLE_${plname} "Enable Library Building for ${plname} plugin" ON)
  if (ENABLE_${plname})
    option (HDF_${plname}_USE_EXTERNAL "Use External Library Building for ${PLUGIN_NAME} plugin" 0)
    if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT" OR H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
      set (HDF_${plname}_USE_EXTERNAL 1 CACHE BOOL "Use External Library Building for ${PLUGIN_NAME} plugin" FORCE)
      if (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "GIT")
        set (HDF_${plname}_URL ${HDF_${plname}_GIT_URL})
        set (HDF_${plname}_BRANCH ${HDF_${plname}_GIT_BRANCH})
      elseif (H5PL_ALLOW_EXTERNAL_SUPPORT MATCHES "TGZ")
        if (NOT TGZPATH)
          set (TGZPATH ${H5PL_SOURCE_DIR})
       endif ()
        set (HDF_${plname}_URL ${TGZPATH}/${HDF_${plname}_TGZ_NAME})
      endif ()
    endif ()
    add_subdirectory (${plname})
    set_global_variable (H5PL_LIBRARIES_TO_EXPORT "${H5PL_LIBRARIES_TO_EXPORT};${H5${plname}_LIBRARIES_TO_EXPORT}")
  endif ()
endmacro ()

#-------------------------------------------------------------------------------
macro (PACKAGE_PL_LIBRARY plname compress_type)
  if (${compress_type} MATCHES "GIT" OR ${compress_type} MATCHES "TGZ")
    add_dependencies (${plname}-GenHeader-Copy ${plname})
  endif ()
endmacro ()
