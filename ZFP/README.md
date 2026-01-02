# H5Z-ZFP Filter Plugin

This directory integrates the H5Z-ZFP filter plugin from LLNL as a git submodule.

## Upstream Source

- **Repository**: https://github.com/LLNL/H5Z-ZFP
- **License**: BSD 3-Clause (see H5Z-ZFP/LICENSE)
- **Maintainer**: Mark C. Miller (LLNL)
- **Documentation**: https://h5z-zfp.readthedocs.io

## About H5Z-ZFP

H5Z-ZFP is a highly flexible floating point and integer compression plugin for HDF5 using the ZFP compression library. It supports all modes of ZFP compression:
- **Rate**: Target a fixed compressed size
- **Accuracy**: Limit absolute error
- **Precision**: Limit relative error
- **Expert**: Fine-grained control
- **Lossless**: Reversible compression

The filter supports 1-4 dimensional datasets of single and double precision integer and floating point data.

## Building

### Initializing the Submodule

When cloning hdf5_plugins:
```bash
# Clone with submodules
git clone --recursive https://github.com/HDFGroup/hdf5_plugins.git

# Or initialize after cloning
git submodule update --init --recursive
```

### Build Requirements

H5Z-ZFP requires:
1. **HDF5 library** - Set `HDF5_ROOT` environment variable
2. **ZFP compression library** - The parent build system will fetch this automatically

### Building as Part of hdf5_plugins

```bash
cd hdf5_plugins
export HDF5_ROOT=/path/to/hdf5
mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_ZFP=ON \
  -DH5PL_ALLOW_EXTERNAL_SUPPORT=TGZ \
  -DH5PL_TGZPATH=../libs \
  ..

cmake --build .
```

## Updating to Newer Versions

To update to a newer version of H5Z-ZFP:

```bash
cd ZFP/H5Z-ZFP
git fetch origin
git checkout master  # or specific version tag like v1.1.1
cd ../..
git add ZFP/H5Z-ZFP
git commit -m "Update H5Z-ZFP to <version>"
```

## Reporting Issues

### Filter Issues
Issues with the ZFP filter itself (bugs, feature requests, compression behavior) should be reported at:
**https://github.com/LLNL/H5Z-ZFP/issues**

### Integration Issues
Issues with how H5Z-ZFP integrates with the hdf5_plugins build system should be reported at:
**https://github.com/HDFGroup/hdf5_plugins/issues**

## Policy: Upstream-First Development

HDF Group does not maintain a fork of H5Z-ZFP. This directory uses the upstream repository directly via git submodule.

If you need changes to the filter:
1. Open an issue at https://github.com/LLNL/H5Z-ZFP/issues
2. Submit a pull request to upstream
3. Update the submodule pointer after the change is merged upstream

This ensures the HDF community benefits from improvements and the upstream maintainers remain authoritative.
