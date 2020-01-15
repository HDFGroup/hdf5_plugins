## Process this file with automake to produce Makefile.in
#
#  Automake Makefile for the JPEG library
#
#  This file is written by Bob Friesenhahn, Guido Vollbeding
#

ACLOCAL_AMFLAGS = '-I m4'

# Other documentation files
DOCS= README install.txt usage.txt wizard.txt example.c libjpeg.txt \
        structure.txt coderules.txt filelist.txt change.log

# Makefiles for various systems
MKFILES= configure Makefile.in makefile.ansi makefile.unix makefile.bcc \
        makefile.mc6 makefile.dj makefile.wat makefile.vc makejdsw.vc6 \
        makeadsw.vc6 makejdep.vc6 makejdsp.vc6 makejmak.vc6 makecdep.vc6 \
        makecdsp.vc6 makecmak.vc6 makeddep.vc6 makeddsp.vc6 makedmak.vc6 \
        maketdep.vc6 maketdsp.vc6 maketmak.vc6 makerdep.vc6 makerdsp.vc6 \
        makermak.vc6 makewdep.vc6 makewdsp.vc6 makewmak.vc6 makejsln.v10 \
        makeasln.v10 makejvcx.v10 makejfil.v10 makecvcx.v10 makecfil.v10 \
        makedvcx.v10 makedfil.v10 maketvcx.v10 maketfil.v10 makervcx.v10 \
        makerfil.v10 makewvcx.v10 makewfil.v10 makeproj.mac makcjpeg.st \
        makdjpeg.st makljpeg.st maktjpeg.st makefile.manx makefile.sas \
        makefile.mms makefile.vms makvms.opt

# Configuration files
CONFIGFILES= jconfig.cfg jconfig.bcc jconfig.mc6 jconfig.dj jconfig.wat \
        jconfig.vc jconfig.mac jconfig.st jconfig.manx jconfig.sas \
        jconfig.vms

# Support scripts for configure
CONFIGUREFILES= config.guess config.sub install-sh ltmain.sh depcomp missing

SUBDIRS = src examples

# Test support files
TESTFILES= testorig.jpg testimg.ppm testimg.bmp testimg.jpg testprog.jpg \
        testimgp.jpg

# libtool libraries to build
lib_LTLIBRARIES = libjpeg.la

# Library sources for libjpeg.la
libjpeg_la_SOURCES = $(LIBSOURCES)

# LDFLAGS for libjpeg.la
libjpeg_la_LDFLAGS = -no-undefined \
        -version-info $(JPEG_LIB_VERSION)

if HAVE_LD_VERSION_SCRIPT
  libjpeg_la_LDFLAGS += -Wl,--version-script=$(srcdir)/libjpeg.map
endif

# Executables to build
bin_PROGRAMS = cjpeg djpeg jpegtran rdjpgcom wrjpgcom

# Executable sources & libs
cjpeg_SOURCES    = cjpeg.c rdppm.c rdgif.c rdtarga.c rdrle.c rdbmp.c \
        rdswitch.c cdjpeg.c
cjpeg_LDADD      = libjpeg.la
djpeg_SOURCES    = djpeg.c wrppm.c wrgif.c wrtarga.c wrrle.c wrbmp.c \
        rdcolmap.c cdjpeg.c
djpeg_LDADD      = libjpeg.la
jpegtran_SOURCES = jpegtran.c rdswitch.c cdjpeg.c transupp.c
jpegtran_LDADD   = libjpeg.la
rdjpgcom_SOURCES = rdjpgcom.c
wrjpgcom_SOURCES = wrjpgcom.c

# Manual pages to install
man_MANS = $(DISTMANS)

# Headers to install
include_HEADERS = $(INSTINCLUDES)

# Other distributed headers
noinst_HEADERS = $(OTHERINCLUDES)

# Install jconfig.h
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(includedir)
	$(INSTALL_HEADER) jconfig.h $(DESTDIR)$(includedir)/jconfig.h

# Uninstall jconfig.h
uninstall-local:
	rm -f $(DESTDIR)$(includedir)/jconfig.h
