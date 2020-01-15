# Copyright by The HDF Group. All rights reserved.

# This is the Makefile.am for the HDF5 BLOSC filter library. This allows
# the use of BLOSC compression/decompression on HDF5 datasets.
#
# Allen Byrne, Ed Hartnett 1/15/19

# This is where HDF5 wants us to install plugins.
plugindir = @HDF5_PLUGIN_DIR@

# The libh5lz4 library for plugin module.
# Build it as shared library.
plugin_LTLIBRARIES = libh5blosc.la
libh5blosc_la_SOURCES = H5Zblosc.c #blosc.c blosclz.c shuffle.c
