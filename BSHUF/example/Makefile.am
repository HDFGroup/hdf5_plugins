# Copyright by The HDF Group. All rights reserved.

# This builds the BSHUF example directory.

# Ed Hartnett 1/15/19

# Link to our filter library.
AM_LDFLAGS = ${top_builddir}/src/libh5bshuf.la
LDADD = ${top_builddir}/src/libh5bshuf.la

# Build example program and run it as a test.
check_PROGRAMS = h5ex_d_bshuf
TESTS = h5ex_d_bshuf

# Clean up HDF5 file created by example.
CLEANFILES = *.h5
