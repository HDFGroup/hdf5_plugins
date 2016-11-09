## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   ENABLE_TESTING()
##   INCLUDE(CTest)
set (CTEST_NIGHTLY_START_TIME "18:00:00 CST")
set (CTEST_PROJECT_NAME "HDF5Plugiins")

set (CTEST_DROP_METHOD "http")
if (CDASH_LOCAL)
  set (CTEST_DROP_SITE "cdash-internal.hdfgroup.org")
  set (CTEST_DROP_LOCATION "/submit.php?project=HDF5Plugins")
else (CDASH_LOCAL)
  set (CTEST_DROP_SITE "cdash.hdfgroup.org")
  set (CTEST_DROP_LOCATION "/submit.php?project=HDF5Plugins")
endif (CDASH_LOCAL)
set (CTEST_DROP_SITE_CDASH TRUE)

set (UPDATE_TYPE git)

set (CTEST_TESTING_TIMEOUT 1200) 
set (DART_TESTING_TIMEOUT 1200) 
