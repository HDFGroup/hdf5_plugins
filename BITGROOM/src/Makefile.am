# Copyright by The HDF Group. All rights reserved.

# This is the Makefile.am for the HDF5 BITGROOM filter library. This allows
# the use of BITGROOM compression/decompression on HDF5 datasets.
#
# Ed Hartnett 1/15/19

# This is where HDF5 wants us to install plugins.
plugindir = @HDF5_PLUGIN_DIR@

# This linker flag specifies libtool version info.
# See http://www.gnu.org/software/libtool/manual/libtool.html#Libtool-versioning
# for information regarding incrementing `-version-info`.
libh5bitgroom_la_LDFLAGS = -version-info 0:0:0

# The libh5bitgroom library for plugin module.
# Build it as shared library.
#plugin_LTLIBRARIES = libh5bitgroom.la
#libh5bitgroom_la_SOURCES = H5Zbitgroom.c
