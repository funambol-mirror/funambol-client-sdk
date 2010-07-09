## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = fsync

SDKDIR = $(prefix)
INCDIR = $(SDKDIR)/include/funambol
SRCDIR =../../src

AM_CPPFLAGS = -I$(INCDIR)/posix -I$(INCDIR)/common

# fsync sources
SOURCES = $(SRCDIR)/FSyncConfig.cpp           \
          $(SRCDIR)/FSyncUpdater.cpp          \
          $(SRCDIR)/FSyncOpt.cpp              \
          $(SRCDIR)/fsync.cpp


fsync_SOURCES = $(SOURCES)
fsync_LDFLAGS = -L$(SDKDIR)/lib
fsync_LDADD = -lfunambol -lcurl
