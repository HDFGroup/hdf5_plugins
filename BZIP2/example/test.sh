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


ENVCMD="env HDF5_PLUGIN_PATH=`pwd`/../src/.libs"

case $CC in
*/*)    H5DUMP=`echo $CC | sed -e 's/\/[^/]*$/\/h5dump/'`;
        test -x $H5DUMP || H5DUMP=h5dump;;
*)      H5DUMP=h5dump;;
esac


case `echo "testing\c"; echo 1,2,3`,`echo -n testing; echo 1,2,3` in
  *c*,-n*) ECHO_N= ECHO_C='
' ;;
  *c*,*  ) ECHO_N=-n ECHO_C= ;;
  *)       ECHO_N= ECHO_C='\c' ;;
esac
ECHO_N="echo $ECHO_N"


exout() {
    echo '*******************************'
    echo '*  Output of example program  *'
    echo '*******************************'
    echo
    $ENVCMD $*
}

dumpout() {
    echo '**********************'
    echo '*  Output of h5dump  *'
    echo '**********************'
    echo
    $H5DUMP $*
}


topics="bzip2"


return_val=0

for topic in $topics
do
    fname=h5ex_d_$topic
    $ECHO_N "Testing example/$fname...$ECHO_C"
    exout ./$fname >tmp.test
    status=$?
    if test $status -eq 1
    then
        echo "  Unsupported feature"
    else
        cmp -s tmp.test $srcdir/testfiles/$fname.tst
        status=$?
        if test $status -ne 0
        then
            echo "  FAILED!"
        else
        dumpout $fname.h5 >tmp.out
        # rm -f $fname.h5
        cmp -s tmp.out $srcdir/testfiles/$fname.ddl
          status=$?
          if test $status -ne 0
          then
              echo "  FAILED!"
          else
              echo "  Passed"
          fi
        fi
        return_val=`expr $status + $return_val`
    fi
done


rm -f tmp.test tmp.out *.h5
echo "$return_val tests failed in example/"
exit $return_val
