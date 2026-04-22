#!/bin/sh
# This script runs the LZF examples in the hdf5_plugins project.

# Set the plugin path to find plugin.
export HDF5_PLUGIN_PATH=../src/.libs

# Run the example.
./h5ex_d_lzf
