# Git Submodule vs Subtree Analysis for H5Z-ZFP Integration

## Overview

This document compares git submodule and git subtree approaches for integrating upstream H5Z-ZFP into hdf5_plugins.

## Quick Comparison

| Aspect | Submodule | Subtree |
|--------|-----------|---------|
| **Upstream tracking** | Points to specific commit | Merges upstream history |
| **Clone simplicity** | Requires `--recursive` flag | Works with normal `git clone` |
| **User experience** | Must init submodules | Transparent to users |
| **Tarball releases** | Requires special handling | Contents included automatically |
| **Updating** | `git submodule update` | `git subtree pull` |
| **Repository size** | Small (pointer only) | Larger (full history) |
| **Separation** | Clear upstream/local boundary | Mixed history |
| **Maintenance** | Simple, well-documented | More complex commands |

## Git Subtree Advantages

### 1. Better User Experience

**Submodule:**
```bash
git clone https://github.com/HDFGroup/hdf5_plugins.git
cd hdf5_plugins
git submodule update --init --recursive  # Extra step required!
```

**Subtree:**
```bash
git clone https://github.com/HDFGroup/hdf5_plugins.git
cd hdf5_plugins
# Everything is already there, ready to build
```

### 2. Tarball Releases Just Work

- `git archive` automatically includes subtree contents
- No special packaging scripts needed
- Users downloading tarball get complete source

### 3. CI/CD Simplicity

**Submodule CI:**
```yaml
- uses: actions/checkout@v4
  with:
    submodules: recursive  # Must remember this
```

**Subtree CI:**
```yaml
- uses: actions/checkout@v4
  # That's it - everything is there
```

### 4. No "Detached HEAD" Confusion

Submodules often confuse developers with detached HEAD states. Subtrees avoid this entirely.

## Git Submodule Advantages

### 1. Clear Upstream Boundary

- Submodule directory is clearly "not ours"
- Prevents accidental local modifications
- Obvious that changes should go upstream

### 2. Smaller Repository

- Only stores commit pointers, not full upstream history
- hdf5_plugins repo stays smaller
- Faster clone for developers who work on many filters

### 3. Easier to Update

**Update to specific version:**
```bash
cd ZFP/H5Z-ZFP
git fetch
git checkout v1.1.1
cd ../..
git add ZFP/H5Z-ZFP
git commit -m "Update to H5Z-ZFP v1.1.1"
```

Clear and straightforward.

### 4. Standard in Open Source

More developers are familiar with submodules. Better documented and tooling support.

## Git Subtree Disadvantages

### 1. Complex Update Process

Updating upstream requires understanding subtree merge:

```bash
# Add remote if not already done
git remote add h5z-zfp https://github.com/LLNL/H5Z-ZFP.git

# Pull updates
git subtree pull --prefix=ZFP/H5Z-ZFP h5z-zfp master --squash

# Or without squash (preserves upstream history)
git subtree pull --prefix=ZFP/H5Z-ZFP h5z-zfp master
```

This is more error-prone and less intuitive.

### 2. Mixed History

If not using `--squash`, upstream commits appear in hdf5_plugins history, making it harder to distinguish HDF Group changes from upstream.

### 3. Harder to Contribute Back

If HDF Group accidentally makes changes in subtree:
- Harder to extract and send to upstream
- With submodule, you just push to a fork

### 4. Potential for Accidental Changes

Since subtree contents are "real" files, developers might accidentally modify upstream code without realizing it.

## Hybrid Approach: Subtree for Distribution, Submodule for Development?

Some projects use both:
- **Development**: Use submodule (developers need full control)
- **Release tarballs**: Generate with subtree contents included
- **Problem**: Maintaining two parallel systems is complex

## Recommendation for hdf5_plugins

### Use **Subtree** if:
- ✅ User experience is priority #1
- ✅ Many users download tarballs (not git clone)
- ✅ CI/CD simplicity is important
- ✅ HDF Group rarely needs to update to latest upstream
- ✅ You want "batteries included" distribution

### Use **Submodule** if:
- ✅ Clear upstream/local separation is important
- ✅ Developers frequently update to new upstream versions
- ✅ Want to prevent accidental upstream modifications
- ✅ Repository size matters
- ✅ Following standard open source patterns

## Specific Recommendation for H5Z-ZFP

**I recommend SUBTREE** for these reasons:

1. **HDF Group's use case**: You want to track upstream but don't need to update frequently. Updates happen when new ZFP versions are released, not daily.

2. **User base**: Many HDF5 users download release tarballs, not git repos. Subtree makes this seamless.

3. **Simplicity**: New contributors don't need to learn about submodule initialization.

4. **CI/CD**: No need to modify GitHub Actions to add `submodules: recursive`.

5. **Policy enforcement**: Can still maintain "upstream-first" policy through documentation and code review, even with subtree.

## Migration Path from Submodule to Subtree

If you want to switch:

```bash
# Remove submodule
git rm ZFP/H5Z-ZFP
git commit -m "Remove H5Z-ZFP submodule"

# Add as subtree
git subtree add --prefix=ZFP/H5Z-ZFP \
    https://github.com/LLNL/H5Z-ZFP.git master --squash

# Keep wrapper CMakeLists.txt and README.md in ZFP/
```

## Updating with Subtree

```bash
# One-time: Add remote for easier updates
git remote add h5z-zfp https://github.com/LLNL/H5Z-ZFP.git

# Update to latest
git subtree pull --prefix=ZFP/H5Z-ZFP h5z-zfp master --squash

# Update to specific version
git subtree pull --prefix=ZFP/H5Z-ZFP h5z-zfp v1.1.1 --squash
```

## Documentation for Subtree

In ZFP/README.md:

```markdown
## About This Directory

H5Z-ZFP is integrated using git subtree from:
https://github.com/LLNL/H5Z-ZFP

### Updating to Newer Versions

Maintainers only:

\`\`\`bash
git remote add h5z-zfp https://github.com/LLNL/H5Z-ZFP.git
git subtree pull --prefix=ZFP/H5Z-ZFP h5z-zfp <version-tag> --squash
\`\`\`

**Important**: Never modify files in ZFP/H5Z-ZFP/ directly.
Changes should be submitted to upstream.
```

## Conclusion

**Subtree is probably better for hdf5_plugins** because:
- Better user experience (no submodule init)
- Works seamlessly with tarball distribution
- Simpler CI/CD
- Updates are infrequent enough that complex update commands are acceptable

The trade-off is slightly more complex update procedure for maintainers, but this happens rarely (when new H5Z-ZFP versions are released).

Current submodule implementation works fine, but subtree would be more user-friendly.
