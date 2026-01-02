#!/bin/sh
git remote add upstream https://github.com/HDFGroup/hdf5_plugins.git
git fetch upstream
git rebase -Xtheirs upstream/master
