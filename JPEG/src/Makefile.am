# Copyright by The HDF Group. All rights reserved.

# This is the Makefile.am for the HDF5 LZ4 filter library. This allows
# the use of LZ4 compression/decompression on HDF5 datasets.
#
# Ed Hartnett 1/15/19

# This is where HDF5 wants us to install plugins.
plugindir = @HDF5_PLUGIN_DIR@

# This linker flag specifies libtool version info.
# See http://www.gnu.org/software/libtool/manual/libtool.html#Libtool-versioning
# for information regarding incrementing `-version-info`.
libh5jpeg_la_LDFLAGS = -version-info 0:0:0

# The libh5lz4 library for plugin module.
# Build it as shared library.
plugin_LTLIBRARIES = libh5jpeg.la
libh5jpeg_la_SOURCES = H5Zjpeg.c
