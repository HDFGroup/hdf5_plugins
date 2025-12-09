# Copyright by The HDF Group. All rights reserved.

# This file builds main directory for the hdf5_plugings project.

# Edward Hartnett 12/24/2019

# This directory stores libtool macros, put there by aclocal.
ACLOCAL_AMFLAGS = -I m4

# Does the user want to build bzip2?
if BUILD_BZIP2
BZIP2 = BZIP2
endif

# Does the user want to build lz4?
if BUILD_LZ4
LZ4 = LZ4
endif

# Does the user want to build JPEG?
if BUILD_JPEG
JPEG = JPEG
endif

# Does the user want to build LZF?
if BUILD_LZF
LZF = LZF
endif

# Build the desired subdirectories.
SUBDIRS = libs $(BZIP2) $(LZ4) $(JPEG) $(LZF)
