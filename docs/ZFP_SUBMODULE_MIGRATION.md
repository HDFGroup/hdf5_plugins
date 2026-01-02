# ZFP Migration Strategy - Using Git Subtree

## Overview

This document describes the migration of the ZFP filter plugin from a local copy to using the upstream LLNL/H5Z-ZFP repository via **git subtree**. This approach ensures HDF Group does not maintain third-party code and stays up-to-date with upstream.

## Previous State

The `ZFP/` directory previously contained:
1. **H5Z-ZFP filter source code** copied from https://github.com/LLNL/H5Z-ZFP
2. **HDF Group build infrastructure** added on top (config/, bin/, Additional_Legal/)

This created maintenance burden and code divergence from upstream.

## Solution: Git Subtree

Use the upstream H5Z-ZFP repository directly via **git subtree** instead of submodules.

### Why Subtree Over Submodule?

- ✅ **Better user experience**: No `git submodule update --init` required
- ✅ **Tarball releases work**: `git archive` includes everything
- ✅ **Simpler CI/CD**: No special checkout configuration needed
- ✅ **No detached HEAD issues**: Everything appears as normal files

See [SUBMODULE_VS_SUBTREE.md](SUBMODULE_VS_SUBTREE.md) for detailed comparison.

### Directory Structure (After Migration)

```
ZFP/
├── H5Z-ZFP/                    # Git submodule -> https://github.com/LLNL/H5Z-ZFP
│   ├── src/
│   ├── test/
│   ├── cmake/
│   ├── CMakeLists.txt
│   └── README.md
├── CMakeLists.txt              # Wrapper for hdf5_plugins integration
└── README.md                   # Documents submodule usage
```

## Implementation

### Phase 1: Preparation

1. **Identify HDF Group modifications**
   - Compare current `ZFP/src/` with upstream
   - Document any changes that should be contributed upstream
   - Open PRs at https://github.com/LLNL/H5Z-ZFP for any needed changes

2. **Verify upstream compatibility**
   - Confirm upstream H5Z-ZFP builds with current HDF5 versions
   - Test that upstream's CMake works with our integration approach

### Phase 2: Migration (COMPLETED)

1. **Remove current ZFP directory**
   ```bash
   git rm -rf ZFP
   git commit -m "Remove local ZFP copy"
   ```

2. **Add as subtree**
   ```bash
   git subtree add --prefix=ZFP/H5Z-ZFP \
       https://github.com/LLNL/H5Z-ZFP.git master --squash
   ```

3. **Create wrapper CMakeLists.txt**

   Created `ZFP/CMakeLists.txt`:
   ```cmake
   cmake_minimum_required(VERSION 3.18)

   # No submodule check needed - subtree contents are always present

   # H5Z-ZFP expects HDF5_ROOT and ZFP_ROOT (for the ZFP compression library)
   # These are set by the parent build system

   # Add upstream H5Z-ZFP
   add_subdirectory(H5Z-ZFP)
   ```

4. **Create ZFP/README.md**
   ```markdown
   # H5Z-ZFP Filter Plugin

   This directory integrates the H5Z-ZFP filter plugin as a git submodule.

   ## Upstream
   - Repository: https://github.com/LLNL/H5Z-ZFP
   - License: BSD 3-Clause
   - Maintainer: Mark C. Miller (LLNL)

   ## Building

   The submodule must be initialized:
   ```bash
   git submodule update --init --recursive
   ```

   ## Reporting Issues

   - Filter bugs/features: https://github.com/LLNL/H5Z-ZFP/issues
   - Integration issues: https://github.com/HDFGroup/hdf5_plugins/issues
   ```

### Phase 3: Update Root Build System

1. **Update root CMakeLists.txt**
   - Modify `FILTER_OPTION(ZFP)` macro usage if needed
   - Ensure ZFP library (compression) is still fetched via `H5PL_ALLOW_EXTERNAL_SUPPORT`

2. **Update config/CacheURLs.cmake**
   - Keep ZFP library URLs (for the compression library itself)
   - The filter plugin now comes from the submodule, not a tarball

3. **Update build documentation**
   - Add submodule initialization to build instructions
   - Update README.md and Building.txt

### Phase 4: Testing

1. **Verify builds work**
   - Test with CMake presets
   - Test standalone ZFP filter build
   - Test as part of full hdf5_plugins build

2. **Verify tests pass**
   - Run CTest for ZFP examples
   - Compare test results with previous local copy

## Updating the Subtree

To update to a newer H5Z-ZFP version:

```bash
# One-time: Add upstream remote (if not already done)
git remote add h5z-zfp https://github.com/LLNL/H5Z-ZFP.git

# Update to latest master
git subtree pull --prefix=ZFP/H5Z-ZFP h5z-zfp master --squash

# Update to specific version
git subtree pull --prefix=ZFP/H5Z-ZFP h5z-zfp v1.1.1 --squash
```

The `--squash` flag is recommended to avoid importing full upstream history.

## Cloning the Repository

Users can clone normally - no special steps needed:

```bash
git clone https://github.com/HDFGroup/hdf5_plugins.git
# Everything is ready to build!
```

This is a major advantage over submodules.

## Benefits

- No maintaining third-party code
- Automatic access to upstream bug fixes and features
- Clear separation between upstream code and HDF Group integration
- Changes to filter go upstream via PRs
- Easier version tracking and updates

## Considerations

### Tarball Releases

**Advantage**: Tarball releases automatically include subtree contents via `git archive`. No special handling needed!

### CI/CD

**Advantage**: No changes needed to GitHub Actions workflows. Standard checkout works:
```yaml
- uses: actions/checkout@v4
  # That's it - subtree contents are included
```

## Future: Extending to Other Filters

This same approach can be applied to other third-party filters:
- BLOSC / BLOSC2
- BSHUF (bitshuffle)
- LZ4, LZF, BZIP2, ZSTD

Each would follow the same pattern:
1. Remove local copy
2. Add upstream repository as subtree with `git subtree add --prefix=... --squash`
3. Create wrapper CMakeLists.txt for integration if needed
4. Update documentation
