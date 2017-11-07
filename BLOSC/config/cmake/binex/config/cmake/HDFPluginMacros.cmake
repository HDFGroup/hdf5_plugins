#-------------------------------------------------------------------------------
macro (BASIC_SETTINGS varname)
    #-----------------------------------------------------------------------------
    # Instructions for use : Normal Build
    #
    # For standard build of HDF5 plugin examples.
    #-----------------------------------------------------------------------------
    set (CMAKE_NO_SYSTEM_FROM_IMPORTED 1)

    #-----------------------------------------------------------------------------
    # Allow Visual Studio solution directories
    #-----------------------------------------------------------------------------
    # Provide a way for Visual Studio Express users to turn OFF the new FOLDER
    # organization feature. Default to ON for non-Express users. Express users must
    # explicitly turn off this option to build HDF5 in the Express IDE...
    #
    option (HDFPL_USE_FOLDERS "Enable folder grouping of projects in IDEs." ON)
    mark_as_advanced (HDFPL_USE_FOLDERS)
    if (HDFPL_USE_FOLDERS)
      set_property (GLOBAL PROPERTY USE_FOLDERS ON)
    endif ()

    #-----------------------------------------------------------------------------
    # Define some CMake variables for use later in the project
    #-----------------------------------------------------------------------------
    set (H5PL_RESOURCES_DIR        ${PROJECT_SOURCE_DIR}/config/cmake)
    set (HDF5PL_SRC_DIR            ${PROJECT_SOURCE_DIR}/example)

    #-----------------------------------------------------------------------------
    # Include some macros for reusable code
    #-----------------------------------------------------------------------------
    include (${H5PL_RESOURCES_DIR}/HDFMacros.cmake)
    include (${H5PL_RESOURCES_DIR}/HDFPLMacros.cmake)

    #-----------------------------------------------------------------------------
    # Setup output Directories
    #-----------------------------------------------------------------------------
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
    )
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries"
    )
    set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries."
    )

    #-----------------------------------------------------------------------------
    # When building utility executables that generate other (source) files :
    # we make use of the following variables defined in the root CMakeLists.
    # Certain systems may add /Debug or /Release to output paths
    # and we need to call the executable from inside the CMake configuration
    #-----------------------------------------------------------------------------
    set (EXE_EXT "")
    if (WIN32)
      set (EXE_EXT ".exe")
      add_definitions (-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
      add_definitions (-D_CRT_SECURE_NO_WARNINGS)
      add_definitions (-D_CONSOLE)
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
    # Plugins are MODULEs that must be built as Shared libs
    #-----------------------------------------------------------------------------
    set (BUILD_SHARED_LIBS ON CACHE BOOL "Build Shared Libraries" FORCE)
    set (LIB_TYPE SHARED)
    set (CMAKE_POSITION_INDEPENDENT_CODE ON)

    #-----------------------------------------------------------------------------
    # Option to allow the user to disable compiler warnings
    #-----------------------------------------------------------------------------
    option (HDF_DISABLE_COMPILER_WARNINGS "Disable compiler warnings" OFF)
    if (HDF_DISABLE_COMPILER_WARNINGS)
      # MSVC uses /w to suppress warnings.  It also complains if another
      # warning level is given, so remove it.
      if (MSVC)
        set (HDF_WARNINGS_BLOCKED 1)
        string (REGEX REPLACE "(^| )([/-])W[0-9]( |$)" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /w")
        string (REGEX REPLACE "(^| )([/-])W[0-9]( |$)" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w")
      endif ()
      if (WIN32)
        add_definitions (-D_CRT_SECURE_NO_WARNINGS)
      endif ()
      # Borland uses -w- to suppress warnings.
      if (BORLAND)
        set (HDF_WARNINGS_BLOCKED 1)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w-")
      endif ()

      # Most compilers use -w to suppress warnings.
      if (NOT HDF_WARNINGS_BLOCKED)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
      endif ()
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
    # All examples need the main include directories
    #-----------------------------------------------------------------------------
    INCLUDE_DIRECTORIES (${PROJECT_BINARY_DIR})
endmacro ()

macro (HDF5_SUPPORT link_hdf)
  set (CMAKE_MODULE_PATH ${H5PL_RESOURCES_DIR} ${CMAKE_MODULE_PATH})
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
      set (HDF5_INCLUDE_DIR_GEN ${HDF5_INCLUDE_DIR})
      message (STATUS "HDF5 found: INC=${HDF5_INCLUDE_DIR} TOOLS=${HDF5_TOOLS_DIR}")
    else ()
      message (FATAL_ERROR " HDF5 is Required for plugin library")
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
endmacro ()
