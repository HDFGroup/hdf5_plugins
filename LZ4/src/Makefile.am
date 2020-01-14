# Copyright by The HDF Group. All rights reserved.

# This is the Makefile.am for the HDF5 LZ4 filter library. This allows
# the use of LZ4 compression/decompression on HDF5 datasets.
#
# Allen Byrne, Ed Hartnett 1/14/19

# This is where HDF5 wants us to install plugins.
plugindir = /usr/local/hdf5/lib/plugin

# The libh5lz4 library for plugin module.
# Build it as shared library.
plugin_LTLIBRARIES = libh5lz4.la
libh5lz4_la_SOURCES = H5Zlz4.c

