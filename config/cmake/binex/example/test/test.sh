#! /bin/sh
#
# Copyright by The HDF Group.
# Copyright by the Board of Trustees of the University of Illinois.
# All rights reserved.
#
# This file is part of HDF5.  The full HDF5 copyright notice, including
# terms governing use, modification, and redistribution, is contained in
# the files COPYING and Copyright.html.  COPYING can be found at the root
# of the source code distribution tree; Copyright.html can be found at the
# root level of an installed copy of the electronic HDF5 document set and
# is linked from the top-level documents page.  It can also be found at
# http://hdfgroup.org/HDF5/doc/Copyright.html.  If you do not have
# access to either file, you may request a copy from help@hdfgroup.org.

# This file is for use of h5cc created with the autotools process
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
*/*)    H5DUMP=`echo $H5CC | sed -e 's/\/[^/]*$/\/h5dump/'`;
        test -x $H5DUMP || H5DUMP=h5dump;
        H5REPACK=`echo $H5CC | sed -e 's/\/[^/]*$/\/h5repack/'`;
        test -x $H5REPACK || H5REPACK=h5repack;;
*)      H5DUMP=h5dump;
        H5REPACK=h5repack;;
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


$H5CC -shlib $srcdir/h5ex_d_blosc.c -o h5ex_d_blosc
$H5CC -shlib $srcdir/h5ex_d_bzip2.c -o h5ex_d_bzip2
$H5CC -shlib $srcdir/h5ex_d_lz4.c -o h5ex_d_lz4
$H5CC -shlib $srcdir/h5ex_d_lzf.c -o h5ex_d_lzf
$H5CC -shlib $srcdir/h5ex_d_mafisc.c -o h5ex_d_mafisc
$H5CC -shlib $srcdir/h5ex_d_zfp.c -o h5ex_d_zfp


SRC_TESTFILES="$srcdir/testfiles"
LIST_TEST_FILES="
$SRC_TESTFILES/h5repack_layout.h5
$SRC_TESTFILES/h5ex_d_blosc.ddl
$SRC_TESTFILES/h5ex_d_blosc.tst
$SRC_TESTFILES/h5ex_d_bzip2.ddl
$SRC_TESTFILES/h5ex_d_bzip2.tst
$SRC_TESTFILES/h5ex_d_lz4.ddl
$SRC_TESTFILES/h5ex_d_lz4.tst
$SRC_TESTFILES/h5ex_d_lzf.ddl
$SRC_TESTFILES/h5ex_d_lzf.tst
$SRC_TESTFILES/h5ex_d_mafisc.ddl
$SRC_TESTFILES/h5ex_d_mafisc.tst
$SRC_TESTFILES/h5ex_d_zfp.ddl
$SRC_TESTFILES/h5ex_d_zfp.tst
$SRC_TESTFILES/h5repack_layout.h5-ud_pl_convert.ddl
$SRC_TESTFILES/h5repack_layout.h5-ud_pl_lz4_convert.ddl
$SRC_TESTFILES/h5repack_layout.h5-ud_pl_blosc_convert.ddl
$SRC_TESTFILES/h5repack_layout.h5-ud_pl_bz2_convert.ddl
$SRC_TESTFILES/ud_convert.h5repack_layout.h5.tst
$SRC_TESTFILES/ud_bz2_convert.h5repack_layout.h5.tst
$SRC_TESTFILES/ud_blosc_convert.h5repack_layout.h5.tst
$SRC_TESTFILES/ud_lz4_convert.h5repack_layout.h5.tst
$SRC_TESTFILES/ud_mafisc_convert.h5repack_layout.h5.tst
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

DUMPTEST1() {
    expect="$1"
    actual="`basename $1 .ddl`.out"
    actual_err="`basename $1 .ddl`.err"
    actual_ext="`basename $1 .ddl`.ext"
    actual_sav=${actual}-sav
    shift

    # Run test.
    TESTING H5DUMP $@
    (
      $ENVCMD $H5DUMP --enable-error-stack -p "$@"
    ) >$actual 2>$actual_err
    cat $actual_err >> $actual

    # save actual and actual_err in case they are needed later.
    cp $actual $actual_sav
    STDOUT_FILTER $actual

    # Extract file name, line number, version and thread IDs because they may be different
    sed -e 's/{ \([0-9]\) \([-]*[0-9]*\) \([0-9] [0-9] [0-9] [0-9] [0-9] [0-9]\) }/{ \1 XXXX \3 }/' \
     $actual > $actual_ext

    CMP_OUTPUT $expect $actual_ext

    # Clean up output file
#    rm -f $actual $actual_err
}

#first in the sequence
REPACKTEST0()
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
        $ENVCMD $H5REPACK "$@" $infile "out-$outfile"
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

REPACKTEST1()
{
    infile=$1
    outfile=$2
    expect="$2.tst"
    actual="$2.out1"
    actual_ext="$2.ext"
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

    # Extract file name, line number, version and thread IDs because they may be different
    sed -e 's/PARAMS { \(\([0-9]\)\) \([-]?[0-9]*\) \(([0-9] [0-9] [0-9] [0-9] [0-9] [0-9])\) }/PARAMS { \1 XXXX \3 }/' \
     $actual > $actual_ext

     if [ $RET != 0 ] ; then
        echo "*FAILED*"
        nerrors="`expr $nerrors + 1`"
     else
        echo " PASSED"

        CMP_OUTPUT $expect $actual_ext
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

# same as EXETEST except mask compression number
EXETEST1() {
    fname=$1
    expect="$2"
    actual="`basename $2 .tst`.out1"
    actual_ext="`basename $1 .tst`.ext"
    actual_err="`basename $2 .tst`.err1"
    actual_sav=${actual}-sav

    # Run test.
    TESTING $fname
    (
      $ENVCMD `pwd`/$fname
    ) >$actual 2>$actual_err

    # save actual in case it is needed later.
    cp $actual $actual_sav
    STDOUT_FILTER $actual

    # Extract file name, line number, version and thread IDs because they may be different
    sed -e 's/PARAMS { \(\([0-9]\)\) \([-]?[0-9]*\) \(([0-9] [0-9] [0-9] [0-9] [0-9] [0-9])\) }/PARAMS { \1 XXXX \3 }/' \
     $actual > $actual_ext

    CMP_OUTPUT $expect $actual_ext
#    rm -f $actual $actual_err
}

################################
#########  THE TESTS   #########
################################

echo $ENVCMD

COPY_TESTFILES_TO_TESTDIR

EXETEST h5ex_d_blosc h5ex_d_blosc.tst
DUMPTEST h5ex_d_blosc.ddl h5ex_d_blosc.h5

EXETEST h5ex_d_bzip2 h5ex_d_bzip2.tst
DUMPTEST h5ex_d_bzip2.ddl h5ex_d_bzip2.h5

EXETEST h5ex_d_lz4 h5ex_d_lz4.tst
DUMPTEST h5ex_d_lz4.ddl h5ex_d_lz4.h5

EXETEST h5ex_d_lzf h5ex_d_lzf.tst
DUMPTEST h5ex_d_lzf.ddl h5ex_d_lzf.h5

EXETEST h5ex_d_mafisc h5ex_d_mafisc.tst
DUMPTEST1 h5ex_d_mafisc.ddl h5ex_d_mafisc.h5

EXETEST h5ex_d_zfp h5ex_d_zfp.tst
DUMPTEST h5ex_d_zfp.ddl h5ex_d_zfp.h5

REPACKTEST0 h5repack_layout.h5 ud_bz2_convert.h5repack_layout.h5 --enable-error-stack -v -f /dset1:UD=307,0,1,9
DUMPTEST h5repack_layout.h5-ud_pl_bz2_convert.ddl --enable-error-stack -pH out-ud_bz2_convert.h5repack_layout.h5

REPACKTEST ud_bz2_convert.h5repack_layout.h5 ud_blosc_convert.h5repack_layout.h5 --enable-error-stack -v -f /dset2:UD=32001,0,7,0,0,0,0,5,1,0
DUMPTEST h5repack_layout.h5-ud_pl_blosc_convert.ddl --enable-error-stack -pH out-ud_blosc_convert.h5repack_layout.h5

REPACKTEST ud_blosc_convert.h5repack_layout.h5 ud_lz4_convert.h5repack_layout.h5 --enable-error-stack -v -f /dset3:UD=32004,0,1,3 -l /dset1:CHUNK=4x8
DUMPTEST h5repack_layout.h5-ud_pl_lz4_convert.ddl --enable-error-stack -pH out-ud_lz4_convert.h5repack_layout.h5

REPACKTEST1 ud_lz4_convert.h5repack_layout.h5 ud_mafisc_convert.h5repack_layout.h5 --enable-error-stack -v -f /dset4:UD=32013,1,0,0 -l CHUNK=10x10
DUMPTEST h5repack_layout.h5-ud_pl_convert.ddl --enable-error-stack -pH out-ud_mafisc_convert.h5repack_layout.h5

#rm -f ud_convert.h5repack_layout.h5

echo "$nerrors tests failed in example"
exit $nerrors
