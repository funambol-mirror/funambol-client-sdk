## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = fsync

INCDIR  = ../../../../src/include/
SRCDIR=../../src

AM_CPPFLAGS = -I$(INCDIR)/posix -I$(INCDIR)/common
AM_LDFLAGS = -L@FUNAMBOL_LIB_PATH@ -lfunambol

# fsync sources
SOURCES = $(SRCDIR)/FSyncConfig.cpp           \
          $(SRCDIR)/FSyncUpdater.cpp          \
          $(SRCDIR)/FSyncOpt.cpp              \
          $(SRCDIR)/fsync.cpp


fsync_SOURCES = $(SOURCES)
