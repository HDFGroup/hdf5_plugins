#! /bin/sh
#
# Copyright by The HDF Group.
# All rights reserved.
#
# This file is part of HDF5.  The full HDF5 copyright notice, including
# terms governing use, modification, and redistribution, is contained in
# the COPYING file, which can be found at the root of the source code
# distribution tree, or in https://www.hdfgroup.org/licenses.
# If you do not have access to either file, you may request a copy from
# help@hdfgroup.org.

# This file is for use of h5cc created with the CMake process
# HDF5_HOME is expected to be set

srcdir=..
builddir=.
verbose=yes
nerrors=0

# HDF5 compile commands, assuming they are in your $PATH.
H5CC=$HDF5_HOME/bin/h5cc
LD_LIBRARY_PATH=$HDF5_HOME/lib
export LD_LIBRARY_PATH

if ! test -f $H5CC; then
    echo "Set paths for H5CC and LD_LIBRARY_PATH in test.sh"
    echo "Set environment variable HDF5_HOME to the hdf5 install dir"
    echo "h5cc was not found at $H5CC"
    exit $EXIT_FAILURE
fi

case $H5CC in
*/*)    H5DUMP=`echo $H5CC | sed -e 's/\/[^/]*$/\/h5dump-shared/'`;
        test -x $H5DUMP || H5DUMP=h5dump-shared;
        H5REPACK=`echo $H5CC | sed -e 's/\/[^/]*$/\/h5repack-shared/'`;
        test -x $H5REPACK || H5REPACK=h5repack-shared;;
*)      H5DUMP=h5dump-shared;
        H5REPACK=h5repack-shared;;
esac

# Shell commands used in Makefiles
RM="rm -rf"
DIFF="diff -c"
CMP="cmp -s"
GREP='grep'
CP="cp -p"  # Use -p to preserve mode,ownership,timestamps
DIRNAME='dirname'
LS='ls'
AWK='awk'

# setup plugin path
ENVCMD="env HDF5_PLUGIN_PATH=$LD_LIBRARY_PATH/plugin"

TESTDIR=$builddir


$H5CC $srcdir/h5ex_d_lzf.c -o h5ex_d_lzf

SRC_TESTFILES="$srcdir/testfiles"
LIST_TEST_FILES="
$SRC_TESTFILES/h5repack_layout.h5
$SRC_TESTFILES/h5ex_d_lzf.ddl
$SRC_TESTFILES/h5ex_d_lzf.tst
$SRC_TESTFILES/h5repack_layout.h5-ud_convert.ddl
$SRC_TESTFILES/ud_convert.h5repack_layout.h5.tst
"

#
# copy test files and expected output files from source dirs to test dir
#
COPY_TESTFILES="$LIST_TEST_FILES"

COPY_TESTFILES_TO_TESTDIR()
{
    # copy test files. Used -f to make sure get a new copy
    for tstfile in $COPY_TESTFILES
    do
        # ignore '#' comment
        echo $tstfile | tr -d ' ' | grep '^#' > /dev/null
        RET=$?
        if [ $RET -eq 1 ]; then
            # skip cp if srcdir is same as destdir
            # this occurs when build/test performed in source dir and
            # make cp fail
            SDIR=`$DIRNAME $tstfile`
            INODE_SDIR=`$LS -i -d $SDIR | $AWK -F' ' '{print $1}'`
            INODE_DDIR=`$LS -i -d $TESTDIR | $AWK -F' ' '{print $1}'`
            if [ "$INODE_SDIR" != "$INODE_DDIR" ]; then
                $CP -f $tstfile $TESTDIR
                if [ $? -ne 0 ]; then
                    echo "Error: FAILED to copy $tstfile ."

                    # Comment out this to CREATE expected file
                    exit $EXIT_FAILURE
                fi
            fi
        fi
    done
}

# Print a $* message left justified in a field of 70 characters
#
MESSAGE() {
   SPACES="                                                               "
   echo "$* $SPACES" | cut -c1-70 | tr -d '\012'
}

# Print a line-line message left justified in a field of 70 characters
# beginning with the word "Testing".
#
TESTING() {
    SPACES="                                                               "
    echo "Testing $* $SPACES" | cut -c1-70 | tr -d '\012'
}

# Print a line-line message left justified in a field of 70 characters
# beginning with the word "Verifying".
#
VERIFY() {
    MESSAGE "Verifying $*"
}

# This is different from $srcdir/../../bin/output_filter.sh
STDOUT_FILTER() {
    result_file=$1
    tmp_file=/tmp/h5test_tmp_$$
    # Filter name of files.
    cp $result_file $tmp_file
    sed -e '/^Opening file/d' -e '/^Making file/d' \
    < $tmp_file > $result_file
    # cleanup
    rm -f $tmp_file
}

# Compare the two text files
# PASS if same
# FAIL if different, and show the diff
#
# Assumed arguments:
# $1 is text file1 (expected output)
# $2 is text file2 (actual output)
CMP_OUTPUT()
{
    expect=$1
    actual=$2

    VERIFY $@
    if [ ! -f $expect ]; then
        # Create the expect file if it doesn't yet exist.
        echo " Expected Missing"
    elif $CMP $expect $actual; then
        echo " PASSED"
    else
        echo "*FAILED*"
        echo "    Expected output differs from actual output"
        nerrors="`expr $nerrors + 1`"
        test yes = "$verbose" && $DIFF $expect $actual |sed 's/^/    /'
    fi
}


# Run a test and print PASS or *FAIL*.  If a test fails then increment
# the `nerrors' global variable and (if $verbose is set) display the
# difference between the actual output and the expected output. The
# expected output is given as the first argument to this function and
# the actual output file is calculated by replacing the `.ddl' with
# `.out'.  The actual output is not removed if $HDF5_NOCLEANUP has a
# non-zero value.
# ADD_H5_TEST
DUMPTEST() {
    expect="$1"
    actual="`basename $1 .ddl`.out"
    actual_err="`basename $1 .ddl`.err"
    shift

    # Run test.
    TESTING $DUMPER $@
    (
      $ENVCMD $H5DUMP --enable-error-stack -p "$@"
    ) >$actual 2>$actual_err
    cat $actual_err >> $actual

    CMP_OUTPUT $expect $actual

    # Clean up output file
#    rm -f $actual $actual_err

}

REPACKTEST()
{
    infile=$1
    outfile=$2
    expect="$2.tst"
    actual="$2.out1"
    actual_err="$2.err1"
    shift
    shift

    # Run test.
    TESTING H5REPACK $@
    (
        $ENVCMD $H5REPACK "$@" "out-$infile" "out-$outfile"
    ) >$actual 2>$actual_err
    RET=$?
    STDOUT_FILTER $actual
    cat $actual_err >> $actual

     if [ $RET != 0 ] ; then
        echo "*FAILED*"
        nerrors="`expr $nerrors + 1`"
    else
        echo " PASSED"

        CMP_OUTPUT $expect $actual
    fi
#    rm -f $actual $actual_err
}


EXETEST() {
    fname=$1
    expect="$2"
    actual="`basename $2 .tst`.out1"
    actual_err="`basename $2 .tst`.err1"

    # Run test.
    TESTING $fname
    (
      $ENVCMD `pwd`/$fname
    ) >$actual 2>$actual_err
    cat $actual_err >> $actual

    CMP_OUTPUT $expect $actual
#    rm -f $actual $actual_err
}

################################
#########  THE TESTS   #########
################################

echo $ENVCMD

COPY_TESTFILES_TO_TESTDIR

EXETEST h5ex_d_lzf h5ex_d_lzf.tst

DUMPTEST h5ex_d_lzf.ddl h5ex_d_lzf.h5

REPACKTEST h5repack_layout.h5 out-ud_convert.h5repack_layout.h5 -v -f UD=32000,0,3,0,0,0 -l CHUNK=4x8

DUMPTEST h5repack_layout.h5-ud_convert.ddl -pH out-ud_convert.h5repack_layout.h5

#rm -f ud_convert.h5repack_layout.h5

echo "$nerrors tests failed in example"
exit $nerrors
