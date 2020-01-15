#
# Copyright by The HDF Group.
# All rights reserved.
#
# HDF5 BZIP2 Library Plugin makefile(.in)
#
# Allen Byrne, Ed Hartnett, 1/12/19

AM_CPPFLAGS = -I$(HDF5_ROOT)/include -I$(BZ2_ROOT)/include

# This is where HDF5 wants us to install plugins.
plugindir = @HDF5_PLUGIN_DIR@

# The libh5bz2 library for plugin module.
# Build it as shared library.
plugin_LTLIBRARIES = libh5bz2.la
libh5bz2_la_SOURCES = H5Zbzip2.c

