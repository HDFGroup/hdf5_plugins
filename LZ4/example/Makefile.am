# Copyright by The HDF Group. All rights reserved.

# This builds the LZ4 example directory.

# Ed Hartnett 1/14/19

# Link to our filter library.
LDADD = ${top_builddir}/src/libh5lz4.la

# Build example program.
check_PROGRAMS = h5ex_d_lz4
TESTS = h5ex_d_lz4

# Clean up HDF5 file created by example.
CLEANFILES = *.h5
