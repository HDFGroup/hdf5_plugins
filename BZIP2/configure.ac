##                                               -*- Autoconf -*-
## Process this file with autoconf to produce a configure script.
##
## Copyright by The HDF Group.
## Copyright by the Board of Trustees of the University of Illinois.
## All rights reserved.
##
## This file is part of HDF5.  The full HDF5 copyright notice, including
## terms governing use, modification, and redistribution, is contained in
## the files COPYING and Copyright.html.  COPYING can be found at the root
## of the source code distribution tree; Copyright.html can be found at the
## root level of an installed copy of the electronic HDF5 document set and
## is linked from the top-level documents page.  It can also be found at
## http://hdfgroup.org/HDF5/doc/Copyright.html.  If you do not have
## access to either file, you may request a copy from help@hdfgroup.org.

AC_PREREQ(2.59)
AC_INIT(H5BZ2, 0.1, help@hdfgroup.org)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([bin])
## AM_INIT_AUTOMAKE takes a list of options that should be applied to
## every Makefile.am when automake is run.
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER(config.h)

## Turn off automake rebuild rules so make doesn't try to run automake
## (which probably won't work).
AM_MAINTAINER_MODE

AC_CANONICAL_HOST
AC_PREFIX_DEFAULT([`pwd`/plugins])

LT_INIT

## Source any special files that we need. These files normally aren't
## present but can be used by the maintainers to fine tune things like
## turning on debug or profiling flags for the compiler. The search order
## is:
##
##         CPU-VENDOR-OS
##         VENDOR-OS
##         CPU-OS
##         CPU-VENDOR
##         OS
##         VENDOR
##         CPU
##
## If the `OS' ends with a version number then remove it. For instance,
## `freebsd3.1' would become `freebsd'
##
case "$host_os" in
  aix4.*)       host_os_novers="aix4.x"     ;;
  aix5.*)       host_os_novers="aix5.x"     ;;
  freebsd*)     host_os_novers="freebsd"    ;;
  irix5.*)      host_os_novers="irix5.x"    ;;
  irix6.*)      host_os_novers="irix6.x"    ;;
  osf4.*)       host_os_novers="osf4.x"     ;;
  osf5.*)       host_os_novers="osf5.x"     ;;
  solaris2.*)   host_os_novers="solaris2.x" ;;
  *)            host_os_novers="$host_os"   ;;
esac

host_config="none"
for f in $host_cpu-$host_vendor-$host_os        \
         $host_cpu-$host_vendor-$host_os_novers \
         $host_vendor-$host_os                  \
         $host_vendor-$host_os_novers           \
         $host_cpu-$host_os                     \
         $host_cpu-$host_os_novers              \
         $host_cpu-$host_vendor                 \
         $host_os                               \
         $host_os_novers                        \
         $host_vendor                           \
         $host_cpu ; do
  AC_MSG_CHECKING([for config $f])
  if test -f "$srcdir/config/$f"; then
    host_config=$srcdir/config/$f
    AC_MSG_RESULT([found])
    break
  fi
  AC_MSG_RESULT([no])
done
if test "X$host_config" != "Xnone"; then
  CC_BASENAME="`echo $CC | cut -f1 -d' ' | xargs basename 2>/dev/null`"
  . $host_config
fi

## Argument for static linking
AC_ARG_ENABLE(shared,
    [AS_HELP_STRING([--disable-shared],
        [force static linking])],
    [if test "$enableval" = "no"; then
        shared_suffix=""
        echo lo!
    else
        shared_suffix=" -shlib"
    fi],
    [shared_suffix=" -shlib"])

## Sets compiler.
AC_PROG_CC([h5cc])
AC_PROG_FC([h5fc])
AC_PROG_INSTALL

## Fix up the INSTALL macro if its a relative path. We want the
## full-path to the binary instead.
case "$INSTALL" in
  *install-sh*)
    INSTALL='\${top_srcdir}/bin/install-sh -c'
    ;;
