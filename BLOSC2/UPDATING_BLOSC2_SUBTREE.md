# Updating the Blosc2 Subtree

This document explains how to update the Blosc2 filter plugin (filter ID 32026) to a newer upstream version.

## Overview

The Blosc2 filter is integrated using **git subtree**, which embeds the upstream Blosc/HDF5-Blosc2 repository
directly into `BLOSC2/HDF5-Blosc2/`. The wrapper `BLOSC2/src/CMakeLists.txt` compiles only the filter sources
(`BLOSC2/HDF5-Blosc2/src/blosc2_filter.c` and `BLOSC2/HDF5-Blosc2/src/blosc2_plugin.c`) against the c-blosc2 library that hdf5_plugins builds or finds.
The upstream `CMakeLists.txt` downloads c-blosc2 at build time and is not used.

## Automated Update Check

The `update-subtrees.yml` workflow runs every Monday and can be started by hand. When HDF5-Blosc2 publishes a new release tag,
it runs steps 2 to 4 below on a `subtree/` branch and opens a pull request that lists the upstream
changes. Continue from step 5 on that branch. CI runs on the pull request only when the
`SUBTREE_UPDATE_TOKEN` secret is set; otherwise close and reopen the pull request to start it.

## Prerequisites

1. A clean working tree: `git status` shows no uncommitted changes
2. A feature branch for the update

## Step-by-Step Update Process

### 1. Add the Upstream Remote (One-Time Setup)

```bash
git remote add hdf5-blosc2 https://github.com/Blosc/HDF5-Blosc2
```

### 2. Fetch Upstream Changes

```bash
git fetch hdf5-blosc2 --tags --force
```

### 3. Choose the Version

```bash
git ls-remote --tags hdf5-blosc2
git log --oneline <current-split-commit>..hdf5-blosc2/main -- src
```

Prefer a release tag. The subtree was first added from tag v3.0.1.

### 4. Pull the Update

```bash
git subtree pull --prefix=BLOSC2/HDF5-Blosc2 hdf5-blosc2 <tag-or-commit> --squash -m "Update Blosc2 subtree to <version>"
```

Compare the `git-subtree-split:` line of the squash commit with the upstream commit you intended.

### 5. Review What Changed Upstream

```bash
git diff HEAD~2 HEAD -- BLOSC2/HDF5-Blosc2/src
```

Check in particular for:
- changes to the filter name, defaults, or `cd_values` layout (these change test reference output)
- new source files that must be added to `BLOSC2/src/CMakeLists.txt` and `BLOSC2/src/Makefile.am`
- changes to the license files listed below

### 6. Test the Build

```bash
export HDF5_ROOT=/path/to/hdf5
cmake --preset ci-StdShar-GNUC -B build-blosc2-update -DHDF5_ROOT=$HDF5_ROOT
cmake --build build-blosc2-update
cd build-blosc2-update && ctest -R "BLOSC2|blosc2"
```

If a test fails only because the expected output changed (for example, the compression ratio or
the filter name), update the reference files in `BLOSC2/example/testfiles/`,
`BLOSC2/config/cmake/binex/example/testfiles/`, and `config/cmake/binex/example/testfiles/`.
Do not edit references for files shipped in `testfiles/`: they keep the filter name they were
written with. Also run the installed examples (`HDFPLExamples`) from an install of the full build.

### 7. Commit and Push

The subtree pull already created a commit. Add any reference or build updates in a separate
commit, then push the branch and open a pull request.

**Merge the pull request with a merge commit (or rebase), not a squash merge.** A squash merge
drops the subtree's `git-subtree-dir:` and `git-subtree-split:` metadata, and later
`git subtree pull` commands then fail.

## Troubleshooting

### Checking the Current Upstream Version

```bash
git log --grep="git-subtree-dir: BLOSC2/HDF5-Blosc2" --format="%H %s%n%b" -1
```

The commit after `git-subtree-split:` is the upstream commit currently in the tree.

### Merge Conflicts

Conflicts only happen if files in `BLOSC2/HDF5-Blosc2/` were edited locally, which the policy below forbids.
Resolve them in favor of upstream and send any needed change upstream instead.

## Update Checklist

- [ ] Fetched the upstream remote and chose a tag or reviewed commit
- [ ] Pulled with `--squash`; `git-subtree-split:` names the intended upstream commit
- [ ] Reviewed upstream changes to `BLOSC2/HDF5-Blosc2/src`
- [ ] Build succeeds with `-DENABLE_BLOSC2=ON`
- [ ] `ctest -R "BLOSC2|blosc2"` passes, and the installed `HDFPLExamples` tests pass
- [ ] License notices installed from `BLOSC2/HDF5-Blosc2` (`LICENSE.txt`, `LICENSES/BLOSC.txt`, `LICENSES/H5PY.txt`) still exist at those paths
- [ ] `BLOSC2/README.md` names the new upstream version
- [ ] Pull request will be merged without squashing

## Policy Reminder

**Never modify files in `BLOSC2/HDF5-Blosc2/` directly!**

To change the filter:
1. Open an issue at https://github.com/Blosc/HDF5-Blosc2/issues
2. Submit a pull request upstream
3. After upstream merges, use `git subtree pull` to update

## Additional Resources

- Upstream repository: https://github.com/Blosc/HDF5-Blosc2
- Git subtree documentation: `man git-subtree` or https://git-scm.com/docs/git-subtree
