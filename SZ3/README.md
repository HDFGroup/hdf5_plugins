# H5Z-SZ3 Filter Plugin (ID 32024)

This directory integrates the H5Z-SZ3 filter plugin from the SZ3 project using **git subtree**.

## Upstream Source

- **Repository**: https://github.com/szcompressor/SZ3 (the filter is in `tools/H5Z-SZ3`)
- **Version**: v3.4.0
- **License**: BSD-style (see SZ3/copyright-and-BSD-license.txt); the bundled Zstd is BSD 3-Clause (see SZ3/tools/zstd/LICENSE)
- **Maintainer**: Kai Zhao (lead developer of SZ3)
- **Documentation**: https://github.com/szcompressor/SZ3/tree/master/tools/H5Z-SZ3

## About H5Z-SZ3

SZ3 is a modular error-bounded lossy compression framework for scientific datasets.
H5Z-SZ3 compresses float, double and 1, 2, 4 or 8-byte integer datasets in host byte
order, with chunks of up to 4 dimensions longer than 1, within a user-set error bound:
- **ABS**: Limit absolute error
- **REL**: Limit error relative to the value range
- **PSNR**: Target a peak signal-to-noise ratio
- **L2NORM**: Limit the L2 norm of the error

Without `cd_values` the filter uses SZ3's default settings, the `ALGO_INTERP_LORENZO`
algorithm with an absolute error bound of 1e-3:

```c
H5Pset_chunk(dcpl, rank, chunk);
H5Pset_filter(dcpl, 32024, H5Z_FLAG_MANDATORY, 0, NULL);
```

```sh
h5repack -f UD=32024,0 in.h5 out.h5
```

`cd_values[0]` is the SZ3 data format version, followed by the SZ3 configuration
serialized by SZ3. To choose the algorithm and error bound, use `H5Pset_sz3()` from the
H5Z-SZ3 library, or `cdvalueHelper` from SZ3 to print the `cd_values` for `h5repack`.
Decompression reads the settings from the compressed data.

## Building

### No Special Setup Required

The SZ3 source, including the H5Z-SZ3 filter and the Zstd library bundled in SZ3, is
included directly in this repository via git subtree. Simply clone and build:

```bash
git clone https://github.com/HDFGroup/hdf5_plugins.git
# Everything is ready - no submodule initialization needed!
```

### Build Requirements

H5Z-SZ3 requires:
1. **HDF5 library** - Set `HDF5_ROOT` environment variable
2. **A C++17 compiler** and **CMake 3.19 or newer** - SZ3 is a header-only C++17 library

No external compression library is needed: SZ3 is header-only, and the wrapper builds
SZ3's bundled Zstd (`SZ3_USE_BUNDLED_ZSTD`) into the plugin. The filter therefore builds
with any `H5PL_ALLOW_EXTERNAL_SUPPORT` setting, including `NO`. It is not built on MinGW
or with `DISABLE_H5PL_ENCODER`, as H5Z-SZ3 has no decode-only build.

### Building as Part of hdf5_plugins

```bash
cd hdf5_plugins
export HDF5_ROOT=/path/to/hdf5
mkdir build && cd build

cmake -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_SZ3=ON \
  ..

cmake --build .
```

The plugin is SZ3's `hdf5sz3` target (`libhdf5sz3.so`, `hdf5sz3.dll`), aliased as `h5sz3`.
It is copied to `build/plugins` for testing and installed to `lib/plugin`
(`H5Z_SZ3_PLUGIN_INSTALL_DIR`). As SZ3's own install, the SZ3 headers, the H5Z-SZ3 header
and library, and SZ3's CMake package are installed too. The SZ3 and Zstd license notices
are installed with the plugin.

### Building Standalone

The directory can also be configured on its own:

```bash
cmake -S hdf5_plugins/SZ3 -B build-sz3 -DCMAKE_BUILD_TYPE=Release -DH5PL_BUILD_TESTING=ON
cmake --build build-sz3
```

## Updating to Newer Versions (Maintainers Only)

HDF Group maintainers can update to newer SZ3 versions using git subtree:

```bash
# One-time: Add upstream remote (if not already done)
git remote add sz3 https://github.com/szcompressor/SZ3.git

# Update to specific version
git subtree pull --prefix=SZ3/SZ3 sz3 v3.4.1 --squash
```

**Important**: The `--squash` flag is recommended to avoid importing full upstream history into hdf5_plugins.

See [UPDATING_SZ3_SUBTREE.md](UPDATING_SZ3_SUBTREE.md) for the full procedure.

## Reporting Issues

### Filter Issues
Issues with the SZ3 filter itself (bugs, feature requests, compression behavior) should be reported at:
**https://github.com/szcompressor/SZ3/issues**

### Integration Issues
Issues with how H5Z-SZ3 integrates with the hdf5_plugins build system should be reported at:
**https://github.com/HDFGroup/hdf5_plugins/issues**

## Policy: Upstream-First Development

HDF Group does not maintain a fork of SZ3. This directory uses the upstream repository directly via git subtree.

**Important**: Never modify files in `SZ3/SZ3/` directly!

If you need changes to the filter or its build:
1. Open an issue at https://github.com/szcompressor/SZ3/issues
2. Submit a pull request to upstream
3. After upstream merges, use `git subtree pull` to update (see above)

This ensures the HDF community benefits from improvements and the upstream maintainers remain authoritative.
