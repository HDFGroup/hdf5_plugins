find_path(BLOSC_INCLUDE_DIR blosc.h)

find_library(BLOSC_LIBRARY NAMES blosc)

if (BLOSC_INCLUDE_DIR AND BLOSC_LIBRARY)
    set(BLOSC_FOUND TRUE)
    message(STATUS "Found BLOSC library: ${BLOSC_LIBRARY}")
else ()
    message(STATUS "No BLOSC library found!")
endif ()
