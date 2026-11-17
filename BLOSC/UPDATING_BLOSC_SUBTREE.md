# Updating the Blosc Subtree

This document explains how to update the Blosc filter plugin (filter ID 32001) to a newer upstream version.

## Overview

The Blosc filter is integrated using **git subtree**, which embeds the upstream Blosc/hdf5-blosc repository
directly into `BLOSC/hdf5-blosc/`. The wrapper `BLOSC/src/CMakeLists.txt` compiles only the filter sources
(`BLOSC/hdf5-blosc/src/blosc_filter.c` and `BLOSC/hdf5-blosc/src/blosc_plugin.c`) against the c-blosc library that hdf5_plugins builds or finds.
The upstream `CMakeLists.txt` downloads c-blosc at build time and is not used.

## Automated Update Check

The `update-subtrees.yml` workflow runs every Monday and can be started by hand. When a new commit on hdf5-blosc `master` changes `src/` or `LICENSES/`,
it runs steps 2 to 4 below on a `subtree/` branch and opens a pull request that lists the upstream
changes. Continue from step 5 on that branch. CI runs on the pull request only when the
`SUBTREE_UPDATE_TOKEN` secret is set; otherwise close and reopen the pull request to start it.

## Prerequisites

1. A clean working tree: `git status` shows no uncommitted changes
2. A feature branch for the update

## Step-by-Step Update Process

### 1. Add the Upstream Remote (One-Time Setup)

```bash
git remote add hdf5-blosc https://github.com/Blosc/hdf5-blosc
```

### 2. Fetch Upstream Changes

```bash
git fetch hdf5-blosc --tags --force
```

### 3. Choose the Version

```bash
git ls-remote --tags hdf5-blosc
git log --oneline <current-split-commit>..hdf5-blosc/master -- src
```

Prefer a release tag. The subtree was first added from `master` (19c34cf), because the latest tag at the time (v1.0.1) predated fixes in `master`; if upstream has not tagged a newer release, review the commits before pulling an untagged commit.

### 4. Pull the Update

```bash
git subtree pull --prefix=BLOSC/hdf5-blosc hdf5-blosc <tag-or-commit> --squash -m "Update Blosc subtree to <version>"
```

Compare the `git-subtree-split:` line of the squash commit with the upstream commit you intended.

### 5. Review What Changed Upstream

```bash
git diff HEAD~2 HEAD -- BLOSC/hdf5-blosc/src
```

Check in particular for:
- changes to the filter name, defaults, or `cd_values` layout (these change test reference output)
- new source files that must be added to `BLOSC/src/CMakeLists.txt` and `BLOSC/src/Makefile.am`
- changes to the license files listed below

### 6. Test the Build

```bash
export HDF5_ROOT=/path/to/hdf5
cmake --preset ci-StdShar-GNUC -B build-blosc-update -DHDF5_ROOT=$HDF5_ROOT
cmake --build build-blosc-update
cd build-blosc-update && ctest -R "BLOSC_|blosc$|blosc-"
```

If a test fails only because the expected output changed (for example, the compression ratio or
the filter name), update the reference files in `BLOSC/example/testfiles/`,
`BLOSC/config/cmake/binex/example/testfiles/`, and `config/cmake/binex/example/testfiles/`.
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
git log --grep="git-subtree-dir: BLOSC/hdf5-blosc" --format="%H %s%n%b" -1
```

The commit after `git-subtree-split:` is the upstream commit currently in the tree.

### Merge Conflicts

Conflicts only happen if files in `BLOSC/hdf5-blosc/` were edited locally, which the policy below forbids.
Resolve them in favor of upstream and send any needed change upstream instead.

## Update Checklist

- [ ] Fetched the upstream remote and chose a tag or reviewed commit
- [ ] Pulled with `--squash`; `git-subtree-split:` names the intended upstream commit
- [ ] Reviewed upstream changes to `BLOSC/hdf5-blosc/src`
- [ ] Build succeeds with `-DENABLE_BLOSC=ON`
- [ ] `ctest -R "BLOSC_|blosc$|blosc-"` passes, and the installed `HDFPLExamples` tests pass
- [ ] License notices installed from `BLOSC/hdf5-blosc` (`LICENSES/BLOSC_HDF5.txt`, `LICENSES/BLOSC.txt`, `LICENSES/H5PY.txt`) still exist at those paths
- [ ] `BLOSC/README.md` names the new upstream version
- [ ] Pull request will be merged without squashing

## Policy Reminder

**Never modify files in `BLOSC/hdf5-blosc/` directly!**

To change the filter:
1. Open an issue at https://github.com/Blosc/hdf5-blosc/issues
2. Submit a pull request upstream
3. After upstream merges, use `git subtree pull` to update

## Additional Resources

- Upstream repository: https://github.com/Blosc/hdf5-blosc
- Git subtree documentation: `man git-subtree` or https://git-scm.com/docs/git-subtree
