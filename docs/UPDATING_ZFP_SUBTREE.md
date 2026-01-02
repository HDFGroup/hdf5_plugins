# Updating the ZFP Subtree

This document explains how to update the H5Z-ZFP filter plugin to a newer upstream version.

## Overview

The ZFP filter is integrated using **git subtree**, which embeds the upstream LLNL/H5Z-ZFP repository directly into `ZFP/H5Z-ZFP/`. This allows users to clone and build without any additional setup while maintaining a clear connection to upstream.

## Prerequisites

Before updating, ensure:
1. You have a clean working tree: `git status` shows no uncommitted changes
2. You're on the appropriate branch (usually `master` or a feature branch)
3. You have push access to the hdf5_plugins repository

## Step-by-Step Update Process

### 1. Add Upstream Remote (One-Time Setup)

If you haven't already added the upstream H5Z-ZFP remote:

```bash
git remote add h5z-zfp https://github.com/LLNL/H5Z-ZFP.git
```

Verify the remote was added:

```bash
git remote -v
```

You should see `h5z-zfp` pointing to the LLNL repository.

### 2. Fetch Upstream Changes

Before pulling, fetch the latest upstream changes:

```bash
git fetch h5z-zfp
```

### 3. Check Available Versions

List available upstream tags to see what versions are available:

```bash
git ls-remote --tags h5z-zfp
```

### 4. Pull the Update

#### Option A: Update to Latest Master

To update to the latest development version:

```bash
git subtree pull --prefix=ZFP/H5Z-ZFP h5z-zfp master --squash -m "Update H5Z-ZFP to latest upstream master"
```

#### Option B: Update to Specific Version Tag

To update to a specific release (recommended for stability):

```bash
git subtree pull --prefix=ZFP/H5Z-ZFP h5z-zfp v1.1.1 --squash -m "Update H5Z-ZFP to v1.1.1"
```

Replace `v1.1.1` with the desired version tag.

### 5. Verify the Update

After pulling, verify the update was successful:

```bash
# Check that ZFP/H5Z-ZFP directory contains the new code
ls -la ZFP/H5Z-ZFP/

# View the commit created by subtree pull
git log -1

# Check git status
git status
```

### 6. Test the Build

Before pushing, ensure the updated filter builds correctly:

```bash
# Set HDF5_ROOT to your HDF5 installation
export HDF5_ROOT=/path/to/hdf5

# Create a clean build directory
rm -rf build
mkdir build && cd build

# Configure with ZFP enabled
cmake -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_ZFP=ON \
  -DH5PL_ALLOW_EXTERNAL_SUPPORT=TGZ \
  -DH5PL_TGZPATH=../libs \
  ..

# Build
cmake --build .

# Run tests
ctest -R ZFP
```

### 7. Commit and Push

If the build and tests pass:

```bash
# The subtree pull already created a commit, so just push
git push origin <your-branch>
```

## Understanding the --squash Flag

The `--squash` flag is **recommended** for subtree updates. Here's why:

- **Without --squash**: Imports the entire upstream commit history into hdf5_plugins
  - Pro: Preserves full upstream history
  - Con: Makes hdf5_plugins history cluttered with upstream commits

- **With --squash**: Combines all upstream changes into a single commit
  - Pro: Keeps hdf5_plugins history clean
  - Con: Loses individual upstream commit details (but you can still view them in the upstream repo)

**Recommendation**: Always use `--squash` unless you have a specific reason to preserve full upstream history.

## Troubleshooting

### Merge Conflicts

If you encounter merge conflicts during `git subtree pull`:

1. **Resolve conflicts**: Edit the conflicting files in `ZFP/H5Z-ZFP/`
2. **Stage resolved files**: `git add ZFP/H5Z-ZFP/<conflicted-file>`
3. **Complete the merge**: `git commit`

**Note**: Conflicts are rare if you never modify files in `ZFP/H5Z-ZFP/` directly (which is the policy).

### Checking Current Upstream Version

To see what upstream commit the current subtree is based on:

```bash
git log --grep="git-subtree-split" --format="%H %s" -1
```

Look for the commit hash after `git-subtree-split:` in the commit message.

### Reverting a Failed Update

If an update fails or causes issues:

```bash
# Find the commit hash before the subtree pull
git log --oneline -5

# Reset to before the update
git reset --hard <commit-before-update>
```

## Update Checklist

Before pushing an H5Z-ZFP update to the main repository:

- [ ] Upstream remote is configured correctly
- [ ] Fetched latest upstream changes
- [ ] Pulled the desired version using `--squash`
- [ ] Verified `ZFP/H5Z-ZFP/` contains expected changes
- [ ] Build succeeds with `-DENABLE_ZFP=ON`
- [ ] ZFP tests pass: `ctest -R ZFP`
- [ ] Commit message clearly indicates version/branch updated
- [ ] Ready to push to hdf5_plugins repository

## Policy Reminder

**Never modify files in `ZFP/H5Z-ZFP/` directly!**

If you need changes to the H5Z-ZFP filter:
1. Open an issue at https://github.com/LLNL/H5Z-ZFP/issues
2. Submit a pull request to upstream
3. After upstream merges, use `git subtree pull` to update

This ensures:
- The HDF community benefits from improvements
- Upstream maintainers remain authoritative
- Updates remain straightforward without merge conflicts

## Additional Resources

- H5Z-ZFP Documentation: https://h5z-zfp.readthedocs.io
- H5Z-ZFP Repository: https://github.com/LLNL/H5Z-ZFP
- Git Subtree Documentation: `man git-subtree` or https://git-scm.com/docs/git-subtree
