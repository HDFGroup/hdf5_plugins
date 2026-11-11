# Copyright by The HDF Group. All rights reserved.

# This is the Makefile.am for the HDF5 BLOSC filter library. This allows
# the use of BLOSC compression/decompression on HDF5 datasets.
#
# Allen Byrne, Ed Hartnett 1/15/19

# This is where HDF5 wants us to install plugins.
plugindir = @HDF5_PLUGIN_DIR@

# This linker flag specifies libtool version info.
# See http://www.gnu.org/software/libtool/manual/libtool.html#Libtool-versioning
# for information regarding incrementing `-version-info`.
libh5blosc_la_LDFLAGS = -version-info 0:0:0

# The libh5blosc library for plugin module.
# Build it as shared library.
plugin_LTLIBRARIES = libh5blosc.la
# The filter sources come from the upstream git subtree in ../hdf5-blosc.
libh5blosc_la_CPPFLAGS = -I$(srcdir)/../hdf5-blosc/src
libh5blosc_la_SOURCES = ../hdf5-blosc/src/blosc_filter.c ../hdf5-blosc/src/blosc_plugin.c
