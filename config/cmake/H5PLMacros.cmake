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
        if (NOT H5PL_COMP_TGZPATH)
          set (H5PL_COMP_TGZPATH ${H5PL_SOURCE_DIR}/libs)
       endif ()
        set (HDF_${plname}_URL ${H5PL_COMP_TGZPATH}/${HDF_${plname}_TGZ_NAME})
      endif ()
    endif ()
    add_subdirectory (${plname})
    set_global_variable (H5PL_LIBRARIES_TO_EXPORT "${H5PL_LIBRARIES_TO_EXPORT};${H5${plname}_LIBRARIES_TO_EXPORT}")
  endif ()
endmacro ()
