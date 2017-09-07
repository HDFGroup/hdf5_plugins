# H5Z-ZFP

A highly flexible floating point
compression plugin for the HDF5 library using ZFP compression.

[![Build Status](https://travis-ci.org/LLNL/H5Z-ZFP.svg?branch=master)](https://travis-ci.org/LLNL/H5Z-ZFP)
[![Documentation Status](https://readthedocs.org/projects/h5z-zfp/badge/?version=latest)](http://h5z-zfp.readthedocs.io)

For information about ZFP compression and the BSD-Licensed ZFP
library, see...

- https://computation.llnl.gov/projects/floating-point-compression
- https://github.com/LLNL/zfp

For information about HDF5 filter plugins, see...

- https://support.hdfgroup.org/HDF5/doc/Advanced/DynamicallyLoadedFilters

This H5Z-ZFP plugin was originally written for ZFP version 0.5.0.

This plugin uses the *registered* HDF5 plugin filter id 32013

The  HDF5  filter  plugin  code here is also part of the Silo library.
However, we have made an  effort to also support  it as a  stand-alone
package  due  to  the  likely  broad  appeal  and  utility  of the ZFP
compression library.

This plugin supports all 4 modes of the ZFP compression library, *rate*,
*accuracy*, *precision* and *expert*. It supports 1, 2 and 3 dimensional
datasets of single and double precision floating point data.  It can be applied
to HDF5 datasets of more than 3 dimensions as  long as no more than 3 
dimensions of the chunking are of size greater than 1.

[**Full documentation**](http://h5z-zfp.readthedocs.io)
