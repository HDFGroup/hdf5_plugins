# Copyright by The HDF Group. All rights reserved.

# This is the Makefile.am for the HDF5 BLOSC2 filter library. This allows
# the use of BLOSC2 compression/decompression on HDF5 datasets.
#
# Allen Byrne, Ed Hartnett 1/15/19

# This is where HDF5 wants us to install plugins.
plugindir = @HDF5_PLUGIN_DIR@

# This linker flag specifies libtool version info.
# See http://www.gnu.org/software/libtool/manual/libtool.html#Libtool-versioning
# for information regarding incrementing `-version-info`.
libh5blosc2_la_LDFLAGS = -version-info 0:0:0

# The libh5blosc2 library for plugin module.
# Build it as shared library.
plugin_LTLIBRARIES = libh5blosc2.la
# The filter sources come from the upstream git subtree in ../HDF5-Blosc2.
libh5blosc2_la_CPPFLAGS = -I$(srcdir)/../HDF5-Blosc2/src
libh5blosc2_la_SOURCES = ../HDF5-Blosc2/src/blosc2_filter.c ../HDF5-Blosc2/src/blosc2_plugin.c
