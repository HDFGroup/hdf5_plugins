# Updating the SZ3 Subtree

This document explains how to update the H5Z-SZ3 filter plugin (SZ3) to a newer upstream version.

## Overview

The SZ3 filter is integrated using **git subtree**, which embeds the upstream szcompressor/SZ3 repository directly into `SZ3/SZ3/`. The filter itself is SZ3's `tools/H5Z-SZ3`, built by SZ3's own CMake from the wrapper `SZ3/CMakeLists.txt`. This allows users to clone and build without any additional setup while maintaining a clear connection to upstream.

## Prerequisites

Before updating, ensure:
1. You have a clean working tree: `git status` shows no uncommitted changes
2. You're on the appropriate branch (usually `master` or a feature branch)
3. You have push access to the hdf5_plugins repository

## Step-by-Step Update Process

### 1. Add Upstream Remote (One-Time Setup)

If you haven't already added the upstream SZ3 remote:

```bash
git remote add sz3 https://github.com/szcompressor/SZ3.git
```

Verify the remote was added:

```bash
git remote -v
```

You should see `sz3` pointing to the szcompressor repository.

### 2. Fetch Upstream Changes

Before pulling, fetch the latest upstream changes:

```bash
git fetch sz3 --tags --force
```

### 3. Check Available Versions

List available upstream tags to see what versions are available:

```bash
git ls-remote --tags sz3
```

### 4. Pull the Update

Update to a release tag (recommended for stability):

```bash
git subtree pull --prefix=SZ3/SZ3 sz3 v3.4.1 --squash -m "Update SZ3 to v3.4.1"
```

Replace `v3.4.1` with the desired version tag. Check that the commit the tag points to (`git ls-remote --tags sz3 v3.4.1`) is the one you expect, and
compare it with the `git-subtree-split:` line of the squash commit after the pull.

### 5. Verify the Update

After pulling, verify the update was successful:

```bash
# Check that SZ3/SZ3 directory contains the new code
ls -la SZ3/SZ3/

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

# Configure with SZ3 enabled (no external libraries are needed)
cmake -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_SZ3=ON \
  -DH5PL_ALLOW_EXTERNAL_SUPPORT=NO \
  -DH5PL_BUILD_TESTING=ON \
  ..

# Build
cmake --build .

# Run tests
ctest -R "SZ3|sz3"
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

1. **Resolve conflicts**: Edit the conflicting files in `SZ3/SZ3/`
2. **Stage resolved files**: `git add SZ3/SZ3/<conflicted-file>`
3. **Complete the merge**: `git commit`

**Note**: Conflicts are rare if you never modify files in `SZ3/SZ3/` directly (which is the policy).

### Checking Current Upstream Version

To see what upstream commit the current subtree is based on:

```bash
git log --grep="git-subtree-dir: SZ3/SZ3" --format="%H %s%n%b" -1
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

Before pushing an SZ3 update to the main repository:

- [ ] Upstream remote is configured correctly
- [ ] Fetched latest upstream changes
- [ ] Pulled the desired version using `--squash`
- [ ] Verified `SZ3/SZ3/` contains expected changes
- [ ] `git-subtree-split:` names the upstream commit of the intended tag
- [ ] License notices in `SZ3/Additional_Legal/` still match `SZ3/SZ3/copyright-and-BSD-license.txt` and `SZ3/SZ3/tools/zstd/LICENSE`
- [ ] `docs/PluginLibraries.txt` names the new SZ3 version
- [ ] Build succeeds with `-DENABLE_SZ3=ON`
- [ ] SZ3 tests pass: `ctest -R "SZ3|sz3"`
- [ ] Commit message clearly indicates version/branch updated
- [ ] Ready to push to hdf5_plugins repository

## Policy Reminder

**Never modify files in `SZ3/SZ3/` directly!**

If you need changes to the H5Z-SZ3 filter or to SZ3's build:
1. Open an issue at https://github.com/szcompressor/SZ3/issues
2. Submit a pull request to upstream
3. After upstream merges, use `git subtree pull` to update

This ensures:
- The HDF community benefits from improvements
- Upstream maintainers remain authoritative
- Updates remain straightforward without merge conflicts

## Additional Resources

- H5Z-SZ3 Documentation: https://github.com/szcompressor/SZ3/tree/master/tools/H5Z-SZ3
- SZ3 Repository: https://github.com/szcompressor/SZ3
- Git Subtree Documentation: `man git-subtree` or https://git-scm.com/docs/git-subtree
