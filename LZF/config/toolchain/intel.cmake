# Uncomment the following to use cross-compiling
#set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_COMPILER_VENDOR "intel")

<<<<<<< HEAD
set(CMAKE_C_COMPILER icx)
set(CMAKE_CXX_COMPILER icpx)
set(CMAKE_Fortran_COMPILER ifx)
=======
if (USE_LLVM)
  set(CMAKE_C_COMPILER icx)
  set(CMAKE_CXX_COMPILER icpx)
  set(CMAKE_Fortran_COMPILER ifx)
  set(INTEL_CLANG ON)
else ()
  set(CMAKE_C_COMPILER icc)
  set(CMAKE_CXX_COMPILER icpc)
  set(CMAKE_Fortran_COMPILER ifort)
endif ()
>>>>>>> branch 'master' of https://github.com/HDFGroup/hdf5_plugins.git

# the following is used if cross-compiling
set(CMAKE_CROSSCOMPILING_EMULATOR "")
