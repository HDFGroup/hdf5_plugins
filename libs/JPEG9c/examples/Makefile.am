## Process this file with automake to produce Makefile.in
#
#  Automake Makefile for the JPEG library
#
#  This file is written by Bob Friesenhahn, Guido Vollbeding
#


# Headers which are not installed
OTHERINCLUDES = cdjpeg.h  transupp.h

AM_CPPFLAGS = -I$(srcdir)/../src

# Executables to build
noinst_PROGRAMS = cjpeg djpeg jpegtran rdjpgcom wrjpgcom

# Executable sources & libs
cjpeg_SOURCES    = cjpeg.c rdppm.c rdgif.c rdtarga.c rdrle.c rdbmp.c \
        rdswitch.c cdjpeg.c
cjpeg_LDADD      = ../src/libjpeg.la
djpeg_SOURCES    = djpeg.c wrppm.c wrgif.c wrtarga.c wrrle.c wrbmp.c \
        rdcolmap.c cdjpeg.c
djpeg_LDADD      = ../src/libjpeg.la
jpegtran_SOURCES = jpegtran.c rdswitch.c cdjpeg.c transupp.c
jpegtran_LDADD   = ../src/libjpeg.la
rdjpgcom_SOURCES = rdjpgcom.c
wrjpgcom_SOURCES = wrjpgcom.c

# Files to be cleaned
CLEANFILES = testout.ppm testout.bmp testout.jpg testoutp.ppm testoutp.jpg \
        testoutt.jpg

# Run tests
test: check-local
check-local:
	rm -f testout*
	./djpeg -dct int -ppm -outfile testout.ppm  $(top_srcdir)/testfiles/testorig.jpg
	./djpeg -dct int -bmp -colors 256 -outfile testout.bmp  $(top_srcdir)/testfiles/testorig.jpg
	./cjpeg -dct int -outfile testout.jpg  $(top_srcdir)/testfiles/testimg.ppm
	./djpeg -dct int -ppm -outfile testoutp.ppm $(top_srcdir)/testfiles/testprog.jpg
	./cjpeg -dct int -progressive -opt -outfile testoutp.jpg $(top_srcdir)/testfiles/testimg.ppm
	./jpegtran -outfile testoutt.jpg $(top_srcdir)/testfiles/testprog.jpg
	cmp $(top_srcdir)/testfiles/testimg.ppm testout.ppm
	cmp $(top_srcdir)/testfiles/testimg.bmp testout.bmp
	cmp $(top_srcdir)/testfiles/testimg.jpg testout.jpg
	cmp $(top_srcdir)/testfiles/testimg.ppm testoutp.ppm
	cmp $(top_srcdir)/testfiles/testimgp.jpg testoutp.jpg
	cmp $(top_srcdir)/testfiles/testorig.jpg testoutt.jpg

