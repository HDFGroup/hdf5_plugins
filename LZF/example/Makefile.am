# Copyright by The HDF Group. All rights reserved.

# This builds the LZF example directory.

# Ed Hartnett 1/15/19

# Link to our filter library.
AM_LDFLAGS = ${top_builddir}/src/libh5lzf.la
LDADD = ${top_builddir}/src/libh5lzf.la

# Build example program and run it as a test.
check_PROGRAMS = h5ex_d_lzf
#TESTS = h5ex_d_lzf

# Clean up HDF5 file created by example.
CLEANFILES = *.h5
