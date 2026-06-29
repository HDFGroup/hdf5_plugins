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
## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   ENABLE_TESTING()
##   INCLUDE(CTest)
set (CTEST_PROJECT_NAME "HDF5PLUGINS")
set (CTEST_NIGHTLY_START_TIME "18:00:00 CST")

set (CTEST_DROP_METHOD "https")
set (CTEST_DROP_SITE "cdash.hdfgroup.org")
set (CTEST_DROP_LOCATION "/submit.php?project=HDF5PLUGINS")
set (CTEST_DROP_SITE_CDASH TRUE)

set (UPDATE_TYPE git)

set (CTEST_TESTING_TIMEOUT 1200)
set (DART_TESTING_TIMEOUT 1200)
