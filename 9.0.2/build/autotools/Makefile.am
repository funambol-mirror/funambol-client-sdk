## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = foreign
SUBDIRS = include src test
ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libfunambol.pc

deb: 
	@dpkg-buildpackage -rfakeroot -b -us -uc
