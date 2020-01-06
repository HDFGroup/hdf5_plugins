# The following line will use cross-compiling
set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_COMPILER_VENDOR "CrayLinuxEnvironment")

set(CMAKE_C_COMPILER cc)
set(CMAKE_Fortran_COMPILER ftn)

# the following is used if cross-compiling
set(CMAKE_CROSSCOMPILING_EMULATOR "")

# option to use pre-generated H5Tinit.c file
set(HDF5_USE_PREGEN OFF)
