TESTS = @CPPUNIT_TESTS@
EXTRA_PROGRAMS = client-test
check_PROGRAMS = @CPPUNIT_TESTS@

CXXDIR  = $(srcdir)/../../../src/c++
INCDIR  = $(srcdir)/../../../src/include
TESTDIR = $(srcdir)/../../../test

LDADD = ../src/libsync4j.la 
AM_LDFLAGS = @CURL_LDFLAGS@ @CPPUNIT_LDFLAGS@
AM_CXXFLAGS = @CURL_CFLAGS@ @CPPUNIT_CXXFLAGS@
AM_CPPFLAGS = -I$(INCDIR)/posix -I$(INCDIR)/common -DPOSIX

vpath %.cpp	$(TESTDIR) \
		$(TESTDIR)/common/base/util \
		$(TESTDIR)/common/base \
		$(TESTDIR)/posix/spdm/ \
		$(TESTDIR)/common/syncml

# Test runner sources
SOURCES = client-test-main.cpp client-test.cpp ClientTest.cpp

# Testcase sources
TESTS_BASE = \
    ArrayListTest.cpp \
    BasicTimeTest.cpp \
    StringBufferTest.cpp \
    ParserTest.cpp \
    ObjectDelTest.cpp \
    base64Test.cpp
    
TESTCASES = $(TESTS_BASE)

# With --disable-shared autotools links against libsync4j.a which does not
# pull any of the test suites into the test binary, so they would not be
# executed. The workaround is to explicitly set them as undefined on the
# link line.
client_test_SOURCES = $(SOURCES) $(TESTCASES)
#client_test_LDFLAGS = libsync4j.la

# ensure that files are in the current directory before the check runs
check-am check: client-test-files
.PHONY: client-test-files
client-test-files:
	@ [ -e synccompare.pl ] || ln -s $(TESTDIR)/synccompare.pl .
	@ [ -d testcases ] || ln -s $(TESTDIR)/testcases .
	@ [ -d Algorithm ] || ln -s $(TESTDIR)/Algorithm

# also install files in a test sub directory:
# in source files the API internal definitions need to
# be removed so that the files can be used externally
source_files = \
	test/ClientTest.h \
	test/ClientTest.cpp \
	test/client-test-main.cpp
verbatim_files = \
	test/Algorithm/Artistic \
	test/Algorithm/copyright \
	test/Algorithm/Diff.pm \
	test/Algorithm/README \
	test/testcases/ical20.ics \
	test/testcases/itodo20.ics \
	test/testcases/vcal10.ics \
	test/testcases/vcard21.vcf \
	test/testcases/vcard30.vcf \
	test/testcases/imemo20.ics \
	test/testcases/ex1.xml \
	test/testcases/ex2.xml \
	test/testcases/syncML1.xml \
	test/testcases/syncML2.xml \
	test/testcases/syncML3.xml \
	test/testcases/syncML4.xml \
	test/testcases/syncML5.xml \
	test/testcases/syncML6.xml \
	test/testcases/syncML7.xml \
	test/testcases/syncML8.xml \
	test/testcases/wrongex.xml \
        test/README.txt \
        test/synccompare.pl

nobase_pkginclude_HEADERS = \
	$(source_files) \
	$(verbatim_files)


CLEANFILES = $(nobase_pkginclude_HEADERS)

# fix files for public distribution
$(source_files): %: ../../../%
	mkdir -p ${@D}
	sed -e 's/WCHAR/char/g' -e 's/= *NULL/= 0/g' -e 's/__declspec([^)]*)//g' -e 's/__cdecl//g' -e 's/EXTRA_SECTION_[0123456789]*//g' $< >$@

# just copy for distribution
$(verbatim_files): %: ../../../%
	mkdir -p ${@D}
	cp $< $@
