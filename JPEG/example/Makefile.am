# Copyright by The HDF Group. All rights reserved.

# This builds the JPEG example directory.

# Edward Hartnett 9/15/25

# Link to our filter library.
LDADD = ${top_builddir}/src/libh5jpeg.la

# Build example program and run it as a test.
check_PROGRAMS = h5ex_d_jpeg
TESTS = h5ex_d_jpeg

# Clean up HDF5 file created by example.
CLEANFILES = *.h5
