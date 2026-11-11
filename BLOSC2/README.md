# Blosc2 Filter Plugin (ID 32026)

This directory integrates the Blosc2 HDF5 filter from the Blosc project using **git subtree**.

## Upstream Source

- **Repository**: https://github.com/Blosc/HDF5-Blosc2
- **Version**: v3.0.1 (commit 1ca9d34)
- **License**: MIT (see `HDF5-Blosc2/LICENSE.txt` and `HDF5-Blosc2/LICENSES/`)
- **Compression library**: c-blosc2, built from `libs/c-blosc2-*.tar.gz` or found on the system

The same filter source is used by PyTables (as a git submodule) and by the `hdf5plugin`
Python package.

This filter only handles Blosc2 (filter ID 32026). Blosc1 data (filter ID 32001) is handled
by the separate filter in `../BLOSC`.

## Filter Parameters

`cd_values[0]` to `cd_values[3]` are reserved and filled in by the filter. For chunks of
rank 2 or more (up to c-blosc2's `B2ND_MAX_DIM`), the filter also stores the chunk rank in
`cd_values[7]` and the chunk shape after it, and compresses with Blosc2's multidimensional
B2ND format. The optional user values are:

| Slot | Meaning | Default |
|---|---|---|
| `cd_values[4]` | Compression level, 0 to 9 | 5 |
| `cd_values[5]` | Filter: 0 none, 1 byte shuffle, 2 bit shuffle, 3 delta, 4 truncate precision | 1 |
| `cd_values[6]` | Compressor: 0 BloscLZ, 1 LZ4, 2 LZ4HC, 4 Zlib, 5 Zstd | 0 (BloscLZ) |

```c
unsigned int cd_values[7] = {0, 0, 0, 0, 5, 1, 0};
H5Pset_chunk(dcpl, rank, chunk);
H5Pset_filter(dcpl, 32026, H5Z_FLAG_OPTIONAL, 7, cd_values);
```

```sh
h5repack -f UD=32026,0,7,0,0,0,0,5,1,0 in.h5 out.h5
```

If the data reaching the filter is not exactly one chunk (for example, when a Fletcher32
checksum is applied before Blosc2), the filter uses plain Blosc2 instead of B2ND. The
previous in-tree copy of the filter lacked this check and wrote unreadable data in that case.

## Building

The filter source is part of this repository; no submodule initialization is needed.

The wrapper (`src/CMakeLists.txt`) compiles only `HDF5-Blosc2/src/blosc2_filter.c` and
`HDF5-Blosc2/src/blosc2_plugin.c`. The upstream `HDF5-Blosc2/CMakeLists.txt` downloads
c-blosc2 from GitHub at build time and is not used.

```bash
cd hdf5_plugins
export HDF5_ROOT=/path/to/hdf5
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_BLOSC2=ON
cmake --build build
```

The plugin is `libh5blosc2.so` (`h5blosc2.dll` on Windows). It is copied to `build/plugins`
for testing and installed to `lib/plugin`.

`DISABLE_H5BLOSC2_ENCODER` has no effect: the upstream filter always includes the encoder.

## Thread Safety

The upstream filter registers with the `H5Z_class2_t` structure, without the HDF5 2.3
`H5Z_FEAT_FILTER_THREADSAFE` and `H5Z_FEAT_FILTER_NO_H5_CALLS` flags, so HDF5 does not call
it from multiple threads at once. Declaring those flags needs a change in the filter
upstream: it calls `blosc2_init()` on every chunk and `blosc2_destroy()` on every failed
chunk, which is not safe when chunks are decompressed concurrently.

## Updating to Newer Versions (Maintainers Only)

```bash
# One-time: add the upstream remote
git remote add hdf5-blosc2 https://github.com/Blosc/HDF5-Blosc2.git

git fetch --tags hdf5-blosc2
git subtree pull --prefix=BLOSC2/HDF5-Blosc2 hdf5-blosc2 <tag> --squash
```

See [UPDATING_BLOSC2_SUBTREE.md](UPDATING_BLOSC2_SUBTREE.md) for the full procedure.

## Reporting Issues

- Filter behavior (bugs, features, compression): https://github.com/Blosc/HDF5-Blosc2/issues
- Integration with the hdf5_plugins build and tests: https://github.com/HDFGroup/hdf5_plugins/issues

## Policy: Upstream-First Development

HDF Group does not maintain a fork of HDF5-Blosc2.

**Never modify files in `BLOSC2/HDF5-Blosc2/` directly.** To change the filter, open an
issue or pull request at https://github.com/Blosc/HDF5-Blosc2, and after it is merged,
update the subtree with `git subtree pull`.