esac

## ----------------------------------------------------------------------
## Check which archiving tool to use. This needs to be done before
## the AC_PROG_LIBTOOL macro.
##
if test -z "$AR"; then
  AC_CHECK_PROGS([AR], [ar xar], [:], [$PATH])
fi

AC_SUBST([AR])

## Export the AR macro so that it will be placed in the libtool file
## correctly.
export AR

AC_PROG_MAKE_SET
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

## Post processing to patch up some deficiencies in libtool
case $host_os in
  linux*)
    # If gcc is not used, need to set $wl to use "-Wl,"
    if $CC -v 2>&1 | grep '^gcc' > /dev/null ; then
      : using gcc
    else
      echo 'fixing $wl in' $ofile
ed - $ofile <<EOF 2> /dev/null
/^wl=""/s//wl="-Wl,"/
w
q
EOF

    fi
    ;;
esac


AC_SUBST([LD_LIBRARY_PATH]) LD_LIBRARY_PATH="$LD_LIBRARY_PATH"


## Add the suffix to CC for shared linking.  Cannot just set as an option
## because it must be first.
if test "$shared_suffix" && test ! `echo $CC | grep "$shared_suffix"`; then
    CC=${CC}${shared_suffix}
fi

## ======================================================================
## Checks for library functions
## ======================================================================

AC_MSG_CHECKING([for math library support])
AC_TRY_LINK([#include <math.h>], [volatile x = 37.927; k=1.2; sinh(x*k/cosh(x/k)],
            [AC_MSG_RESULT([yes])],
            [AC_MSG_RESULT([no]); LIBS="$LIBS -lm"])


## Checks for libraries.

## Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])

## Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

## Checks for library functions.
AC_FUNC_ERROR_AT_LINE
##AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memset])

## Check which plugins to build, if no environmental variables are set, build
## all.
if test ! "$PLUGIN_H5BZ2"
then
  PLUGIN_H5BZ2=1
fi
AM_CONDITIONAL(H5BZ2, test "$PLUGIN_H5BZ2")

## Check if a Fortran 2003 compiler was used for h5fc
HAVE_FORTRAN_2003="no"

AC_MSG_CHECKING([if h5fc was compiled with Fortran 2003 enabled])
if (${FC} -showconfig 2>&1 | grep 'Fortran 2003 Compiler: yes') > /dev/null; then
  HAVE_FORTRAN_2003="yes"
fi
echo $HAVE_FORTRAN_2003 

## Check if we have Fortran 2003 and Intel compiler; Intel does not work with RECURSIVE used by h5ex_g_traverse.f90
## The example will not be built when Intel compiler is used (EIP 2011/10/14)

if test "X$HAVE_FORTRAN_2003" = "Xyes"; then
  HAVE_FORTRAN_2003_NOTINTEL="yes"
  AC_MSG_CHECKING([if h5fc is an Intel Fortran compiler])
  if (${FC} -showconfig 2>&1 | grep 'Intel(R) Fortran ') > /dev/null; then
    HAVE_FORTRAN_2003_NOTINTEL="no"
  fi
  if test "X$HAVE_FORTRAN_2003_NOTINTEL" = "Xyes"; then
    echo "no"
  else
    echo "yes"
  fi 
fi
## End check if we have Fortran 2003 and Intel compiler.

AM_CONDITIONAL([FORTRAN_2003_CONDITIONAL_F], [test "X$HAVE_FORTRAN_2003" = "Xyes"])
AM_CONDITIONAL([FORTRAN_2003_NOTINTEL_CONDITIONAL_F], [test "X$HAVE_FORTRAN_2003_NOTINTEL" = "Xyes"])

LT_OUTPUT

## Set subdirectories
AC_CONFIG_FILES([makefile src/makefile example/makefile ])

## Configure
AC_OUTPUT
