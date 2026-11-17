# Blosc Filter Plugin (ID 32001)

This directory integrates the Blosc HDF5 filter from the Blosc project using **git subtree**.

## Upstream Source

- **Repository**: https://github.com/Blosc/hdf5-blosc
- **Version**: `master` at commit 19c34cf (2025-11-19); the most recent release tag, v1.0.1, predates fixes this build relies on
- **License**: MIT (see `hdf5-blosc/LICENSES/`); the filter was originally derived from the h5py LZF filter (see `hdf5-blosc/LICENSES/H5PY.txt`)
- **Compression library**: c-blosc (1.x), built from `libs/c-blosc-*.tar.gz` or found on the system

The same filter source is used by PyTables and by the `hdf5plugin` Python package.

This filter writes Blosc1-format data. Blosc2 (filter ID 32026) is a separate filter; see `../BLOSC2`.

## Filter Parameters

`cd_values[0]` to `cd_values[3]` are reserved and filled in by the filter. The optional
user values are:

| Slot | Meaning | Default |
|---|---|---|
| `cd_values[4]` | Compression level, 0 to 9 | 5 |
| `cd_values[5]` | Shuffle: 0 none, 1 byte shuffle, 2 bit shuffle | 1 |
| `cd_values[6]` | Compressor: 0 BloscLZ, 1 LZ4, 2 LZ4HC, 3 Snappy, 4 Zlib, 5 Zstd | 0 (BloscLZ) |

```c
unsigned int cd_values[7] = {0, 0, 0, 0, 5, 1, 0};
H5Pset_chunk(dcpl, rank, chunk);
H5Pset_filter(dcpl, 32001, H5Z_FLAG_OPTIONAL, 7, cd_values);
```

```sh
h5repack -f UD=32001,0,7,0,0,0,0,5,1,0 in.h5 out.h5
```

> **Change from earlier hdf5_plugins releases:** the default compressor is BloscLZ, as in
> upstream. The previous in-tree copy of the filter defaulted to Zstd. Files written by
> either version read correctly with the other; only the compression ratio of data
> written without an explicit compressor changes.

## Building

The filter source is part of this repository; no submodule initialization is needed.

The wrapper (`src/CMakeLists.txt`) compiles only `hdf5-blosc/src/blosc_filter.c` and
`hdf5-blosc/src/blosc_plugin.c`. The upstream `hdf5-blosc/CMakeLists.txt` downloads c-blosc
from GitHub at build time and is not used.

```bash
cd hdf5_plugins
export HDF5_ROOT=/path/to/hdf5
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_BLOSC=ON
cmake --build build
```

The plugin is `libh5blosc.so` (`h5blosc.dll` on Windows). It is copied to `build/plugins`
for testing and installed to `lib/plugin`.

`DISABLE_H5BLOSC_ENCODER` has no effect: the upstream filter always includes the encoder.

## Updating to Newer Versions (Maintainers Only)

```bash
# One-time: add the upstream remote
git remote add hdf5-blosc https://github.com/Blosc/hdf5-blosc.git

git fetch hdf5-blosc
git subtree pull --prefix=BLOSC/hdf5-blosc hdf5-blosc <tag-or-commit> --squash
```

See [UPDATING_BLOSC_SUBTREE.md](UPDATING_BLOSC_SUBTREE.md) for the full procedure.

## Reporting Issues

- Filter behavior (bugs, features, compression): https://github.com/Blosc/hdf5-blosc/issues
- Integration with the hdf5_plugins build and tests: https://github.com/HDFGroup/hdf5_plugins/issues

## Policy: Upstream-First Development

HDF Group does not maintain a fork of hdf5-blosc.

**Never modify files in `BLOSC/hdf5-blosc/` directly.** To change the filter, open an issue
or pull request at https://github.com/Blosc/hdf5-blosc, and after it is merged, update the
subtree with `git subtree pull`.
